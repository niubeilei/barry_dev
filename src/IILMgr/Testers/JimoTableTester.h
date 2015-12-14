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
// Created By Ken Lee, 2014/09/11
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IILMgr_Testers_JimoTableTester_h
#define Aos_IILMgr_Testers_JimoTableTester_h

#include "Random/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/IILObj.h"
#include "Tester/TestPkg.h"
#include "Thread/Ptrs.h"
#include "Util/HashUtil.h"
#include "Util/Opr.h"
#include "Util/Ptrs.h"
#include "IILMgr/IIL.h"
#include "IILMgr/JimoTable.h"

#include <vector>
using namespace std;


class AosJimoTableTester : public OmnTestPkg
{
	AosRundataPtr			mRundata;
	OmnString				mIILName;
	u64						mIILID;
	AosJimoTablePtr			mIIL;
	AosXmlTagPtr			mBAVConfig;

public:
	AosJimoTableTester();
	~AosJimoTableTester() {}

	virtual bool			start();

private:
	bool				config();
	bool				singleAdd();
	bool				batchAdd();
	bool				getIIL();

	bool				addEntries(const i64 &idx);
	OmnString 			getRandomStr(const i64 &idx);

	bool				addBuffArrayVar();
	AosBuffArrayVarPtr	createBuffArrayVar();
};
#endif

