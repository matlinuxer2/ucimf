/*
 * UCIMF - Unicode Console InputMethod Framework                                                    
 *
 * Copyright (c) 2006-2007 Chun-Yu Lee (Mat) and Open RazzmatazZ Laboratory (OrzLab)
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

#include <ucimf.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <linux/keyboard.h>
#include <linux/input.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <dirent.h>
#include <ltdl.h>
#include <vector>

#include "imf.h"
#include "widget.h"
#include "cwm.h"
#include "options.h"

using namespace std;

bool prev_focus;
//ConsoleFocus *focus = ConsoleFocus::getInstance();
//CursorPosition *pos = CursorPosition::getInstance();
//CurrentImfStatus *sts = CurrentImfStatus::getInstance();
Status *stts = Status::getInstance();
Preedit *prdt = Preedit::getInstance();
LookupChoice *lkc = LookupChoice::getInstance();

Shift *status_shift = new StatusShift;
Shift *preedit_shift = new PreeditShift;
Shift *lookupchoice_shift = new LookupChoiceShift;

Cwm *cwm = Cwm::getInstance();

Imf *imf;
vector<Imf*> imfs;
size_t current_imf;

void scanImf()
{
  current_imf =0;
  imfs.clear();
  createImf_t* create_imf = NULL;
  destroyImf_t* destroy_imf = NULL;

  Options* option= Options::getInstance();
  char* imf_mod_path = option->getOption("IMF_MODULE_DIR");
  cerr << "IMF Modules scan path: " << imf_mod_path << endl;
 
  lt_dlinit();
  lt_dlsetsearchpath( imf_mod_path );

  DIR *dir = opendir( imf_mod_path );
  if( dir )
  {
    struct dirent *d_ent;
    while( ( d_ent = readdir(dir) ) != NULL )
    {
      if( strstr( d_ent->d_name, ".so") )
      {
	  lt_dlhandle handle = lt_dlopen( d_ent->d_name );
	  if( handle == NULL){
	    fprintf(stderr, "lt_dlopen %s failed\n", d_ent->d_name );
	  }
	  else{ 
	    create_imf = (createImf_t*) lt_dlsym( handle, "createImf" );
	    destroy_imf = (destroyImf_t*) lt_dlsym( handle, "destroyImf" );
	  }

	  if( !create_imf || !destroy_imf ){
	     fprintf(stderr, "lt_dlsym %s failed\n", d_ent->d_name );
	  }
	  
	  Imf* i;
	  i=create_imf();
	  if( i!=0 )
	  {
	    imfs.push_back(i);
	    cerr << "  Load Module[" << imfs.size() << "]: " << d_ent->d_name << endl;
	  }

      }
    }
    closedir(dir);
  }

}



Imf* nextImf()
{
  if( imfs.empty() )
  {
    return 0;
  }

  current_imf+=1;

  if( current_imf >= imfs.size() )
  {
    current_imf =0;
  }

  return imfs[current_imf];

}




void ucimf_init()
{
  prev_focus = false;
  imf = 0;
  cwm->attachWindow( stts->getWindow(), status_shift );
  cwm->attachWindow( prdt->getWindow(), preedit_shift );
  cwm->attachWindow( lkc->getWindow(), lookupchoice_shift );
  cwm->set_focus( false );
  scanImf();
  imf = imfs[current_imf];
  cerr << "UCIMF core intialized." << endl;
}

void ucimf_exit()
{
  if( imf !=0 )
  {
    delete imf;
    imf=0;
  }
}

void ucimf_switch( unsigned char *buf, int *p_buf_len )
{ 

  if( *p_buf_len !=5 || buf[0] != 27 || buf[1] !=91 || buf[2] != 50 || buf[4] != 126 )
  {
    return;
  }
  else
  {
      if(  buf[3] == 52 ) // F12
      {
	cwm->set_focus( !cwm->get_focus() );
	if(imf!=0)
	{
	   imf->refresh();
	}
      }
      else if( cwm->get_focus() && buf[3] == 51 ) // IM=ON && F11
      {
	if( cwm->get_focus() && imf !=0 )
	  imf->switch_im();
	else
	  cwm->set_focus( true );
      }
      else if( cwm->get_focus() && buf[3] == 49 ) // IM=ON && F10
      {
	prdt->clear();
	lkc->clear();
	stts->clear();

	imf = nextImf();
	if( imf!=0 )
	{
	  imf->refresh();
	}
      }
      else
      {
	return;
      }

      // Clear input buffer
      *p_buf_len = 0;
      buf[0]='\0';
  }


}


char* ucimf_process_stdin( char *buf, int *p_ret )
{
  string input( buf, *p_ret );
  string output;

  if( cwm->get_focus() == false || input.size() == 0 || imf == 0  ) 
  {
    ; // do nothing...
  }
  else
  {
    // clean input buffer
    (*p_ret)=0; 
    strcpy(buf,"");

    output = imf->process_input( input );
    (*p_ret) = output.copy( buf, string::npos ); // need to be checked!
  }

  return buf;
}


void ucimf_cursor_position( int x, int y)
{
  cwm->set_position( x, y );
}

void ucimf_refresh_begin()
{
  prev_focus = cwm->get_focus();
  cwm->set_focus( false );
}

void ucimf_refresh_end()
{
  cwm->set_focus( prev_focus );
}
