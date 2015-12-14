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
// 01/18/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSengTester_DomainOprArdTester_h
#define AosSengTester_DomainOprArdTester_h

#include "Rundata/Ptrs.h"
#include "SengTorturer/SengTester.h"
#include "Tester/TestMgr.h"
#include "Thread/Ptrs.h"
#include "SengTorturer/StUtil.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosDomainOprArdTester : virtual public AosSengTester
{
private:

public:
	AosDomainOprArdTester(const bool regflag);
	AosDomainOprArdTester();
	~AosDomainOprArdTester();

	virtual bool test();
	AosSengTesterPtr clone()
	{
		return OmnNew AosDomainOprArdTester();
	}

private:
	bool createDomainOprArd();
	bool modifyDomainOprArd();
	bool removeDomainOprArd();
};
#endif

