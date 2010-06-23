#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>

#define UCIMF_ERR(format...)    	{extern int LogFd; if( LogFd >=0 ){ dprintf( LogFd, "[Err]:"format  );} }
#define UCIMF_WARNING(format...)        {extern int LogFd; if( LogFd >=0 ){ dprintf( LogFd, "[WARN]:"format );} } 
#define UCIMF_INFO(format...)           {extern int LogFd; if( LogFd >=0 ){ dprintf( LogFd, "[INFO]:"format );} } 
#define UCIMF_DEBUG(format...)          {extern int LogFd; if( LogFd >=0 ){ dprintf( LogFd, "[DEBUG]:"format);} } 

#define UrINFO(format...) UCIMF_INFO(format)
#define UrDEBUG(format...) UCIMF_DEBUG(format)
