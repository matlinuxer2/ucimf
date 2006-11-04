/* 
 *
 * ustring implementation 
 *
 * */

#include "type.h"
#include <iconv.h>

ustring::ustring(const char* encoding, const char* data)
{

  unsigned long tmp[64];
  char* outbuf = (char*)tmp;
  char* inbuf = (char*)data; 
  size_t inbytesleft = sizeof(char) * strlen(data);
  size_t outbytesleft=sizeof(tmp);

  iconv_t conv_codec = iconv_open( "UTF-32", encoding );
  size_t nconv = iconv( conv_codec, &inbuf, &inbytesleft, &outbuf, &outbytesleft );
  iconv_close( conv_codec );
  
  udata.clear();
  int count = ( sizeof(tmp) - outbytesleft )/sizeof(unsigned long);
  //for( int i=0; i< count; i++)
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
  unsigned long tmp[64];
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
  size_t nconv = iconv( conv_codec, &inbuf, &inbytesleft, &outbuf, &outbytesleft );
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
  for( int i=0; i< ustr.udata.size(); i++ )
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


