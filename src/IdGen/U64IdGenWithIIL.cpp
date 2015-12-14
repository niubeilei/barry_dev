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
// 02/10/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IdGen/U64IdGenWithIIL.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEModules/SiteMgr.h"
#include "XmlUtil/XmlTag.h"



AosU64IdGenWithIIL::AosU64IdGenWithIIL(
		const AosXmlTagPtr &def,
		const OmnString &tagname,
		const OmnString &idName,
		const u64 &iilid)
:
mIdName(idName),
mIILID(iilid),
mInitValue(0),
mBlockSize(1),
mStartId(0),
mEndId(0)
{
	AosXmlTagPtr tag;
	if (!def || !(tag = def->getFirstChild(tagname)))
	{
		OmnThrowException("Missing definition");
		return;
	}

	int64_t vv = tag->getAttrU64("init_value", -1);
	if (vv < 0) 
	{
		OmnThrowException("Invalid init value");
		return;
	}

	mInitValue = (u64)vv;

	int bs = tag->getAttrInt("block_size", 1);
	if (bs <= 0) bs = 1;
	mBlockSize = bs;
}


AosU64IdGenWithIIL::~AosU64IdGenWithIIL()
{
}


u64 
AosU64IdGenWithIIL::nextId(const u32 siteid, const AosRundataPtr &rdata)
{
	if (mStartId >= mEndId)
	{
		AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
		aos_assert_rr(iilclient, rdata, 0);
		int num = 10;
		while(num--)
		{
			mStartId = iilclient->nextId1(siteid, mIILID, 
					mInitValue, mIdName, mBlockSize, rdata);
			if (mStartId < mInitValue)
			{
				AosDbSetStrValueDocUniqueToTable(mIILID, false,
					mIdName, mInitValue, false, rdata);
			}
			else
			{
				break;
			}
		}
		mEndId = mStartId + mBlockSize;
	}
	u64 id = mStartId++;
	return id;
}


u64
AosU64IdGenWithIIL::nextId()
{
	if (mStartId >= mEndId)
	{
		AosRundataPtr rdata = OmnApp::getRundata();
		AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
		aos_assert_rr(iilclient, rdata, 0);
		// Chen Ding, 2013/02/11
		// u32 siteid = AosSiteMgr::getDftSiteid();
		u32 siteid = AosGetDftSiteId();
		mStartId = iilclient->nextId1(
				siteid, mIILID, mInitValue, mIdName, mBlockSize, rdata);
		mEndId = mStartId + mBlockSize;
	}
	u64 id = mStartId++;
	return id;
}

