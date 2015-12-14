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
// 02/28/2013 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CounterUtil_RecordFormat_h
#define Aos_CounterUtil_RecordFormat_h

#include "Rundata/Ptrs.h"
#include "Util/ValueRslt.h"
#include "CounterUtil/StatTypes.h"
#include "CounterUtil/Ptrs.h"
#include "CounterUtil/CounterTimeInfo.h"

class AosRecordFormat : virtual public OmnRCObject
{
	OmnDefineRCObject;        

protected:
	struct AosCounterIds
	{
		vector<int64_t>         values[AosStatType::eMaxEntry];
		int64_t                 accumulates[AosStatType::eMaxEntry];
		map<OmnString, int> 	index;

		AosCounterIds()
		{
			for (int i = 0; i<AosStatType::eMaxEntry; i++) accumulates[i] = 0;
		}

		~AosCounterIds(){}

		int64_t getValue(const OmnString &key, const AosStatType::E stat_type, const int64_t &dft)
		{
			OmnString kk = AosStatType::toString(stat_type);
			kk <<"."<<key;
			if (index.count(kk) == 0) return dft;
			int idx = index[kk];
			aos_assert_r(idx >= 0 && (u32)idx < values[stat_type].size(), dft);
			return values[stat_type][idx];
		}

		void updateValue(const OmnString &key, const AosStatType::E stat_type, const int64_t &value)
		{
			OmnString kk = AosStatType::toString(stat_type);
			kk <<"."<< key;
			if (index.count(kk) != 0) 
			{
				int idx = index[kk];
				aos_assert(idx >= 0 && (u32)idx < values[stat_type].size());
				values[stat_type][idx] = values[stat_type][idx] + value; 
			}
			else
			{
				index[kk] = values[stat_type].size(); 
				values[stat_type].push_back(value);
			}
		}

	};

	struct Filling
	{
		OmnString				type;
		int                     filling_field;
		u64                     crt_time;
		OmnString               template_cname;
	};

	AosCounterTimeInfoPtr			mTime;
	map<OmnString, AosCounterIds>	mCounterIds;
	double 							mFactor;
	vector<AosStatType::E>  		mStatTypes;
	vector<OmnString>       		mKeys;
	map<OmnString, int>				mKeysIndex;
	int64_t							mDftValue;
	bool							mFillingFlag;
	Filling							mFilling;

public:
	AosRecordFormat();
	~AosRecordFormat();

	virtual bool addValue(
			const OmnString &counterid,
			const AosStatType::E stattype,
			const OmnString &cname,
			const u64 &docid,
			const AosRundataPtr &rdata) = 0;

	virtual OmnString createRecord(i64 &index, const AosRundataPtr &rdata) = 0;

	static AosRecordFormatPtr createFormat(const AosXmlTagPtr &term, const AosRundataPtr &rdata);

	void setFactor(const double &b){mFactor = b;}
	void setStatTypes(const vector<AosStatType::E> &b) {mStatTypes = b;}
	void setTimeObj(const AosCounterTimeInfoPtr &b) {mTime = b;}
	void setFilling(const OmnString &type, const int &filling_field, const int64_t &dft)
	{
		aos_assert(mTime);
		mFillingFlag = true;
		mFilling.type = type;
		mFilling.filling_field = filling_field;
		mFilling.crt_time = mTime->convertUniTime(mTime->getStartTime());
		mFilling.template_cname = "";
		mDftValue = dft;
	}

	void resetFillingCrtTime()
	{
		aos_assert(mTime);
		if (mTime->isValidTimeRange())
		{
			mFilling.crt_time = mTime->convertUniTime(mTime->getStartTime());
		}
	}

	void setCounterIds(const vector<OmnString> &vv)
	{
		for (u32 i = 0; i < vv.size(); i++)
		{
			AosCounterIds counterIdObj; 
			mCounterIds[vv[i]] = counterIdObj;	
		}
	}

	int getNumValues(){	return mKeys.size(); }

protected:

	bool filling(
			OmnString &cname, 
			int64_t &value, 
			const AosRundataPtr &rdata);

	bool fillingFinished(
			OmnString &cname, 
			int64_t &value, 
			const AosRundataPtr &rdata);

	OmnString splitName(const OmnString &cname);

	OmnString convertDataType(
			const OmnString &name,
			const int64_t &v1,
			const AosRundataPtr &rdata);
};

#endif

