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

#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "imapi.h"

#define OFFSET(TYPE, MEMBER) ((size_t)(&(((TYPE *)0)->MEMBER)))
#define MSG(a) ((Message *)(a))

static int imfd = -1;
static ImCallbacks cbs;
static char pending_msg_buf[10240];
static unsigned pending_msg_buf_len = 0;
static int im_active = 0;

void register_im_callbacks(ImCallbacks callbacks)
{
	cbs = callbacks;
}

int get_im_socket()
{
	static char init = 0;
	if (!init) {
		init = 1;

		char *val = getenv("FBTERM_IM_SOCKET");
		if (val) {
			char *tail;
			int fd = strtol(val, &tail, 0);
			if (!*tail) imfd = fd;
		}
	}

	return imfd;
}

void connect_fbterm(char raw)
{
	get_im_socket();
	if (imfd == -1) return;

	Message msg;
	msg.type = Connect;
	msg.len = sizeof(msg);
	msg.raw = (raw ? 1 : 0);
	int ret = write(imfd, (char *)&msg, sizeof(msg));
}

void put_im_text(char *text, unsigned short len)
{
	if (imfd == -1 || !im_active || !text || !len || (OFFSET(Message, texts) + len > UINT16_MAX)) return;

	char buf[OFFSET(Message, texts) + len];

	MSG(buf)->type = PutText;
	MSG(buf)->len = sizeof(buf);
	memcpy(MSG(buf)->texts, text, len);

	int ret = write(imfd, buf, MSG(buf)->len);
}

void set_im_windows(ImWin *rects, unsigned short num)
{
	if (imfd == -1 || !im_active || (!rects && num) || num > NR_IM_WINS) return;

	char buf[OFFSET(Message, wins) + num * sizeof(ImWin)];

	MSG(buf)->type = SetWins;
	MSG(buf)->len = sizeof(buf);
	if (num) {
		memcpy(MSG(buf)->wins, rects, num * sizeof(ImWin));
	}

	int ret = write(imfd, buf, MSG(buf)->len);

	int ack = 0;
	while (!ack) {
		char *cur = pending_msg_buf + pending_msg_buf_len;
		int len = read(imfd, cur, sizeof(pending_msg_buf) - pending_msg_buf_len);

		if (len == -1 && errno == ECONNRESET) { // FbTerm has exited
			close(imfd);
			imfd = -1;
			return;
		} else if (len <= 0) continue;

		pending_msg_buf_len += len;

		char *end = cur + len;
		for (; cur < end && MSG(cur)->len <= (end - cur); cur += MSG(cur)->len) {
			if (MSG(cur)->type == AckWins) {
				memcpy(cur, cur + MSG(cur)->len, end - cur - MSG(cur)->len);
				pending_msg_buf_len -= MSG(cur)->len;

				ack = 1;
				break;
			}
		}
	}
}

static int process_message(Message *msg)
{
	int exit = 0;

	switch (msg->type) {
	case Disconnect:
		close(imfd);
		imfd = -1;
		exit = 1;
		break;

	case FbTermInfo:
		if (cbs.fbterm_info) {
			cbs.fbterm_info(&msg->info);
		}
		break;

	case Active:
		im_active = 1;
		if (cbs.active) {
			cbs.active();
		}
		break;

	case Deactive:
		if (cbs.deactive) {
			cbs.deactive();
		}
		im_active = 0;
		break;

	case ShowUI:
		if (cbs.show_ui) {
			cbs.show_ui();
		}
		break;

	case HideUI: {
		if (cbs.hide_ui) {
			cbs.hide_ui();
		}

		Message msg;
		msg.type = AckHideUI;
		msg.len = sizeof(msg);
		int ret = write(imfd, (char *)&msg, sizeof(msg));
		break;
		}

	case SendKey:
		if (im_active && cbs.send_key) {
				cbs.send_key(msg->keys, msg->len - OFFSET(Message, keys));
		}
		break;

	case CursorPosition:
		if (im_active && cbs.cursor_position) {
			cbs.cursor_position(msg->cursor.x, msg->cursor.y);
		}
		break;

	case TermMode:
		if (im_active && cbs.term_mode) {
			cbs.term_mode(msg->term.crWithLf, msg->term.applicKeypad, msg->term.cursorEscO);
		}
		break;

	default:
		break;
	}

	return exit;
}

int check_im_message()
{
	char buf[sizeof(pending_msg_buf)];
	int len;

	if (pending_msg_buf_len) {
		len = pending_msg_buf_len;
		pending_msg_buf_len = 0;

		memcpy(buf, pending_msg_buf, len);
	} else {
		if (imfd == -1) return 0;

		len = read(imfd, buf, sizeof(buf));

		if (len == -1 && errno == ECONNRESET) {
			close(imfd);
			imfd = -1;
			return 0;
		} else if (len <= 0) return 1;
	}

	char *cur = buf, *end = cur + len;
	int exit = 0;

	for (; cur < end && MSG(cur)->len <= (end - cur); cur += MSG(cur)->len) {
		exit |= process_message(MSG(cur));
	}

	return !exit;
}
