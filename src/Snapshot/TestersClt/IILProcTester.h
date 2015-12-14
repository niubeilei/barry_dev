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
// 08/09/2013	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Snapshot_TestersClt_IILProcTester_h
#define Aos_Snapshot_TestersClt_IILProcTester_h

#include "Random/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/IILObj.h"
#include "Snapshot/SnapShot.h"
#include "Util/HashUtil.h"
#include "Util/Opr.h"
#include "Util/Ptrs.h"
#include "IILMgr/IIL.h"
#include "Tester/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

class AosIILProcTester : public OmnRCObject 
{
	OmnDefineRCObject;
public:
	enum
	{
		eNumAddIILEntries = 100 
	};

	struct LKey 
	{
		OmnString key;
		u64 docid;

		LKey(const OmnString &k, const u64 d)
		:
		key(k),
		docid(d)
		{
		}

		~LKey()
		{
		}

		bool operator < (const LKey &rhs) const
		{
			if (key == rhs.key)
			{
				return docid < rhs.docid;
			}
			else
			{
				//return key < rhs.key;
				return (strcmp(key.data(), rhs.key.data()) < 0);
			}
		}
	};

	struct LValue
	{
		LValue(const int n1, const int nn)
		:
		num(n1),
		checknum(nn)
		{

		}

		~LValue() {};

		int num;
		int checknum;
	};
	typedef map<LKey, LValue> 					map_t; 
	typedef map<LKey, LValue>::iterator 		mapitr_t; 

private:

	AosRundataPtr			mRundata;
	OmnString				mIILName;
	u64 					mIILID;
	AosSnapShot::Status		mSts;
	map_t					mMaps;
	int64_t					mNumDocs;
	int						mIdx;

public:
	AosIILProcTester(
		const AosRundataPtr &rdata,
		const OmnString &iilname,
		const int idx);

	~AosIILProcTester();

	bool					basicTest();
	bool					createSnapShot();
	bool					commitSnapShot();
	bool					rollBackSnapShot();
	bool					clearMap();
	bool					serializeTo(const AosBuffPtr &conf_buff);
	bool					serializeFrom(const AosBuffPtr &conf_buff);
	bool					checkValue();
	OmnString				getIILName(){return mIILName;}
	int						getIdx(){return mIdx;}
private:

	bool					addEntries();
	bool					addEntry();

	void 					getIIL();
	OmnString 				getRandomStr(u32 length);

	bool					createIIL();
	bool					isCommit();
	bool					addKey(const OmnString &key, const u64 docid);
	bool					checkKey(const OmnString &key, const u64 docid);
	bool					verify();
};
#endif

