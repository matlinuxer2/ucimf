#include <ime-core/imi_view.h>
#include <ime-core/imi_options.h>
#include <ime-core/utils.h>

#include <imf/imf.h>
#include <string>

class UcimfWindowHandler : public CIMIWinHandler, public Imf
{
	// 繼承 CIMIWinHandler 的部分
	public:
		virtual void updatePreedit(const IPreeditString* ppd);
		virtual void updateCandidates(const ICandidateList* pcl);
		virtual void updateStatus(int key, int value);
		virtual void commit(const TWCHAR* str);

	// 繼承 Imf 的部分
	public:
		std::string process_input( const std::string& input );
		void refresh();
		void switch_im(){};
		void switch_im_reverse(){};
		void switch_lang(){};
		void switch_im_per_lang(){};

	public:
		~UcimfWindowHandler();
		static Imf* getInstance();

	protected:
		UcimfWindowHandler();
		static Imf* _instance;

		static std::string commit_buf;
		CIMIView* view;
};

extern "C" Imf* createImf();
extern "C" void destroyImf( Imf* imf );
