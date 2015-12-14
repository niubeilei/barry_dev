////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HashMgr.cpp
// Description:
// 	This class is used to manage all hash tables. It's mainly
// 	designed to manage memory.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util/HashMgr.h"

#include "Debug/Debug.h"
#include "Thread/Mutex.h"
#include "Util/HashTable.h"



OmnVList<OmnHashTable*>		OmnHashMgr::mHashTables;
static OmnMutex		sgLock;


bool			
OmnHashMgr::addHashTable(OmnHashTable *table)
{
	sgLock.lock();
	mHashTables.append(table);
	sgLock.unlock();

	return true;
}


bool			
OmnHashMgr::removeHashTable(OmnHashTable *table)
{
	sgLock.lock();
	mHashTables.reset();
	while (mHashTables.hasMore())
	{
		if (mHashTables.crtValue() == table)
		{
			mHashTables.eraseCrt();
			sgLock.unlock();
			return true;
		}

		mHashTables.next();
	}

	sgLock.unlock();
	return false;
}


int 
OmnHashMgr::getNumHashTables()
{
	return mHashTables.entries();
}


int
OmnHashMgr::getHashedObj()
{
	OmnTrace << "Number of hash tables: " << mHashTables.entries() << endl;

	sgLock.lock();
	mHashTables.reset();

	int objs = 0;
	while (mHashTables.hasMore())
	{
		objs += (mHashTables.crtValue())->getHashedObj();
		mHashTables.next();
	}

	sgLock.unlock();
	return objs;
}

