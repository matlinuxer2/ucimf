/*
 *   Copyright © 2008-2009 dragchan <zgchan317@gmail.com>
 *   This file is part of FbTerm.
 *
 *   Extended modification: Chun-Yu Lee (Mat) <matlinuxer2@gmail.com> 
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation; either version 2
 *   of the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <fontconfig/fontconfig.h>
#include <ft2build.h>
#include FT_GLYPH_H
#include "font.h"

#define OFFSET(TYPE, MEMBER) ((size_t)(&(((TYPE *)0)->MEMBER)))
#define SUBS(a, b) ((a) > (b) ? (a) - (b) : (b) - (a))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

static FcCharSet *unicodeMap;
static FcFontSet *fontList;
 
static FT_Library ftlib;
static FT_Face *fontFaces;
static u32 *fontFlags;

static Font::Glyph **glyphCache;
static bool *glyphCacheInited;

static void openFont(u32 index);

s8  Font::mFontNames[64]="mono";
u32 Font::mFontSize=14;
s32 Font::mFontWidth=0;

DEFINE_INSTANCE(Font)

Font *Font::createInstance()
{
	FcInit();

	FcPattern *pat = FcNameParse((FcChar8 *)(*mFontNames ? mFontNames : "mono"));

	FcPatternAddDouble(pat, FC_PIXEL_SIZE, (double)mFontSize);

	FcPatternAddString(pat, FC_LANG, (FcChar8 *)"en");

	FcConfigSubstitute(NULL, pat, FcMatchPattern);
	FcDefaultSubstitute(pat);

	FcResult result;
	FcFontSet *fs = FcFontSort(NULL, pat, FcTrue, &unicodeMap, &result);

	if (fs) {
		fontList = FcFontSetCreate();

		FcObjectSet *family = FcObjectSetCreate();
		FcObjectSetAdd(family, FC_FAMILY);

		for (u32 i = 0; i < fs->nfont; i++) {
			FcPattern *font = FcFontRenderPrepare(NULL, pat, fs->fonts[i]);
			if (!font) continue;

			bool same = false;
			for (u32 j = 0; j < fontList->nfont; j++) {
				if (FcPatternEqualSubset(fontList->fonts[j], font, family)) {
					same = true;
					break;
				}
			}

			if (same) {
				FcPatternDestroy(font);
			} else {
				FcFontSetAdd(fontList, font);
			}
		}

		FcObjectSetDestroy(family);
	}

	FcPatternDestroy(pat);
	if (fs) FcFontSetDestroy(fs);

	if (fontList && fontList->nfont) return new Font();

	if (unicodeMap) FcCharSetDestroy(unicodeMap);
	if (fontList) FcFontSetDestroy(fontList);
	FcFini();
	return 0;
}

Font::Font()
{
	mHeight = mWidth = 0;

	fontFaces = new FT_Face[fontList->nfont];
	fontFlags = new u32[fontList->nfont];
	memset(fontFaces, 0, sizeof(FT_Face) * fontList->nfont);

	glyphCache = new Glyph *[256 * 256];
	glyphCacheInited = new bool[256];
	memset(glyphCacheInited, 0, sizeof(bool) * 256);

	FT_Init_FreeType(&ftlib);
	openFont(0);

	FT_Face face = fontFaces[0];
	if (face == (FT_Face)-1) return;

	if (face->face_flags & FT_FACE_FLAG_SCALABLE) {
		mHeight = face->size->metrics.height >> 6;
		mWidth = face->size->metrics.max_advance >> 6;
	} else if (face->num_fixed_sizes) {
		double dsize;
		FcPatternGetDouble(fontList->fonts[0], FC_PIXEL_SIZE, 0, &dsize);

		FT_Bitmap_Size *sizes = face->available_sizes;
		u32 index = 0, diffmin = (u32)-1;
		for (u32 i = 0; i < face->num_fixed_sizes; i++) {
			u32 diff = SUBS(sizes[i].size >> 6, (u32)dsize);
			if (diff < diffmin ) {
				index = i;
				diffmin = diff;
			}
		}

		mHeight = sizes[index].height;
		mWidth = sizes[index].width;
	}

	if (!(face->face_flags & FT_FACE_FLAG_FIXED_WIDTH)) mWidth = MIN(mWidth, (mHeight + 1) / 2);

	if (mFontWidth) {
		mWidth = mFontWidth;
	}
}

Font::~Font()
{
	for (u32 i = 0; i < 256; i++) {
		if (!glyphCacheInited[i]) continue;

		for (u32 j = 0; j < 256; j++) {
			if (glyphCache[i * 256 + j]) {
				delete[] (u8 *)glyphCache[i * 256 + j];
			}
		}
	}

	delete[] glyphCache;
	delete[] glyphCacheInited;

	for (u32 i = 0; i < fontList->nfont; i++) {
		if (fontFaces[i] && fontFaces[i] != (FT_Face)-1) {
			FT_Done_Face(fontFaces[i]);
		}
	}

	delete[] fontFaces;
	delete[] fontFlags;

	FT_Done_FreeType(ftlib);
	FcCharSetDestroy(unicodeMap);
	FcFontSetDestroy(fontList);
	FcFini();
}

void Font::showInfo(bool verbose)
{
	if (!verbose) return;

	printf("[font] width: %dpx, height: %dpx, ordered list: ", mWidth, mHeight);

	u32 index;
	FcChar8 *family;
	for (index = 0; index < fontList->nfont - 1; index++) {
		FcPatternGetString(fontList->fonts[index], FC_FAMILY, 0, &family);
		printf("%s, ", family);
	}

	FcPatternGetString(fontList->fonts[index], FC_FAMILY, 0, &family);
	printf("%s\n", family);
}

void Font::setInfo( const s8* font_names, const s32 font_size, const s32 font_width )
{
	size_t font_names_len = strlen( font_names );
	if( font_names_len < 64 ) {
		memcpy( mFontNames, font_names, font_names_len );
		mFontNames[font_names_len] = 0;
	}

	mFontSize = font_size ? font_size : mFontSize;
	mFontWidth = font_width ? font_width : mFontWidth;

	delete mpFont;
	mpFont = createInstance();
}

static void openFont(u32 index)
{
	if (index >= fontList->nfont) return;

	FcPattern *pattern = fontList->fonts[index];

	FcChar8 *name = (FcChar8 *)"";
	FcPatternGetString(pattern, FC_FILE, 0, &name);

	int id = 0;
	FcPatternGetInteger (pattern, FC_INDEX, 0, &id);

	FT_Face face;
	if (FT_New_Face(ftlib, (const char *)name, id, &face)) {
		fontFaces[index] = (FT_Face)-1;
		return;
	}

	double ysize;
	FcPatternGetDouble(pattern, FC_PIXEL_SIZE, 0, &ysize);
	FT_Set_Pixel_Sizes(face, 0, (FT_UInt)ysize);

	int load_flags = FT_LOAD_DEFAULT;

	FcBool scalable, antialias;
	FcPatternGetBool(pattern, FC_SCALABLE, 0, &scalable);
	FcPatternGetBool(pattern, FC_ANTIALIAS, 0, &antialias);

	if (scalable && antialias) load_flags |= FT_LOAD_NO_BITMAP;

	if (antialias) {
		FcBool hinting;
		int hint_style;
		FcPatternGetBool(pattern, FC_HINTING, 0, &hinting);
		FcPatternGetInteger(pattern, FC_HINT_STYLE, 0, &hint_style);

		if (!hinting || hint_style == FC_HINT_NONE) {
			load_flags |= FT_LOAD_NO_HINTING;
		} else {
			load_flags |= FT_LOAD_TARGET_LIGHT;
		}
	} else {
		load_flags |= FT_LOAD_TARGET_MONO;
	}

	fontFaces[index] = face;
	fontFlags[index] = load_flags;
}

static int fontIndex(u32 unicode)
{
	if (!FcCharSetHasChar(unicodeMap, unicode)) return -1;

	FcCharSet *charset;
	for (u32 i = 0; i < fontList->nfont; i++) {
		FcPatternGetCharSet(fontList->fonts[i], FC_CHARSET, 0, &charset);
		if (FcCharSetHasChar(charset, unicode)) return i;
	}

	return -1;
}

Font::Glyph *Font::getGlyph(u32 unicode)
{
	if (unicode >= 256 * 256) return 0;

	if (!glyphCacheInited[unicode >> 8]) {
		glyphCacheInited[unicode >> 8] = true;
		memset(&glyphCache[unicode & 0xff00], 0, sizeof(Glyph *) * 256);
	}

	if (glyphCache[unicode]) return glyphCache[unicode];

	int i = fontIndex(unicode);
	if (i == -1) return 0;

	if (!fontFaces[i]) openFont(i);
	if (fontFaces[i] == (FT_Face)-1) return 0;

	FT_Face face = fontFaces[i];
	FT_UInt index = FT_Get_Char_Index(face, (FT_ULong)unicode);
	if (!index) return 0;

	FT_Load_Glyph(face, index, FT_LOAD_RENDER | fontFlags[i]);
	FT_Bitmap &bitmap = face->glyph->bitmap;

	u32 x, y, w, h, nx, ny, nw, nh;
	x = y = 0;
	w = nw = bitmap.width;
	h = nh = bitmap.rows;

	Glyph *glyph = (Glyph *)new u8[OFFSET(Glyph, pixmap) + nw * nh];
	glyph->left = face->glyph->metrics.horiBearingX >> 6;
	glyph->top = mHeight - 1 + (face->size->metrics.descender >> 6) - (face->glyph->metrics.horiBearingY >> 6);
	glyph->width = face->glyph->metrics.width >> 6;
	glyph->height = face->glyph->metrics.height >> 6;
	glyph->pitch = nw;

	u8 *buf = bitmap.buffer;
	for (y = 0; y < h; y++, buf += bitmap.pitch) {
		for (x = 0; x < w; x++) {
			nx = x, ny = y;

			glyph->pixmap[ny * nw + nx] =
				(bitmap.pixel_mode == FT_PIXEL_MODE_MONO) ? ((buf[(x >> 3)] & (0x80 >> (x & 7))) ? 0xff : 0) : buf[x];
		}
	}

	glyphCache[unicode] = glyph;
	return glyph;
}


// wrapper

struct_Font_Glyph *call_Font_getGlyph(u32 unicode){
	return (struct_Font_Glyph *)Font::instance()->getGlyph(unicode);
}


u32 call_Font_width(){
	return Font::instance()->width();
}

u32 call_Font_height(){
	return Font::instance()->height();
}

void call_Font_showInfo(){
	Font::instance()->showInfo( true ); 
}

void call_Font_setInfo( const s8* font_names, const s32 font_size, const s32 font_width ){
	return Font::instance()->setInfo( font_names, font_size, font_width );
}

