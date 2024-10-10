#pragma once

#include<string>

using std::string;
using std::wstring;

using std::to_string;
using std::to_wstring;

// basic unicode support for strings

#ifdef UNICODE

#ifndef STRINGB
#define STRINGB wstring	// unicode string
#endif

#ifndef CHARB
#define CHARB wchar_t
#endif

#ifndef REMOVEB
#define REMOVEB _wremove
#endif

#ifndef TO_STRINGB
#define TO_STRINGB to_wstring
#endif

#else

#ifndef STRINGB
#define STRINGB string	// simple ascii string
#endif

#ifndef CHARB
#define CHARB char
#endif

#ifndef REMOVEB
#define REMOVEB remove
#endif

#ifndef TO_STRINGB
#define TO_STRINGB to_string
#endif

#endif