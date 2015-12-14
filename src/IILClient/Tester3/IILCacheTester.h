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
// 	Created: 2015/03/11	by liuwei
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILClient_Tester3_IILCacheTester_h
#define AOS_IILClient_Tester3_IILCacheTester_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Util/Opr.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/Buff.h"
#include "IILUtil/IILTypes.h"

#include "list"


class AosIILCacheTester : public OmnTestPkg
{
private:
	
	struct I64Entry
	{
		i64 value;
		u64 docid;
		I64Entry()
		:
		value(0),
		docid(0)
		{
		}
		I64Entry(const i64 v, const u64 d)
		:
		value(v),
		docid(d)
		{
		}

		bool operator < (const I64Entry& b1) const
		{
			if(value < b1.value) return true;
			if(value > b1.value) return false;
			return docid < b1.docid;
		}
		bool operator > (const I64Entry& b1) const
		{
			if(value > b1.value) return true;
			if(value < b1.value) return false;
			return docid > b1.docid;
		}
		static int compare(const I64Entry &e1, const I64Entry &e2)
		{
			if(e1.value > e2.value)return 1;
			if(e1.value < e2.value)return -1;
			if(e1.docid > e2.docid)return 1;
			if(e1.docid < e2.docid)return -1;
			return 0;
		}
		
	};
	struct D64Entry
	{
		d64 value;
		u64 docid;
		D64Entry()
		:
		value(0),
		docid(0)
		{
		}
		D64Entry(const d64 v, const u64 d)
		:
		value(v),
		docid(d)
		{
	}

		bool operator < (const D64Entry& b1) const
		{
			if(value < b1.value) return true;
			if(value > b1.value) return false;
			return docid < b1.docid;
		}
		bool operator > (const D64Entry& b1) const
		{
			if(value > b1.value) return true;
			if(value < b1.value) return false;
			return docid > b1.docid;
		}
		
		static int compare(const D64Entry &e1, const D64Entry &e2)
		{
			if(e1.value > e2.value)return 1;
			if(e1.value < e2.value)return -1;
			if(e1.docid > e2.docid)return 1;
			if(e1.docid < e2.docid)return -1;
			return 0;
		}
	};
	struct U64Entry
	{
		u64 value;
		u64 docid;
		U64Entry()
		:
		value(0),
		docid(0)
		{
		}
		U64Entry(const u64 v, const u64 d)
		:
		value(v),
		docid(d)
		{
		}

		bool operator < (const U64Entry& b1) const
		{
			if(value < b1.value) return true;
			if(value > b1.value) return false;
			return docid < b1.docid;
		}
		bool operator > (const U64Entry& b1) const
		{
			if(value > b1.value) return true;
			if(value < b1.value) return false;
			return docid > b1.docid;
		}
		
		static int compare(const U64Entry &e1, const U64Entry &e2)
		{
			if(e1.value > e2.value)return 1;
			if(e1.value < e2.value)return -1;
			if(e1.docid > e2.docid)return 1;
			if(e1.docid < e2.docid)return -1;
			return 0;
		}
	};

	struct StrEntry
	{
		u64 value;
		u64 docid;
		StrEntry()
		:
		value(0),
		docid(0)
		{
		}
		StrEntry(const u64 v, const u64 d)
		:
		value(v),
		docid(d)
		{
		}

		bool operator < (const StrEntry& b1) const
		{
			if(value < b1.value) return true;
			if(value > b1.value) return false;
			return docid < b1.docid;
		}
		bool operator > (const StrEntry& b1) const
		{
			if(value > b1.value) return true;
			if(value < b1.value) return false;
			return docid > b1.docid;
		}
		
		static int compare(const StrEntry &e1, const StrEntry &e2)
		{
			if(e1.value > e2.value)return 1;
			if(e1.value < e2.value)return -1;
			if(e1.docid > e2.docid)return 1;
			if(e1.docid < e2.docid)return -1;
			return 0;
		}
	};

	enum
	{
		eMinGroupSize = 1,
		eMaxGroupSize = 2000,
		eMaxBlockSizes = 1683840,
	};

    u64 mIIL_num = 10000;//总数
    u64 mIIL_flag = 0;//计数器
    
	//各类IIL上限计数器
	u64 mOneEntry; //1条Entry 45%
    u64 mTenEntry; //2-10条Entry 20%
    u64 mHundredEntry;//11-100条Entry 15%
    u64 mThousandEntry; //101-1000条Entry 10% 
    u64 mOverThousandEntry;//大IIL：10% 

	vector<I64Entry>  ** mI64Lists;
	//mI64Lists[3] = OmnNew vector<I64Entry>;

	vector<D64Entry>  ** mD64Lists;

	vector<U64Entry>  ** mU64Lists;

	vector<StrEntry>  ** mStrLists;
	
    AosBuffPtr		mI64Buff;
	AosBuffPtr		mD64Buff;
	AosBuffPtr		mU64Buff;
	AosBuffPtr		mStrBuff;
	
	AosRundataPtr 	mRundata;

	OmnString		mI64IILName;
	OmnString		mD64IILName;
	OmnString		mU64IILName;
	OmnString		mStrIILName;
    
    OmnString		mI64IILName_now;
	OmnString		mD64IILName_now;
	OmnString		mU64IILName_now;
	OmnString		mStrIILName_now;

	AosQueryRsltObjPtr mQuery_rslt1;
	AosQueryRsltObjPtr mQuery_rslt2;
	AosQueryRsltObjPtr mQuery_rslt3;
	AosQueryRsltObjPtr mQuery_rslt4;
	AosQueryRsltObjPtr mQuery_rslt5;

	i64				mI64_val1;
	i64				mI64_val2;
	d64				mD64_val1;
	d64				mD64_val2;
	u64				mU64_val1;
	u64				mU64_val2;
	u64				mStr_val1;
	u64				mStr_val2;
	AosOpr			mOpr;

	u64				mAdd_times;
	u64				mRemove_times;
	u64				mQuery_times;
	
	OmnString		mZeroStr; 
	bool			mDirty;
public:
	AosIILCacheTester();
	~AosIILCacheTester();
	virtual bool 	start();
//	virtual bool	addEntry();
	virtual bool	addEntries();
//	virtual bool	removeEntry();
	virtual bool	removeEntries();
	virtual bool	query();
private:
	i64 getI64Val();
	i64 getI64Val2();
	d64 getD64Val();
	d64 getD64Val2();
	u64 getU64Val();
	u64 getU64Val2();
	u64 getStrVal();
	u64 getStrVal2();
	bool queryforI64(u64 IILList);
	bool queryforD64(u64 IILList);
	bool queryforU64(u64 IILList);
	bool queryforStr(u64 IILList);
	bool selectOpr();
	bool splitStrIIL();
	bool splitU64IIL();
	bool splitI64IIL();
	bool splitD64IIL();

	u64	randnum(const u64, const u64);
	bool checkRslt(const u64);
	u64 random_block_size(const u64 &remain);
	OmnString toString(u64);
	bool	percent(const int i)
	{
		return (rand()%100 < i);
	}
};
#endif
