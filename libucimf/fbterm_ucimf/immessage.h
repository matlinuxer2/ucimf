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

#ifndef IM_MESSAGE_H
#define IM_MESSAGE_H

/*
 * messages from IM to Fbterm:
 *
 * Connect:         IM send it to FbTerm after ending initization
 * PutText:         IM put translated input method text in this message, then send back to FbTerm
 * SetWins:         IM tell FbTerm that rectangles contained in the message will be used to draw IM's UI,
 *                  and FbTerm should NOT paint on these areas
 *
 *
 * messages from FbTerm to IM:
 *
 * FbTermInfo:      FbTerm send font name/size etc to IM, help IM to keep same UI with FbTerm
 * Disconnect:      FbTerm request IM to exit
 * Active:          FbTerm tell IM that user switch input method state on
 * Deactive:        FbTerm tell IM that user siwtch input method state off
 * SendKey:         when input method state is on, FbTerm send all keybord keys to IM
 * CursorPosition:  FbTerm tell IM that cursor poisition has been changed
 * AckWins:         when receive message SetWins, FbTerm send this message to IM,
 *                  on the other side, after sending SetWins, IM will keep waiting until receive AckWins from FbTerm
 * TermMode:        FbTerm send term mode to IM, help IM to translate keycode to terminal input
 *
 */

typedef enum {
	Connect = 0, Disconnect,
	Active, Deactive,
	SendKey, PutText,
	SetWins, AckWins,
	CursorPosition, FbTermInfo, TermMode,
	ShowUI, HideUI, AckHideUI
} MessageType;

typedef struct {
	unsigned char rotate;
	unsigned short fontSize, fontHeight, fontWidth;
	char fontName[0];
} Info;

#define NR_IM_WINS 10

typedef struct {
	unsigned x, y;
	unsigned w, h;
} ImWin;

typedef struct {
	unsigned short type;  // message's type, see enum MessageType
	unsigned short len;  // message's length, including head and body

	union {
		char keys[0]; // for SendKey
		char texts[0]; // for PutText
		ImWin wins[0]; // for SetWins
		Info info; // for FbTermInfo
		char raw; // for Connect, 1: when IM actived, FbTerm sets keyboard mode to K_MEDIUMRAW
				  //              0: keep K_UNICODE mode

		struct {
			char crWithLf;
			char applicKeypad;
			char cursorEscO;
		} term; // for TermMode;

		struct {
			unsigned x, y;
		} cursor; // for CursorPosition
	};
} Message;

#endif
