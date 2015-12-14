////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 03/27/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILUtil_IILLog_h
#define AOS_IILUtil_IILLog_h

#include "IILUtil/IILLogType.h"
#include "IILUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Buff.h"

#include <algorithm>
#include <vector>
using namespace std;

class AosIILLog : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:

	struct SORT_HIT 
	{
		bool operator ()(const AosIILLogPtr &s1, const AosIILLogPtr &s2)
		{
			return s1->mDocid < s2->mDocid;
		}
	} sort_hit;


public:

	u32			mTransid;
	u32			mIILID;
	OmnString	mValueStr;
	OmnString	mOldValueStr;
	bool		mValueUnique;
	bool		mDocidUnique;
	bool		mNeedRemov;
	bool		mNeedUpdate;
	u64			mValueU64;
	u64			mOldValueU64;
	u64			mValue2U64;
	u64			mDocid;
	u64			mDocid2;

	AosIILLogType::E mLogType;
	vector<AosIILLogPtr> mHitList;
	vector<AosIILLogPtr> mStrList;
	vector<AosIILLogPtr> mU64List;

	OmnString	mType;

public:

	AosIILLog();
	~AosIILLog();
	bool 	parse(AosBuff &buff);
	bool	addLog(OmnString type);
	bool	sort();
	bool	exec();
};

#endif

