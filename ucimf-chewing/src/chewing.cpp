#include "uchewing.h"
#include <imf/widget.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <string>
#include <iconv.h>
#include <string.h>
using namespace std;

static int selKey_define[ 11 ] = {'1','2','3','4','5','6','7','8','9','0',0}; /* Default */


vector<uint32_t> _str_to_utf32( char* input, const char* enc ){
	vector<uint32_t> result;

	size_t inbuf_len  = strlen( input );	
	size_t outbuf_len = 256;

	char output[outbuf_len];

	char* inbuf  = (char*)input; 
	char* outbuf = (char*)output;
	size_t inbytesleft  = sizeof(char) * inbuf_len;
	size_t outbytesleft = sizeof(char) * outbuf_len;

	iconv_t conv_codec = iconv_open( "UTF-32", enc );
	iconv( conv_codec, &inbuf, &inbytesleft, &outbuf, &outbytesleft );
	iconv_close( conv_codec );

	// 計算字數, 一個 UTF-32 的字有 4 bytes.
	int count = ( outbuf_len - outbytesleft ) / 4;

	// The first byte of UTF-32 word( == output[0]) is byte-order header, so to ignore it!
	for( int i=1; i< count; i++) {
		result.push_back( ((uint32_t*)output)[i] );
	}

	return result;
}

char* _utf32_to_str( vector<uint32_t> input, const char* enc ){
	size_t inbuf_len  = input.size()*4;
	size_t outbuf_len = inbuf_len / 4 * 8;

	char tmp[inbuf_len];
	char output[outbuf_len];

	for( int i=0; i< input.size(); i++ ){
		((uint32_t*)tmp)[i] = input[i];
	}

	char* inbuf = (char*)tmp;
	char* outbuf = (char*)output;
	size_t inbytesleft = inbuf_len;
	size_t outbytesleft = outbuf_len;

	iconv_t conv_codec = iconv_open( enc, "UTF-32" );
	iconv( conv_codec, &inbuf, &inbytesleft, &outbuf, &outbytesleft );
	iconv_close( conv_codec );

	output[outbuf_len-outbytesleft]=0;

	string result( output );

	return const_cast<char*>(result.c_str() );
}


Imf* UcimfChewingHandler::_instance = 0;

Imf* UcimfChewingHandler::getInstance()
{
  if( _instance == 0 )
    _instance = new UcimfChewingHandler;

  return _instance;
}

UcimfChewingHandler::UcimfChewingHandler()
{
	/* Initialize libchewing */
	/* for the sake of testing, we should not change existing hash data */
	char *prefix = const_cast<char*>(CHEWING_DATA_PREFIX);
	chewing_Init( prefix, NULL );

	/* Request handle to ChewingContext */
	ctx = chewing_new();

	/* Set keyboard type */
	chewing_set_KBType( ctx, chewing_KBStr2Num( const_cast<char*>("KB_DEFAULT") ) );

	/* Fill configuration values */
	chewing_set_candPerPage( ctx, 9 );
	chewing_set_maxChiSymbolLen( ctx, 16 );
	chewing_set_addPhraseDirection( ctx, 1 );
	chewing_set_selKey( ctx, selKey_define, 10 );
	chewing_set_spaceAsSelection( ctx, 1 );
	chewing_set_autoShiftCur( ctx, 1 );
	chewing_set_phraseChoiceRearward( ctx, 1 );

}

UcimfChewingHandler::~UcimfChewingHandler()
{
	/* Free Chewing IM handle */
	chewing_delete( ctx );

	/* Termate Chewing services */
	chewing_Terminate();

}

string UcimfChewingHandler::commit_buf="";


void UcimfChewingHandler::refresh()
{
	Status *stts = Status::getInstance();
	stts->set_imf_status( const_cast<char*>("Chewing"), const_cast<char*>("新酷音"), const_cast<char*>(""));
}

string UcimfChewingHandler::process_input( const string& buf )
{
	string result("");

	int buf_len = buf.size();                                                                     

	if( buf_len == 1 ) {
		switch( buf[0] ){
			case 9:   chewing_handle_Tab( ctx ); break;
			case 13:  chewing_handle_Enter( ctx ); break;
			case 27:  chewing_handle_Esc( ctx ); break;
			case 32:  chewing_handle_Space( ctx ); break;
			case 126: chewing_handle_Del( ctx ); break;
			case 127: chewing_handle_Backspace( ctx ); break;
			default: {
					 chewing_handle_Default( ctx, buf[0] ); 
					 break;
				 }
		}
	}
	else if( buf_len == 3 && buf[0]==27 && buf[1]==91 ) {
		switch( buf[2] )
		{
			case 65: chewing_handle_Up( ctx ); break;
			case 66: chewing_handle_Down( ctx ); break;
			case 67: chewing_handle_Right( ctx ); break;
			case 68: chewing_handle_Left( ctx ); break;
			default:
			break;
		}
	}
	else if( buf_len == 4 && buf[0]==27 && buf[1]==91 && buf[4]==126 ) {
		switch( buf[3] )
		{
			case 49: chewing_handle_Home( ctx ); break;
			case 52: chewing_handle_End( ctx ); break;
			case 53: break; // PAGE_UP
			case 54: break; // PAGE_DOWN
			default:
			break;
		}
	}
	else {
		; // do nothing
	}

	int *prdt_count;
	
	string s; stringstream prdt_pre(s);
	char *prdt_text = chewing_zuin_String( ctx, prdt_count);
	char *prdt_text2 = chewing_buffer_String( ctx );

	vector<uint32_t> utf32_prdt_text  = _str_to_utf32( prdt_text, "UTF-8" );
	vector<uint32_t> utf32_prdt_text2 = _str_to_utf32( prdt_text2,"UTF-8" );

	// 取得虛擬遊標的位置
	int cursor = chewing_cursor_Current( ctx );

	// 計算斷詞的區間
	chewing_interval_Enumerate( ctx );
	while ( chewing_interval_hasNext( ctx ) ) {
		IntervalType it;
		chewing_interval_Get( ctx, &it ); 

		prdt_pre << "[" ;
		for( int i=it.from; i<it.to; i++ ){
			if( cursor == i ){
				prdt_pre << "." ;
			}

			if( i >=0 && i < utf32_prdt_text2.size() ){
				vector<uint32_t> utf32_word;
				utf32_word.push_back( utf32_prdt_text2[i] );
				prdt_pre << _utf32_to_str( utf32_word, "UTF-8" );
			}
			//prdt_pre << it.from << "-" << it.to ;
			if( i+1 == cursor && cursor == utf32_prdt_text2.size() ){
				prdt_pre << "." ;
			}
		}
		prdt_pre << "]" ;
	}


	Preedit *prdt = Preedit::getInstance();
	prdt->clear();
	prdt->append( const_cast<char*>(prdt_pre.str().c_str()) );
	prdt->append( prdt_text );
	prdt->render();

	LookupChoice *lkc = LookupChoice::getInstance();
	lkc->clear();
	{
		int i = 1;
		int currentPageNo;
		char str[ 20 ];
		char *cand_string;

		chewing_cand_Enumerate( ctx );
		while ( chewing_cand_hasNext( ctx ) ) { 
			if ( i > chewing_cand_ChoicePerPage( ctx ) ) {
				break;
			}

			cand_string = chewing_cand_String( ctx );

			string s2; stringstream lkc_pre(s2);
			lkc_pre << i << ". " ;

			lkc->append_next( const_cast<char*>(lkc_pre.str().c_str()) );
			lkc->append( cand_string );

			free( cand_string );
			i++;
		}   
	}
	lkc->render();



	if ( chewing_commit_Check( ctx ) ) {
		char *s;
		s = chewing_commit_String( ctx );
		commit_buf = s;
		free( s );
	}

	if( commit_buf.size() > 0 )
	{
		result = commit_buf;
		commit_buf.clear();
	}

	return result;
}

Imf* createImf() { 
	return UcimfChewingHandler::getInstance(); 
}

void destroyImf( Imf* imf ){ 
	delete imf; 
}
