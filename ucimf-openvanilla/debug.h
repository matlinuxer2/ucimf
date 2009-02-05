#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef USE_DEBUG
#define UrDEBUG(format...) fprintf( stderr, format)
#else
#define UrDEBUG(format...)
#endif

#define UrINFO(format...) fprintf( stdout, format)
