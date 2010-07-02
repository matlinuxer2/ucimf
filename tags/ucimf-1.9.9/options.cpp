#include <stdexcept>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include "options.h"
using namespace std;

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

  if ( access(conf.c_str(), R_OK ) != 0 )
  {
    conf = "/etc/ucimf.conf";
  }
  
  ifstream input_file( conf.c_str() );
  
  if (!input_file)
  {
      throw runtime_error("Could not open config file!");
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
    }
}

char* Options::getOption( const char* opt_str )
{
    map<string,string>::iterator iter;
    iter = _opts.find( string(opt_str) );
    char* result;
    if ( iter == _opts.end())
    {   
	result="nothing";
    }
    else
    {
	result=(char*) (*iter).second.c_str();
    }
    return result;
}

