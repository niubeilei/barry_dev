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
#ifndef Aos_Actions_Testers_ActAddBlockTester_h
#define Aos_Actions_Testers_ActAddBlockTester_h

#include "Actions/Ptrs.h"
#include "IILAssembler/Ptrs.h"
#include "IILUtil/IILMatchType.h"
#include "IILUtil/IILEntrySelType.h"
#include "IILUtil/IILValueType.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "TaskMgr/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/Opr.h"
#include "Util/File.h"
#include "Util/Ptrs.h"
#include "Util/HashUtil.h"
#include "Util/String.h"
#include "Util/StrU64Array.h"
#include "XmlUtil/XmlTag.h"
#include <vector>
using namespace std;


class AosActAddBlockTester : virtual public OmnTestPkg
{

private:
	AosRundataPtr					mRundata;
	AosIILAssemblerPtr				mTableIILAssembler;

public:
	AosActAddBlockTester();
	~AosActAddBlockTester();

	virtual bool start();

private:
	bool		basicTest();
	bool		createData();
	bool		config();
};
#endif
