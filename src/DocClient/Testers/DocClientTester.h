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
// 05/14/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocLClient_Testers_DocClientTester_h
#define Aos_DocLClient_Testers_DocClientTester_h

#include "Util/Opr.h"
#include "Rundata/Ptrs.h"
#include "Tester/TestPkg.h"


class AosDocClientTester : public OmnTestPkg
{
private:
	enum
	{
		eNormalTries = 100
	};

	OmnString mValue;

public:
	AosDocClientTester();
	~AosDocClientTester() {}

	virtual bool		start();

private:
	void createPctrs(const AosRundataPtr &rdata);
	bool basicTest();
	bool test(const AosRundataPtr &rdata);
	AosXmlTagPtr createBinaryDoc(const AosRundataPtr &rdata);
	AosXmlTagPtr retrieveBinaryDoc(const AosXmlTagPtr xml, const AosRundataPtr &rdata);
	bool deleteBinaryDoc(const AosXmlTagPtr xml, const AosRundataPtr &rdata);
};


#endif

