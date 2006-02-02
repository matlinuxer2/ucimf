/* $TOG: fontxlfd.h /main/7 1998/02/09 10:49:25 kaleb $ */

/*

Copyright 1990, 1994, 1998  The Open Group

All Rights Reserved.

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from The Open Group.

*/
/* $XFree86: xc/lib/font/include/fontxlfd.h,v 1.4 1999/07/17 05:30:48 dawes Exp $ */

/*
 * Author:  Keith Packard, MIT X Consortium
 */

#ifndef _FONTXLFD_H_
#define _FONTXLFD_H_


/* import macro from "FS.h" */
#define MSBFirst                1

//#include "FSproto.h"
#include "Xmd.h"
/* import declaration from FSproto.h to reduce dependency  */
#define sz_fsRange		4


#ifndef Bool
#  ifndef _XTYPEDEF_BOOL
#   define _XTYPEDEF_BOOL
typedef int Bool;
#  endif
#endif
/*
typedef unsigned short CARD16;
typedef unsigned char  CARD8;
#ifdef LONG64
typedef unsigned int CARD32;
#else
typedef unsigned long CARD32;
#endif

#ifndef I_NEED_OS2_H
typedef CARD8		BYTE;
typedef CARD8		BOOL;
#else
#define BYTE	CARD8
#define BOOL	CARD8
#endif

#define B32 :32
#define B16 :16
*/

typedef struct {
    CARD8	min_char_high;
    CARD8	min_char_low;
    CARD8	max_char_high;
    CARD8	max_char_low;
}           fsRange;

typedef CARD32	fsTimestamp;

typedef struct {
    BYTE        type;
    BYTE        request;
    CARD16 	sequenceNumber B16;
    CARD32 	length B32;
    fsTimestamp	timestamp;
    CARD8	major_opcode;
    CARD8	minor_opcode;
    CARD16	pad B16;
    fsRange	range;
}	    fsRangeError;

/* Constants for values_supplied bitmap */

#define SIZE_SPECIFY_MASK		0xf

#define PIXELSIZE_MASK			0x3
#define PIXELSIZE_UNDEFINED		0
#define PIXELSIZE_SCALAR		0x1
#define PIXELSIZE_ARRAY			0x2
#define PIXELSIZE_SCALAR_NORMALIZED	0x3	/* Adjusted for resolution */

#define POINTSIZE_MASK			0xc
#define POINTSIZE_UNDEFINED		0
#define POINTSIZE_SCALAR		0x4
#define POINTSIZE_ARRAY			0x8

#define PIXELSIZE_WILDCARD		0x10
#define POINTSIZE_WILDCARD		0x20

#define ENHANCEMENT_SPECIFY_MASK	0x40

#define CHARSUBSET_SPECIFIED		0x40

#define EPS		1.0e-20
#define XLFD_NDIGITS	3		/* Round numbers in pixel and
					   point arrays to this many
					   digits for repeatability */

typedef struct _FontScalable {
    int		values_supplied;	/* Bitmap identifying what advanced
					   capabilities or enhancements
					   were specified in the font name */
    double	pixel_matrix[4];
    double	point_matrix[4];

    /* Pixel and point fields are deprecated in favor of the
       transformation matrices.  They are provided and filled in for the
       benefit of rasterizers that do not handle the matrices.  */

    int		pixel,
		point;

    int         x,
                y,
                width;
    char	*xlfdName;
    int		nranges;
    fsRange	*ranges;
}           FontScalableRec, *FontScalablePtr;


extern double xlfd_round_double ( double x );
extern Bool FontParseXLFDName ( char *fname, FontScalablePtr vals, int subst );
extern fsRange *FontParseRanges ( char *name, int *nranges );

#define FONT_XLFD_REPLACE_NONE	0
#define FONT_XLFD_REPLACE_STAR	1
#define FONT_XLFD_REPLACE_ZERO	2
#define FONT_XLFD_REPLACE_VALUE	3

#endif				/* _FONTXLFD_H_ */
