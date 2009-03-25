/*
 * UCIMF - Unicode Console InputMethod Framework                                                    
 *
 * Copyright (c) 2006-2007 Chun-Yu Lee (Mat) and Open RazzmatazZ Laboratory (OrzLab)
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

/* 
 *
 * ustring implementation 
 *
 * */

#include "type.h"
#include <iconv.h>
#include "font.h"
#include <stdint.h>

ustring::ustring(const char* encoding, const char* data)
{

  uint32_t tmp[64];
  char* outbuf = (char*)tmp;
  char* inbuf = (char*)data; 
  size_t inbytesleft = sizeof(char) * strlen(data);
  size_t outbytesleft=sizeof(tmp);

  iconv_t conv_codec = iconv_open( "UTF-32", encoding );
  iconv( conv_codec, &inbuf, &inbytesleft, &outbuf, &outbytesleft );
  iconv_close( conv_codec );
  
  udata.clear();
  int count = ( sizeof(tmp) - outbytesleft )/sizeof(uint32_t);
  
  // The first byte of UTF-32 word( == tmp[0]) is byte-order header, so to ignore it!
  for( int i=1; i< count; i++)
  {
    udata.push_back( tmp[i] );
  }
  
}

ustring::ustring(const string& encoding, const string& data) 
{
  ustring( encoding.c_str(), data.c_str() );
}


string ustring::out(const char* encoding) const
{
  uint32_t tmp[64];
  for( int i=0; i< 64; i++ )
  {
    tmp[i]=udata[i];
  }

  char* inbuf = (char*)tmp;
  size_t inbytesleft = udata.size();

  char tmp2[256];
  char* outbuf = (char*)tmp2;
  size_t outbytesleft = 256;
  
  iconv_t conv_codec = iconv_open( encoding, "UTF-32" );
  iconv( conv_codec, &inbuf, &inbytesleft, &outbuf, &outbytesleft );
  iconv_close( conv_codec );

  string result( outbuf );
  return result;
  
}


const ustring& ustring::operator=(const ustring& ustr)
{
  if( &ustr == this )
  {
    return ustr;
  }

  udata = ustr.udata;

  return *this;

}


ustring ustring::operator+(const ustring& ustr) const
{
  ustring result;
  result.udata = udata;
  for( size_t i=0; i< ustr.udata.size(); i++ )
  {
    result.udata.push_back( ustr.udata[i] );
  }

  return result;
}

bool ustring::operator==(const ustring& ustr) const
{
  if( udata == ustr.udata )
    return true;
  
  return false;
}

int ustring::length() const
{
  if( udata.empty() )
  {
    return 0;
  }

  Font* font= Font::getInstance();
  int result = 0;

  for( size_t i=0; i< udata.size(); i++ )
  {
    result += font->length( udata[i] );
  }

  return result;

}
