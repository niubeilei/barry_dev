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
// 09/18/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CounterUtil_CounterQuery_h
#define Aos_CounterUtil_CounterQuery_h

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "CounterUtil/Ptrs.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "Util/DataTypes.h"
#include "Rundata/Ptrs.h"
#include "Util/Opr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include <map>
#include <vector>

#define AOSCOUNTERQUERY_ME 			"mergeEntries"
#define AOSCOUNTERQUERY_MC 			"mergeCount"
#define AOSCOUNTERQUERY_CE			"conversionEntries"
#define AOSCOUNTERQUERY_FE			"findMaxMinEntries"


class AosCounterQuery : public OmnRCObject 
{
	OmnDefineRCObject;

public:
	enum Type 
	{	
		eInvalid,
		eMerge,
		eMergeCount,	// Ken Lee, 2014/08/16
		eConversion,
		eFindMaxMin,
		eMax
	};

private:
	Type						mType;
	int							mMergeFieldIndex;

	//conversion entries 
	AosXmlTagPtr				mConvertConfig;				

	map<OmnString, u64>			mMap;
	map<OmnString, u64>			mMin;

	bool						mNeedFilterValue;
	AosOpr						mValueFilterOpr;
	u64							mValueFilterValue;

public:
	AosCounterQuery();
	~AosCounterQuery();

	void setAggregateType(const OmnString &ss){	mType = toEnum(ss); }
	OmnString getAggregateType() {return toStr(mType);}
	
	void setMergeFieldIndex(const int &index){	mMergeFieldIndex = index; }
	int getMergeFieldIndex() {return mMergeFieldIndex;}

	void setConvertEntriesConfig(const AosXmlTagPtr &term);

	void clear();

	bool serializeToXml(AosXmlTagPtr &xml);

	bool serializeFromXml(const AosXmlTagPtr &xml);

	bool aggregateMultiple(
			const AosQueryRsltObjPtr &query_rslt,
			const AosRundataPtr &rdata);

	bool copyData(
			const AosQueryRsltObjPtr &query_rslt,
			const AosRundataPtr &rdata);

	u32 getMapSize(){return mMap.size();}

	bool filterValue(const u64 &value);

	void setValueFilterCond(
			const AosOpr &opr,
			const u64 &value)
	{
		mNeedFilterValue = true;
		mValueFilterOpr = opr;
		mValueFilterValue = value;
	}

private:
	Type toEnum(const OmnString &name)
	{
		if (name == "") return eInvalid;
		const char *data = name.data();
		switch (data[0])
		{
		case 'c':
			 if (name == AOSCOUNTERQUERY_CE) return eConversion;
			 break;

		case 'f':
			 if (name == AOSCOUNTERQUERY_FE) return eFindMaxMin;
			 break;

		case 'm':
			 if (name == AOSCOUNTERQUERY_ME) return eMerge;
			 if (name == AOSCOUNTERQUERY_MC) return eMergeCount;
			 break;

		default:
			 break;
		}
		return eInvalid;
	}

	OmnString toStr(const Type &type)
	{
		switch (type)
		{
		case eConversion: 
			 return AOSCOUNTERQUERY_CE;

		case eFindMaxMin:
			 return AOSCOUNTERQUERY_FE;

		case eMerge:
			 return AOSCOUNTERQUERY_ME;

		case eMergeCount:
			 return AOSCOUNTERQUERY_MC;

		default:
			 return "";
		}
		return "";
	}
	bool mergeEntries(
			const AosQueryRsltObjPtr &query_rslt,
			const AosRundataPtr &rdata);

	bool mergeCount(
			const AosQueryRsltObjPtr &query_rslt,
			const AosRundataPtr &rdata);

	bool conversionEntries(
			const AosQueryRsltObjPtr &query_rslt,
			const AosRundataPtr &rdata);

	bool findMaxMinValue(
			const AosQueryRsltObjPtr &query_rslt,
			const AosRundataPtr &rdata);

	int findBoundary(
			const vector<OmnString> &value,
			const OmnString &vv,
			const AosDataType::E &data_type);

	int cmp(const AosDataType::E &data_type,
			const OmnString &lhs, 
			const OmnString &rhs);

	bool parseConfig(
			int &mergefield_index2,
			AosDataType::E &data_type,
			vector<OmnString> &value,
			vector<OmnString> &key);

	bool copyData1(
			const AosQueryRsltObjPtr &query_rslt,
			const AosRundataPtr &rdata);
};
#endif



