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
// 06/12/2012 Created by Xu Lei
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataAssembler_Testers_DataAssemblerTester_h
#define Aos_DataAssembler_Testers_DataAssemblerTester_h

#include "IILUtil/IILMatchType.h"
#include "IILUtil/IILEntrySelType.h"
#include "IILUtil/IILValueType.h"
#include "Tester/TestPkg.h"
#include "Util/Opr.h"
#include "Util/String.h"
#include "Util/StrU64Array.h"
#include "XmlUtil/XmlTag.h"


class AosDataAssemblerTester : virtual public OmnTestPkg
{
private:
	AosRundataPtr 	mRundata;

public:
	AosDataAssemblerTester();
	~AosDataAssemblerTester() {};

	virtual bool start();

private:
	bool		basicTest();
};
#endif
