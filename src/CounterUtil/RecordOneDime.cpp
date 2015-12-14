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
// 08/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CounterUtil/RecordOneDime.h"

#include "CounterUtil/CounterUtil.h"
#include "CounterUtil/RecordFormatType.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"
#include "Util/BuffArray.h"

AosOneDime::AosOneDime()
{
}


AosOneDime::AosOneDime(const AosXmlTagPtr &term, const AosRundataPtr &rdata)
{
}


AosOneDime::~AosOneDime()
{
}

bool
AosOneDime::addValue(
		const OmnString &counterid,
		const AosStatType::E stattype,
		const OmnString &cname, 
		const u64 &docid, 
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	while (1)
	{
		OmnString name = cname;
		int64_t vv = docid;
		rslt = filling(name, vv, rdata);
		if (name == "") break;

		OmnString key = AosCounterUtil::getAllTerm2(name);
		aos_assert_r(key != "", false);

		if (mKeysIndex.count(key) == 0)
		{
			mKeys.push_back(key);
			mKeysIndex[key] = mKeys.size();
		}
		mCounterIds[counterid].updateValue(key, stattype, vv);
		if (!rslt) break;
	}
	return true;
}


OmnString
AosOneDime::createRecord(i64 &index, const AosRundataPtr &rdata)
{
	aos_assert_r(index >= 0 && index < (i64)mKeys.size(), "");

	OmnString key = mKeys[index];
	aos_assert_r(key != "", "");

	OmnString record = "<record ";
	record << splitName(AosCounterUtil::getCounterName(key));

	int k = 0;
	map<OmnString, AosCounterIds>::iterator iter; 
	for (iter = mCounterIds.begin(); iter != mCounterIds.end(); iter ++, k++)
	{
		OmnString counter_id = iter->first;
		record << AOSTAG_COUNTER_ID << k << "=\"" << counter_id << "\" ";

		for (u32 i = 0; i< mStatTypes.size(); i++)
		{
			AosStatType::E stattype = mStatTypes[i];
			OmnString name = "zky_";
			name << counter_id << k << "_" <<AosStatType::toString(stattype);

			int64_t v1 = (iter->second).getValue(key, stattype, mDftValue);
			(iter->second).accumulates[stattype] += v1; 
			int64_t v2 = (iter->second).accumulates[stattype];

			record << convertDataType(name, v1, rdata);
			name << "_accum";
			record << convertDataType(name, v2, rdata);
		}
	}

	record << "/>";
	return record;
}

