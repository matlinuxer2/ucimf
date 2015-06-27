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

#include "openvanilla.h"
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "debug.h"


int LogFd=-1;

// path for openvanilla.so to locate libovimgeneric,... *.so 
#define OV_MODULEDIR LIBDIR"/openvanilla/"

// path for libovimgeneric.so, to locate *.cin , and data per modules
#define OV_MODULEDIR2 DATADIR"/openvanilla/"

// path for libovimgeneric.so, mostly unused.
#define OV_MODULEDIR3 LIBDIR"/openvanilla/"

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

OVInputMethod* OVImf::im = 0;

OVImf::OVImf()
{
	
	char name[64];
	snprintf(name, sizeof(name), "%s/%s", getenv("HOME"), ".ucimf-log");

        extern int LogFd;
	LogFd = open(name, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
	if ( LogFd == -1){
		printf("open log file error");
	}  

  current_module = 0;
  current_im_name = const_cast<char*>("");
  cxt = 0;

  preedit = new OVImfBuffer;
  lookupchoice = new OVImfCandidate;
  srv = new OVImfService;
  dict = new OVImfDictionary;
  dict->setString( "matchOneChar","?" );
  dict->setString( "matchZeroOrMoreChar","*" );
  dict->setInteger( "maxKeySequenceLength", 32 );
  dict->setInteger( "autoCompose", 1 );
  


  stts = Status::getInstance();

  UrDEBUG( "OVIMF starting \n" );
  UrDEBUG( "OVIMF Module Dir: %s \n", OV_MODULEDIR );
  UrDEBUG( "OVIMF Module Dir2: %s \n", OV_MODULEDIR2 );
  UrDEBUG( "OVIMF Module Dir3: %s \n", OV_MODULEDIR3 );


  // check *.cin search path 
  bool inUserDir = false;
  bool inLibDir = false;
  bool inDataDir = false;

  struct stat stb;
  char user_dir[64];
  snprintf( user_dir, sizeof(user_dir), "%s/%s", getenv("HOME"), ".openvanilla/OVIMGeneric/");

  if (stat( user_dir, &stb) == 0 && S_ISDIR(stb.st_mode)){
	  inUserDir = true;
	  UrDEBUG( "Found *.cin search path: %s \n", user_dir );
  }
  if (stat(OV_MODULEDIR2"/OVIMGeneric/", &stb) == 0 && S_ISDIR(stb.st_mode)){
	  inDataDir = true;
	  UrDEBUG( "Found *.cin search path: %s \n", OV_MODULEDIR2"/OVIMGeneric/" );
  }
  if (stat(OV_MODULEDIR"/OVIMGeneric/", &stb) == 0 && S_ISDIR(stb.st_mode)){
	  inLibDir = true;
	  UrDEBUG( "Found *.cin search path: %s \n", OV_MODULEDIR"/OVIMGeneric/" );
  }


  lt_dlinit();
  lt_dlsetsearchpath( OV_MODULEDIR );

  struct dirent **namelist;

  int n = scandir( OV_MODULEDIR, &namelist, 0, alphasort);
  if (n < 0){
	  UrDEBUG("%s\n", OV_MODULEDIR" is not found" );
  }  
  else {
	  while (n--) {
		  UrDEBUG("%s\n", namelist[n]->d_name);
		  if( strstr( namelist[n]->d_name, ".so") ) {
			  OVLibrary* mod = new OVLibrary();

			  UrDEBUG( "loading .so: %s \n", namelist[n]->d_name );

			  mod->handle = lt_dlopen( namelist[n]->d_name );
			  if(mod->handle == NULL){
				  UrDEBUG( "lt_dlopen loading error: %s \n", lt_dlerror() );
				  delete mod;
				  continue;
			  }
			  else{ 
				  mod->getModule = (TypeGetModule)lt_dlsym( mod->handle, "OVGetModuleFromLibrary" );
				  mod->getLibVersion = (TypeGetLibVersion)lt_dlsym( mod->handle, "OVGetLibraryVersion" );
				  mod->initLibrary = (TypeInitLibrary)lt_dlsym( mod->handle, "OVInitializeLibrary" );
			  }

			  if( !mod->getModule || !mod->getLibVersion || !mod->initLibrary ){
				  UrDEBUG( " the loading module is not compatible with current spec \n" );
				  delete mod;
				  continue;
			  }

			  if( mod->getLibVersion() < OV_VERSION ){
				  UrDEBUG( "Library Version is not match: %d \n", OV_VERSION );
				  delete mod;
				  continue;
			  }


			  if(mod){
				  OVModule* m;

				  // Check *.cin search path in order
				  if ( inUserDir ){
					  mod->initLibrary(this->srv, user_dir );
					  UrDEBUG( "Set *.cin search path to: %s \n", user_dir );
				  }
				  else if ( inDataDir ){
					  mod->initLibrary(this->srv, OV_MODULEDIR2 );
					  UrDEBUG( "Set *.cin search path to: %s \n", OV_MODULEDIR2"/OVIMGeneric/" );
				  }
				  else if ( inLibDir ){
					  mod->initLibrary(this->srv, OV_MODULEDIR );
					  UrDEBUG( "Set *.cin search path to: %s \n", OV_MODULEDIR"/OVIMGeneric/" );
				  }
				  else {
					  mod->initLibrary(this->srv, user_dir );
					  UrDEBUG( "Set *.cin search path to: %s \n", user_dir );
				  }

				  for(int i=0; m = mod->getModule(i); i++) {
					  string str1= m->moduleType();
					  string str2= "OVInputMethod";
					  if( str1 == str2 )
					  {
						  this->mod_vector.push_back(m);
					  }
				  }
				  delete mod;
			  }

		  }

	  free(namelist[n]);

	  }  
  free(namelist);
  }  


   
  //OVInputMethod* im = dynamic_cast<OVInputMethod*>(mod_vector[ current_module ]);
  
  if( cxt != 0 )
  {
    delete cxt;
  }
  
  if( !mod_vector.empty() )
  {
    im = static_cast<OVInputMethod*>(mod_vector[ current_module ]);
    
    if( im !=0 )
    {
      im->initialize( dict, srv, OV_MODULEDIR3 );
      cxt = im->newContext();
      cxt->start( preedit, lookupchoice, srv );
      
      current_im_name =(char*) im->localizedName( srv->locale() );
      refresh();
      preedit->clear();
      lookupchoice->clear();
    }
  }
  else
  {
    im = 0;
  }

}

OVImf::~OVImf()
{
	extern int LogFd;
	if( LogFd >=0 )
	{
		close(LogFd);
	}
}

string OVImf::commit_buf="";

void OVImf::commitBuffer( string input )
{
  commit_buf = input;
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
	      keycode = keychar; break;
  };
  return keycode;
}

void OVImf::refresh()
{
  //stts->set_imf_name( "OpenVanilla" );
  //stts->set_im_name( current_im_name );
  stts->set_imf_status( const_cast<char*>("OpenVanilla"),current_im_name, const_cast<char*>(""));
}

string OVImf::process_input( const string& buf )
{
	if( im == 0 ) { return string(""); }

  int keychar,keycode,modifier;
  int buf_len = buf.size();

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
  
  string result;

  if( preedit->isEmpty() && keyevent->code() == ovkBackspace )
  {
	  result = "\x7f";
	  return result;
  }
  else if( preedit->isEmpty() && keyevent->code() == ovkReturn )
  {
	  result = "\x0d";
	  return result;
  }
  else if( preedit->isEmpty() && keyevent->code() == ovkRight )
  {
	  result = "\x1b\x5b\x43";
	  return result;
  }
  else if( preedit->isEmpty() && keyevent->code() == ovkLeft )
  {
	  result = "\x1b\x5b\x44";
	  return result;
  }
	
  cxt->keyEvent( keyevent, preedit, lookupchoice, srv);


  if( commit_buf.size() > 0 )
  {
    result = commit_buf;
    commit_buf.clear();
  }
  
  return result;

}

void OVImf::switch_lang()
{
  // most are zh_TW, zh_CN
}

void OVImf::switch_im()
{
	if( im == 0 ) { return ; }

  current_module += 1;
  if( current_module >= mod_vector.size() ){
    current_module = 0;
  }
  im = static_cast<OVInputMethod*>( mod_vector[ current_module ] );
  //OVInputMethod* im = dynamic_cast<OVInputMethod*>( mod_vector[ current_module ] );
  if( cxt != 0 )
  {
    delete cxt; // clean old data
  }

  if( im !=0 )
  {
    im->initialize( dict, srv, OV_MODULEDIR3 );
    cxt = im->newContext();
    cxt->start( preedit, lookupchoice, srv );
    
    current_im_name = (char*) im->localizedName( srv->locale() );
    refresh();
    preedit->clear();
    lookupchoice->clear();
  }
}

void OVImf::switch_im_reverse()
{
	if( im == 0 ) { return ; }

  current_module -= 1;
  if( current_module < 0 ){
    current_module = mod_vector.size()-1;
  }
  im = static_cast<OVInputMethod*>( mod_vector[ current_module ] );
  //OVInputMethod* im = dynamic_cast<OVInputMethod*>( mod_vector[ current_module ] );
  if( cxt != 0 )
  {
    delete cxt; // clean old data
  }

  if( im !=0 )
  {
    im->initialize( dict, srv, OV_MODULEDIR3 );
    cxt = im->newContext();
    cxt->start( preedit, lookupchoice, srv );
    
    current_im_name = (char*) im->localizedName( srv->locale() );
    refresh();
    preedit->clear();
    lookupchoice->clear();
  }
}

void OVImf::switch_im_per_lang()
{
  switch_im();
}


Imf* createImf() { 
	return OVImf::getInstance(); 
}

void destroyImf( Imf* imf ){ 
	delete imf; 
}
