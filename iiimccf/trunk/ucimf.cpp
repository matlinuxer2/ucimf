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

#include <ucimf.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <linux/keyboard.h>
#include <stdlib.h>
#include <string>
#include <dirent.h>
#include <ltdl.h>
#include <vector>

#include "imf.h"
#include "widget.h"
#include "cwm.h"

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
int current_imf;

enum
{
  CTRL_SPACE = 203,
  CTRL_SHIFT = 204,
  CTRL_F5 = 205,
  CTRL_F6 = 206,
  CTRL_F7 = 207,
};

typedef struct _new_key
{
  int table;
  int key;
  int new_key;
}
new_key;

static new_key nkeys[] = {
  /* [table]        [key]         [new_key] */
  {(1 << KG_CTRL), KEY_SPACE, K(KT_LATIN, CTRL_SPACE)},	/* Ctrl + Space */
  {(1 << KG_CTRL), KEY_LEFTSHIFT, K(KT_LATIN, CTRL_SHIFT)},	/* Ctrl + Shift */
  {(1 << KG_CTRL), KEY_F5, K(KT_LATIN, CTRL_F5)},	/* Ctrl + F5 */
  {(1 << KG_CTRL), KEY_F6, K(KT_LATIN, CTRL_F6)},	/* Ctrl + F6 */
  {(1 << KG_CTRL), KEY_F7, K(KT_LATIN, CTRL_F7)},	/* Ctrl + F7 */
};

int setup_keys()
{
  int i;
  for (i = 0; i < sizeof (nkeys) / sizeof (new_key); i++)
  {
    struct kbentry entry;
    struct kbentry tmp;
    entry.kb_table = nkeys[i].table;
    entry.kb_index = nkeys[i].key;

        /* save current value */
    ioctl (0, KDGKBENT, &entry);
    tmp.kb_value = entry.kb_value;

    entry.kb_value = nkeys[i].new_key;
    ioctl (0, KDSKBENT, &entry);

        /* save current value */
    nkeys[i].new_key = tmp.kb_value;
  }
  return 0;
}

void restore_keys()
{
  int i;
  for (i = 0; i < sizeof (nkeys) / sizeof (new_key); i++)
  {
    struct kbentry entry;
    entry.kb_table = nkeys[i].table;
    entry.kb_index = nkeys[i].key;
    entry.kb_value = nkeys[i].new_key;
    ioctl (0, KDSKBENT, &entry);
  }

}

void scanImf()
{
  current_imf =0;
  imfs.clear();
  createImf_t* create_imf;
  destroyImf_t* destroy_imf;

  char* IMF_MODULEDIR=getenv("IMF_MODULE_DIR");
  lt_dlinit();
  lt_dlsetsearchpath( IMF_MODULEDIR );

  DIR *dir = opendir( IMF_MODULEDIR );
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
  setup_keys();
  prev_focus = false;
  imf = 0;
  cwm->attachWindow( stts->getWindow(), status_shift );
  cwm->attachWindow( prdt->getWindow(), preedit_shift );
  cwm->attachWindow( lkc->getWindow(), lookupchoice_shift );
}

void ucimf_exit()
{
  restore_keys();
  if( imf !=0 )
  {
    delete imf;
    imf=0;
  }
}

void ucimf_switch( unsigned char *buf, int *p_buf_len )
{ 

  if(  *p_buf_len !=1  )
  {
    return ;
  }
  else
  {
      if(  buf[0] == 203 )
      {
	cwm->set_focus( !cwm->get_focus() );
	if(imf!=0)
	{
	   imf->refresh();
	}
      }
      else if( buf[0] == 204 )
      {
	if( cwm->get_focus() && imf !=0 )
	  imf->switch_im();
	else
	  cwm->set_focus( true );
      }
      else if( buf[0] == 205 )
      {
	scanImf();
	stts->clear();
      }
      else if( buf[0] == 206 )
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
