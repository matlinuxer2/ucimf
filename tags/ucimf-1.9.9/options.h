#ifndef __OPTIONS_H_
#define __OPTIONS_H_

#include <string>
#include <iostream>
#include <map>
using namespace std;

class Options
{
  public:
    static Options* getInstance();
    ~Options();

    char* getOption( const char * );

  protected:
    Options();
    static Options* _instance;
    
  private:
    bool parse_file(ifstream &input);
    map<string, string> _opts;
};

#endif
