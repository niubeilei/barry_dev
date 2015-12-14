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
// 2010/10/23	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IDTransMap_Tester_TransBucketTester_h
#define Aos_IDTransMap_Tester_TransBucketTester_h

#include "Util/Opr.h"
#include "XmlUtil/XmlTag.h"
#include "Tester/TestPkg.h"

class AosTransBucketTester : public OmnTestPkg
{

public:
	AosTransBucketTester();
	~AosTransBucketTester() {};

	virtual bool		start();
	bool basicTest();
	bool doOneTest();
};
#endif
