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
// 10/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IILTransMap_Testers_IILTransTestThrd_h
#define Aos_IILTransMap_Testers_IILTransTestThrd_h

#include "Util/Opr.h"
#include "Util/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Tester/TestPkg.h"
#include "TransServer/Ptrs.h"

#include <vector>


class AosIILTransTestThrd : public OmnTestPkg, public OmnThreadedObj
{
private:
	enum
	{
		eBuffSize = 100000, 
		eBuffIncSize = 10000,
		
		eMaxHitIILs = 10000,
		eMaxStrIILs = 10000,
		eMaxStrValues = 10000,
		eMaxU64IILs = 10000
	};

	int				mGroup;
	OmnThreadPtr	mThread;
	u64				mTransid;
	u64				mDocid;
	int				mNumHitIILs;
	int				mNumStrIILs;
	int				mStrValue;
	int				mNumU64IILs;
	int				mU64Value;
	AosTransServerPtr	mTransServer;

//	OmnString		mHitIILNames[eMaxHitIILs];
//	bool			mHitPersis[eMaxHitIILs];
//	OmnString		mStrIILNames[eMaxStrIILs];
//	bool			mStrPersis[eMaxStrIILs];
//	bool			mStrValueUnique[eMaxStrIILs];	
//	bool			mStrDocidUnique[eMaxStrIILs];	
//	OmnString		mU64IILNames[eMaxU64IILs];
//	bool			mU64Persis[eMaxU64IILs];
//	bool			mU64ValueUnique[eMaxU64IILs];	
//	bool			mU64DocidUnique[eMaxU64IILs];	


public:
	AosIILTransTestThrd(const int &idx, const AosTransServerPtr &server);
	~AosIILTransTestThrd();

	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

private:
	bool		basicTest();
	
	bool		addHitDoc(vector<AosBuffPtr> &buffs, const AosRundataPtr &rdata);
	OmnString	getHitIILName(bool &isPersis);
	
	bool		addStrDoc(vector<AosBuffPtr> &buffs, const AosRundataPtr &rdata);
	OmnString	getStrIILName(
					bool &isPersis,
					bool &valueUnique,
					bool &docidUnique);
	OmnString	getStrValue(const bool &valueUnique);
	
	bool		addU64Doc(vector<AosBuffPtr> &buffs, const AosRundataPtr &rdata);
	OmnString	getU64IILName(
					bool &isPersis,
					bool &valueUnique,
					bool &docidUnique);
	u64			getU64Value(const bool &valueUnique);
};


#endif

