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


#ifndef __Imf
#define __Imf

#include <string>
using namespace std;

class Imf{
  public:
    virtual char* name()=0;
    virtual string process_input( const string& input )=0;
    virtual void  switch_lang()=0;
    virtual void  switch_im()=0;
    virtual void  switch_im_per_lang()=0;
};

typedef Imf* createImf_t();
typedef void destroyImf_t( Imf* );

#endif
