#include "openvanilla.h"


/*
 * main procedure for loading modules
 */

// OV_MODULEDIR is defined in Makefile.am !!

scim_module_init() {
   lt_dlinit();
   lt_dlsetsearchpath(OV_MODULEDIR);
}

scim_module_exit() {
    lt_dlexit();
}

scim_imengine_module_init() {
    return scan_ov_modules();
}


static OVLibrary* open_module(const char* modname){
   OVLibrary* mod = new OVLibrary();
   mod->handle = lt_dlopen(modname);
   if(mod->handle == NULL){
      fprintf(stderr, "dlopen %s failed\n", modname);
      goto OPEN_FAILED;
   }
   mod->getModule = (TypeGetModule)lt_dlsym( mod->handle, 
                                             "OVGetModuleFromLibrary" );
   mod->getLibVersion = (TypeGetLibVersion)lt_dlsym( mod->handle, 
                                             "OVGetLibraryVersion" );
   mod->initLibrary = (TypeInitLibrary)lt_dlsym( mod->handle,
                                             "OVInitializeLibrary" );
   if( !mod->getModule || !mod->getLibVersion || !mod->initLibrary ){
      fprintf(stderr, "dlsym %s failed\n", modname);
      goto OPEN_FAILED;
   }
   if( mod->getLibVersion() < OV_VERSION ){
      fprintf(stderr, "%s %d is too old\n", modname, mod->getLibVersion());
      goto OPEN_FAILED;
   }
   return mod;

OPEN_FAILED:
   delete mod;
   return NULL;
}

static int scan_ov_modules(){
   DIR* dir = opendir(OV_MODULEDIR);
   DummyService srv;
   if(dir){
      struct dirent *d_ent;
      while( (d_ent = readdir(dir)) != NULL ){
         if( strstr( d_ent->d_name, ".so" ) ){
            fprintf(stderr,  "Load OV module: %s\n", d_ent->d_name);
            OVLibrary* mod = open_module(d_ent->d_name);
            if(mod){
               OVModule* m;
               mod->initLibrary(&srv, OV_MODULEDIR);
               for(int i=0; m = mod->getModule(i); i++)
                  mod_vector.push_back(m);
               delete mod;
            }
         }
      }
      closedir(dir);
   }
   return mod_vector.size();
}


/*
 * implementation of DummyKeyCode
 */

DummyKeyCode::DummyKeyCode (int p=0)  { chr=p; shift=capslock=ctrl=alt=0; }
int DummyKeyCode::code()              { return chr; }
int DummyKeyCode::isShift()           { return shift; }
int DummyKeyCode::isCapslock()        { return capslock; }
int DummyKeyCode::isCtrl()            { return ctrl; }
int DummyKeyCode::isAlt()             { return alt; }
int DummyKeyCode::isOpt()             { return alt; }
int DummyKeyCode::isNum()             { return 0; }

void DummyKeyCode::setCode(int x)     { chr=x; }
void DummyKeyCode::setShift(int x)    { shift=x; }
void DummyKeyCode::setCapslock(int x) { capslock=x; }
void DummyKeyCode::setCtrl(int x)     { ctrl=x; }
void DummyKeyCode::setAlt(int x)      { alt=x; }

/*
 * implementation of DummyBuffer
 */

DummyBuffer::DummyBuffer(DIMEInstance *i) {
    //im=i;
    prdt = new Prdt;
    strcpy(buf, "");
}

OVBuffer* DummyBuffer::clear() {
    strcpy(buf, "");
    //prdt->clear();
    //im->update_preedit_string(WideString());
    //im->hide_preedit_string();
    return this;
}

OVBuffer* DummyBuffer::append(const char *s) {
    strcat(buf, s);
    return this;
}

OVBuffer* DummyBuffer::send() {
    WideString bs=utf8_mbstowcs(buf);
    clear();
    //im->commit_string(bs);
    return this;
}

OVBuffer* DummyBuffer::update() {
    //im->update_preedit_string(utf8_mbstowcs(buf));
    if (strlen(buf)) //im->show_preedit_string();
    else //im->hide_preedit_string();     
    return this;
}

OVBuffer* DummyBuffer::update(int cursorPos, int markFrom=-1, int markTo=-1) {
    return update();
}

int DummyBuffer::isEmpty() {
    if (!strlen(buf)) return 1;
    return 0;
}


/*
 * implementation of DummyCandidate
 */

DummyCandidate::DummyCandidate(DIMEInstance *i) {
    //im=i;
    onscreen=0;
    strcpy(buf, "");
}

OVCandidate* DummyCandidate::clear() {
    strcpy(buf, "");
    return this;
}
OVCandidate* DummyCandidate::append(const char *s) {
    strcat(buf, s);
    return this;
}
OVCandidate* DummyCandidate::hide() {
    if (onscreen) {
	//im->hide_aux_string();
	onscreen=0;
    }
    return this;
}
OVCandidate* DummyCandidate::show() {
    if (!onscreen) {
	//im->show_aux_string();
	onscreen=1;
    }
    return this;
}
OVCandidate* DummyCandidate::update() {
    //im->update_aux_string(utf8_mbstowcs(buf));
    return this;
} 
int DummyCandidate::onScreen() {
    return onscreen;
}


/*
 * implementation of DummyService
 */

void DummyService::beep() { }
void DummyService::notify(const char *msg) { fprintf(stderr, "%s\n", msg); }
const char *DummyService::locale(){ return "zh_TW"; }
const char *DummyService::userSpacePath(const char *modid) { return "/tmp"; }
const char *DummyService::pathSeparator() { return "/"; }
const char *DummyService::toUTF8(const char *encoding, const char *src) { 
  return src; 
}

const char *DummyService::fromUTF8(const char *encoding, const char *src) { 
  return src; 
}

const char *DummyService::UTF16ToUTF8(unsigned short *src, int len) {
    return VPUTF16ToUTF8(src, len);
}

int DummyService::UTF8ToUTF16(const char *src, unsigned short **rcvr) {
    return 0;
}


/*
 * implementation of OVIIIMCCF
 */

OVIIIMCCF::OVIIIMCCF()
{

}

OVIIIMCCF::~OVIIIMCCF()
{

}

bool OVIIIMCCF::process( int keychar, int keycode, int modifier )
{


}


