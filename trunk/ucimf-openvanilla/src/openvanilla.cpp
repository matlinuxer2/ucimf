/*
 * UCIMF - Unicode Console InputMethod Framework
 * Copyright (C) <2006>  Chun-Yu Lee (Mat) <Matlinuxer2@gmail.com> 
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "openvanilla.h"
#include <iconv.h>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"

/*
 * implementation of OVImfKeyCode
 */

OVImfKeyCode::OVImfKeyCode (int p=0)  { chr=p; shift=capslock=ctrl=alt=0; }
int OVImfKeyCode::code()              { return chr; }
bool OVImfKeyCode::isShift()           { return shift!=0; }
bool OVImfKeyCode::isCapslock()        { return capslock!=0; }
bool OVImfKeyCode::isCtrl()            { return ctrl!=0; }
bool OVImfKeyCode::isAlt()             { return alt!=0; }
bool OVImfKeyCode::isOpt()             { return alt!=0; }
bool OVImfKeyCode::isNum()             { return false; }

void OVImfKeyCode::setCode(int x)     { chr=x; }
void OVImfKeyCode::setShift(int x)    { shift=x; }
void OVImfKeyCode::setCapslock(int x) { capslock=x; }
void OVImfKeyCode::setCtrl(int x)     { ctrl=x; }
void OVImfKeyCode::setAlt(int x)      { alt=x; }

/*
 * implementation of OVImfBuffer
 */

OVImfBuffer::OVImfBuffer() {
  prdt = Preedit::getInstance();
  buf.clear();
}

OVBuffer* OVImfBuffer::clear() {
  prdt->clear();  
  buf.clear();
  return this;
}

OVBuffer* OVImfBuffer::append(const char *s) {
  prdt->append( (char*)s );
  buf+= s;
  return this;
}

OVBuffer* OVImfBuffer::send() {
    OVImf::commitBuffer( buf );
    clear();
    return this;
}

OVBuffer* OVImfBuffer::update() {
    prdt->render();
    return this;
}

OVBuffer* OVImfBuffer::update(int cursorPos, int markFrom, int markTo) {
    return update();
}

bool OVImfBuffer::isEmpty() {
  if ( buf.empty() )
  {
    return true;
  }
  else
  {
    return false;
  }
}


/*
 * implementation of OVImfCandidate
 */

OVImfCandidate::OVImfCandidate() {
    onscreen=0;
    lkc = LookupChoice::getInstance();
    lkc->clear();
}

OVCandidate* OVImfCandidate::clear() {
    lkc->clear();
    return this;
}

OVCandidate* OVImfCandidate::append(const char *s) {
    lkc->append( (char*) s);
    return this;
}

OVCandidate* OVImfCandidate::hide() {
    if (onscreen) {
	onscreen=0;
    }
    lkc->clear();
    return this;
}

OVCandidate* OVImfCandidate::show() {
    if (!onscreen) {
	onscreen=1;
    }
    return this;
}

OVCandidate* OVImfCandidate::update() {
    lkc->render();
    return this;
}

bool OVImfCandidate::onScreen() {
    return onscreen;
}


/*
 * implementation of OVImfService
 */

void OVImfService::beep() { }

void OVImfService::notify(const char *msg) {
	UrDEBUG( "%s\n", msg );
}

const char *OVImfService::locale(){
  setlocale( LC_CTYPE, "" );
  string lc_ctype = setlocale( LC_CTYPE, NULL );
  string current_locale;
  if( lc_ctype.find(".") == string::npos )
  {
    current_locale = lc_ctype;
  }
  else
  {
    current_locale = lc_ctype.substr( 0, lc_ctype.find(".") );
  }
  return current_locale.c_str(); 
}
const char *OVImfService::userSpacePath(const char *modid){
	const string path = string( getenv("HOME") ) + string( "/.openvanilla/" )+string( modid );
	return path.c_str();
}
const char *OVImfService::pathSeparator() { return "/"; }

const char *OVImfService::toUTF8(const char *encoding, const char *src) { 
  iconv_t conv_codec;
  size_t in_bytes, out_bytes, final_bytes;
  char* out_buf=NULL;
  in_bytes = strlen(src)+1;
  out_bytes = 1024;
  memset(internal, 0, 1024);
  out_buf = internal;
  conv_codec = iconv_open( "UTF-8", encoding );
  final_bytes = iconv( conv_codec, (char**)&src, &in_bytes, &out_buf, &out_bytes );
  iconv_close( conv_codec );
  return internal;
}

const char *OVImfService::fromUTF8(const char *encoding, const char *src) { 
  iconv_t conv_codec;
  size_t in_bytes, out_bytes, final_bytes;
  char* out_buf=NULL;
  in_bytes = strlen(src)+1;
  out_bytes= 1024;
  memset(internal, 0, 1024);
  out_buf = internal;
  conv_codec = iconv_open(encoding, "UTF-8");
  final_bytes = iconv( conv_codec, (char**)&src, &in_bytes, &out_buf, &out_bytes );
  iconv_close( conv_codec );
  return internal;
}

enum { bit7=0x80, bit6=0x40, bit5=0x20, bit4=0x10, bit3=8, bit2=4, bit1=2, bit0=1 };

const char *OVImfService::UTF16ToUTF8(unsigned short *s, int l) {
            char *b = internal;
            for (int i=0; i<l; i++)
            {
                    if (s[i] < 0x80)
                    {
                            *b++=s[i];
                    }
                    else if (s[i] < 0x800)
                    {
                            *b++=(0xc0 | s[i]>>6);
                            *b++=(0x80 | s[i] & 0x3f);
                    }
                    else if (s[i] < 0xd800 || s[i] > 0xdbff)
                    {
                            *b++ = (0xe0 | s[i]>>12);
                            *b++ = (0x80 | s[i]>>6 & 0x3f);
                            *b++ = (0x80 | s[i] & 0x3f);

                    }
                    else
                    {
                            unsigned int offset= 0x10000 - (0xd800 << 10) - 0xdc00;
                            unsigned int codepoint=(s[i] << 10) + s[i+1]+offset;
                            i++;
                            *b++=(0xf0 | codepoint>>18);
                            *b++=(0x80 | codepoint>>12 & 0x3f);
                            *b++=(0x80 | codepoint>>6 & 0x3f);
                            *b++=(0x80 | codepoint & 0x3F);
                    }
            }

            *b=0;
            return internal;
}

int OVImfService::UTF8ToUTF16(const char *src, unsigned short **rcvr) {
	char *s1=(char*)src;
	int len=0;
	char a, b, c;
	while (*s1)
	{
		a=*s1++;
		if ((a & (bit7|bit6|bit5))==(bit7|bit6)) { // 0x000080-0x0007ff
			b=*s1++;

			u_internal[len] = ((a & (bit4|bit3|bit2)) >> 2) * 0x100;
			u_internal[len] += (((a & (bit1|bit0)) << 6) | (b & (bit5|bit4|bit3|bit2|bit1|bit0)));
		}
		else if ((a & (bit7|bit6|bit5|bit4))==(bit7|bit6|bit5)) // 0x000800-0x00ffff
		{
			b=*s1++;
			c=*s1++;

			u_internal[len] = (((a & (bit3|bit2|bit1|bit0)) << 4) | ((b & (bit5|bit4|bit3|bit2)) >> 2)) * 0x100;
			u_internal[len] += (((b & (bit1|bit0)) << 6) | (c & (bit5|bit4|bit3|bit2|bit1|bit0)));
		}
		else 
		{
			u_internal[len] =(0);
			u_internal[len] +=(a);
		}
		len++;
	}
	*rcvr = u_internal;
	return len;
}


/*
 * Implementation of OVImfDictionary
 */

bool OVImfDictionary::keyExist(const char *key) {
   return _dict.find(key) != _dict.end();
}

int OVImfDictionary::getInteger(const char *key) {
    return atoi(_dict[key].c_str());
}

int OVImfDictionary::setInteger(const char *key, int value) {
   std::stringstream ss;
   ss << value;
   _dict.insert( std::make_pair(key, ss.str()) );
    return value;
}

const char* OVImfDictionary::getString(const char *key) {
    return _dict[key].c_str();
}

const char* OVImfDictionary::setString(const char *key, const char *value) {
   _dict.insert( std::make_pair(key, value) );
   return value;
}
