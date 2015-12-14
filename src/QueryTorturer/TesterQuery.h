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
// 10/26/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryTorturer_TesterQuery_h
#define Aos_QueryTorturer_TesterQuery_h

#include "QueryTorturer/Ptrs.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/Opr.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

#include <vector>
#include <set>
#include <map>
using namespace std;

class AosTesterQueryUnit
{
	OmnDefineRCObject;
public:
	u64			mDocid;
	OmnString	mStrValue;
	u64			mU64Value;
};

class AosTesterQuery
{
	OmnDefineRCObject;
public:
	enum
	{
		eNumWords = 5,
		eMaxRoundNum = 10000,
		eMaxOprNum = 50,
		eMaxDocs = 100000,
		eHitRate = 0,
		eStrRate = 50, 
		eU64Rate = 0,		
	};

private:
	OmnString		*mHitName;
	OmnString		*mStrName;
	OmnString		*mU64Name;

//	set<u64>		*mHitDocs;
//	set<u64>		*mStrDocs;
//	set<u64>		*mU64Docs;
	
	map<u64,AosTesterQueryUnitPtr> *mHitDocs; 
	map<u64,AosTesterQueryUnitPtr> *mStrDocs; 
	map<u64,AosTesterQueryUnitPtr> *mU64Docs; 

	set<u64>		mAllDocs;
	int				mNumDocs;

	u32				mSiteId;
	OmnString		mSsid;
	u64				mUrldocid;

	int				mRoundNum;
	AosRundataPtr	mRundata;
public:
	AosTesterQuery();
	~AosTesterQuery();

	bool		basicTest();
	void		setSiteId(const u32 siteid){mSiteId = siteid;}
	void		setSsid(const OmnString &ssid){mSsid = ssid;}
	void		setUrldocid(const u64 &id){mUrldocid = id;}
	void		setRundata(const AosRundataPtr &rdata){mRundata = rdata;}
	bool		init();
private:

	bool		procDocs();
	bool		procDoc();
	
	bool		procQuery();

	bool		addDoc();
	bool		removeDoc();
	bool		modifyDoc();

	inline bool 
	valueMatch(
		const OmnString &v1, 
		const AosOpr opr, 
		const OmnString &value)
	{
		return valueMatch(v1.data(),opr,value);
	}

	inline bool 
	valueMatch(
		const char *v1, 
		const AosOpr opr, 
		const OmnString &value)
	{
		int rslt = strcmp(v1, value.data());

		switch (opr)
		{
			case eAosOpr_an: return true;
			case eAosOpr_le: return rslt <= 0;
			case eAosOpr_lt: return rslt < 0;
			case eAosOpr_eq: return rslt == 0;
			case eAosOpr_gt: return rslt > 0;
			case eAosOpr_ge: return rslt >= 0;
			case eAosOpr_ne: return rslt != 0;
			case eAosOpr_like: return strstr(v1, value.data()) != NULL;
			default: return false;
		}
		return false;
	}

	inline bool 
	valueMatch(
		const u64 &v1, 
		const AosOpr opr, 
		const u64 &value)
	{
		int rslt = v1 - value;

		switch (opr)
		{
			case eAosOpr_le: return rslt <= 0;
			case eAosOpr_lt: return rslt < 0;
			case eAosOpr_eq: return rslt == 0;
			case eAosOpr_gt: return rslt > 0;
			case eAosOpr_ge: return rslt >= 0;
			case eAosOpr_ne: return rslt != 0;
			default: return false;
		}
		return false;
	}
private:

	bool query(int num_all,
				AosSengAdmin::QueryType *types,
				OmnString *attrs,
				AosOpr *oprs,
				OmnString *values,
				bool *orders,
				bool *reverses,
				map<u64,AosTesterQueryUnitPtr> *ordermap,
				set<u64> &expect,
				bool error_check);
};
#endif

