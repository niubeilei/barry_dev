////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SerializerTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aosUtil/Tester/SerializerTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "aosUtil/Serializer.h"

#include "aosUtil/Serializer.h"



bool AosSerializerTester::start()
{
	// 
	// Test default constructor
	//
	basicTest();
	return true;
}


bool AosSerializerTester::basicTest()
{
	OmnBeginTest << "Test AosSerializer";
	mTcNameRoot = "SerializerTest";

	AosSerializer_t *bag;
	u32 len = 999+1;
	int ret = AosSerializer_init(&bag, len);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc; 
	check(__FILE__, __LINE__, bag, 4, len);

	// 
	// Add 99+1 integers
	//
	int value = 99+1;
	int numInt = 99+1;
	int pos = 4;
	int i;
	for (i=0; i<numInt; i++)
	{
		ret = AosSerializer_addInt(bag, value++);
		OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc; 
		pos += 5;
		check(__FILE__, __LINE__, bag, pos, len);
	}

	char *buff = AosSerializer_finish(bag);
	AosSerializer_t *bag1;
	ret = AosSerializer_set(&bag1, buff, bag->cursor);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc; 
	OmnTC(OmnExpected<int>(5), OmnActual<int>(bag1->cursor)) << endtc; 

	pos = 5;
	len = bag->cursor;
	value = 99+1;
	int v;
	// for	(i=0; i<numInt; i++)
	for	(i=0; i<1; i++)
	{
		ret = AosSerializer_getInt(bag1, &v);
		OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc; 
		pos += 5;
		check(__FILE__, __LINE__, bag1, pos, len);
		OmnTC(OmnExpected<int>(value), OmnActual<int>(v)) << endtc; 
	}
		
	return true;
}


bool AosSerializerTester::check(
			const char *file, 
			const int line,
			AosSerializer_t *bag, 
			const u32 cursor, 
			const u32 bufsize)
{
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(bag->buff)) 
		<< "Called from: " << file << ":" << line << endtc; 
	OmnTC(OmnExpected<int>(cursor), OmnActual<int>(bag->cursor)) 
		<< "Called from: " << file << ":" << line << endtc; 
	OmnTC(OmnExpected<int>(bufsize), OmnActual<int>(bag->buff_size)) 
		<< "Called from: " << file << ":" << line << endtc; 
	
	return true;
}

