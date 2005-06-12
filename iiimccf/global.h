// vi:ts=4:shiftwidth=4:expandtab
/***************************************************************************
                          global.h  -  global definition
                             -------------------
    begin                : Sun Mar 18 2001
    copyright            : (C) 2001 by ejoy
    email                : ejoy@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef GLOBAL_H
#define GLOBAL_H
//for locale support
//#if defined(linux)
    #include <libintl.h>
    #include <locale.h>
//#elif defined(__FreeBSD__)
//    #include "/usr/local/include/libintl.h"
//#endif
#define _(String) gettext (String)
#define gettext_noop(String) (String)

#include "graphdev.h"
#include "hzdecoder.h"
//class Console;
class Zhcon;
//class InputManager;
extern GraphDev* gpScreen;
extern HzDecoder* gpDecoder;
extern Zhcon* gpZhcon;

enum Encode {NOCHANGE, ASCII, GB2312, GBK, BIG5, JIS, KSC};

#define BuildColor(fg,bg) (((bg) << 4) + (fg))
#define FgColor(fg) ((fg) & 0x0f)
#define BgColor(bg) ((bg) >> 4)

void SignalHandle(int signo);
void SignalChild(int signo);
void Beep(void);
void Beep(bool onoff);
#endif
