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

#include <stdexcept>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include "options.h"
#include "debug.h" 

using namespace std;

#define UCIMFRC_SYS SYSCONFDIR"/ucimf.conf"

Options* Options::_instance = 0;

Options* Options::getInstance()
{
  if ( _instance == 0 )
  {
    _instance = new Options;
  }
  
  return _instance;
}

Options::Options()
{
  string conf = getenv("HOME");
  conf += "/.ucimf.conf";

  UrDEBUG("Checking config file: %s \n", conf.c_str() );
  if ( access(conf.c_str(), R_OK ) != 0 )
  {
    UrDEBUG("Could not open config file: %s \n",conf.c_str() );
    conf = UCIMFRC_SYS;
  }

  UrDEBUG("Checking config file: %s \n", conf.c_str() );
  if ( access(conf.c_str(), R_OK ) != 0 )
  {
    UrDEBUG("Could not open config file: %s \n", conf.c_str() );
    throw runtime_error("Could not open config file!");
  }

  UrDEBUG("Reading config file: %s \n", conf.c_str() );
  
  ifstream input_file( conf.c_str() );
  
  if (!input_file)
  {
      UrDEBUG("Could not reading config file: %s \n", conf.c_str() );
      throw runtime_error("Could not reading config file!");
  }

  parse_file( input_file );

}


bool Options::parse_file( ifstream &input)
{
    string s, o, v;
    const char *p;
    while (getline(input, s)) {
        if (!s.empty() && s[0] == '#')
            continue;
        o = "";
        v = "";
        p = s.c_str();
        while(*p && *p == ' ')
            p++;
        while (*p && *p != ' ' && *p != '\t' && *p != '=')
            o += *p++;
        while (*p && (*p == ' ' || *p == '\t' || *p == '='))
            p++;
        while (*p && (*p == ' ' || *p == '\t'))
            p++;
        while (*p && *p != ' ' && *p != '\t')
            v += *p++;
        if (o == "")
            continue;
        _opts.insert( pair<string, string>(o,v) );
	UrDEBUG( "Option: %s = %s \n ", o.c_str() ,v.c_str() );
    }
    // FIXME: error handling
    return true;
}

char* Options::getOption( const char* opt_str )
{
    map<string,string>::iterator iter;
    iter = _opts.find( string(opt_str) );
    char* result;
    if ( iter == _opts.end())
    {   
	result= (char*) "nothing";
    }
    else
    {
	result=(char*) (*iter).second.c_str();
    }

    // to avoid to return NULL;
    if( !result ){
	result=const_cast<char*>("");
    }

    return result;
}

