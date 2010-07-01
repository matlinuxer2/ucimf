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

#ifndef FONT_H
#define FONT_H

//#include "type.h"
typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;



#ifdef __cplusplus

//#include "instance.h"
#define DECLARE_INSTANCE(classname) \
public: \
	static classname *instance(); \
	static void uninstance(); \
protected: \
	classname(); \
	virtual ~classname(); \
private: \
	static classname *createInstance(); \
	static classname *mp##classname;

#define DEFINE_INSTANCE(classname) \
classname *classname::mp##classname = 0; \
 \
classname *classname::instance() \
{ \
	if (!mp##classname) { \
		mp##classname = createInstance(); \
	} \
	 \
	return mp##classname; \
} \
 \
void classname::uninstance() \
{ \
	if (mp##classname) { \
		delete mp##classname; \
		mp##classname = 0; \
	} \
}

#define DEFINE_INSTANCE_DEFAULT(classname) \
DEFINE_INSTANCE(classname) \
 \
classname *classname::createInstance() \
{ \
	return new classname(); \
}


class Font {
	DECLARE_INSTANCE(Font)
public:
	struct Glyph {
		s16 pitch, width, height;
		s16 left, top;
		u8 pixmap[0];
	};

	Glyph *getGlyph(u32 unicode);
	u32 width() {
		return mWidth;
	}
	u32 height() {
		return mHeight;
	}
	void showInfo(bool verbose);
	void setInfo( const s8* font_names, const s32 font_size, const s32 font_width );

private:
	u32 mWidth, mHeight;
	static s8  mFontNames[64];
	static u32 mFontSize;
	static s32 mFontWidth;
};

#endif //__cplusplus


#ifdef __cplusplus
extern "C" {
#endif

typedef struct the_Font_Glyph {
	s16 pitch, width, height;
	s16 left, top;
	u8 pixmap[0];
} struct_Font_Glyph;

struct_Font_Glyph *call_Font_getGlyph(u32 unicode);
u32 call_Font_width();
u32 call_Font_height();
void call_Font_showInfo();
void call_Font_setInfo( const s8* font_names, const s32 font_size, const s32 font_width );

#ifdef __cplusplus                                                                                  
}
#endif


#endif
