
#pragma once

#include <vector>
#include <utility>
#include <string>
#include "tchar.h"

#if defined(_UNICODE) || defined(UNICODE)

// strings 
#define testrlen wcsnlen
#define testrcat wcscat_s
#define testrcpy wcscpy_s
#define testrchr wcschr
#define testrrchr wcsrchr
#define testrrev _wcsrev

#define tesvsprintf _vsnwprintf_s

// maths
#define tesatoi	_wtoi
#define tesatoi64 _wtoi64
#define tesatof _wtof
#define tesitoch _itow_s

#else

#define testrlen strnlen
#define testrcat strcat_s
#define testrcpy strcpy_s
#define testrchr strchr
#define testrrchr strrchr
#define testrrev _strrev

#define tesvsprintf vsnprintf_s

#define tesatoi	atoi
#define tesatoi64 _atoi64
#define tesatof atof
#define tesitoch _itoa_s

#endif

// string and char types
#if defined(_UNICODE) || defined(UNICODE)

#define testring std::wstring
#define techar wchar_t
#define TEStr(s) _T(s)

#define TELPSTR wchar_t*
#define TELPCSTR const wchar_t*

#else

#define testring std::string
#define techar char
#define TEStr(s) s

#define TELPSTR char*
#define TELPCSTR const char*

#endif

// export lib code
#ifdef __cplusplus
extern "C"
{
#endif


#define TE_MAX_PATH 260

// char

// check strings equal
extern __forceinline bool	IsEqualString(const techar* in_ch1, const techar* in_ch2);
// check strings equal
extern __forceinline bool	IsEqualStringA(const char* in_ch1, const char* in_ch2);

// string convertation to MBCS
extern __forceinline bool	testringToMBCS(const techar* in_chIn, char* out_ANSI, size_t out_size = TE_MAX_PATH);
// string convertation from MBCS
extern __forceinline bool	MBCSTotestring(const char* in_chIn, techar* out_str, size_t out_size = TE_MAX_PATH);

// std::string

// separate file name from extension 
extern __forceinline bool	SGetFileBaseName(const testring* in_pstrFileWithExtension, testring* out_pstrFileName);
// get file extension
extern __forceinline bool	SGetFileExtension( const testring* in_pstrFileWithExt, testring* out_pstrFileExt );
// separate file with extension from path
extern __forceinline bool	SGetFileName(const testring* in_pstrFilePath, testring* out_pstrFileName);
// folder
extern __forceinline bool	SGetFileFolder(const testring* in_pstrFullFilePath, testring* out_pstrPathFolder);


// devide string by word
extern __forceinline bool	DivideStringApart(const testring* c_pstrIn, std::vector<testring>* p_stdvOut, techar chSimbol);
// devide string by word
extern __forceinline bool	DivideStringApartA(const std::string* c_pstrIn, std::vector<std::string>* p_stdvOut, char chSimbol);

// clear all symbols in string
extern __forceinline bool	SClearySimbols(const testring* c_pstrIn, testring* p_strOut, techar chSimbol);
// clear empty symbols at the begin and at the end of sth string
extern __forceinline bool	SClearEmptySimbols(const testring* c_pstrIn, testring* p_strOut);

#ifdef __cplusplus
}
#endif