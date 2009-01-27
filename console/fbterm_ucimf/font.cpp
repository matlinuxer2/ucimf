/*
 *   Copyright © 2008 dragchan <zgchan317@gmail.com>
 *   This file is part of FbTerm.
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

#include <string.h>
#include <fontconfig/fontconfig.h>
#include <ft2build.h>
#include FT_GLYPH_H
#include "font.h"
#include "screen.h"

#define OFFSET(TYPE, MEMBER) ((size_t)(&(((TYPE *)0)->MEMBER)))

DEFINE_INSTANCE(Font)

static char fontName[64];
static unsigned short fontSize;

void Font::setFontInfo(char *name, unsigned short pixelsize)
{
	if (!name || strlen(name) >= sizeof(fontName)) return;

	memcpy(fontName, name, strlen(name) + 1);
	fontSize = pixelsize;
}

Font *Font::createInstance()
{
	if (!fontName[0] || !fontSize) return 0;

	FcInit();

	FcPattern *pat = FcNameParse((FcChar8 *)fontName);
	FcPatternAddDouble(pat, FC_PIXEL_SIZE, (double)fontSize);
	FcPatternAddString(pat, FC_LANG, (FcChar8 *)"en");

	FcConfigSubstitute(NULL, pat, FcMatchPattern);
	FcDefaultSubstitute(pat);

	FcResult result;
	FcCharSet *cs;
	FcFontSet *fs = FcFontSort(NULL, pat, FcTrue, &cs, &result);

	FontRec *fonts = 0;
	unsigned index = 0;
	if (fs) {
		fonts = new FontRec[fs->nfont];
	
		for (int i = 0; i < fs->nfont; i++) {
			FcPattern *font = FcFontRenderPrepare(NULL, pat, fs->fonts[i]);
			if (font) {
				fonts[index].face = 0;
				fonts[index++].pattern = font;
			}
		}
	}

	FcPatternDestroy(pat);
	if (fs) FcFontSetDestroy(fs);

	if (!index) {
		if (cs) {
			FcCharSetDestroy(cs);
			delete[] fonts;
		}

		FcFini();
		return 0;
	}

	return new Font(fonts, index, cs);
}

static FT_Library ftlib = 0;
static Font::Glyph **glyphCache;
static bool *glyphCacheInited;

Font::Font(FontRec *fonts, unsigned num, void *unicover)
{
	mpFontList = fonts;
	mFontNum = num;
	mpUniCover = unicover;
	mMonospace = false;
	glyphCache = new Glyph *[256 * 256];
	glyphCacheInited = new bool[256];
	bzero(glyphCacheInited, sizeof(bool) * 256);

	int spacing;
	if (FcPatternGetInteger((FcPattern*)mpFontList[0].pattern, FC_SPACING, 0, &spacing) == FcResultMatch)
		mMonospace = (spacing != FC_PROPORTIONAL);

	FT_Init_FreeType(&ftlib);

	openFont(0);
	mHeight = ((FT_Face)mpFontList[0].face)->size->metrics.height >> 6;
	mWidth = ((FT_Face)mpFontList[0].face)->size->metrics.max_advance >> 6;

	if (!mMonospace) {
		Glyph *glyph = getGlyph('a');
		if (mWidth > glyph->advance * 2) mWidth /= 2;
	}
}

Font::~Font()
{
	FcCharSetDestroy((FcCharSet*)mpUniCover);

	for (unsigned i = 0; i < mFontNum; i++) {	
		FcPatternDestroy((FcPattern*)mpFontList[i].pattern);

		FT_Face face = (FT_Face)mpFontList[i].face;
		if (face > 0) {
			FT_Done_Face(face);
		}
	}

	FT_Done_FreeType(ftlib);
	FcFini();

	delete[] mpFontList;	

	for (unsigned i = 0; i < 256; i++) {
		if (!glyphCacheInited[i]) continue;

		for (unsigned j = 0; j < 256; j++) {
			if (glyphCache[i * 256 + j]) {
				delete[] (char *)glyphCache[i * 256 + j];
			}
		}
	}
	
	delete[] glyphCache;
	delete[] glyphCacheInited;
}

void Font::openFont(unsigned index)
{
	if (index >= mFontNum) return;

	FcPattern *pattern = (FcPattern*)mpFontList[index].pattern;

	FcChar8 *name = (FcChar8 *)"";
	FcPatternGetString(pattern, FC_FILE, 0, &name);

	int id = 0;
	FcPatternGetInteger (pattern, FC_INDEX, 0, &id);

	FT_Face face;
	if (FT_New_Face(ftlib, (const char *)name, id, &face)) {
		mpFontList[index].face = (void *)-1;
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

	mpFontList[index].face = face;
	mpFontList[index].load_flags = load_flags;
}

int Font::fontIndex(unsigned unicode)
{
	if (!FcCharSetHasChar((FcCharSet *)mpUniCover, (FcChar32)unicode)) return -1;

	FcCharSet *charset;
	for (unsigned i = 0; i < mFontNum; i++) {
		FcPatternGetCharSet((FcPattern *)mpFontList[i].pattern, FC_CHARSET, 0, &charset);
		if (FcCharSetHasChar(charset, unicode)) return i;
	}

	return -1;
}

Font::Glyph *Font::getGlyph(unsigned unicode)
{
	if (unicode >= 256 * 256) return 0;
	
	if (!glyphCacheInited[unicode >> 8]) {
		glyphCacheInited[unicode >> 8] = true;
		bzero(&glyphCache[unicode & 0xff00], sizeof(Glyph *) * 256);
	}

	if (glyphCache[unicode]) return glyphCache[unicode];

	int i = fontIndex(unicode);
	if (i == -1) return 0;

	if (!mpFontList[i].face) openFont(i);
	if (mpFontList[i].face == (void *)-1) return 0;

	FT_Face face = (FT_Face)mpFontList[i].face;
	FT_UInt index = FT_Get_Char_Index(face, (FT_ULong)unicode);
	if (!index) return 0;

	FT_Load_Glyph(face, index, FT_LOAD_RENDER | mpFontList[i].load_flags);
	FT_Bitmap &bitmap = face->glyph->bitmap;
	
	unsigned x, y, w, h, nx, ny, nw, nh;
	x = y = 0;
	w = nw = bitmap.width;
	h = nh = bitmap.rows;
	Screen::rotateRect(x, y, nw, nh);

	Glyph *glyph = (Glyph *)new char[OFFSET(Glyph, pixmap) + nw * nh];
	glyph->left = face->glyph->metrics.horiBearingX >> 6;
	glyph->top = mHeight - 1 + (face->size->metrics.descender >> 6) - (face->glyph->metrics.horiBearingY >> 6);
	glyph->advance = face->glyph->metrics.horiAdvance >> 6;
	glyph->width = face->glyph->metrics.width >> 6;
	glyph->height = face->glyph->metrics.height >> 6;
	glyph->pitch = nw;
	glyph->isbitmap = false;

	unsigned char *buf = bitmap.buffer;
	for (y = 0; y < h; y++, buf += bitmap.pitch) {
		for (x = 0; x < w; x++) {
			nx = x, ny = y;
			Screen::rotatePoint(w, h, nx, ny);

			glyph->pixmap[ny * nw + nx] = 
				(bitmap.pixel_mode == FT_PIXEL_MODE_MONO) ? ((buf[(x >> 3)] & (0x80 >> (x & 7))) ? 0xff : 0) : buf[x];
		}
	}
	
	glyphCache[unicode]	= glyph;
	return glyph;
}
