#include <chewing.h>
#include <global.h>
#include <imf/imf.h>
#include <string>

class UcimfChewingHandler : public Imf
{
	// 繼承 Imf 的部分
	public:
		std::string process_input( const std::string& input );
		void refresh();
		void switch_im(){};
		void switch_im_reverse(){};
		void switch_lang(){};
		void switch_im_per_lang(){};

	public:
		~UcimfChewingHandler();
		static Imf* getInstance();

	protected:
		UcimfChewingHandler();
		static Imf* _instance;
		ChewingContext *ctx; 
		static std::string commit_buf;
};

extern "C" Imf* createImf();
extern "C" void destroyImf( Imf* imf );
