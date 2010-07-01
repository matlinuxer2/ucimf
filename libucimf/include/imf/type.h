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

#ifndef __Type__
#define __Type__

#include <string>
#include <vector>
using namespace std;


class ustring 
{
  public:
    ustring(){};
    ustring(const ustring& ustr){ udata = ustr.udata; };
    ustring(const char* encoding, const char* data);
    ustring(const string& encoding, const string& data); 
    ~ustring(){};

    string out(const char* encoding) const;
    unsigned long operator[](int i) const{ return udata[i]; };
    const ustring& operator=(const ustring& ustr);
    ustring operator+(const ustring& str) const;
    bool operator==(const ustring& ustr) const;

    int size() const{ return udata.size(); };
    void clear(){ udata.clear(); };
    
  private:
    vector<unsigned long> udata;
  
};

#endif
