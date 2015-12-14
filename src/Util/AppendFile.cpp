////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// An 'AppendFile' is a special file that supports append only. 
//   
//
// Modification History:
// 02/19/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/AppendFile.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "Porting/FileOpenErr.h"
#include "Porting/GetErrno.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "util_c/strutil.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


AosAppendFile::AosAppendFile(const OmnString &name)
:
mName(name),
mCrtPos(-1),
mFile(0)
{
	aos_assert(mName != "");
	mFile = ::fopen64(mName.data(), "r+b");
	if (!mFile)
	{
		mFile = ::fopen64(mName.data(), "w+b");
	}

	if (!mFile) return;
	getCrtPos();
}


AosAppendFile::AosAppendFile(const OmnString &name, const bool enforce_create_new)
:
mName(name),
mCrtPos(-1),
mFile(0)
{
	aos_assert(mName != "");
	mFile = ::fopen64(mName.data(), "r+b");
	if (!mFile)
	{
		mFile = ::fopen64(mName.data(), "w+b");
	}
	else if(enforce_create_new)
	{
		mFile = 0;
		int rsltInt = unlink(name.data());
		if(rsltInt != 0) return;
		mFile = ::fopen64(mName.data(), "w+b");
	}

	if (!mFile) return;
	getCrtPos();
}


AosAppendFile::~AosAppendFile()
{
	if (mFile)
	{
		::fclose(mFile);
		mFile = 0;
	}
}


bool
AosAppendFile::closeFile()
{
	aos_assert_r(mFile, false);
	::fclose(mFile);
	mFile = 0;
	return true;
}


int64_t
AosAppendFile::append(const char *buff, const int len, const bool toFlush)
{
	// It appends the contents 'buff' to the file. If failed, it returns -1.
	// Otherwise, it returns the position at which 'buff' was appended.
	aos_assert_r(mFile, -1);
	if (len <= 0)
	{
		return -1;
	}

	int asize = ::fwrite(buff, 1, len, mFile);
	if (asize != len)
	{
		OmnAlarm << "Failed appending: " << feof(mFile) << ":" << ferror(mFile) << enderr;
		mCrtPos = getCrtPos();
		return -1;
	}

	if (toFlush)
	{
		OmnFlushFile(mFile);
	}
	
	int64_t pos = mCrtPos;
	mCrtPos += len;

	return pos;
}

