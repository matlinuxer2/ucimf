#include <stdio.h>

#define UCIMF_ERR(format, ...)    	{extern int LogFd; if( LogFd >=0 ){ dprintf( LogFd, "[Err]:"format, ##__VA_ARGS__);} }
#define UCIMF_WARNING(format, ...)        {extern int LogFd; if( LogFd >=0 ){ dprintf( LogFd, "[WARN]:"format, ##__VA_ARGS__);} }
#define UCIMF_INFO(format, ...)           {extern int LogFd; if( LogFd >=0 ){ dprintf( LogFd, "[INFO]:"format, ##__VA_ARGS__);} }
#define UCIMF_DEBUG(format, ...)          {extern int LogFd; if( LogFd >=0 ){ dprintf( LogFd, "[DEBUG]:"format, ##__VA_ARGS__);} }

#define UrINFO(format, ...) UCIMF_INFO(format, ##__VA_ARGS__)
#define UrDEBUG(format, ...) UCIMF_DEBUG(format, ##__VA_ARGS__)
