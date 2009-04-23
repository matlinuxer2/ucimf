/*
 *   Copyright © 2008-2009 dragchan <zgchan317@gmail.com>
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

#ifndef IM_API_H
#define IM_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include "immessage.h"

typedef void (*ActiveFun)();
typedef void (*DeactiveFun)();
typedef void (*ShowUIFun)();
typedef void (*HideUIFun)();
typedef void (*SendKeyFun)(char *keys, unsigned short len);
typedef void (*CursorPositionFun)(unsigned x, unsigned y);
typedef void (*FbTermInfoFun)(Info *info);
typedef void (*TermModeFun)(char crlf, char appkey, char curo);

typedef struct {
	ActiveFun active; // called when receive message Active
	DeactiveFun deactive; // for message Deactive
	ShowUIFun show_ui; // for message ShowUI
	HideUIFun hide_ui; // for message HideUI
	SendKeyFun send_key;  // for message SendKey
	CursorPositionFun cursor_position; // for message CursorPoition
	FbTermInfoFun fbterm_info; // for message FbTermInfo
	TermModeFun term_mode; // for message TermMode
} ImCallbacks;

extern void register_im_callbacks(ImCallbacks callbacks);

// get file id of the socket connected to FbTerm
extern int get_im_socket();

// receive message from FbTerm, and call functions registered with register_im_callbacks()
// when receive message DisconnectIM, return false to indicate IM program should exit, otherwise return true.
extern int check_im_message();

// send message Connect
// if raw == 1, reuest FbTerm to set keyboard input mode to K_MEDIUMRAW when IM actived
// if raw == 0, FbTerm will keep K_UNICODE mode
extern void connect_fbterm(char raw);

// send message PutText, text should be encoded with utf8
extern void put_im_text(char *text, unsigned short len);

// send message SetWins to FbTerm, and wait until AckWins has arrived
extern void set_im_windows(ImWin *rects, unsigned short num);

#ifdef __cplusplus
}
#endif

#endif
