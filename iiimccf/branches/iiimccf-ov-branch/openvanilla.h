#include <OpenVanilla/OpenVanilla.h>
#include "imf.h"
#include <vector>

#include "layer.h"
#include "preedit.h"
#include "lookupchoice.h"
#include "observer.h"

typedef OVModule* (*TypeGetModule)(int);
typedef int (*TypeInitLibrary)(OVService*, const char*);
typedef unsigned int (*TypeGetLibVersion)();
struct OVLibrary {
     dlhandle handle;
     TypeGetModule getModule;
     TypeInitLibrary initLibrary;
     TypeGetLibVersion getLibVersion;
};

class OVImf::Imf
{
  public:
    OVImf();
    ~OVImf(); 
    //OVIIIMCCF(OVInputMethodContext *c, OVSCIMFactory *factory, const String& encoding, int id=-1);

    char* process_keyevent( int keychar, int keycode, int modifier );
    switch_im();
    switch_lang();
    switch_im_per_lang();
    switch_im_custom();
    
    // important 
    std::vector<OVModule*> mod_vector; 

    OVInputMethodContext *cxt;
    OVBuffer preedit;
    OVCandidate lookupchoice;
    OVService srv;
    OVDictionary dict;
};

class OVImfKeyCode : public OVKeyCode  {
public:
    OVImfKeyCode (int p=0);
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
    OVImfBuffer(DIMEInstance *i);
    virtual OVBuffer* clear(); 
    virtual OVBuffer* append(const char *s);
    virtual OVBuffer* send();
    virtual OVBuffer* update();
    virtual OVBuffer* update(int cursorPos, int markFrom=-1, int markTo=-1);
    virtual int isEmpty();

protected:
    char buf[512];
    Prdt *prdt;
    //DIMEInstance *im;
};

class OVImfCandidate : public OVCandidate ;
public:
    OVImfCandidate(DIMEInstance *i);
    virtual OVCandidate* clear();
    virtual OVCandidate* append(const char *s);
    virtual OVCandidate* hide();
    virtual OVCandidate* show();
    virtual OVCandidate* update();
    virtual int onScreen();

protected:
    //DIMEInstance *im;
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
};

