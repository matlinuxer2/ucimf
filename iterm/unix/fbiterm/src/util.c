/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained 
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#include <string.h>
#include <wchar.h>
#include <wctype.h>

#define mbrtowc(pwc, s, n, ps) (mbrtowc) (pwc, s, n, 0)

/* return the number of bytes parsed from the multibyte sequence starting
   at src, if a non-L'\0' wide character was recognized. It returns 0, 
   if a L'\0' wide character was recognized. It  returns (size_t)(-1), 
   if an invalid multibyte sequence was encountered. It returns (size_t)(-2) 
   if it couldn't parse a complete  multibyte character.  */
int
get_char_len (char *src, mbstate_t * ps)
{
  size_t tmp;

  tmp = mbrtowc (NULL, (const char *) src, (size_t) strlen (src), ps);
  if (tmp == (size_t) (-2))
    {
      /* shorted to compose multibyte char */
      memset (ps, 0, sizeof (mbstate_t));
      return -2;
    }
  else if (tmp == (size_t) (-1))
    {
      /* invalid to compose multibyte char */
      /* initialize the conversion state */
      memset (ps, 0, sizeof (mbstate_t));
      return -1;
    }
  else if (tmp == (size_t) (0))
    return 0;
  else
    return (int) tmp;
}
