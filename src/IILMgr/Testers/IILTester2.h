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
// 08/09/2013	Created by Young Pan
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IILMgr_Testers_IILTester2_h
#define Aos_IILMgr_Testers_IILTester2_h

#include "Random/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/IILObj.h"
#include "Thread/ThrdShellProc.h"
#include "Tester/TestPkg.h"
#include "Thread/Ptrs.h"
#include "Util/HashUtil.h"
#include "Util/Opr.h"
#include "Util/Ptrs.h"
#include "IILMgr/IIL.h"
#include <vector>
using namespace std;


class AosIILTester2 : public OmnTestPkg
{
private:
	struct RunTest : public OmnThrdShellProc                                     
	{
		OmnDefineRCObject;

		AosIILTester2* 	mCaller;
		const AosRundata*		mRundata;
		const int 				mId;

		RunTest(AosIILTester2* caller, const AosRundata* rdata, const int id):
			OmnThrdShellProc("AosIILTester2"), mCaller(caller), mRundata(rdata), mId(id) {}

		bool run()                
		{
			mCaller->testStart(mId, mRundata);
			return true;
		}
		bool procFinished()
		{
			return true;
		}

	};

	AosRundataPtr			mRundata;

public:
	AosIILTester2();
	~AosIILTester2();

	virtual bool			start();
		
	bool testStart(const int id, const AosRundata* rdata);
};
#endif

