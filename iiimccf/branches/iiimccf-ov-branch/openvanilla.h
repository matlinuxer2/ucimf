#include <OpenVanilla/OpenVanilla.h>
#include "layer.h"
#include "preedit.h"
#include "lookupchoice.h"
#include "observer.h"


class DummyKeyCode : public OVKeyCode  {
public:
    DummyKeyCode (int p=0);
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
class DummyBuffer : public OVBuffer {
public:
    DummyBuffer(DIMEInstance *i);
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

class DummyCandidate : public OVCandidate ;
public:
    DummyCandidate(DIMEInstance *i);
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


// we borrow this function from OVPhoneticLib.cpp to help us do UTF-16->UTF-8
const char *VPUTF16ToUTF8(unsigned short *s, int l);

class DummyService : public OVService {
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

class OVIIIMCCF
{
public:
	//OVIIIMCCF(OVInputMethodContext *c, OVSCIMFactory *factory, const String& encoding, int id=-1);
	virtual ~OVIIIMCCF();

	virtual bool process( int keychar, int keycode, int modifier );
	//virtual void select_candidate( unsigned int index );
	//virtual void update_lookup_table_page_size( unsigned int page_size );
	//virtual void lookup_table_page_up();
	//virtual void lookup_table_page_down();
	virtual void reset();
	virtual void focus_on();
	virtual void focus_off();
	
	// important 
	OVInputMethodContext *cxt;
	
	DummyBuffer buf;
	DummyCandidate candi;
	DummyService srv;
	DummyDictionary dict;
};
