#include <iconv.h>
#include <string.h>

int main()
{
  size_t in_len=4, out_len=4;
  char *inbuf = (char* ) malloc(in_len) ;
  char *outbuf = (char*) malloc(out_len) ;
  
  strcpy( inbuf,"勘" );
  
  iconv_t icd= iconv_open( "UTF8", "UTF8");
  iconv( icd, &inbuf, &in_len, &outbuf, &out_len );
  iconv_close( icd );
  
  
  long int charcode = 0x52d8;
  return 0;
}
