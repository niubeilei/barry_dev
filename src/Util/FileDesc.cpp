////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FileDesc.cpp
// Description:
//	This is the file descriptor.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util/FileDesc.h"

#include <sys/stat.h>
#include "Debug/Rslt.h"
#include "Porting/GetErrno.h"
#include "Util/SrchStr.h"

#ifdef OMN_PLATFORM_UNIX
OmnFileDesc::OmnFileDesc(const OmnString &fullname)
:
OmnFileObj(eFile),
mUserOwner(0),
mGroupOwner(0)
{
/* Not implemented yet
	char buffer[1000];
	if (fullname.getBuffer()[0] != '/') 
	{
		// It is a relative path
		getcwd(buffer,1000);
        OmnString tmp(buffer);
        tmp << "/" << fullname;
		mFullName = OmnFileDesc::removeDotDot(tmp);
	}
	else {
		mFullName = OmnFileDesc::removeDotDot(fullname);
	}

	struct stat fileStat;
	if (stat(mFullName, &fileStat) == 0)
	{
		mFileLength = fileStat.st_size;
		mLastAccessTime = fileStat.st_atime;
		mLastWriteTime = fileStat.st_mtime;

		mUserOwner = fileStat.st_uid;
		mGroupOwner = fileStat.st_gid;
		mExist = true;
	}
	else {
		mExist = false;
	}
	mFileName = OmnFileDesc::getFileName(mFullName);
*/
}


OmnRslt	
OmnFileDesc::getAbsoluteName(const OmnString &orig, 
							OmnString &absPath, 
							OmnString &filename,
							int64_t &filesize,
							bool &exist)
{
	char buffer[1000];
	OmnString fullname;
	if (orig.getBuffer()[0] != '/') 
	{
		// It is a relative path
		getcwd(buffer,1000);
        OmnString tmp(buffer);
        tmp << "/" << orig;
		fullname = OmnFileDesc::removeDotDot(tmp);
	}
	else {
		fullname = OmnFileDesc::removeDotDot(orig);
	}

	struct stat fileStat;
	int mode = 0;
	if (stat(fullname.data(), &fileStat) == 0)
	{
		filesize = fileStat.st_size;
		mode = fileStat.st_mode;
		exist = true;
	}
	else {
		exist = false;
	}

	//
	// Check whether it is the directory
	//
	if ( S_ISDIR(mode) )
	{ // It is a directory
			absPath = fullname;
			filename = ".";
			filesize = 0;
	}
	else 
	{ // It is a file
		filename = OmnFileDesc::getFileName(fullname);
		int aLen = fullname.length();
		int len = filename.length();
		OmnString path(fullname);
		if (len > 0)
		{
			path.remove(aLen-len-1,len+1);
		}
		absPath = path;
	}

	return true;
}

#else

#include <windows.h>
#include <string.h>
#include <stdio.h>

OmnFileDesc::OmnFileDesc(const OmnString &fullname)
:
OmnFileObj(eFile),
mFileAttributes(0)
{
	// 
	// This constructor creates a file descriptor based on the full name
	// passed in. If not found, it shall throw an exception. The path 
	// can be either an absolute path or a relative path.
	//
	// Note that on Microsoft, path is separated by '\', while on Unix
	// systems, path is separated by '/'. We assume that path does not use
	// '\' and '/' for other than separating directories.
	//
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	hFind = FindFirstFile(fullname, &findFileData);

	if (hFind == INVALID_HANDLE_VALUE) 
	{
		OmnAlarm << OmnErrId::eFileError 
			<< ": Invalid file handle. Error is: " 
			<< OmnGetErrno() << ". Filename: " << fullname << enderr;
		mExist = false;
	} 
	else 
	{
		mFullName = fullname;
		set(findFileData);
		FindClose(hFind);
		mExist = true;
   }
}


void
OmnFileDesc::set(WIN32_FIND_DATA findFileData) 
{
		mFileName = findFileData.cFileName;
		mFileAttributes = findFileData.dwFileAttributes;

		SYSTEMTIME stUTC,stLocal;
		char tmpStr[15];
		FileTimeToSystemTime(&findFileData.ftCreationTime, &stUTC);
		SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
		sprintf(tmpStr, "%4d%02d%02d%02d%02d%02d",
				stLocal.wYear, stLocal.wMonth, stLocal.wDay, 
				stLocal.wHour, stLocal.wMinute, stLocal.wSecond);
		mCreationTime = tmpStr;

		FileTimeToSystemTime(&findFileData.ftLastAccessTime, &stUTC);
		SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
		sprintf(tmpStr, "%4d%02d%02d%02d%02d%02d",
				stLocal.wYear, stLocal.wMonth, stLocal.wDay, 
				stLocal.wHour, stLocal.wMinute, stLocal.wSecond);
		mLastAccessTime = tmpStr;

		FileTimeToSystemTime(&findFileData.ftLastWriteTime, &stUTC);
		SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
		sprintf(tmpStr, "%4d%02d%02d%02d%02d%02d",
				stLocal.wYear, stLocal.wMonth, stLocal.wDay, 
				stLocal.wHour, stLocal.wMinute, stLocal.wSecond);
		mLastWriteTime = tmpStr;

		// 
		// Chen Ding, 09/08/2005
		//
		// mFileLength = OmnLL(findFileData.nFileSizeHigh, 
		mFileLength = findFileData.nFileSizeHigh;
		mFileLength = (mFileLength << 32);
		mFileLength += findFileData.nFileSizeLow;
}

/*
Attribute 	Meaning
FILE_ATTRIBUTE_ARCHIVE 	    The file or directory is an archive file or directory. Applications use this attribute to mark files for backup or removal.
FILE_ATTRIBUTE_COMPRESSED 	The file or directory is compressed. For a file, this means that all of the data in the file is compressed. For a directory, this means that compression is the default for newly created files and subdirectories.
FILE_ATTRIBUTE_DEVICE 	    Reserved; do not use.
FILE_ATTRIBUTE_DIRECTORY 	The handle identifies a directory.
FILE_ATTRIBUTE_ENCRYPTED 	The file or directory is encrypted. For a file, this means that all data streams in the file are encrypted. For a directory, this means that encryption is the default for newly created files and subdirectories.
FILE_ATTRIBUTE_HIDDEN 	    The file or directory is hidden. It is not included in an ordinary directory listing.
FILE_ATTRIBUTE_NORMAL 	    The file or directory has no other attributes set. This attribute is valid only if used alone.
FILE_ATTRIBUTE_NOT_CONTENT_INDEXED 	The file will not be indexed by the content indexing service.
FILE_ATTRIBUTE_OFFLINE 	    The data of the file is not immediately available. This attribute indicates that the file data has been physically moved to offline storage. This attribute is used by Remote Storage, the hierarchical storage management software. Applications should not arbitrarily change this attribute.
FILE_ATTRIBUTE_READONLY 	The file or directory is read-only. Applications can read the file but cannot write to it or delete it. In the case of a directory, applications cannot delete it.
FILE_ATTRIBUTE_REPARSE_POINT 	The file or directory has an associated reparse point.
FILE_ATTRIBUTE_SPARSE_FILE 	The file is a sparse file.
FILE_ATTRIBUTE_SYSTEM 	    The file or directory is part of, or is used exclusively by, the operating system.
FILE_ATTRIBUTE_TEMPORARY 	The file is being used for temporary storage. File systems avoid writing data back to mass storage if sufficient cache memory is available, because often the application deletes the temporary file shortly after the handle is closed. In that case, the system can entirely avoid writing the data. Otherwise, the data will be written after the handle is closed.
*/
/*
// GetLastWriteTime - Retrieves the last-write time and converts the
//                   time to a string
// Return value - TRUE if successful, FALSE otherwise
// hFile      - Valid file handle
// lpszString - Pointer to buffer to receive string

BOOL GetLastWriteTime(HANDLE hFile, LPTSTR lpszString)
{
    FILETIME ftCreate, ftAccess, ftWrite;
    SYSTEMTIME stUTC, stLocal;

    // Retrieve the file times for the file.
    if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
        return FALSE;

    // Convert the last-write time to local time.
    FileTimeToSystemTime(&ftWrite, &stUTC);
    SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

    // Build a string showing the date and time.
    wsprintf(lpszString, TEXT("%02d/%02d/%d  %02d:%02d"),
        stLocal.wMonth, stLocal.wDay, stLocal.wYear,
        stLocal.wHour, stLocal.wMinute);

    return TRUE;
}
*/
/*
// SetFileToCurrentTime - sets last write time to current system time
// Return value - TRUE if successful, FALSE otherwise
// hFile  - must be a valid file handle

BOOL SetFileToCurrentTime(HANDLE hFile)
{
    FILETIME ft;
    SYSTEMTIME st;
    BOOL f;

    GetSystemTime(&st);              // gets current time
    SystemTimeToFileTime(&st, &ft);  // converts to file time format
    f = SetFileTime(hFile,           // sets last-write time for file
        (LPFILETIME) NULL, (LPFILETIME) NULL, &ft);

    return f;
}
*/



OmnRslt	
OmnFileDesc::getAbsoluteName(const OmnString &orig, 
							OmnString &absPath, 
							OmnString &filename,
							int64_t &filesize,
							bool &exist)
{
	LPTSTR *lpFilePart = 0;
	char buf[1000];
	GetFullPathName(orig,1000,buf,lpFilePart);

	WIN32_FIND_DATA findFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	hFind = FindFirstFile(buf, &findFileData);

	if (hFind == INVALID_HANDLE_VALUE) 
	{
		OmnAlarm << OmnErrId::eFileError 
			<< "Invalid file handle." << enderr;
		absPath = buf;
		filename="";
		exist = false;
	} 
	else 
	{
		//check whether it is the directory
		if ( findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{ // It is a directory
			absPath = buf;
			filename = ".";
			filesize = 0;
		}
		else { // It is a file
			OmnString path(buf);
			//
			//	Get the file size
			//
			struct stat fileSizeVal;
			if (stat(path, &fileSizeVal) == 0)
			{
				filesize = fileSizeVal.st_size;
			}

			filename = findFileData.cFileName;
			int aLen = path.length();
			int len = filename.length();
			if (len > 0)
			{
				path.remove(aLen-len-1,len+1);
			}
			absPath = path;

		}
        FindClose(hFind);
	    exist = true;
	}

	absPath.chgBackSlashes();
	return true;
}
#endif

OmnFileDesc::~OmnFileDesc()
{
}

OmnString
OmnFileDesc::toString() const
{
	OmnString str;
	str << "Class OmnFileDesc:"
		<< "\n-----------------"
		<< "\nFullName:       " << mFullName
		<< "\nLastAccessTime: " << mLastAccessTime
		<< "\nLastWriteTime:  " << mLastWriteTime
		<< "\nFileSize:       " << mFileLength
		<< "\nFileName:       " << mFileName;
#ifdef OMN_PLATFORM_UNIX
	str	<< "\nUserOwner:	  " << mUserOwner
		<< "\nGroupOwner:	  " << mGroupOwner;
#else
	str	<< "\nFileAttributes: " << mFileAttributes
		<< "\nCreationTime:   " << mCreationTime;
#endif


	return str;
}


OmnString		
OmnFileDesc::getPath() const
{
	int aLen = mFullName.length();
	int len = mFileName.length();
	OmnString path(mFullName);
	if (len > 0)
	{
		path.remove(aLen-len-1,len+1);
	}

	return path;
}


OmnString		
OmnFileDesc::getName() 
{
	if (mFileName.length() == 0)
	{
		mFileName = getFileName(mFullName);
	}
	return mFileName;
}


OmnString		
OmnFileDesc::getFileName(OmnString fullname)
{
	OmnSrchStr fName(fullname);
	fName.setSrchChar('\\') ;
	fName.setSrchChar('/') ;
	fName.resetLoop();
	OmnString path = fName.trimPrev();
	int len = path.length() + 1;
	OmnString filename = fullname;
	filename.remove(0,len);
	return filename;
}


OmnString
OmnFileDesc::removeDotDot(OmnString fullname)
{
	OmnSrchStr path(fullname);
	path.resetLoop();
	path.setSrchChar('\\');
	path.setSrchChar('/');
	OmnString nextToken, newPath;

	while (path.hasMore())
	{
		nextToken = path.nextToken();
		if (nextToken == "..")
		{
			path.removeToken();
			path.removeToken();
		}
		else if (nextToken == ".")
		{
			path.removeToken();
		}
	}
	newPath = path.crtValue();
	return newPath;
}

