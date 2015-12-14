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
// 2012/03/07	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataFormat_Testers_DataFormatTester_h
#define Aos_DataFormat_Testers_DataFormatTester_h

#include "DataFormat/Ptrs.h"
#include "Tester/TestPkg.h"


class AosDataFormatTester : public OmnTestPkg
{

public:
	AosDataFormatTester();
	~AosDataFormatTester() {}

	virtual bool		start();

private:
	bool		basicTest();
	OmnString	createFormatXml(OmnString &doc);
	OmnString	getElemType();
};


#endif

