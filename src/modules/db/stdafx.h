// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#if defined _MSC_VER
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// *NOTE*: work around quirky MSVC...
#define NOMINMAX

// Windows Header Files
#include <windows.h>
#endif

// C RunTime Header Files
//#include <sstream>
#include <string>

// System Library Header Files
#include "ace/config-lite.h"
#include "ace/Global_Macros.h"
#include "ace/Log_Msg.h"

#if defined (_MSC_VER)
#define uint unsigned int
#define ulong unsigned long
#include "mysql.h"
#else
#include "mysql/mysql.h"
#endif

// Local Header Files
#include "common.h"
#include "common_macros.h"

#include "stream_common.h"
#include "stream_macros.h"

#include "stream_db_exports.h"
