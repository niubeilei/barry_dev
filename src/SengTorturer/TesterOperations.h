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
// 01/07/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSengTester_OperationsTester_h
#define AosSengTester_OperationsTester_h

#include "Rundata/Ptrs.h"
#include "SengTorturer/SengTester.h"
#include "Tester/TestMgr.h"
#include "Thread/Ptrs.h"
#include "SengTorturer/StUtil.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosOperationsTester : virtual public AosSengTester
{
private:
	enum
	{
		eDftDefineOperations = 50,
		eDftAddOperations = 50,
		eDftRemoveOperations = 40,
		eDftCheckOperations = 50,
	};

	int		mWtDefineOperations;
	int		mWtAddOperations;
	int		mWtRemoveOperations;
	int		mWtCheckOperations;

public:
	AosOperationsTester(const bool regflag);
	AosOperationsTester();
	~AosOperationsTester();

	virtual bool test();
	AosSengTesterPtr clone()
	{
		return OmnNew AosOperationsTester();
	}

private:
	bool configTester(const AosXmlTagPtr &config);
	bool defineOperations();
	bool addOperations();
	bool modifyOperations();
	bool removeOperations();
	bool checkOperations();
	bool checkOperation();
};
#endif

