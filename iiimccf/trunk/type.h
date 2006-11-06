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
    
    // !! temporary for hacking. return the string length in PIXEl!!
    int length() const; 
    
  private:
    vector<unsigned long> udata;
  
};

#endif
