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
#ifndef AosSengTorUtil_StIIL_h
#define AosSengTorUtil_StIIL_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "SengTorUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"


class AosStIIL : virtual public OmnRCObject
{
protected:
	OmnString		mIILName;
	u64				mIILID;
	bool			mKeyUnique;
	bool			mDocidUnique;

public:
	AosStIIL(const OmnString &iilname, 
				const u64 iilid,
				const bool key_unique,
				const bool docid_unique);
	~AosStIIL();

	virtual bool pickEntry(
						std::string &key,
						u64 &docid,
						bool &key_unique,
						bool &docid_unique, 
						bool &added,
						AosRundata *rdata)
	{
		OmnShouldNeverComeHere;
		return false;
	}

	virtual bool pickEntry(
						u64 &key,
						u64 &docid,
						bool &key_unique,
						bool &docid_unique,
						bool &added,
						AosRundata *rdata)
	{
		OmnShouldNeverComeHere;
		return false;
	}

	virtual i64 getIILSize() = 0;
	virtual bool isStrIIL() const {return false;}
	OmnString &getIILName(){return mIILName;}
	u64 	  getIILID(){return mIILID;}
	virtual bool nextValue(i64 &idx, OmnString &key, u64 &docid) = 0;
	virtual bool getKeyUnique() {return mKeyUnique;}
	virtual bool getDocidUnique() {return mDocidUnique;}
};
#endif

