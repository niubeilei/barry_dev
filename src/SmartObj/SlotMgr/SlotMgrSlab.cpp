////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 12/18/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartObj/SlotMgr/SlotMgrSlab.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SmartObj/SlotMgr/SobjSlotMgr.h"
#include "Util/StrSplit.h"
#include "XmlUtil/XmlTag.h"


AosSlotMgrSlab::AosSlotMgrSlab(
		const int idx,
		const AosSobjSlotMgrPtr &mgr, 
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
mMgr(mgr),
mMinSize(-1), 
mMaxSize(-1),
mNextTries(-1),
mIndex(idx)
{
	bool rslt = init(def, rdata);
	if (!rslt)
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


AosSlotMgrSlab::~AosSlotMgrSlab()
{
}


bool
AosSlotMgrSlab::init(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	//	<def AOSTAG_MIN_SIZE="xxx"
	//		 AOSTAG_MAX_SIZE="xxx"
	//		 AOSTAG_NEXT_TRIES="xxx">
	//		 start,size,...
	//	</def>
	mStarts.clear();
	mSizes.clear();
	aos_assert_rr(def, rdata, false);
	mMinSize = def->getAttrInt64(AOSTAG_MIN_SIZE, -1);
	if (mMinSize < 0)
	{
		AosSetError(rdata, AOSLT_INVALID_SIZE);
		OmnAlarm << rdata->getErrmsg() << ". " << def->toString() << enderr;
		return false;
	}

	mMaxSize = def->getAttrInt64(AOSTAG_MAX_SIZE, -1);
	if (mMaxSize < 0)
	{
		AosSetError(rdata, AOSLT_INVALID_SIZE);
		OmnAlarm << rdata->getErrmsg() << ". " << def->toString() << enderr;
		return false;
	}

	if (mMinSize > mMaxSize)
	{
		AosSetError(rdata, AOSLT_INVALID_SIZE);
		OmnAlarm << rdata->getErrmsg() << ". " << def->toString() << enderr;
		return false;
	}

	mNextTries = def->getAttrInt(AOSTAG_NEXT_TRIES, 0);
	if (mNextTries < 0) mNextTries = 0;

	OmnString body = def->getNodeText();
	vector<OmnString> items;
	bool finished = false;
	while (!finished)
	{
		int num = AosStrSplit::splitStr(body, ",", items, eMaxSplit, finished);
		if (num <= 0) break;
		for (u32 i=0; i<items.size(); i+=2)
		{
			int64_t start = atoll(items[i]);
			int64_t size = atoll(items[i+1]);
			if (checkSize(size, rdata) && mMgr->addEntry(start, size, rdata))
			{
				mStarts.push_back(start);
				mSizes.push_back(size);
			}
		}
	}

	return true;
}


bool 
AosSlotMgrSlab::addSlot(
		const int64_t &start, 
		const int64_t &size, 
		const AosXmlTagPtr &parms, 
		const AosRundataPtr &rdata)
{
	if (!checkSize(size, rdata)) return false;
	mStarts.push_back(start);
	mSizes.push_back(size);
	return true;
}


bool 
AosSlotMgrSlab::getSlot(int64_t &start, 
					int64_t &size, 
					const AosXmlTagPtr &parms, 
					const AosRundataPtr &rdata)
{
	if (mStarts.size() <= 0) return false;
	start = mStarts.back();
	size = mSizes.back();
	mStarts.pop_back();
	mSizes.pop_back();
	return true;
}


bool
AosSlotMgrSlab::checkSize(
		const int64_t &size, 
		const AosRundataPtr &rdata)
{
	if (size >= mMinSize && size <= mMaxSize) return true;
	
	OmnString errmsg = AOSLT_INVALID_SIZE;
	errmsg << ". " << AOSLT_SIZE << ": " << size;
	rdata->setError(errmsg, __FILE__, __LINE__);
	mMgr->logErrors(AOSLT_ADD_SLOT, errmsg, rdata);
	return false;
}


bool
AosSlotMgrSlab::serializeTo(
		OmnString &contents, 
		const AosRundataPtr &rdata)
{
	contents << "<record "
		<< AOSTAG_MIN_SIZE << "=\"" << mMinSize << "\" "
		<< AOSTAG_MAX_SIZE << "=\"" << mMaxSize << "\" "
		<< AOSTAG_NEXT_TRIES << "=\"" << mNextTries << "\">";
	for (u32 i=0; i<mStarts.size(); i++)
	{
		contents << mStarts[i] << "," << mSizes[i];
	}
	contents << "</record>";
	return true;
}

