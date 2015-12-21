////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DirDesc.cpp
// Description:
//	This is the directory descriptor.    
//
// Modification History:
// Add Linux implementation by Jenny Gu
////////////////////////////////////////////////////////////////////////////
#include "Util/DirDesc.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "Debug/Except.h"
#include <boost/filesystem.hpp>

OmnDirDesc::OmnDirDesc(const OmnString &dir)
:
OmnFileObj(eDirectory)
{
	mDirName = dir;
	if ((mDirH = opendir(dir.data())) == NULL)
    {
		OmnAlarm << "Invalid dir: " << dir << enderr;
		OmnThrowException("Invalid dir");
		return;
	}
}


OmnDirDesc::~OmnDirDesc()
{
}


bool
OmnDirDesc::dirExist(const OmnString &dirname)
{
	DIR *dir = opendir(dirname.data());
	if (dir) return true;
	return false;
}

bool
OmnDirDesc::createDir(const OmnString &dirname)
{
	try
	{
		boost::filesystem::create_directories(dirname.data());
	}
	catch(...)
	{
		OmnAlarm << "Failed to create directory: " << dirname << enderr;
		return false;
	}

	return true;
}

//realpath() to get the absolute path
bool
OmnDirDesc::getFirstFile(OmnString &fullName)
{
	aos_assert_r(mDirH, false);
	struct dirent *dirp;
	dirp = readdir(mDirH);
	mCount = 0;
	while (dirp)
	{
		if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
		{
			dirp = readdir(mDirH);
			continue;
		}

		mCount++;
       	fullName = mDirName;
       	fullName << "/" << dirp->d_name;
		return true;
	}

	fullName = "";
	return false;
}

bool
OmnDirDesc::getNextFile(OmnString &fullName)
{
	struct dirent *dirp;
	mStatus = false;
	dirp = readdir(mDirH);
	while (dirp)
	{
		if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
		{
			dirp = readdir(mDirH);
			continue;
		}

        fullName = mDirName;
        fullName << "/" << dirp->d_name;
		mCount++;
		return true;
	}

	fullName = "";
	return false;
}


bool 
OmnDirDesc::close()
{
	if (mDirH)
	{
		closedir(mDirH);
	}
	return true;
}

/* Below are Microsoft implementations
#include <windows.h>
#include <string.h>
#include <stdio.h>


OmnDirDesc::OmnDirDesc(const OmnString &dir)
:
OmnFileObj(eDirectory)
{
	mDirName = dir;
	mHandleFind = INVALID_HANDLE_VALUE;
}


OmnDirDesc::~OmnDirDesc()
{
}


bool
OmnDirDesc::getFirstFile(OmnString &fullName)
{
	char searchName[255];
	OmnString dir(mDirName);
	dir << "\\*";
	strncpy(searchName, dir.getBuffer(), dir.length());
	mHandleFind = FindFirstFile (searchName, &mFindFileData);
	if (mHandleFind == INVALID_HANDLE_VALUE) 
	{
      printf ("Invalid file handle. Error is %u\n", GetLastError());
	  mCount = 0;
	  mStatus = false;
	} 
	else 
	{
	  fullName = mDirName;
	  fullName << "\\" << mFindFileData.cFileName;
	  mCount = 1;
	  mStatus = true;
	}
	return mStatus;
}


bool
OmnDirDesc::getNextFile(OmnString &fullName)
{
	mStatus = false;
	int i = FindNextFile (mHandleFind, &mFindFileData);
	if (i>0) 
	{
		mCount++;
		fullName = mDirName;
		fullName << "\\" << mFindFileData.cFileName;
		mStatus = true;
	}
	return mStatus;
}


bool 
OmnDirDesc::close()
{
	int i = FindClose (mHandleFind);
	mStatus = false;
	if (i==1) 
	{
		mStatus = true;
	}
	return mStatus;
}
*/

int
OmnDirDesc::numObjs() const
{
	return mCount;
}

