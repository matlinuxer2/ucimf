#include <ime-core/imi_view.h>
#include <ime-core/imi_options.h>
#include <ime-core/utils.h>
#include <portability.h>

#include <iostream>
#include <string>
#include <iconv.h>
using namespace std;


class UcimfWindowHandler : public CIMIWinHandler
{
	public:
	virtual void updatePreedit(const IPreeditString* ppd);
	virtual void updateCandidates(const ICandidateList* pcl);
	virtual void updateStatus(int key, int value);
	virtual void commit(const TWCHAR* str){};

	char m_buf[128];
	char m_iconv[128];
};

bool bUseShuangpin = true;
EShuangpinType SPScheme = MS2003;                                                                 
bool bFuzzySegmentation = false;
bool bFuzzyInnerSegmentation = false;

int main( int argc, char* argv[])
{

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

	CIMIView* view = fac.createSession();

	UcimfWindowHandler* uwh = new UcimfWindowHandler();
	view->attachWinHandler( uwh );

	//view->getIC()->setCharsetLevel(1);
	//view->setCandiWindowSize( 10 );
	{
		// key => 'z'
		unsigned keycode = 44; //event->keyval;
		unsigned keyvalue = 0x7a; //event->keyval;

		if (keyvalue < 0x20 && keyvalue > 0x7E){
			keyvalue = 0;
		}

		CKeyEvent key_event(keycode, keyvalue, NULL );
		view->onKeyEvent(key_event);

	}

	fac.destroySession ( view );
	delete uwh;

	return 0;
}

void UcimfWindowHandler::updatePreedit(const IPreeditString* ppd){
			cerr << "updatePreedit begin" << endl;

			size_t ss_max_bytes = 128 ; // 最多能處理的 bytes
			size_t handled_bytes = 0; // 實際處理的 bytes

			char ss[ss_max_bytes]; // 用來接轉換後的字串的 buffer
			handled_bytes = WCSTOMBS( ss, ppd->string(), ss_max_bytes );

			cerr << ss << endl;

			cerr << "updatePreedit end" << endl;
};

void UcimfWindowHandler::updateCandidates(const ICandidateList* pcl){
			cerr << "updateCandidates begin" << endl;

			for ( int i=0; i < pcl->size() ;i++) {
				size_t ss_max_bytes = 128 ; // 最多能處理的 bytes
				size_t handled_bytes = 0; // 實際處理的 bytes
				char ss[ss_max_bytes]; // 用來接轉換後的字串的 buffer
				handled_bytes = WCSTOMBS( ss, pcl->candiString(i), ss_max_bytes );

				cerr << "["<< i << "]" << ss << endl;
			}

    //char * dst = m_buf;
    //size_t dstlen = sizeof(m_buf) - 1;
    //iconv(m_iconv, &src, &srclen, &dst, &dstlen);

			cerr << "updateCandidates end" << endl;

};

void UcimfWindowHandler::updateStatus(int key, int value)
{
	switch (key) {
		case STATUS_ID_CN: {
			cerr << "STATUS_ID_CN" << endl;
			break;
		}
		case STATUS_ID_FULLPUNC: {
			cerr << "STATUS_ID_FULLPUNC" << endl;
			break;
		}
		case STATUS_ID_FULLSYMBOL: {
			cerr << "STATUS_ID_FULLSYMBOL" << endl;
			break;
		}
	}
}

