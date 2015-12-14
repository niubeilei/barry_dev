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
// 2013/08/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "BitmapTrans/TransBitmapQuery.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "BitmapTrans/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/TransBitmapQueryAgentObj.h"


AosTransBitmapQuery::AosTransBitmapQuery(const bool flag)
:
AosBitmapTrans(AosTransType::eBitmapQuery, flag),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
}

AosTransBitmapQuery::AosTransBitmapQuery(
		const AosRundataPtr &rdata, 
		const int cube_id,
		const u64 term_id,
		const vector<OmnString> &iilnames,
		const vector<AosQueryRsltObjPtr> &node_list,
		const vector<AosBitmapObjPtr> &partial_bitmaps, 
		const vector<u32> &expected_sections)
:
AosBitmapTrans(AosTransType::eBitmapQuery, cube_id,true, false, true),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
	mAgent = AosTransBitmapQueryAgentObj::getAgent(
			rdata,
			term_id,
			iilnames,
			node_list,
			partial_bitmaps,
			expected_sections);

	AosTransBitmapQueryPtr thisPtr(this, false);
	mAgent->setTrans(thisPtr);
	aos_assert(mAgent);
}

AosTransBitmapQuery::~AosTransBitmapQuery()
{
}

bool 
AosTransBitmapQuery::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosBitmapTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	return mAgent->serializeTo(buff);
}


bool 
AosTransBitmapQuery::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosBitmapTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	if(!mAgent)
	{
		vector<OmnString> iilnames;
		vector<AosQueryRsltObjPtr> node_list;
		vector<AosBitmapObjPtr> partial_bitmaps; 
		vector<u32> expected_sections;
		mAgent = AosTransBitmapQueryAgentObj::getAgent(
				mRdata,
				0,
				iilnames,
				node_list,
				partial_bitmaps,
				expected_sections);
		AosTransBitmapQueryPtr thisPtr(this, false);
		mAgent->setTrans(thisPtr);
	}
	
	// 1. Retrieve 
	return mAgent->serializeFrom(buff);
}


bool 
AosTransBitmapQuery::proc()
{
	mAgent->setRundata(mRdata);
	aos_assert_r(mAgent,false);
	return mAgent->proc();
}




AosTransPtr 
AosTransBitmapQuery::clone()
{
	return OmnNew AosTransBitmapQuery(false);
}

bool
AosTransBitmapQuery::respCallBack()
{
    // Ketty 2013/07/20
    AosBuffPtr resp = getResp();
    bool svr_death = isSvrDeath();

	OmnString errmsg;
	
	u64 and_term_id = resp->getU64(0);
	u32 error_flag = resp->getU32(0);
	u64 bitmap_flag;
	AosBitmapObjPtr bitmap;
	errmsg = resp->getOmnStr("");
	bitmap_flag = resp->getU32(0);
	if(bitmap_flag)
	{
		u32 len = resp->getU32(0);
		if(len > 0)
		{
			AosBuffPtr bb = resp->getBuff(len, false AosMemoryCheckerArgs);
			bitmap = AosGetBitmap();
			bitmap->loadFromBuff(bb);
		} 
	}
	
	if (errmsg != "") 
	{
	    OmnAlarm << "Query failed: " << errmsg<< enderr;
	    return false;
	}

	if(and_term_id == eTestTermID)
	{
		mRslt = bitmap;
		mCondVar->signal();
		return true;
	}
	if(!mAgent)
	{
		vector<OmnString> iilnames;
		vector<AosQueryRsltObjPtr> node_list;
		vector<AosBitmapObjPtr> partial_bitmaps; 
		vector<u32> expected_sections;
		mAgent = AosTransBitmapQueryAgentObj::getAgent(
				mRdata,
				0,
				iilnames,
				node_list,
				partial_bitmaps,
				expected_sections);
		AosTransBitmapQueryPtr thisPtr(this, false);
		mAgent->setTrans(thisPtr);
	}
	return mAgent->procResp(mRdata, and_term_id, errmsg, bitmap);
}




