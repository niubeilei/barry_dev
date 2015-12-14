////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DvBlackList.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "DataServer/DvBlackList.h"

#include "Alarm/Alarm.h"
#include "Database/Database.h"
#include "DataServer/DataMgr.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Util/Integer.h"
#include "Util1/Time.h"


OmnSingletonImpl(OmnDvBlackListSingleton,
				 OmnDvBlackList,
				 OmnDvBlackListSelf,
				"OmnDvBlackList");


extern OmnTime *	OmnTimeSelf;


OmnDvBlackList::OmnDvBlackList()
:
mTableLock(OmnNew OmnMutex())
{
}


OmnDvBlackList::~OmnDvBlackList()
{
}


bool
OmnDvBlackList::dvAdded(const OmnString &deviceId)
{
	// 
	// Remove the device from the hash tables, if it is 
	// in the hash tables.
	//
	mTableLock->lock();
	OmnIntegerPtr tick;
	mHashTable.get(deviceId, tick, true);
	mTableLock->unlock();
	return true;
}


bool
OmnDvBlackList::dvModified(const OmnString &oldDeviceId)
{
	// 
	// Remove the device from this black list.
	//
	OmnTraceCP << "Remove device from black list: " 
		<< oldDeviceId << endl;
	mTableLock->lock();
	OmnIntegerPtr tick;
	mHashTable.get(oldDeviceId, tick, true);
	mTableLock->unlock();
	return true;
}


bool
OmnDvBlackList::dvDeleted(const OmnString &deviceId)
{
	//
	// Do not add the device on until the device 
	// sends something.
	//
	return true;
}


void
OmnDvBlackList::add(const OmnString &deviceId)
{
		
	mTableLock->lock();

	// 
	// Check whether it is already on the list. If yes, do nothing.
	//
	OmnIntegerPtr tick;
	if (mHashTable.get(deviceId, tick, false))
	{
		// 
		// THis is not correct. Raise a warning
		//
		mTableLock->unlock();
		OmnWarn << OmnErrId::eWarnProgError	
			<< "To add device to blacklist but it is already in: " 
			<< deviceId << enderr;
		return;
	}

 	tick = OmnNew OmnInteger(OmnTimeSelf->getSecTick());
	mHashTable.add(deviceId, tick);
	mTableLock->unlock();
	return;
}


bool
OmnDvBlackList::isOnList(const OmnString &deviceId)
{
	OmnIntegerPtr tick;
	return mHashTable.get(deviceId, tick, false);
}


