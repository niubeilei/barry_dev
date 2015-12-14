////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UtilTestSuite.h
// Description:
//   
//
// Modification History:
// 12/18/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSmartObj_SlotMgr_Tester_TestSuite_h
#define AosSmartObj_SlotMgr_Tester_TestSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"


class AosSlotMgrTestSuite
{
private:
	OmnTestSuitePtr		mSuite;

public:
	AosSlotMgrTestSuite() {}
	~AosSlotMgrTestSuite() {}

	static OmnTestSuitePtr		getSuite(const AosXmlTagPtr &config);
};
#endif

