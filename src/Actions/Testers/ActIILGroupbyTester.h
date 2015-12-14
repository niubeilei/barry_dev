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
// 05/44/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Actions_Testers_ActIILGroupTester_h
#define Aos_Actions_Testers_ActIILGroupTester_h

#include "Actions/Ptrs.h"
#include "IILClient/Ptrs.h"
#include "IILUtil/IILMatchType.h"
#include "IILUtil/IILEntrySelType.h"
#include "IILUtil/IILValueType.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "TaskMgr/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/Opr.h"
#include "Util/HashUtil.h"
#include "Util/String.h"
#include "Util/StrU64Array.h"
#include "XmlUtil/XmlTag.h"
#include <vector>
using namespace std;


class AosActIILGroupbyTester : virtual public OmnTestPkg
{
private:
	AosStrU64Array		mSums;
	AosRundataPtr		mRundata;
	AosXmlTagPtr		mAction1Tag;
	AosXmlTagPtr		mAction2Tag;
	AosActionObjPtr		mAction1;
	AosActionObjPtr		mAction2;
	AosIILTransAssemblerPtr	mAssembler1;
	AosIILTransAssemblerPtr	mAssembler2;
	AosIILTransAssemblerPtr	mAssembler4;
	vector<OmnString>	mUPNs;
	AosStr2U64_t		mCPNPrefixMap;
	AosStr2U64_t		mUPNPrefixMap;
	AosStr2U32_t		mCallsMap;
	AosStr2U32_t		mCPNTownMap;
	AosTaskPtr			mTask;	
	OmnString			mIILName3;

public:
	AosActIILGroupbyTester();
	~AosActIILGroupbyTester() {};

	virtual bool start();

private:
	bool		basicTest();
	bool		resetData();
	bool		createIIL1();
	bool		createIIL2();
	bool		createIIL4();
	bool		createIILsToDb();
	bool		verifyPhase1Results();
	bool		verifyPhase2Results();
	bool		runPhase1Action();
	bool		runPhase2Action();
	bool		retrieveIIL3();
	bool		createCPNTowncode();
	u32			pickUCitycode();
	u32			pickCCitycode();
};
#endif
