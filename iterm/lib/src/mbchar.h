/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

/**
 * @file mbchar.h
 */

#ifndef __CHAR__
#define __CHAR__

/**
 * @struct Char
 * Char class.
 * Char in the cell representing multibyte character
 * This struct is typedefed to `Char'.
 */
typedef struct _Char
{
  unsigned char *mb;
      /**< bytes representing multibyte character */

  unsigned int length; 
      /**< number of bytes of this character */
  unsigned char width;
      /**< column width of this character */
  unsigned char printable; 
      /**< 1 if this character is printable, 0 for not
         according to the locale  */
} Char;
/**< typedefed struct of struct _Char */

#define IS_COMBINED(mbchar) (mbchar->width == 0)
/**< macro to distinguish if character is combined character or not.
 * @param mbchar reference to Char object.\n
 * NOTE:  For now if width of mbchar is 0, then assume the character is
 * combined.  As you know this doesn't work sometime ;-), but we have no
 * APIs to determine combined character.
 */

#define Char_get_width(mbchar) (mbchar->width)
#define Char_get_length(mbchar) (mbchar->length)
#define Char_get_string(mbchar) (mbchar->mb)
#define Char_get_first_byte(mbchar) (mbchar->mb[0])
#define Char_is_printable(mbchar) (mbchar->printable)

#define Char_init(ch,mbs,len,wid,print) \
do {\
  ch.mb = mbs;\
  ch.length = len;\
  ch.width = wid;\
  ch.printable = print;\
} while(0)
/**< macro to initialize Char object.
 *
 */

#endif
