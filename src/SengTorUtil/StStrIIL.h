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
// 2014/10/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSengTorUtil_StStrIIL_h
#define AosSengTorUtil_StStrIIL_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SengTorUtil/Ptrs.h"
#include "SengTorUtil/StIIL.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/BuffArrayVar.h"


class AosStStrIIL : public AosStIIL
{
	OmnDefineRCObject;

private:
	int					mMinLen;
	int					mMaxLen;
	//int					mIdx;
	AosBuffArrayVarPtr	mCacheKey;
	AosBuffArrayVarPtr	mCacheDocid;

	enum
	{
		eDftMinLen = 5,
		eDftMaxLen = 5 
	};

public:
	AosStStrIIL(const OmnString &name,
				const u64 iilid,
				const bool key_unique,
				const bool docid_unique,
				const bool stable);
	~AosStStrIIL();

	virtual bool pickEntry(
						std::string &key,
						u64 &docid,
						bool &key_unique,
						bool &docid_unique, 
						bool &added,
						AosRundata *rdata);

	virtual bool isStrIIL() const {return true;}
	virtual i64 getIILSize();
	virtual bool nextValue(i64 &idx, OmnString &key, u64 &docid);
private:
};
#endif

