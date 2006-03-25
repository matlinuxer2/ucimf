#include <iiimcf.h>
#include <iiimp.h>
#include "observer.h"
#include "iiimccf-int.h"
#include "layer.h"
#include "graphdev.h"

Subject::Subject(){}
Subject::~Subject(){}

void Subject::notify()
{
  for(int i=0; i< observers.size(); i++ )
  {
    observers[i]->update();
  }
}

void Subject::attach( Observer* obsr )
{
  observers.push_back( obsr );
}


void Subject::detach( Observer* obsr )
{
  observers.erase( find( observers.begin(), observers.end(), obsr) );
}

Observer::Observer(){}
Observer::~Observer(){}

TrackPoint::TrackPoint()
{
  x = 0;
  y = 0;
}
TrackPoint::~TrackPoint(){}

void TrackPoint::get_position( int& old_x, int& old_y)
{
  old_x = x;
  old_y = y;
}

void TrackPoint::set_position( int new_x, int new_y)
{
  if( x != new_x && y != new_y )
  {
    x = new_x;
    y = new_y;
    this->notify();
  }
}


Stts::Stts()
{
  div = new Rectangle;
  title = new Text;
}
Stts::~Stts()
{
  delete div;
  delete title;
}

extern IIIMCCF* iiimccf;

void Stts::update()
{
  hide();
  
  IIIMF_status st; 
  IIIMCF_input_method *pims;
  const IIIMP_card16 *u16idname, *u16hrn, *u16domain;
  int num_of_ims;
  int cur_ims_id=iiimccf->cur_ims_id;
   
  st = iiimcf_get_supported_input_methods(iiimccf->handle, &num_of_ims, &pims);
  iiimcf_get_input_method_desc(pims[cur_ims_id], &u16idname, &u16hrn, &u16domain);
  
  vector<IIIMP_card16> vec_u16hrn(20);
  IIIMP_card16* pr=(IIIMP_card16*)u16idname;
  int vec_len=0;
  for( int i=0 ; (*(char*)pr) ; i++ )
  {
    vec_u16hrn[i]=*pr;
    vec_len++;
    pr++; 
  }
  vec_u16hrn.resize(vec_len);
  String buf4( vec_u16hrn );
  
  //delete title;
  title = new Text;
  title->append( buf4 );

  title->x(200);
  title->y(400);
  title->fh(16);
  title->fw(16);
  title->info();

  div->update( title->x(),
               title->y(),
	       title->x() + title->w(),
	       title->y() + title->h(),
	       0 );
  
  show();
}


void Stts::show()
{
  div->push( tmp );
  div->c(1);
  div->render();
  title->fc(4);
  title->render();
}

void Stts::hide()
{
  div->pop( tmp );
}
