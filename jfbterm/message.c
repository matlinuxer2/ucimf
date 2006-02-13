/*
 * JFBTERM -
 * Copyright (C) 1999 by Noritoshi Masuichi (nmasu@ma3.justnet.ne.jp)
 *
 * KON2 - Kanji ON Console -
 * Copyright (C) 1993 by MAEDA Atusi (mad@math.keio.ac.jp)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY MAEDA ATUSI ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE TERRENCE R. LAMBERT BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 */

#include	<stdio.h>
#include	<errno.h>
#include	<stdarg.h>
#include	<stdlib.h>
#include	<string.h>
#include	<sys/types.h>

#include	"main.h"
#include	"message.h"

void die(const char *format, ...)
{
	va_list args;

	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	exit(EXIT_FAILURE);
}

void die_file_eof(FILE* fp)
{
	die("Unexpectied EOF\n");
}

void print_warn(const char *format, ...)
{
	va_list args;

	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
}

void print_error(const char *format, ...)
{
	va_list args;

	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
}

void print_message(const char *format, ...)
{
	va_list args;

	if (gApp.gOptQuiet)
	    return;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
}

void print_strerror(const char *msg)
{
	print_message("system error - %s: %s\r\n", msg, strerror(errno));
}

void print_strerror_and_exit(const char *message)
{
	fprintf(stderr, "%s: %s\r\n", message, strerror(errno));
	exit(EXIT_FAILURE);
}
