/*
 * JFBTERM -
 * Copyright (C) 1999 Noritoshi MASUICHI (nmasu@ma3.justnet.ne.jp)
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
 * THIS SOFTWARE IS PROVIDED BY NORITOSHI MASUICHI ``AS IS'' AND ANY
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
/*
 * This code is based on the following :
 * 
 * KON2 - Kanji ON Console -
 * Copyright (C) 1992-1996 Takashi MANABE (manabe@papilio.tutics.tut.ac.jp)
 */

#include	<stdio.h>
#include	<string.h>
#include	<unistd.h>
#include	<stdlib.h>

#include	"getcap.h"

#include	"util.h"
#include	"mytypes.h"
#include	"message.h"

#define BUF_SIZE	1024
#define MAX_COLS	256

/*--------------------------------------------------------------------------*/
void tcapValue_init(TCapValue* p)
{
	p->next = NULL;
	p->value = NULL;
}

void tcapValue_final(TCapValue* p)
{
	util_free(p->value);
}

void tcapability_init(TCapability* p)
{
	p->next = NULL;
	p->name = NULL;
	p->values = NULL;
}

void tcapability_del_value_all(TCapability* p)
{
	TCapValue* r;
	TCapValue* rn;

	r = p->values;
	while (r) {
		rn = r->next;
		tcapValue_final(r);
		util_free(r);
		r = rn;
	}
	p->values = NULL;
}

void tcapability_final(TCapability* p)
{
	tcapability_del_value_all(p);
	util_free(p->name);
}

void tcapability_set_name(TCapability* p, const char* name)
{
	util_free(p->name);
	if (name) {
		p->name = strdup(name);
	}
}

void tcapability_add_value(TCapability* p, const char* value)
{
	TCapValue* cp;
	TCapValue* en;

	cp = (TCapValue*)malloc(sizeof(TCapValue));
	tcapValue_init(cp);
	if (p->values) {
		en = p->values;
		while (en->next) {
			en = en->next;
		}
		en->next = cp;
	} else {
		p->values = cp;
	}
	cp->value = strdup(value);
}

void tcaps_init(TCaps* p)
{
	p->next = NULL;
}

void tcaps_final(TCaps* p)
{
	TCapability* r;
	TCapability* rn;
	r = p->next;
	while (r) {
		rn = r->next;
		tcapability_final(r);
		util_free(r);
		r = rn;
	}
	p->next = NULL;
}
/*--------------------------------------------------------------------------*/
#if DEBUG
void tcaps_debug(TCaps* p)
{
	TCapability* np;
	TCapValue* vp;

	np = p->next;
	while (np) {
		print_warn("%s:\n", np->name);
		vp = np->values;
		while (vp) {
			print_warn("	%s\n", vp->value);
			vp = vp->next;
		}
		np = np->next;
	}
}
#endif

TCapability* tcaps_find(TCaps* p, const char *name)
{
	TCapability* cp;
	cp = p->next;
	while (cp) {
		if ((cp->name != NULL) && (strcasecmp(name, cp->name) == 0)) {
			break;
		}
		cp = cp->next;
	}
	return cp;
}

char* tcaps_find_first(TCaps* p, const char *name)
{
	TCapability* cp;
	cp = tcaps_find(p, name);

	if (!cp || !(cp->values)) {
		return NULL;
	} else {
		return cp->values->value;
	}
}

char *
tcaps_find_entry(TCaps *p, const char* prefix, const char *name)
{
	char *key;
	char *val;
	key = (char *)malloc(strlen(prefix) + strlen(name) + 1);
	strcpy(key, prefix);
	strcat(key, name);
	val = tcaps_find_first(p, key);
	free(key);
	return val;
}

void	tcaps_register_nv(TCaps* p, const char *name, const char *value, char f)
{
	TCapability* cp;

	cp = tcaps_find(p, name);
	if (!cp) {
		cp = (TCapability*)malloc(sizeof(TCapability));
		tcapability_init(cp);
		cp->next = p->next;
		p->next = cp;
		tcapability_set_name(cp, name);
	}
	if (f == '=') {
		tcapability_del_value_all(cp);
	}
	tcapability_add_value(cp, value);
}

char* trim_left(char* cp)
{
	while (cp) {
		if (*cp != ' ' && *cp != '\t') {
			break;
		}
		cp++;
	}
	return cp;
}

char* trim_left_right(char* cp)
{
	char* q;
	char c;

	cp = trim_left(cp);
	q = cp + strlen(cp);
	while (cp < q) {
		c = *(--q);
		if (c != ' ' && c != '\t') {
			*(++q) = '\0';
			break;
		}
	}
	return cp;
}

TBool tcaps_register(TCaps* p, const char *cp)
{
	char line[MAX_COLS];
	char* n;
	char* v;
	char* q;

	strcpy(line, cp);
	if ((q = strchr(line, ':')) == NULL) {
		return FALSE;
	}
	*q = '\0';
	n = trim_left_right(line);
	v = trim_left_right(q+1);

	if (*n == '\0') {
		return FALSE;
	}
	
	if (*n == '+') {
		tcaps_register_nv(p, n+1, v, '+');
	} else {
		tcaps_register_nv(p, n, v, '=');
	}

	return TRUE;
}

/*--------------------------------------------------------------------------*/
void tcaps_read(TCaps* p, const char *filename)
{
	FILE* fp;
	char* q;
	char line[MAX_COLS];
	TBool b;
	int nl = 0;
	int len;

	if ((fp = fopen(filename, "r")) == NULL) {
		print_strerror_and_exit(filename);
	}

	print_message("(**) : Configuration file `%s'\n", filename);

	while(fgets(line, MAX_COLS, fp) != NULL) {
		nl++;
		len = strlen(line);
		if (len > 0) {
			line[len-1] = '\0';
		}
		if ((q = strchr(line, '#')) != NULL) {
			*q = '\0';
		}
		q = trim_left(line);
		if (*q == '\0') {
			continue;
		}
		if (strchr(line, ':') == NULL) {
			print_message("(--) : line %d, `%s'\n", nl, line);
			continue;
		}
		b = tcaps_register(p, line);
		if (!b) {
			print_message("(--) : line %d, `%s'\n", nl, line);
		} else {
			print_message("(**) : line %d, `%s'\n", nl, line);
		}
	}
	print_message("(**) : total %d lines read.\n", nl);

	fclose(fp);
}

void tcaps_read_args(TCaps* p, int argc, char *argv[])
{
	TBool b;
	int nl = 0;
	char* q;

	print_warn("(**) : command line option(s)\n");

	for (nl = 1 ; nl < argc ; nl++) {
		q = trim_left(argv[nl]);
		if (*q == '\0') {
			continue;
		}
		if (strlen(argv[nl]) >= MAX_COLS) {
			print_warn("(--) : arg %d, `%s'\n", nl, argv[nl]);
			continue;
		}
		if (strchr(argv[nl], ':') == NULL) {
			print_warn("(--) : arg %d, `%s'\n", nl, argv[nl]);
			continue;
		}
		b = tcaps_register(p, argv[nl]);
		if (!b) {
			print_warn("(--) : arg %d, `%s'\n", nl, argv[nl]);
		}
	}
	print_warn("(**) : total %d args read.\n", nl);

}

