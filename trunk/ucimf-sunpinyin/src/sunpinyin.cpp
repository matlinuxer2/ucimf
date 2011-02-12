#include "sunpinyin.h"
#include <portability.h>
#include <imf/widget.h>

#include <sstream>
#include <iostream>
#include <string>
#include <iconv.h>
using namespace std;

int prdt_len = 0;

void UcimfWindowHandler::updatePreedit(const IPreeditString* ppd)
{
	size_t ss_max_bytes = 128 ; // 最多能處理的 bytes
	size_t handled_bytes = 0; // 實際處理的 bytes

	char ss[ss_max_bytes]; // 用來接轉換後的字串的 buffer
	handled_bytes = WCSTOMBS( ss, ppd->string(), ss_max_bytes );
	
	prdt_len = handled_bytes;

	Preedit *prdt = Preedit::getInstance();
	prdt->clear();
	prdt->append( ss );
	prdt->render();
};


void UcimfWindowHandler::updateCandidates(const ICandidateList* pcl)
{
	LookupChoice *lkc = LookupChoice::getInstance();
	lkc->clear();

	for ( int i=0; i < pcl->size() ;i++) {
		size_t ss_max_bytes = 128 ; // 最多能處理的 bytes
		size_t handled_bytes = 0; // 實際處理的 bytes
		char ss[ss_max_bytes]; // 用來接轉換後的字串的 buffer
		handled_bytes = WCSTOMBS( ss, pcl->candiString(i), ss_max_bytes );
		ss[handled_bytes] = 0x0; 

		string s;
		std::stringstream item(s);
		item << (i+1)%10 <<  ss << " ";
		lkc->append_next( const_cast<char*>(item.str().c_str()) );
	}

	lkc->render();
};

void UcimfWindowHandler::commit(const TWCHAR* str)
{
	size_t ss_max_bytes = 128 ; // 最多能處理的 bytes
	size_t handled_bytes = 0; // 實際處理的 bytes
	char ss[ss_max_bytes]; // 用來接轉換後的字串的 buffer

	handled_bytes = WCSTOMBS( ss, str, ss_max_bytes );
	ss[ handled_bytes ] = 0x0;
	commit_buf = ss;
}

void UcimfWindowHandler::updateStatus(int key, int value) { }

Imf* UcimfWindowHandler::_instance = 0;

Imf* UcimfWindowHandler::getInstance()
{
  if( _instance == 0 )
    _instance = new UcimfWindowHandler;

  return _instance;
}

UcimfWindowHandler::UcimfWindowHandler()
{
	// 目前是將 SunPinyin 的輸入設定寫死
	// TODO: 實作 SunPinyin 動態設定
	bool bUseShuangpin = false;
	EShuangpinType SPScheme = MS2003;                                                                 
	bool bFuzzySegmentation = false;
	bool bFuzzyInnerSegmentation = false;


	CSunpinyinSessionFactory& fac = CSunpinyinSessionFactory::getFactory();

	if ( bUseShuangpin ){
		fac.setPinyinScheme(CSunpinyinSessionFactory::SHUANGPIN);
	}
	else{
		fac.setPinyinScheme(CSunpinyinSessionFactory::QUANPIN);
	}

	// AShuangpinSchemePolicy ... 是在 sunpinyin 中定義的
	AShuangpinSchemePolicy::instance().setShuangpinType(SPScheme);
	AQuanpinSchemePolicy::instance().setFuzzySegmentation(bFuzzySegmentation);
	AQuanpinSchemePolicy::instance().setInnerFuzzySegmentation(bFuzzyInnerSegmentation);

	view = fac.createSession();
	view->attachWinHandler( this );
}

UcimfWindowHandler::~UcimfWindowHandler()
{
	CSunpinyinSessionFactory& fac = CSunpinyinSessionFactory::getFactory();
	UcimfWindowHandler* uwh = (UcimfWindowHandler*) UcimfWindowHandler::getInstance();
	fac.destroySession ( view );
	delete uwh;
}

string UcimfWindowHandler::commit_buf="";


int stdin_to_sunpinyin_keycode( int keychar ) 
{
  int keycode;
  switch( keychar )
  {
    //case 9: keycode=ovkTab; break;
    case 13: keycode=IM_VK_ENTER; break;
    case 27: keycode=IM_VK_ESCAPE; break;
    case 32: keycode=IM_VK_SPACE; break;
    case 126: keycode=IM_VK_DELETE; break;
    case 127: keycode=IM_VK_BACK_SPACE; break;
    default: 
	      keycode = keychar; break;
  };
  return keycode;
}

void UcimfWindowHandler::refresh()
{
	Status *stts = Status::getInstance();
	stts->set_imf_status( const_cast<char*>("SunPinyin"), const_cast<char*>("SunPinyin"), const_cast<char*>(""));
}

string UcimfWindowHandler::process_input( const string& buf )
{
	if( false ) { return string(""); }

  int keychar,keycode,modifier;
  int buf_len = buf.size();

  if( buf_len == 1 )
  {
    keychar = (int)buf[0];
    keycode = stdin_to_sunpinyin_keycode(keychar);
    modifier = 0;
  }

  else if( buf_len == 3 && buf[0]==27 && buf[1]==91 )
  {
    switch( buf[2] )
    {
      case 65:
	keychar = 0;
	keycode = IM_VK_UP;
	modifier = 0; // default as zero.
	break;

      case 66:
	keychar = 0;
	keycode = IM_VK_DOWN;
	modifier = 0; // default as zero.
	break;
	
      case 67:
	keychar = 0;
	keycode = IM_VK_RIGHT;
	modifier = 0; // default as zero.
	break;
	
      case 68:
	keychar = 0;
	keycode = IM_VK_LEFT;
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
	keycode = IM_VK_PAGE_UP;
	modifier = 0; // default as zero.
	break;
	
      case 54:
	keychar = 0;
	keycode = IM_VK_PAGE_DOWN;
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

  
  string result;

  if ( prdt_len == 0 ){
	  if( keycode == IM_VK_BACK_SPACE )
	  {
		  result = "\x7f";
		  return result;
	  }
	  else if( keycode == IM_VK_ENTER )
	  {
		  result = "\x0d";
		  return result;
	  }
	  else if( keycode == IM_VK_RIGHT )
	  {
		  result = "\x1b\x5b\x43";
		  return result;
	  }
	  else if( keycode == IM_VK_LEFT )
	  {
		  result = "\x1b\x5b\x44";
		  return result;
	  }
  }
	
  unsigned keyvalue = keychar;

  if ( keyvalue < 0x20 && keyvalue > 0x7E){
	  keyvalue = 0;
  }

  CKeyEvent key_event(keycode, keyvalue, NULL );
  view->onKeyEvent(key_event);


  if( commit_buf.size() > 0 )
  {
    result = commit_buf;
    commit_buf.clear();
  }
  
  return result;

}

Imf* createImf() { 
	return UcimfWindowHandler::getInstance(); 
}

void destroyImf( Imf* imf ){ 
	delete imf; 
}
