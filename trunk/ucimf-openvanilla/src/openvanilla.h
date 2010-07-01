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

#include <vector>
#include <map>
#include <string>
#include <ltdl.h>
#include <OpenVanilla/OpenVanilla.h>
#include <imf/imf.h>
#include <imf/widget.h>

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
    virtual bool isShift();       
    virtual bool isCapslock();    
    virtual bool isCtrl();        
    virtual bool isAlt();         
    virtual bool isOpt();         
    virtual bool isNum();         
    
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
    virtual bool isEmpty();

protected:
    Preedit *prdt;
    string buf;
};

class OVImfCandidate : public OVCandidate {
public:
    OVImfCandidate();
    virtual OVCandidate* clear();
    virtual OVCandidate* append(const char *s);
    virtual OVCandidate* hide();
    virtual OVCandidate* show();
    virtual OVCandidate* update();
    virtual bool onScreen();

protected:
    char buf[512];
    int onscreen;
    LookupChoice *lkc;
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
    virtual bool keyExist(const char *key);
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
    static void commitBuffer( string );
    static OVInputMethod* im;

    string process_input( const string& input );
    void refresh();
    void switch_im();
    void switch_im_reverse();
    void switch_lang();
    void switch_im_per_lang();
    
    int siz;
  protected:
    OVImf();
    static Imf* _instance;
    static string commit_buf;

    // char* OV_MODULEDIR;
    std::vector<OVModule*> mod_vector; 
    int current_module;
    char* current_im_name;
    Status* stts;
    
    OVInputMethodContext *cxt;
    OVBuffer *preedit;
    OVCandidate *lookupchoice;
    OVService *srv;
    OVDictionary *dict;
};

extern "C" Imf* createImf();
extern "C" void destroyImf( Imf* imf );
