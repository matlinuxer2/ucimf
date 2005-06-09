/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained 
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#ifndef __RGB_H__
#define __RGB_H__

static unsigned short red16[] = {
  0x0000, 0x0000, 0x0000, 0x0000, 0xaaaa, 0xaaaa, 0xaaaa, 0xaaaa,
  0x5555, 0x5555, 0x5555, 0x5555, 0xffff, 0xffff, 0xffff, 0xffff
};

static unsigned short green16[] = {
  0x0000, 0x0000, 0xaaaa, 0xaaaa, 0x0000, 0x0000, 0xaaaa, 0xaaaa,
  0x5555, 0x5555, 0xffff, 0xffff, 0x5555, 0x5555, 0xffff, 0xffff
};

static unsigned short blue16[] = {
  0x0000, 0xaaaa, 0x0000, 0xaaaa, 0x0000, 0xaaaa, 0x0000, 0xaaaa,
  0x5555, 0xffff, 0x5555, 0xffff, 0x5555, 0xffff, 0x5555, 0xffff
};

#endif /* __RGB_H__ */
