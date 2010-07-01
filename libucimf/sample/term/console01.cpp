#include <ucimf.h>

int main()
{
	ucimf_init();
	ucimf_cursor_position( 300, 300);
	ucimf_refresh_begin();
	ucimf_refresh_end();
	ucimf_exit();
}
