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
//	12/17/2009	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Util_Tester_VectorTester_h
#define Aos_Util_Tester_VectorTester_h

#include "Tester/TestPkg.h"
#include "Util/RCObjImp.h"
#include "Util/SmtIdxVList.h"


class OmnTestMgr;


class AosVectorTester : public OmnTestPkg
{

public:
	AosVectorTester() {}
	~AosVectorTester() {}

	bool 	start();
	bool 	torturer();
};
#endif


