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
//   
//
// Modification History:
// 02/19/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Snt_Util_AppendFile_h
#define Snt_Util_AppendFile_h

#include "alarm_c/alarm.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosAppendFile : public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnString	mName;
	int64_t		mCrtPos;
	FILE *		mFile;

public:
	AosAppendFile(const OmnString &name);
	AosAppendFile(const OmnString &name, const bool enforce_create_new);
	virtual ~AosAppendFile();

	bool		closeFile();
	bool		isGood() const {return mFile != 0;}
	OmnString	getFileName() const {return mName;}

	AosAppendFile &	operator << (const char *str)
	{
		aos_assert_r(str, *this);
		append(str, strlen(str), false);
		return *this;
	}
	AosAppendFile &	operator << (const OmnString &str)
	{
		append(str.data(), str.length(), false);
		return *this;
	}

	int64_t 	append(const char *buff, const int len, const bool flush = false);
	int64_t		append(const OmnString &buff, const bool flush = false)
	{
		return append(buff.data(), buff.length(), flush);
	}
	int64_t 	appendWithLen(const OmnString &buff, const bool flush = false)
	{
		OmnString len;
		len << buff.length() << "\n";
		int64_t pos = append(len, false);
		append(buff, flush);
		return pos;
	}

	int64_t getCrtPos() 
	{
		aos_assert_r(mFile, -1);
		::fseeko64(mFile, 0, SEEK_END);
		mCrtPos = ::ftello64(mFile);
		return mCrtPos;
	}

	int64_t getLength()
	{
		aos_assert_r(mFile, -1);
		::fseeko64(mFile, 0, SEEK_END);
		return ftello64(mFile);
	}

	bool gotoEnd()
	{
		aos_assert_r(mFile, false);
		::fseeko64(mFile, 0, SEEK_END);
		return true;
	}
};
#endif

