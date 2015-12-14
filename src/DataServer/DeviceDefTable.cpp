////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DeviceDefTable.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "DataServer/DeviceDefTable.h"

/*
#include "Alarm/Alarm.h"
#include "Database/Database.h"
#include "DataServer/DataMgr.h"
#include "Network/Network.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SysObj/SoDeviceDef.h"
#include "Thread/Mutex.h"

OmnSingletonImpl(OmnDeviceDefTableSingleton,
				 OmnDeviceDefTable,
				 OmnDeviceDefTableSelf,
				"OmnDeviceDefTable");


extern OmnNetwork *		OmnNetworkSelf;


OmnDeviceDefTable::OmnDeviceDefTable()
:
mTableLock(OmnNew OmnMutex())
{
}


OmnDeviceDefTable::~OmnDeviceDefTable()
{
}


OmnSoDeviceDefPtr
OmnDeviceDefTable::getDevice(const OmnString &logicalId)
{
	OmnTraceDb << "To retrieve Device: " << logicalId << endl;

	//
	// Check whether the hashtable has it or not
	//
	OmnSoDeviceDefPtr device;
	mTableLock->lock();
	mDefs.reset();
	while (mDefs.hasMore())
	{
		device = mDefs.crtValue();
		mDefs.next();
		if (device->getLogicalId() == logicalId)
		{
			mTableLock->unlock();
			return device;
		}
	}

	//
	// Not in memory. Try the database
	//
	OmnSoDeviceDefPtr so = OmnNew OmnSoDeviceDef();
	if (!so->getDevice(logicalId))
	{
		//
		// error in retrieving an device from DB.
		//
		OmnString err = "DeviceDef not found: ";
	    err << logicalId;
		OmnTraceDb << err << endl;

		return 0;
	}

	//
	// Found the device from the database. Adds it to the hashtable
	//
	OmnTraceDb << "Found DeviceDef: " << logicalId << endl;

	mTableLock->lock();
	mDefs.append(so);
	mTableLock->unlock();
	return so;
}


bool
OmnDeviceDefTable::remove(const OmnString &logicalId)
{
	mTableLock->lock();
	mDefs.reset();
	while (mDefs.hasMore())
	{
		if ((mDefs.crtValue())->getLogicalId() == logicalId)
		{
			mDefs.eraseCrt1();
			mTableLock->unlock();
			return true;
		}

		mDefs.next();
	}

	mTableLock->unlock();
	return false;
}


bool		
OmnDeviceDefTable::deviceAdded(const OmnSoDeviceDefPtr &so)
{
	// 
	// Currently, we do nothing about it.
	//
	OmnTraceDb << "Device added: " << so->getLogicalId() << endl;
	return true;
}


bool		
OmnDeviceDefTable::deviceDeleted(const OmnSoDeviceDefPtr &so)
{
	return remove(so->getLogicalId());
}


bool		
OmnDeviceDefTable::deviceModified(const OmnSoDeviceDefPtr &so)
{
	return remove(so->getLogicalId());
}
*/

