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
// Modification History:
// 2015/02/11 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Sequence_Sequence_h
#define Aos_Sequence_Sequence_h

#include "Sequence/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"

class AosSequence : virtual public OmnRCObject 
{
	OmnDefineRCObject;

public:
	enum SIZE
	{
	   eDefaultStepLength = 1,
	   eDefaultInitValue = 1,
	   eDefaultCacheNum = 20,
	   eDefaultMaxValue = 0x7FFFFFFF,
	   eDefaultMinValue = 0xFFFFFFFF
	};

private:
	int		mInitValue;
	int		mStepLength;
	int		mMaxValue;
	int		mMinValue;
	int		mCacheNum;
	bool	mIsCycle;
    bool	mIsOrder;
    bool    mIsBegin;
	//bool    mOver;  //judge whether wo can get crtValue or not
	
    OmnString   mIILName;
	OmnString   mIILKey;
	
	OmnMutexPtr	mLock;
	OmnMutex *	mLockRaw;

	int			mCrtValue;
	int			mNum;	

public:
	AosSequence();
	~AosSequence();

	static AosSequencePtr	create(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &def);

    bool	getNextValue(
				const AosRundataPtr &rdata,
				int &value);

	bool	getCrtValue(
				const AosRundataPtr &rdata,
				int &value);

private:
	bool	config(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &def);
};

#endif

