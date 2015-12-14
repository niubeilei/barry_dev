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
// 12/18/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSmartObj_SlotMgr_Tester_SlotMgrTeter_h
#define AosSmartObj_SlotMgr_Tester_SlotMgrTeter_h

#include "aosUtil/Types.h"
#include "Debug/Debug.h"
#include "Rundata/Ptrs.h"
#include "SmartObj/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Util/String.h"
#include "UtilHash/HashedObj.h"
#include "UtilHash/StrObjHash.h"
#include "UtilHash/Ptrs.h"
#include "XmlUtil/Ptrs.h"

class AosSlotMgrTester : public OmnTestPkg
{
private:
	enum
	{
		eDefaultTries = 1000000
	};

	AosRundataPtr 		mRundata;
	AosSobjSlotMgrPtr	mSobjSlotMgr;
	
public:
	AosSlotMgrTester();
	~AosSlotMgrTester() {}

	virtual bool		start();

private:
	bool	basicTest();
	bool	init();
};

#endif

