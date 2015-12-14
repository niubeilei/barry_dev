////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Object.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util/Object.h"

#include "Porting/LongTypes.h"
#include "Porting/Mutex.h"


static OmnMutexType	sgLock = OmnInitMutex(sgLock);
OmnInt64 OmnObject::mTotalActiveObjectCount = 0;
OmnInt64 OmnObject::mTotalObjectCount = 0;
OmnInt64 OmnObject::mClassActiveObjCount[OmnClassId::eLastValidEntry];
OmnInt64 OmnObject::mClassTotalObjCount[OmnClassId::eLastValidEntry];
bool	OmnObject::mCountObjectsFlag = true;
bool	OmnObject::mObjectCountersInit = false;

void
OmnObject::initObjectCounters()
{
	OmnMutexLock(sgLock);
	for (int i=0; i<OmnClassId::eLastValidEntry; i++)
	{
		mClassActiveObjCount[i] = 0;
		mClassTotalObjCount[i] = 0;
	}

	mObjectCountersInit = true;
	OmnMutexUnlock(sgLock);
}


OmnObject::OmnObject(const OmnClassId::E classId)
:
mObjectSecretFlag1(eObjectSecretFlag1),
mClassId(classId),
mObj_File_Line(0),
mObjectSecretFlag2(eObjectSecretFlag2)
{
	mObj_File_Name[0] = 0;

	if (!mCountObjectsFlag)
	{
		return;
	}

	if (!mObjectCountersInit)
	{
		initObjectCounters();
	}

	OmnMutexLock(sgLock);
	mTotalActiveObjectCount++;
	mTotalObjectCount++;
	mClassTotalObjCount[mClassId]++;
	mClassActiveObjCount[mClassId]++;
	OmnMutexUnlock(sgLock);
}


OmnObject::~OmnObject()
{
	mObjectSecretFlag1 = 0;
	mObjectSecretFlag2 = 0;

	if (!mCountObjectsFlag)
	{
		return;
	}

	OmnMutexLock(sgLock);
	mTotalActiveObjectCount--;
	mClassActiveObjCount[mClassId]++;
	OmnMutexUnlock(sgLock);
}


OmnInt64
OmnObject::getClassActiveObjCount(const OmnClassId::E classId)
{
	if (!OmnClassId::isValid(classId))
	{
		return -1;
	}

	return mClassActiveObjCount[classId];
}


OmnInt64
OmnObject::getClassTotalObjCount(const OmnClassId::E classId)
{
	if (!OmnClassId::isValid(classId))
	{
		return -1;
	}

	return mClassTotalObjCount[classId];
}
