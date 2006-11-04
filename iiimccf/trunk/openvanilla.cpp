#include "openvanilla.h"
#include <iconv.h>
#include <iostream>
#include <sstream>
#include <dirent.h>



/* 
 * implementation of OVImf
 */

Imf* OVImf::_instance = 0;

Imf* OVImf::getInstance()
{
  if( _instance == 0 )
    _instance = new OVImf;

  return _instance;
}

char* OVImf::commit_buf="";
int OVImf::commit_buf_len=0;

void OVImf::commitBuffer( char* input )
{
  commit_buf = input;
  commit_buf_len = strlen( commit_buf );
}

OVImf::OVImf()
{
  current_module = 0;
  
  preedit = new OVImfBuffer;
  lookupchoice = new OVImfCandidate;
  srv = new OVImfService;
  dict = new OVImfDictionary;

  // OV_MODULEDIR is defined in Makefile.am !!
  OV_MODULEDIR=getenv("OVMODULE_DIR");
  lt_dlinit();
  lt_dlsetsearchpath( OV_MODULEDIR );
  //char* module_filename=getenv("OVMODULE_FILEPATH");

  DIR *dir = opendir( OV_MODULEDIR );
  if( dir )
  {
    struct dirent *d_ent;
    while( ( d_ent = readdir(dir) ) != NULL )
    {
      if( strstr( d_ent->d_name, ".so") )
      {
	  OVLibrary* mod = new OVLibrary();
	  
	  mod->handle = lt_dlopen( d_ent->d_name );
	  if(mod->handle == NULL){
	    fprintf(stderr, "lt_dlopen %s failed\n", d_ent->d_name );
	    delete mod;
	  }
	  else{ 
	    mod->getModule = (TypeGetModule)lt_dlsym( mod->handle, "OVGetModuleFromLibrary" );
	    mod->getLibVersion = (TypeGetLibVersion)lt_dlsym( mod->handle, "OVGetLibraryVersion" );
	    mod->initLibrary = (TypeInitLibrary)lt_dlsym( mod->handle, "OVInitializeLibrary" );
	  }

	  if( !mod->getModule || !mod->getLibVersion || !mod->initLibrary ){
	     fprintf(stderr, "lt_dlsym %s failed\n", d_ent->d_name );
	     delete mod;
	  }
	  
	  if( mod->getLibVersion() < OV_VERSION ){
	     fprintf(stderr, "%s %d is too old\n", d_ent->d_name, mod->getLibVersion());
	     delete mod;
	  }

	  if(mod){
	     OVModule* m;
	     mod->initLibrary(srv, OV_MODULEDIR);
	     for(int i=0; m = mod->getModule(i); i++)
	     {
	       std::cout << m->identifier() << std::endl; 
		 mod_vector.push_back(m);
	     }
	     delete mod;
	  }

      }
    }
    closedir(dir);
  }
    

  //OVInputMethod* im = dynamic_cast<OVInputMethod*>(mod_vector[0]);
  OVInputMethod* im = static_cast<OVInputMethod*>(mod_vector[ current_module ]);
  im->initialize(dict, srv, OV_MODULEDIR);
  cxt = im->newContext();
  cxt->start( preedit, lookupchoice, srv );
}

OVImf::~OVImf()
{
  lt_dlexit();

}

int stdin_to_openvanila_keycode( int keychar ) 
{
  int keycode;
  switch( keychar )
  {

    case 9: keycode=ovkTab; break;
    case 13: keycode=ovkReturn; break;
    case 27: keycode=ovkEsc; break;
    case 32: keycode=ovkSpace; break;
    case 126: keycode=ovkDelete; break;
    case 127: keycode=ovkBackspace; break;
    default: 
	      keycode = 0; break;
  };
  return keycode;
}


char* OVImf::process_input( char* buf )
{
  int keychar,keycode,modifier;
  int buf_len = strlen( buf );

  if( buf_len == 1 )
  {
    keychar = (int)buf[0];
    keycode = stdin_to_openvanila_keycode(keychar);
    modifier = 0;
  }

  else if( buf_len == 3 && buf[0]==27 && buf[1]==91 )
  {
    switch( buf[2] )
    {
      case 65:
	keychar = 0;
	keycode = ovkUp;
	modifier = 0; // default as zero.
	break;

      case 66:
	keychar = 0;
	keycode = ovkDown;
	modifier = 0; // default as zero.
	break;
	
      case 67:
	keychar = 0;
	keycode = ovkRight;
	modifier = 0; // default as zero.
	break;
	
      case 68:
	keychar = 0;
	keycode = ovkLeft;
	modifier = 0; // default as zero.
	break;

      default:
	break;
    }
  }
  else if( buf_len == 4 && buf[0]==27 && buf[1]==91 && buf[4]==126 )
  {
    switch( buf[3] )
    {
      case 53:
	keychar = 0;
	keycode = ovkPageUp;
	modifier = 0; // default as zero.
	break;
	
      case 54:
	keychar = 0;
	keycode = ovkPageDown;
	modifier = 0; // default as zero.
	break;
      default:
	break;
    }
  }
  else
  {
    keychar = 0;
    keycode = 0;
    modifier = 0; // default as zero.
  }

  OVImfKeyCode* keyevent=new OVImfKeyCode( keychar);
  keyevent->setCode(keycode);
  switch(modifier)
  {
    case 0:
      break;
    case 1:
      keyevent->setShift(1);
      keyevent->setAlt(0);
      keyevent->setCtrl(0);
      break;
    case 2:
      keyevent->setShift(0);
      keyevent->setAlt(1);
      keyevent->setCtrl(0);
      break;
    case 4:
      keyevent->setShift(0);
      keyevent->setAlt(1);
      keyevent->setCtrl(1);
      break;
    case 8:
      keyevent->setShift(0);
      keyevent->setAlt(1);
      keyevent->setCtrl(0);
      break;
    default:
      keyevent->setShift(0);
      keyevent->setAlt(0);
      keyevent->setCtrl(0);
      break;
  }
  
  cxt->keyEvent( keyevent, preedit, lookupchoice, srv);

  if( commit_buf_len > 0 )
    return commit_buf;
  else
    return "";
}

void OVImf::switch_lang()
{
  // most are zh_TW, zh_CN
}

void OVImf::switch_im()
{
  int next_module = current_module + 1;
  if( next_module >= mod_vector.size() ){
    next_module = 0;
  }
  OVInputMethod* im = static_cast<OVInputMethod*>( mod_vector[ next_module ] );
  im->initialize(dict, srv, OV_MODULEDIR);
  delete cxt; // clean old data
  cxt = im->newContext();
  cxt->start( preedit, lookupchoice, srv );
}

void OVImf::switch_im_per_lang()
{
  switch_im();
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

OVImfBuffer::OVImfBuffer() {
    strcpy(buf, "Init!!");
    //Prdt* prdt = new Prdt;
}

OVBuffer* OVImfBuffer::clear() {
  //prdt->clear();  
  strcpy(buf, "");
  return this;
}

OVBuffer* OVImfBuffer::append(const char *s) {
  //prdt->append(s);
  strcat(buf, s);
  return this;
}

OVBuffer* OVImfBuffer::send() {
    OVImf::commitBuffer( buf );
    clear();
    return this;
}

OVBuffer* OVImfBuffer::update() {
    if (strlen(buf)) 
      ;
    else 
      ;
    
    return this;
}

OVBuffer* OVImfBuffer::update(int cursorPos, int markFrom, int markTo) {
    return update();
}

int OVImfBuffer::isEmpty() {
    if (!strlen(buf)) 
      return 1;
    
    return 0;
}


/*
 * implementation of OVImfCandidate
 */

OVImfCandidate::OVImfCandidate() {
    onscreen=0;
    //Lkc* lkc = new Lkc;
    strcpy(buf, "");
}

OVCandidate* OVImfCandidate::clear() {
    //lkc->clear();
    strcpy(buf, "");
    return this;
}

OVCandidate* OVImfCandidate::append(const char *s) {
    //lkc->append( s );
    strcat(buf, s);
    return this;
}

OVCandidate* OVImfCandidate::hide() {
    if (onscreen) {
        //lkc->hide();
	onscreen=0;
    }
    return this;
}

OVCandidate* OVImfCandidate::show() {
    if (!onscreen) {
        //lkc->draw();
        std::cout << buf << std::endl;
	onscreen=1;
    }
    return this;
}

OVCandidate* OVImfCandidate::update() {
    //lkc->update();
    std::cout << buf << std::endl;
    return this;
}

int OVImfCandidate::onScreen() {
    return onscreen;
}


/*
 * implementation of OVImfService
 */

void OVImfService::beep() { }
void OVImfService::notify(const char *msg) { fprintf(stderr, "%s\n", msg); }
const char *OVImfService::locale(){ return "zh_TW"; }
const char *OVImfService::userSpacePath(const char *modid){ return "/tmp"; }
const char *OVImfService::pathSeparator() { return "/"; }

const char *OVImfService::toUTF8(const char *encoding, const char *src) { 
  iconv_t conv_codec;
  size_t in_bytes, out_bytes, final_bytes;
  char* out_buf=NULL;
  in_bytes = strlen(src)+1;
  out_bytes = 1024;
  memset(internal, 0, 1024);
  out_buf = internal;
  conv_codec = iconv_open( "UTF-8", encoding );
  final_bytes = iconv( conv_codec, (char**)&src, &in_bytes, &out_buf, &out_bytes );
  iconv_close( conv_codec );
  return internal;
}

const char *OVImfService::fromUTF8(const char *encoding, const char *src) { 
  iconv_t conv_codec;
  size_t in_bytes, out_bytes, final_bytes;
  char* out_buf=NULL;
  in_bytes = strlen(src)+1;
  out_bytes= 1024;
  memset(internal, 0, 1024);
  out_buf = internal;
  conv_codec = iconv_open(encoding, "UTF-8");
  final_bytes = iconv( conv_codec, (char**)&src, &in_bytes, &out_buf, &out_bytes );
  iconv_close( conv_codec );
  return internal;
}

enum { bit7=0x80, bit6=0x40, bit5=0x20, bit4=0x10, bit3=8, bit2=4, bit1=2, bit0=1 };

const char *OVImfService::UTF16ToUTF8(unsigned short *s, int l) {
            char *b = internal;
            for (int i=0; i<l; i++)
            {
                    if (s[i] < 0x80)
                    {
                            *b++=s[i];
                    }
                    else if (s[i] < 0x800)
                    {
                            *b++=(0xc0 | s[i]>>6);
                            *b++=(0x80 | s[i] & 0x3f);
                    }
                    else if (s[i] < 0xd800 || s[i] > 0xdbff)
                    {
                            *b++ = (0xe0 | s[i]>>12);
                            *b++ = (0x80 | s[i]>>6 & 0x3f);
                            *b++ = (0x80 | s[i] & 0x3f);

                    }
                    else
                    {
                            unsigned int offset= 0x10000 - (0xd800 << 10) - 0xdc00;
                            unsigned int codepoint=(s[i] << 10) + s[i+1]+offset;
                            i++;
                            *b++=(0xf0 | codepoint>>18);
                            *b++=(0x80 | codepoint>>12 & 0x3f);
                            *b++=(0x80 | codepoint>>6 & 0x3f);
                            *b++=(0x80 | codepoint & 0x3F);
                    }
            }

            *b=0;
            return internal;
}

int OVImfService::UTF8ToUTF16(const char *src, unsigned short **rcvr) {
	char *s1=(char*)src;
	int len=0;
	char a, b, c;
	while (*s1)
	{
		a=*s1++;
		if ((a & (bit7|bit6|bit5))==(bit7|bit6)) { // 0x000080-0x0007ff
			b=*s1++;

			u_internal[len] = ((a & (bit4|bit3|bit2)) >> 2) * 0x100;
			u_internal[len] += (((a & (bit1|bit0)) << 6) | (b & (bit5|bit4|bit3|bit2|bit1|bit0)));
		}
		else if ((a & (bit7|bit6|bit5|bit4))==(bit7|bit6|bit5)) // 0x000800-0x00ffff
		{
			b=*s1++;
			c=*s1++;

			u_internal[len] = (((a & (bit3|bit2|bit1|bit0)) << 4) | ((b & (bit5|bit4|bit3|bit2)) >> 2)) * 0x100;
			u_internal[len] += (((b & (bit1|bit0)) << 6) | (c & (bit5|bit4|bit3|bit2|bit1|bit0)));
		}
		else 
		{
			u_internal[len] =(0);
			u_internal[len] +=(a);
		}
		len++;
	}
	*rcvr = u_internal;
	return len;
}


/*
 * Implementation of OVImfDictionary
 */

int OVImfDictionary::keyExist(const char *key) {
   return _dict.find(key) != _dict.end();
}

int OVImfDictionary::getInteger(const char *key) {
    return atoi(_dict[key].c_str());
}

int OVImfDictionary::setInteger(const char *key, int value) {
   std::stringstream ss;
   ss << value;
   _dict.insert( std::make_pair(key, ss.str()) );
    return value;
}

const char* OVImfDictionary::getString(const char *key) {
    return _dict[key].c_str();
}

const char* OVImfDictionary::setString(const char *key, const char *value) {
   _dict.insert( std::make_pair(key, value) );
   return value;
}
