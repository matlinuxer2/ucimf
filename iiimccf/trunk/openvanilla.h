#include <OpenVanilla.h>
#include "imf.h"
#include <vector>
#include <map>
#include <string>
#include <ltdl.h>

typedef OVModule* (*TypeGetModule)(int);
typedef int (*TypeInitLibrary)(OVService*, const char*);
typedef unsigned int (*TypeGetLibVersion)();
struct OVLibrary {
     lt_dlhandle handle;
     TypeGetModule getModule;
     TypeInitLibrary initLibrary;
     TypeGetLibVersion getLibVersion;
};


class OVImfKeyCode : public OVKeyCode  {
public:
    OVImfKeyCode (int p);
    virtual int code();          
    virtual int isShift();       
    virtual int isCapslock();    
    virtual int isCtrl();        
    virtual int isAlt();         
    virtual int isOpt();         
    virtual int isNum();         
    
    virtual void setCode(int x);    
    virtual void setShift(int x);   
    virtual void setCapslock(int x);
    virtual void setCtrl(int x);    
    virtual void setAlt(int x);     
protected:
    int chr;
    int shift, capslock, ctrl, alt;
};

// Abstract interface for the pre-edit and composing buffer.
class OVImfBuffer : public OVBuffer {
public:
    OVImfBuffer();
    virtual OVBuffer* clear(); 
    virtual OVBuffer* append(const char *s);
    virtual OVBuffer* send();
    virtual OVBuffer* update();
    virtual OVBuffer* update(int cursorPos, int markFrom=-1, int markTo=-1);
    virtual int isEmpty();

protected:
    char buf[512];
};

class OVImfCandidate : public OVCandidate {
public:
    OVImfCandidate();
    virtual OVCandidate* clear();
    virtual OVCandidate* append(const char *s);
    virtual OVCandidate* hide();
    virtual OVCandidate* show();
    virtual OVCandidate* update();
    virtual int onScreen();

protected:
    char buf[512];
    int onscreen;
};

class OVImfService : public OVService {
public:
    virtual void beep();
    virtual void notify(const char *msg);
    virtual const char *locale(); 
    virtual const char *userSpacePath(const char *modid);
    virtual const char *pathSeparator();
    virtual const char *toUTF8(const char *encoding, const char *src);
    virtual const char *fromUTF8(const char *encoding, const char *src);
    virtual const char *UTF16ToUTF8(unsigned short *src, int len);
    virtual int UTF8ToUTF16(const char *src, unsigned short **rcvr);
private:
    char internal[1024];
    unsigned short u_internal[1024];
};


// Abstract interface for a simple dictionary. It is recommended that this
// dictionary be implemented as a type-free dictionary, i.e. you can replace
// the key with any value of any type, and type conversion between integer and
// string is done transparently, like what is done in e.g. sqlite3.
class OVImfDictionary : public OVDictionary {
  public:
    virtual int keyExist(const char *key);
    virtual int getInteger(const char *key);
    virtual int setInteger(const char *key, int value);
    virtual const char* getString(const char *key);
    virtual const char* setString(const char *key, const char *value);
  protected:
   std::map<std::string, std::string> _dict;

};


class OVImf : public Imf
{
  public:
    ~OVImf();
    static Imf* getInstance();
    static void commitBuffer( char* );

    char* process_input( char* buf );
    void switch_im();
    void switch_lang();
    void switch_im_per_lang();
    
  protected:
    OVImf();
    static Imf* _instance;
    static char* commit_buf;
    static int commit_buf_len;

    char* OV_MODULEDIR;
    std::vector<OVModule*> mod_vector; 
    int current_module;

    OVInputMethodContext *cxt;
    OVBuffer *preedit;
    OVCandidate *lookupchoice;
    OVService *srv;
    OVDictionary *dict;
};
