/**
 *	@file	dosio.cpp
 *	@brief	define file access behavior
 */

#include "compiler.h"
#include "dosio.h"

//! Current path buffer
static OEMCHAR curpath[MAX_PATH];

//! File name pointer
static OEMCHAR *curfilep = curpath;

/ **
 * Initialization
 * /
void dosio_init(void)
{
}

/ **
 * Release
 * /
void dosio_term(void)
{
}

/ **
 * Open the file
 * @ Param [in] lpPathName file name
 * @return filehandle
 * /
FILEH DOSIOCALL file_open(const OEMCHAR* lpPathName)
{
	FILEH hFile = ::CreateFile(lpPathName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hFile = ::CreateFile(lpPathName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	return hFile;
}

/ **
 * Open the file with read-only
 * @ Param [in] lpPathName file name
 * @return filehandle
 * /
FILEH DOSIOCALL file_open_rb(const OEMCHAR* lpPathName)
{
	return ::CreateFile(lpPathName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}

/ **
 * Create a file
 * @ Param [in] lpPathName file name
 * @return filehandle
 * /
FILEH DOSIOCALL file_create(const OEMCHAR* lpPathName)
{
	return ::CreateFile(lpPathName, GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

/ **
 * Seek file
 * @ Param [in] hFile File handle
 * @ Param [in] pointer Number of bytes to move
 * @ Param [in] method starting point
 * @return file location
 * /
long DOSIOCALL file_seek(FILEH hFile, long pointer, int method)
{
	return static_cast<long>(::SetFilePointer(hFile, pointer, 0, method));
}

/ **
 * File reading
 * @ Param [in] hFile File handle
 * @ Param [out] lpBuffer buffer
 * @ Param [in] cbBuffer buffer size
 * @return read size
 * /
UINT DOSIOCALL file_read(FILEH hFile, void* lpBuffer, UINT cbBuffer)
{
	DWORD dwReadSize;
	if (::ReadFile(hFile, lpBuffer, cbBuffer, &dwReadSize, NULL))
	{
		return dwReadSize;
	}
	return 0;
}

/ **
 * Write file
 * @ Param [in] hFile File handle
 * @ Param [in] lpBuffer buffer
 * @ Param [in] cbBuffer buffer size
 * @return write size
 * /
UINT DOSIOCALL file_write(FILEH hFile, const void* lpBuffer, UINT cbBuffer)
{
	if (cbBuffer != 0)
	{
		DWORD dwWrittenSize;
		if (::WriteFile(hFile, lpBuffer, cbBuffer, &dwWrittenSize, NULL))
		{
			return dwWrittenSize;
		}
	}
	else
	{
		::SetEndOfFile(hFile);
	}
	return 0;
}

/ **
 * Close the file handle
 * @ Param [in] hFile File handle
 * @retval 0 Success
 * /
short DOSIOCALL file_close(FILEH hFile)
{
	::CloseHandle(hFile);
	return 0;
}

/ **
 * Get file size
 * @ Param [in] hFile File handle
 * @return file size
 * /
UINT DOSIOCALL file_getsize(FILEH hFile)
{
	return ::GetFileSize(hFile, NULL);
}

/ **
 * Convert FILETIME to DOSDATE / DOSTIME
 * @ Param [in] ft file time
 * @ Param [out] dosdate DOSDATE
 * @ Param [out] dostime DOSTIME
 * @retval true success
 * @retval false Failure
 * /
static bool convertDateTime(const FILETIME& ft, DOSDATE* dosdate, DOSTIME* dostime)
{
	FILETIME ftLocalTime;
	if (!::FileTimeToLocalFileTime(&ft, &ftLocalTime))
	{
		return false;
	}

	SYSTEMTIME st;
	if (!::FileTimeToSystemTime(&ftLocalTime, &st))
	{
		return false;
	}

	if (dosdate)
	{
		dosdate->year = st.wYear;
		dosdate->month = static_cast<UINT8>(st.wMonth);
		dosdate->day = static_cast<UINT8>(st.wDay);
	}
	if (dostime)
	{
		dostime->hour = static_cast<UINT8>(st.wHour);
		dostime->minute = static_cast<UINT8>(st.wMinute);
		dostime->second = static_cast<UINT8>(st.wSecond);
	}
	return true;
}
* Obtain timestamp of file
 * @ Param [in] hFile File handle
 * @ Param [out] dosdate DOSDATE
 * @ Param [out] dostime DOSTIME
 * @retval 0 Success
 * @retval -1 Failure
 * /
short DOSIOCALL file_getdatetime(FILEH hFile, DOSDATE* dosdate, DOSTIME* dostime)
{
	FILETIME ft;
	if (!::GetFileTime(hFile, NULL, NULL, &ft))
	{
		return -1;
	}
	return (convertDateTime(ft, dosdate, dostime)) ? 0 : -1;
}

/ **
 * Delete files
 * @ Param [in] lpPathName file name
 * @retval 0 Success
 * @retval -1 Failure
 * /
short DOSIOCALL file_delete(const OEMCHAR* lpPathName)
{
	return (::DeleteFile(lpPathName)) ? 0 : -1;
}

/ **
 * Get attribute of file
 * @ Param [in] lpPathName file name
 * @return file attribute
 * /
short DOSIOCALL file_attr(const OEMCHAR* lpPathName)
{
	return static_cast<short>(::GetFileAttributes(lpPathName));
}

/ **
 * Directory creation
 * @ Param [in] lpPathName path
 * @retval 0 Success
 * @retval -1 Failure
 * /
short DOSIOCALL file_dircreate(const OEMCHAR* lpPathName)
{
	return (::CreateDirectory(lpPathName, NULL)) ? 0 : -1;
}



// ---- Current file operation

/ **
 * Current path setting
 * @ Param [in] lpPathName Current file name
 * /
void DOSIOCALL file_setcd(const OEMCHAR* lpPathName) {

	file_cpyname(curpath, lpPathName, NELEMENTS(curpath));
	curfilep = file_getname(curpath);
	*curfilep = '\0';
}

/ **
 * Acquire the current path
 * @ Param [in] lpFilename file name
 * @return pass
 * /
OEMCHAR* DOSIOCALL file_getcd(const OEMCHAR* lpFilename)
{
	file_cpyname(curfilep, lpFilename, NELEMENTS(curpath) - (int)(curfilep - curpath));
	return curpath;
}

/ **
 * Open the current file
 * @ Param [in] lpFilename file name
 * @return filehandle
 * /
FILEH DOSIOCALL file_open_c(const OEMCHAR* lpFilename)
{
	return file_open(file_getcd(lpFilename));
}

/ **
 * Open the current file with read only
 * @ Param [in] lpFilename file name
 * @return filehandle
 * /

FILEH DOSIOCALL file_open_rb_c(const OEMCHAR* lpFilename)
{
	return file_open_rb(file_getcd(lpFilename));
}

/ **
 * Create the current file
 * @ Param [in] lpFilename file name
 * @return filehandle
 * /
FILEH DOSIOCALL file_create_c(const OEMCHAR* lpFilename)
{
	return file_create(file_getcd(lpFilename));
}

 **
 * Delete the current file
 * @ Param [in] lpFilename file name
 * @retval 0 Success
 * @retval -1 Failure
 * /
short DOSIOCALL file_delete_c(const OEMCHAR* lpFilename)
{
	return file_delete(file_getcd(lpFilename));
}

/ **
 * Get the attributes of the current file
 * @ Param [in] lpFilename file name
 * @return file attribute
 * /
short DOSIOCALL file_attr_c(const OEMCHAR* lpFilename)
{
	return file_attr(file_getcd(lpFilename));
}



// ---- File search

/ **
 * Convert WIN32_FIND_DATA to FLINFO
 * @ Param [in] w32fd WIN32_FIND_DATA
 * @ Param [out] fli FLINFO
 * @retval true success
 * @retval false Failure
 * /
static bool DOSIOCALL setFLInfo(const WIN32_FIND_DATA& w32fd, FLINFO *fli)
{
#if !defined(_WIN32_WCE)
	if ((w32fd.dwFileAttributes & FILEATTR_DIRECTORY) && (w32fd.cFileName[0] == '.'))
	{
		return false;
	}
#endif	// !defined(_WIN32_WCE)

	if (fli)
	{
		fli->caps = FLICAPS_SIZE | FLICAPS_ATTR | FLICAPS_DATE | FLICAPS_TIME;
		fli->size = w32fd.nFileSizeLow;
		fli->attr = w32fd.dwFileAttributes;
		convertDateTime(w32fd.ftLastWriteTime, &fli->date, &fli->time);
		file_cpyname(fli->path, w32fd.cFileName, NELEMENTS(fli->path));
	}
	return true;
}

/ **
 * Search for files
 * @ Param [in] lpPathName path
 * @ Param [out] fli Search result
 * @return file search handle
 * /
FLISTH DOSIOCALL file_list1st(const OEMCHAR* lpPathName, FLINFO* fli)
{
	static const OEMCHAR s_szWildCard[] = OEMTEXT("*.*");

	OEMCHAR szPath[MAX_PATH];
	file_cpyname(szPath, lpPathName, NELEMENTS(szPath));
	file_setseparator(szPath, NELEMENTS(szPath));
	file_catname(szPath, s_szWildCard, NELEMENTS(szPath));

	WIN32_FIND_DATA w32fd;
	HANDLE hFile = ::FindFirstFile(szPath, &w32fd);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (setFLInfo(w32fd, fli))
			{
				return hFile;
			}
		} while(::FindNextFile(hFile, &w32fd));
		::FindClose(hFile);
	}
	return FLISTH_INVALID;
}

/ **
 * Search for files
 * @ Param [in] hList file search handle
 * @ Param [out] fli Search result
 * @retval SUCCESS success
 * @retval FAILURE failure
 * /
BRESULT DOSIOCALL file_listnext(FLISTH hList, FLINFO* fli)
{
	WIN32_FIND_DATA w32fd;
	while (::FindNextFile(hList, &w32fd))
	{
		if (setFLInfo(w32fd, fli))
		{
			return SUCCESS;
		}
	}
	return FAILURE;
}

/ **
 * Close the file search handle
 * @ Param [in] hList file search handle
 * /
void DOSIOCALL file_listclose(FLISTH hList)
{
	::FindClose(hList);
}



// ---- File name operation

/ **
 * Get pointer to file name
 * @ Param [in] lpPathName path
 * @return pointer
 * /
OEMCHAR* DOSIOCALL file_getname(const OEMCHAR* lpPathName)
{
	const OEMCHAR* ret = lpPathName;
	while (1 /* EVER */)
	{
		const int cch = milstr_charsize(lpPathName);
		if (cch == 0)
		{
			break;
		}
		else if ((cch == 1) && ((*lpPathName == '\\') || (*lpPathName == '/') || (*lpPathName == ':')))
		{
			ret = lpPathName + 1;
		}
		lpPathName += cch;
	}
	return const_cast<OEMCHAR*>(ret);
}

/ **
 * Delete file name
 * @ Param [in, out] lpPathName path
 * /
void DOSIOCALL file_cutname(OEMCHAR* lpPathName)
{
	OEMCHAR* p = file_getname(lpPathName);
	p[0] = '\0';
}

/ **
 * Get pointer to extension
 * @ Param [in] lpPathName path
 * @return pointer
 * /
OEMCHAR* DOSIOCALL file_getext(const OEMCHAR* lpPathName)
{
	const OEMCHAR* p = file_getname(lpPathName);
	const OEMCHAR* q = NULL;
	while (1 /* EVER */)
	{
		const int cch = milstr_charsize(p);
		if (cch == 0)
		{
			break;
		}
		else if ((cch == 1) && (*p == '.'))
		{
			q = p + 1;
		}
		p += cch;
	}
	if (q == NULL)
	{
		q = p;
	}
	return const_cast<OEMCHAR*>(q);
}

/ **
 * Remove extension
 * @ Param [in, out] lpPathName path
 * /
void DOSIOCALL file_cutext(OEMCHAR* lpPathName)
{
	OEMCHAR* p = file_getname(lpPathName);
	OEMCHAR* q = NULL;
	while (1 /* EVER */)
	{
		const int cch = milstr_charsize(p);
		if (cch == 0)
		{
			break;
		}
		else if ((cch == 1) && (*p == '.'))
		{
			q = p;
		}
		p += cch;
	}
	if (q)
	{
		*q = '\0';
	}
}

/ **
 * Remove path separator
 * @ Param [in, out] lpPathName path
 * /
void DOSIOCALL file_cutseparator(OEMCHAR* lpPathName)
{
	const int pos = OEMSTRLEN(lpPathName) - 1;
	if ((pos > 0) &&								// It must be two characters or more.
		(lpPathName[pos] == '\\') &&				// ケツが \ でー 漢字の2バイト目ぢゃなくてー
		(!milstr_kanji2nd(lpPathName, pos)) &&		// 漢字の2バイト目ぢゃなくてー
		((pos != 1) || (lpPathName[0] != '\\')) &&	// '\\' ではなくてー
		((pos != 2) || (lpPathName[1] != ':')))		// '?:\' ではなかったら
	{
		lpPathName[pos] = '\0';
	}
}

/ **
 * Add path separator
 * @ Param [in, out] lpPathName path
 * @ Param [in] cchPathName buffer length
 * /
void DOSIOCALL file_setseparator(OEMCHAR* lpPathName, int cchPathName)
{
	const int pos = OEMSTRLEN(lpPathName) - 1;
	if ((pos < 0) ||
		((pos == 1) && (lpPathName[1] == ':')) ||
		((lpPathName[pos] == '\\') && (!milstr_kanji2nd(lpPathName, pos))) ||
		((pos + 2) >= cchPathName))
	{
		return;
	}
	lpPathName[pos + 1] = '\\';
	lpPathName[pos + 2] = '\0';
}
