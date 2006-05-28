#include "openvanilla.h"


/*
 * main procedure for loading modules
 */


/* 
 * implementation of OVImf
 */
OVImf::OVImf()
{
  //OVModule m;
  OVInputMethod* im = dynamic_cast<OVInputMethod*>(m);
  //OVInputMethod *im = new OVIMArray;
  OVDictionary dict
  im->initialize(dict, srv, OV_MODULEDIR);
  cxt = im->newContext();
  preedit = new OVImfBuffer;
  lookupchoice = new OVImfCandidate;
  srv = new OVImfService;
  dict = new OVImfDictionary;
  cxt->start( preedit, lookupchoice, srv );
}

int OVImf::load_modules()
{
  // OV_MODULEDIR is defined in Makefile.am !!
  char* OV_MODULEDIR;
  char* module_dir;
  char* module_filename;
  
  OVLibrary* mod = new OVLibrary();

  mod->handle = dlopen(module_filename);
  if(mod->handle == NULL){
    fprintf(stderr, "dlopen %s failed\n", module_filename);
    delete mod;
  }
  else{ 
    mod->getModule = (TypeGetModule)dlsym( mod->handle, "OVGetModuleFromLibrary" );
    mod->getLibVersion = (TypeGetLibVersion)dlsym( mod->handle, "OVGetLibraryVersion" );
    mod->initLibrary = (TypeInitLibrary)dlsym( mod->handle, "OVInitializeLibrary" );
  }

  if( !mod->getModule || !mod->getLibVersion || !mod->initLibrary ){
     fprintf(stderr, "dlsym %s failed\n", module_filename);
     delete mod;
  }
  if( mod->getLibVersion() < OV_VERSION ){
     fprintf(stderr, "%s %d is too old\n", module_filename, mod->getLibVersion());
     delete mod;
  }
  
  if(mod){
     OVModule* m;
     mod->initLibrary(&srv, OV_MODULEDIR);
     for(int i=0; m = mod->getModule(i); i++)
	mod_vector.push_back(m);
     delete mod;
  }

  return mod_vector.size();
}

OVImf::process_keyevent( int keychar, int keycode, int modifier )
{
  OVKeyCode keyevent;
  // setup keyevent's values responding keychar, keycode, modifier
  cxt->keyEvent( keyevent, preedit, lookupchoice, service );
}

/*
 * implementation of OVImfKeyCode
 */

OVImfKeyCode::OVImfKeyCode (int p=0)  { chr=p; shift=capslock=ctrl=alt=0; }
int OVImfKeyCode::code()              { return chr; }
int OVImfKeyCode::isShift()           { return shift; }
int OVImfKeyCode::isCapslock()        { return capslock; }
int OVImfKeyCode::isCtrl()            { return ctrl; }
int OVImfKeyCode::isAlt()             { return alt; }
int OVImfKeyCode::isOpt()             { return alt; }
int OVImfKeyCode::isNum()             { return 0; }

void OVImfKeyCode::setCode(int x)     { chr=x; }
void OVImfKeyCode::setShift(int x)    { shift=x; }
void OVImfKeyCode::setCapslock(int x) { capslock=x; }
void OVImfKeyCode::setCtrl(int x)     { ctrl=x; }
void OVImfKeyCode::setAlt(int x)      { alt=x; }

/*
 * implementation of OVImfBuffer
 */

OVImfBuffer::OVImfBuffer(DIMEInstance *i) {
    //im=i;
    strcpy(buf, "");
}

OVBuffer* OVImfBuffer::clear() {
    strcpy(buf, "");
    //im->update_preedit_string(WideString());
    //im->hide_preedit_string();
    return this;
}

OVBuffer* OVImfBuffer::append(const char *s) {
    strcat(buf, s);
    return this;
}

OVBuffer* OVImfBuffer::send() {
    WideString bs=utf8_mbstowcs(buf);
    clear();
    //im->commit_string(bs);
    return this;
}

OVBuffer* OVImfBuffer::update() {
    //im->update_preedit_string(utf8_mbstowcs(buf));
    if (strlen(buf)) 
      //im->show_preedit_string();
    else 
      //im->hide_preedit_string();     
    
    return this;
}

OVBuffer* OVImfBuffer::update(int cursorPos, int markFrom=-1, int markTo=-1) {
    return update();
}

int OVImfBuffer::isEmpty() {
    if (!strlen(buf)) return 1;
    return 0;
}


/*
 * implementation of OVImfCandidate
 */

OVImfCandidate::OVImfCandidate(DIMEInstance *i) {
    //im=i;
    onscreen=0;
    strcpy(buf, "");
}

OVCandidate* OVImfCandidate::clear() {
    strcpy(buf, "");
    return this;
}

OVCandidate* OVImfCandidate::append(const char *s) {
    strcat(buf, s);
    return this;
}

OVCandidate* OVImfCandidate::hide() {
    if (onscreen) {
	//im->hide_aux_string();
	onscreen=0;
    }
    return this;
}

OVCandidate* OVImfCandidate::show() {
    if (!onscreen) {
	//im->show_aux_string();
	onscreen=1;
    }
    return this;
}

OVCandidate* OVImfCandidate::update() {
    //im->update_aux_string(utf8_mbstowcs(buf));
    return this;
}

int OVImfCandidate::onScreen() {
    return onscreen;
}


/*
 * implementation of OVImfService
 */
#define UserSpacePath "/home/mat"

void OVImfService::beep() { }
void OVImfService::notify(const char *msg) { fprintf(stderr, "%s\n", msg); }
const char *OVImfService::locale(){ return "zh_TW"; }
const char *OVImfService::userSpacePath(const char *modid) { return UserSpacePath; }
const char *OVImfService::pathSeparator() { return "/"; }
const char *OVImfService::toUTF8(const char *encoding, const char *src) { 
  return src; 
}

const char *OVImfService::fromUTF8(const char *encoding, const char *src) { 
  return src; 
}

const char *OVImfService::UTF16ToUTF8(unsigned short *src, int len) {
    return ;//VPUTF16ToUTF8(src, len);
}

int OVImfService::UTF8ToUTF16(const char *src, unsigned short **rcvr) {
    return 0;
}


