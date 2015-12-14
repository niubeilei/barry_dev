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
#include "CounterUtil/RecordTwoDime.h"

#include "API/AosApi.h"
#include "CounterUtil/CounterUtil.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"

AosTwoDime::AosTwoDime()
:
mRowIdx(-1),
mColPsize(-1)
{
}


AosTwoDime::AosTwoDime(const AosXmlTagPtr &term, const AosRundataPtr &rdata)
:
mRowIdx(-1),
mColPsize(-1)
{
	OmnString x = term->getNodeText("zky_x");
	mRowIdx = (x == "" ?-1:atoi(x.data()));
	if (mRowIdx > 0) 
	{
		OmnString psize = term->getNodeText("zky_ypsize");
		mColPsize = (psize == "" ? -1:atoi(psize.data())); 
	}
}


AosTwoDime::~AosTwoDime()
{
}

bool
AosTwoDime::addValue(
		const OmnString &counterid,
		AosStatType::E stattype,
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
		OmnString row_key = AosCounterUtil::getTerm2(key, mRowIdx);
		OmnString col_key = AosCounterUtil::decomposeRemoveTerm(key, mRowIdx, false);
		aos_assert_r(row_key != "" && col_key != "", false);

		if (mKeysIndex.count(row_key) == 0)
		{
			mKeys.push_back(row_key);
			mKeysIndex[row_key] = mKeys.size();
		}

		if (mColumnIndex.count(col_key) == 0)
		{
			mColKeys.push_back(col_key);
			mColumnIndex[col_key] = mColKeys.size();
		}

		OmnString kk = row_key;
		kk << "." << col_key;
		mCounterIds[counterid].updateValue(kk, stattype, vv);
		if (!rslt) break;
	}
	return true;
}



OmnString 
AosTwoDime::createRecord(i64 &index, const AosRundataPtr &rdata)
{
	aos_assert_r(index >= 0 && index < (i64)mKeys.size(), "");

	OmnString row_key = mKeys[index];
	aos_assert_r(row_key != "", "");

	vector<OmnString> records;
	int k = 0;
	map<OmnString, AosCounterIds>::iterator iter; 
	for (iter = mCounterIds.begin(); iter != mCounterIds.end(); iter ++, k++)
	{
		OmnString counter_id = iter->first;
		u64 crtYIdx = 0;
		int crtPsize = 0;
		while (crtYIdx <= mColKeys.size() -1)
		{
			if (mColPsize > 0 && ++crtPsize > mColPsize) break;
			if (crtYIdx >= records.size())
			{
				OmnString record =  "<time "; 
				record << AOSTAG_CNAME <<" =\"" << row_key << "\" ";

				record << splitName(mColKeys[crtYIdx]);
				records.push_back(record);
				aos_assert_r(records.size() -1 == crtYIdx, "");
			}

			for (u32 i = 0; i< mStatTypes.size(); i++)
			{
				AosStatType::E stattype = mStatTypes[i];
				OmnString name = "zky_";
				name << counter_id << k << "_"<< AosStatType::toString(stattype);

				OmnString kk = row_key;
				kk << "." << mColKeys[crtYIdx];
				int64_t v1 = (iter->second).getValue(kk, stattype, mDftValue);
				(iter->second).accumulates[stattype] += v1; 
				int64_t v2 = (iter->second).accumulates[stattype];

				records[crtYIdx] << convertDataType(name, v1, rdata);
				name << "_accum";
				records[crtYIdx] << convertDataType(name, v2, rdata);
			}
			crtYIdx ++;
		}
	}

	OmnString contents = "<record ";
	contents << AOSTAG_USING_XML_AS_RESULT << "=\"" << "true" << "\" ";
	contents << AOSTAG_CNAME << "=\"" << row_key << "\" ";

	k = 0;
	for (iter = mCounterIds.begin(); iter != mCounterIds.end(); iter ++, k++)
	{
		contents << AOSTAG_COUNTER_ID << k << "=\"" << iter->first <<"\" ";
		for (u32 i = 0; i<mStatTypes.size(); i++)
		{
			AosStatType::E stattype = mStatTypes[i];
			int64_t v1 = (iter->second).accumulates[stattype];
			OmnString name = "zky_";
			name << iter->first << k << "_" 
				<< AosStatType::toString(stattype) << "_accum";
			contents << convertDataType(name, v1, rdata);
			(iter->second).accumulates[stattype] = 0;
		}
	}
	contents << ">";

	for (u32 i = 0; i < mColKeys.size(); i++)
	{
		contents << records[i] << "/>";
	}
	contents << "</record>";
	return contents;
}

