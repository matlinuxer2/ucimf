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

#ifndef SCREEN_H
#define SCREEN_H

#include "instance.h"

bool is_double_width(unsigned ucs);

#define NR_COLORS 16
enum { Black = 0, Red, Green, Brown, Blue, Magenta, Cyan, White };

struct Color {
	unsigned char blue, green, red;
};

typedef enum { Rotate0 = 0, Rotate90, Rotate180, Rotate270 } RotateType;

class Screen
{
	DECLARE_INSTANCE(Screen)
public :
	static void setRotateType(RotateType type) { mRotate = type; }

	static unsigned width() { return mScreenw; }
	static unsigned height() { return mScreenh; }

	void fillRect(unsigned x, unsigned y, unsigned w, unsigned h, unsigned color);
	void drawText(unsigned x, unsigned y, unsigned fc, unsigned bc, unsigned num, unsigned short *text, bool *dws);
	void updateYOffset();

private:
	friend class Font;
	static RotateType rotateType() { return mRotate; }
	static void rotateRect(unsigned &x, unsigned &y, unsigned &w, unsigned &h);
	static void rotatePoint(unsigned w, unsigned h, unsigned &x, unsigned &y);

	Screen(int fd);
	void drawGlyphs(unsigned x, unsigned y, unsigned w, unsigned fc, unsigned bc, unsigned num, unsigned short *text, bool *dw);
	int drawGlyph(unsigned x, unsigned y, unsigned fc, unsigned bc, unsigned short code, bool dw, bool fillbg);

	int mFd;
	char *mpMemStart;
	static RotateType mRotate;
	static unsigned mScreenw, mScreenh;
};

#endif
