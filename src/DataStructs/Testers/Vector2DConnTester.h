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
#ifndef Aos_DataStructs_Testers_Vector2DConnTester_h
#define Aos_DataStructs_Testers_Vector2DConnTester_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "TaskMgr/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"
#include "DataStructs/Vector2D.h"


class AosVector2DConnTester : virtual public OmnTestPkg
{
private:

public:
	AosVector2DConnTester();
	~AosVector2DConnTester();

	virtual bool start();

private:
	bool		config();
	bool		basicTest();
	bool		performanceTest();
};
#endif
