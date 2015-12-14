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
#ifndef Aos_Util_LogBag_h
#define Aos_Util_LogBag_h

#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "aosUtil/Types.h"
#include <stdio.h>
#include <string.h>

class OmnString;
class OmnMutex;

class AosLogBagEntry
{
	OmnString		mContents;
	AosLogBagPtr	mLogBag;

public:
	AosLogBagEntry()
	{
	}

	AosLogBagEntry & operator << (const char *c)
	{
		mContents << c;
		return *this;
	}

	AosLogBagEntry & operator << (const u64 &c)
	{
		mContents << c;
		return *this;
	}

	void reset() {mContents = "";}
};

class AosLogBag : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eSize = 500,
		eMaxFileSize = 10000000,
		eMaxFiles = 10
	};

	OmnString    mContents[eSize];
	int			 mCount;
	int			 mCrtIdx;
	OmnMutexPtr	 mLock;
	u32			 mSeqno;
	OmnString	 mFname;
	OmnFilePtr	 mFile;
	int			 mFileIdx;
	int			 mNumRecords;

public:
	AosLogBag(const OmnString &fname, const bool);
	~AosLogBag();

	OmnString & getEntry(const char *file, const int line);
};

#endif

