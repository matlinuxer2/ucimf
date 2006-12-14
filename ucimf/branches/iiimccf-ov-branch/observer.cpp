#include <iiimcf.h>
#include <iiimp.h>
#include "observer.h"
#include "iiimccf-int.h"
#include "layer.h"
#include "graphdev.h"
#include <iostream>
using namespace std;

extern IIIMCCF* iiimccf;

Subject::Subject(){}
Subject::~Subject(){}

void Subject::notify()
{
  cout << "enter notify" << endl;
  for(int i=0; i< observers.size(); i++ )
  {
    cout << "enter observer 1" << endl;
    observers[i]->update();
    cout << "enter observer 2" << endl;
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
  if( x != new_x || y != new_y )
  {
    x = new_x;
    y = new_y;
    this->notify();
  }
}


Stts::Stts()
{
  visible = false;
  div = new Rectangle;
  title = new Text;
  pos_x = pos_y =0;
  shift_x = shift_y = 0;
}
Stts::~Stts()
{
  delete div;
  delete title;
}

extern IIIMCCF* iiimccf;

void Stts::update()
{
  if( isVisible() )
  {
    div->pop( tmp );
  }
  else
  {
    ;
  }
  
  iiimccf->trkpt->get_position( pos_x, pos_y );
  
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

  title->fh(16);
  title->fw(16);

  shift();
  
  title->x( pos_x - shift_x );
  title->y( pos_y - shift_y );
  title->fh(16);
  title->fw(16);
  title->fc(4);
  title->info();

  div->update( title->x(),
               title->y(),
	       title->x() + title->w(),
	       title->y() + title->h(),
	       0 );
  div->c(1);
 
  if( isVisible() )
  {
    div->push( tmp );
    div->render();
    title->render();
  }
  else
  { 
    ;
  }

}


#define X_MIN 0
#define X_MAX 800
#define Y_MIN 0
#define Y_MAX 600

void Stts::shift()
{
  shift_x = (pos_x + title->w()) - X_MAX ;
  if ( shift_x < -8 )
  {
    shift_x = 0 ;
  }
  else
  {
    shift_x = shift_x + 8 ; // shift back
  }

  shift_y = (pos_y + title->h() ) - Y_MAX ;
  if ( shift_y < - title->h() )
  {
    shift_y = - title->h() -1 ;
  }
  else
  {
    shift_y = title->h() +1 ;

  }


}

bool Stts::isVisible()
{
  return visible;
}

void Stts::setShow()
{
  visible = true;
}

void Stts::setHide()
{
  visible = false;
}

void Stts::show()
{
  if( isVisible() )
  {
    return;
  }
  else
  { 
    div->push( tmp );
    div->render();
    title->render();
    setShow();
  }

}

void Stts::hide()
{
  if( isVisible() )
  {
    setHide();
    div->pop( tmp );
  }
  else
  {
    return;
  }
}
