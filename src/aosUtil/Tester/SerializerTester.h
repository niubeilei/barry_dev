////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SerializerTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_AosUtil_Tester_SerializerTester_h
#define Aos_AosUtil_Tester_SerializerTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"

struct AosSerializer;
class AosSerializerTester : public OmnTestPkg
{
private:

public:
	AosSerializerTester()
	{
		mName = "AosSerializerTester";
	}
	~AosSerializerTester() {}

	virtual bool		start();

private:
	bool	basicTest();
	bool 	check(
				const char *file, 
				const int line,
				struct AosSerializer *bag, 
				const u32 cursor, 
				const u32 bufsize);
};
#endif

