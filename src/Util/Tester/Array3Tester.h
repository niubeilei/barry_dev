////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Array3Tester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestUtil_Array3Tester_h
#define Omn_TestUtil_Array3Tester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"



class OmnArray3Tester : public OmnTestPkg
{
private:

public:
	OmnArray3Tester()
	{
		mName = "OmnArray3Tester";
	}
	~OmnArray3Tester() {}

	virtual bool		start();

private:
	bool testInitialization();
	bool testInsert();
};




#endif

