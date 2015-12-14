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
// 08/31/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Util_LogFile_h
#define Aos_Util_LogFile_h

#include "aosUtil/Types.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <ostream>
using namespace std;

class OmnMutex;


class AosLogFile
{
private:
	FILE *		mFile;
	OmnMutex*	mLock;
	bool		mFlag;

public:
	AosLogFile(const char *fname, const bool flag);

	AosLogFile & operator << (const char *c)
	{
		if (!mFlag) return *this;
		int len = strlen(c);
		::fwrite(c, 1, len, mFile);
		fflush(mFile);
		return *this;
	}

	AosLogFile& operator << (const u64 &c)
	{
		if (!mFlag) return *this;
		char buff[50];
		sprintf(buff, "%llx", (unsigned long long int)c);
		int len = strlen(buff);
		::fwrite(buff, 1, len, mFile);
		fflush(mFile);
		return *this;
	}

	AosLogFile & operator << (ostream & (*f)(ostream &outs))
	{
		if (!mFlag) return *this;
		::fwrite("\n", 1, 1, mFile);
		return *this;
	}
};
#endif

