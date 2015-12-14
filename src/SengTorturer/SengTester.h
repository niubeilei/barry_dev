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
// 09/06/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSengTester_SengTester_h
#define AosSengTester_SengTester_h

#include "SengTorturer/TesterId.h"
#include "SengTorturer/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosSengTester : virtual public OmnRCObject
{
	OmnDefineRCObject;
public:
	enum
	{
		eDefaultWeight = 10,
		eMaxWeight = 10000,
		eDftTries = 100000,
		eMaxSingles = 100
	};

private:
	static int	smTotalWeights;

protected:
	OmnMutexPtr             mLock;
	int                     mWeight;
	AosTesterId::E			mTesterId;
	OmnString				mName;
	AosSengTestThrdPtr		mThread;
	AosSengTesterMgrPtr 	mMgr;
	int						mThreadId;
	OmnString				mRequest;
	OmnString				mResponse;
	OmnString				mLog;

	static AosSengTesterPtr smTesters[AosTesterId::eMax];
	static AosTesterId::E *	smTesterInsts;

public:
	AosSengTester(
			const OmnString &name,
			const AosTesterId::E id, 
			const bool regflag);
	AosSengTester(
			const OmnString &name,
			const OmnString &weight_tagname, 
			const AosTesterId::E id); 
	~AosSengTester();

	// Tester Interface
	virtual bool test() = 0;
	virtual bool configTester(const AosXmlTagPtr &config) {return true;}

	// Static functions
	static  int getTotalWeights() {return smTotalWeights;}
	static  bool startTestStatic(
				const AosSengTestThrdPtr &thread,
				const AosSengTesterMgrPtr &mgr);

	bool  	testStatic(
				const AosSengTestThrdPtr &thread,
				const AosSengTesterMgrPtr &mgr,
				AosTesterId::E id);
	static bool 	config(const AosXmlTagPtr &config);
	static int		calculateTotalWeights();
	virtual AosSengTesterPtr clone() = 0;
	int  	getWeight() const {return mWeight;}
	void 	setWeight(const int w) {mWeight = w;}
	OmnString getName() const {return mName;}

	bool config(const AosSengTestThrdPtr &thread, const AosSengTesterMgrPtr &mgr);
	static bool cloneTesters(
					const AosSengTestThrdPtr &thread, 
					AosSengTesterPtr *testers,
					const AosSengTesterMgrPtr &mgr);

protected:
	bool	registerTester(
			const AosTesterId::E id,
			const AosSengTesterPtr &tester);
};
#endif

