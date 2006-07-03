#include "openvanilla.h"
#include <iconv.h>
#include <iostream>

/* 
 * implementation of OVImf
 */
OVImf::OVImf()
{

  // OV_MODULEDIR is defined in Makefile.am !!
  char* OV_MODULEDIR=getenv("OVMODULE_DIR");
  char* module_filename=getenv("OVMODULE_FILEPATH");

  OVLibrary* mod = new OVLibrary();
  preedit = new OVImfBuffer;
  lookupchoice = new OVImfCandidate;
  srv = new OVImfService;
  
  std::cout << module_filename << std::endl;
  mod->handle = dlopen(module_filename, RTLD_LAZY);
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
  /*
  if( mod->getLibVersion() < OV_VERSION ){
     fprintf(stderr, "%s %d is too old\n", module_filename, mod->getLibVersion());
     delete mod;
  }
  */

  if(mod){
     mod->initLibrary(srv, OV_MODULEDIR);
     for(int i=0; OVModule* m = mod->getModule(i); i++)
	mod_vector.push_back(m);
     delete mod;
  }

  //OVModule m;
  //OVInputMethod* im = dynamic_cast<OVInputMethod*>(mod_vector[0]);
  OVInputMethod* im = static_cast<OVInputMethod*>(mod_vector[0]);
  //OVInputMethod *im = new OVIMArray;
  //dict = new OVImfDictionary;
  //OVImfDictionary dict2;
  im->initialize(&dict, srv, OV_MODULEDIR);
  cxt = im->newContext();
  cxt->start( preedit, lookupchoice, srv );
}


char* OVImf::process_keyevent( int keychar, int keycode, int modifier )
{
  
  OVImfKeyCode* keyevent=new OVImfKeyCode(keychar);
  // setup keyevent's values responding keychar, keycode, modifier
  int ovkeycode=keychar;
  /*
  switch (keycode) {
      case SCIM_KEY_Shift_L:
      case SCIM_KEY_Control_L:
      case SCIM_KEY_Alt_L:
      case SCIM_KEY_Left:      ovkeycode=ovkLeft; break;
      case SCIM_KEY_Shift_R:
      case SCIM_KEY_Control_R:
      case SCIM_KEY_Alt_R:
      case SCIM_KEY_Right:     ovkeycode=ovkRight; break;
      case SCIM_KEY_Up:        ovkeycode=ovkUp; break;
      case SCIM_KEY_Down:      ovkeycode=ovkDown; break;
      case SCIM_KEY_Delete:    ovkeycode=ovkDelete; break;
      case SCIM_KEY_Home:      ovkeycode=ovkHome; break;
      case SCIM_KEY_End:       ovkeycode=ovkEnd; break;
      case SCIM_KEY_Tab:       ovkeycode=ovkTab; break;            
      case SCIM_KEY_BackSpace: ovkeycode=ovkBackspace; break;
      case SCIM_KEY_Escape:    ovkeycode=ovkEsc; break;
      case SCIM_KEY_space:     ovkeycode=ovkSpace; break;
      case SCIM_KEY_Return:    ovkeycode=ovkReturn; break;
  }
  */
  keyevent->setCode(ovkeycode);
  switch(modifier)
  {
    case 0:
      break;
    case 1:
      keyevent->setShift(1);
      break;
    case 2:
      keyevent->setAlt(1);
      break;
    case 4:
      keyevent->setCtrl(1);
      break;
    case 8:
      keyevent->setAlt(1);
      break;
    default:
      break;
  }
  
  cxt->keyEvent( keyevent, preedit, lookupchoice, srv);

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
    //im=i;
    strcpy(buf, "i");
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
    //WideString bs=utf8_mbstowcs(buf);
    std::cout << buf << std::endl;
    clear();
    //im->commit_string(bs);
    return this;
}

OVBuffer* OVImfBuffer::update() {
    //im->update_preedit_string(utf8_mbstowcs(buf));
    if (strlen(buf)) 
      ;//im->show_preedit_string();
    else 
      ;//im->hide_preedit_string();     
    
    return this;
}

OVBuffer* OVImfBuffer::update(int cursorPos, int markFrom, int markTo) {
    return update();
}

int OVImfBuffer::isEmpty() {
    if (!strlen(buf)) return 1;
    return 0;
}


/*
 * implementation of OVImfCandidate
 */

OVImfCandidate::OVImfCandidate() {
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
      std::cout << buf << std::endl;
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


