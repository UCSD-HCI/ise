#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H

#include <Windows.h>
#include <iostream>

//FIXME: not work at all
#define DEBUG( s )            \
{                             \
   std::ostringstream os;    \
   os << s << std::endl;                   \
   OutputDebugString( os.str().c_str() );  \
}

#endif