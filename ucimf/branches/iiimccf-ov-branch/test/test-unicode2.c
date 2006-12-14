#include <iconv.h>
#include <string.h>

int main()
{
  char *inbuf, *outbuf;
  size_t in_len, out_len=4;
  inbuf = strcpy( inbuf,"°É" );
  
  iconv_t icd= iconv_open( "UCS-4", "Big5");
  iconv( icd, &inbuf, &in_len, &outbuf, &out_len );
  iconv_close( icd );
  
  
  long int charcode = 0x52d8;
  return 0;
}
