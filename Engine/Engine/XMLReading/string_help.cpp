

#include "string_help.h"

#include <cassert>
#include <locale> // converter
#include <codecvt>
#include <string>

#ifdef __cplusplus
extern "C"
{
#endif



// check strings equal
bool	IsEqualString(const techar* in_ch1, const techar* in_ch2)
{
	assert(in_ch1 != NULL);
	assert(in_ch2 != NULL);

	size_t iLength1 = testrlen(in_ch1, TE_MAX_PATH);
	size_t iLength2 = testrlen(in_ch2, TE_MAX_PATH);

	if (iLength1 != iLength2)
		return false;

	for (size_t i = 0; i < iLength1; i++)
		if (in_ch1[i] != in_ch2[i])
			return false;

	return true;
}

// check strings equal
bool	IsEqualStringA(const char* in_ch1, const char* in_ch2)
{
	assert(in_ch1 != NULL);
	assert(in_ch2 != NULL);

	size_t iLength1 = strnlen(in_ch1, TE_MAX_PATH);
	size_t iLength2 = strnlen(in_ch2, TE_MAX_PATH);

	if (iLength1 != iLength2)
		return false;

	for (size_t i = 0; i < iLength1; i++)
		if (in_ch1[i] != in_ch2[i])
			return false;

	return true;
}

// string convertation to MBCS
bool	testringToMBCS(const techar* in_chIn, char* out_ANSI, size_t out_size)
{
	assert(in_chIn != NULL);
	assert(out_ANSI != NULL);

#if defined(_UNICODE) || defined(UNICODE)

	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	std::string u8str = conv.to_bytes(in_chIn);

	strcpy_s(out_ANSI, out_size, u8str.c_str());
#else

	testrcpy(out_ANSI, out_size, in_chIn);

#endif

	return true;

}

// string convertation from MBCS
bool	MBCSTotestring(const char* in_chIn, techar* out_str, size_t out_size)
{
	assert(in_chIn != NULL);
	assert(out_str != NULL);

#if defined(_UNICODE) || defined(UNICODE)

	size_t size = strlen(in_chIn) + 1;
	out_str[0] = '\0';

	size = __min(size, out_size);
	mbstowcs_s(&out_size, out_str,size, in_chIn, size-1);

	if (size < out_size)
		out_str[size] = '\0';

//#endif

#else

	testrcpy(out_str, out_size, in_chIn);

#endif

	return true;
}

// separate file name from extension 
bool	SGetFileBaseName(const testring* in_pstrFileWithExtension, testring* out_pstrFileName)
{
	assert(in_pstrFileWithExtension);
	assert(out_pstrFileName);

	if (in_pstrFileWithExtension->empty())
		return false;

	size_t sNum = in_pstrFileWithExtension->find_first_of('.');
	if (sNum == testring::npos)
		return false;

	*out_pstrFileName = in_pstrFileWithExtension->substr(0, sNum);

	return true;
}

// get file extension
bool	SGetFileExtension( const testring* in_pstrFileWithExt, testring* out_pstrFileExt )
{
	if( in_pstrFileWithExt == NULL || in_pstrFileWithExt->empty() || out_pstrFileExt == NULL )
		return false;

	size_t sNum = in_pstrFileWithExt->find_last_of( '.' );

	testring Temp = in_pstrFileWithExt->substr( sNum+1, in_pstrFileWithExt->size() );

	out_pstrFileExt->assign( Temp.c_str() );

	return true;
}

// separate file with extension from path
bool	SGetFileName(const testring* in_pstrFilePath, testring* out_pstrFileName)
{
	if (in_pstrFilePath == NULL || in_pstrFilePath->empty() || out_pstrFileName == NULL)
		return false;

	// может не быть символа
	size_t sNum = in_pstrFilePath->find_last_of('/');

	*out_pstrFileName = in_pstrFilePath->substr(sNum + 1);

	return true;
}

// folder
bool SGetFileFolder(const testring* in_pstrFullFilePath, testring* out_pstrPathFolder)
{
	if (in_pstrFullFilePath == NULL || in_pstrFullFilePath->empty() || out_pstrPathFolder == NULL)
		return false;

	size_t sNum = in_pstrFullFilePath->find_last_of('/');

	auto Temp = in_pstrFullFilePath->substr(0, sNum);
	// добавляем слеш
	Temp.append(TEStr("/"));

	out_pstrPathFolder->assign(Temp.c_str());

	return true;
}


// devide string by word
bool	DivideStringApart(const testring* c_pstrIn, std::vector<testring>* p_stdvOut, techar chSimbol)
{
	if (c_pstrIn == NULL || c_pstrIn->empty() || p_stdvOut == NULL)
		return false;

	p_stdvOut->clear();

	size_t sBegin = -1, sEnd = -1, size = 0;
	techar chCurSimbol = -127, chLastSimbol = -126;

	// temp
	testring strWork = *c_pstrIn;
	// devide symbol to the end
	strWork += chSimbol;

	size = strWork.size();

	for (size_t i = 0; i < size; i++)
	{
		chCurSimbol = strWork[i];

		// string begin
		if (i == 0 && chCurSimbol != chSimbol)
			sBegin = 0;

		// find devide symbol
		if (chCurSimbol == chSimbol)
		{
			// the begin of string
			if (sBegin == -1)
				sBegin = i + 1;
			else if (sEnd == -1)
				sEnd = i;

			// two symbols one by one
			if (chLastSimbol == chCurSimbol)
			{
				// saves spaces
				//sBegin = i + 1;
				sEnd = -1;
			}
			// new string
			else if (sBegin != -1 && sEnd != -1)
			{
				p_stdvOut->push_back(strWork.substr(sBegin, sEnd - sBegin));

				sBegin = sEnd + 1;
				sEnd = -1;
			}
		}

		// last symbol
		chLastSimbol = chCurSimbol;
	}

	return true;
}

// devide string by word
bool	DivideStringApartA(const std::string* c_pstrIn, std::vector<std::string>* p_stdvOut, char chSimbol)
{
	if (c_pstrIn == NULL || c_pstrIn->empty() || p_stdvOut == NULL)
		return false;

	// очищаем список
	p_stdvOut->clear();

	size_t sBegin = -1, sEnd = -1, size = 0;
	char chCurSimbol = -127, chLastSimbol = -126;

	std::string strWork = *c_pstrIn;
	strWork += chSimbol;

	size = strWork.size();

	for (size_t i = 0; i < size; i++)
	{
		chCurSimbol = strWork[i];

		if (i == 0 && chCurSimbol != chSimbol)
			sBegin = 0;

		if (chCurSimbol == chSimbol)
		{
			if (sBegin == -1)
				sBegin = i + 1;
			else if (sEnd == -1)
				sEnd = i;

			if (chLastSimbol == chCurSimbol)
			{
				// saves spaces
				sEnd = -1;
			}
			else if (sBegin != -1 && sEnd != -1)
			{
				p_stdvOut->push_back(strWork.substr(sBegin, sEnd - sBegin));

				sBegin = sEnd + 1;
				sEnd = -1;
			}
		}

		chLastSimbol = chCurSimbol;
	}

	return true;
}

// clear all symbols in string
bool	SClearySimbols(const testring* c_pstrIn, testring* p_strOut, techar chSimbol)
{
	if( c_pstrIn == NULL || c_pstrIn->empty() || p_strOut == NULL )
		return false;

	testring strTemp;

	size_t i = 0, size = c_pstrIn->size();

	for( i = 0; i < size; i++ )
		if( c_pstrIn->at(i) != chSimbol )
		{
			strTemp.resize( strTemp.size() + 1 );
			strTemp.back() = c_pstrIn->at(i);
		}

	*p_strOut = strTemp;

	return true;
}

// clear empty symbols at the begin and at the end of sth string
bool	SClearEmptySimbols(const testring* c_pstrIn, testring* p_strOut)
{
	if (c_pstrIn == NULL || c_pstrIn->empty() || p_strOut == NULL)
		return false;

	testring strTemp = *c_pstrIn;
	techar chCurrent = 0;

	size_t i = 0, size = strTemp.size();

	for (i = 0; i < size; i++)
	{
		chCurrent = strTemp[i];

		if (chCurrent == ' ' || chCurrent == '\n' || chCurrent == '\\' || chCurrent == '//')
		{
			strTemp.erase(strTemp.begin() + i);
			i--;
			size--;
		}
		else
			break;
	}

	for (i = size - 1; i < size; i--)
	{
		chCurrent = strTemp[i];

		if (chCurrent == ' ' || chCurrent == '\n' || chCurrent == '\\' || chCurrent == '//')
		{
			strTemp.erase(strTemp.begin() + i);
			i--;
			size--;
		}
		else
			break;
	}

	*p_strOut = strTemp;

	return true;
}



#ifdef __cplusplus
}
#endif

