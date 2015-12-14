////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DeviceTable.cpp
// Description:
// 	This table maintains and manages OmnDevice. When someone wants 
// 	a device, it should ask this table. This table maintains hash
// 	tables to the devices retrieved into memory. 
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "DataServer/DeviceTable.h"

#include "Alarm/Alarm.h"
#include "Database/Database.h"
#include "DataServer/DataMgr.h"
#include "DataServer/DvBlackList.h"
#include "Device/Device.h"
#include "MgcpMonitor/DeviceMtr.h"
#include "MgcpMonitor/DvMtrMgr.h"
#include "Network/Network.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SysObj/SoDevice.h"
#include "Thread/Mutex.h"

OmnSingletonImpl(OmnDeviceTableSingleton,
				 OmnDeviceTable,
				 OmnDeviceTableSelf,
				"OmnDeviceTable");



OmnDeviceTable::OmnDeviceTable()
:
mTableLock(OmnNew OmnMutex())
{
}


OmnDeviceTable::~OmnDeviceTable()
{
}


bool
OmnDeviceTable::start()
{
	OmnSoDevice::resetDeviceStatus();
	return true;
}


bool
OmnDeviceTable::addToTables(const OmnDevicePtr &device)
{
	// 
	// A device has been brought into memory. This function
	// adds the device to the hash table. The caller should
	// not lock the table.
	//
	
	OmnDvMtrMgrSelf->addMonitor(device->getMonitor());

	mTableLock->lock();
	mHash.add(device->getDeviceId(), device);
	mTableLock->unlock();
	return true;
}


bool
OmnDeviceTable::removeFromTables(const OmnString &physicalId)
{
	// 
	// The caller decides to remove the device. Most likely
	// the device has been deleted from the database, or
	// aged out of the memory. To this function, it doesn't
	// care which one. All it does is to remove the device 
	// from the hash tables.
	//
	OmnTraceDb << "To remove Device: " << physicalId << endl;

	OmnDevicePtr device;
	mTableLock->lock();
	mHash.get(physicalId, device, true);
	mTableLock->unlock();

	if (!device.isNull())
	{
		OmnDvMtrMgrSelf->removeMonitor(device->getMonitor());
	}

	return true;
}


OmnDevicePtr
OmnDeviceTable::getDevice(const OmnString &deviceId)
{
	//
	// This class keeps a hashtable in memory for those often used devices.
	// When this function is called, it first checks the hashtable. If found,
	// it returns the device. Otherwise, it retrieves it from the database.
	// If failed to retrieve the device from the database, it is an error.
	// Otherwise, it adds the device into the hashtable and returns 
	// the device.
	//

	OmnTraceDb << "To retrieve Device: " << deviceId << endl;

	//
	// Check whether the hashtable has it or not
	//
	OmnDevicePtr device;
	mTableLock->lock();
	mHash.get(deviceId, device, false);
	mTableLock->unlock();
	if (!device.isNull())
	{
		//
		// Found it.
		//
		OmnTraceDb << "Found device in memory: " << deviceId << endl;
		return device;
	}

	// 
	// Check the black list
	//
	// Chen Ding, 06/01/2003, Change-2003-06-0003
	//
	if (OmnDvBlackListSelf->isOnList(deviceId))
	{
		// 
		// This is a known undefined device. 
		//
		OmnTraceCP << "In BL" << endl;
	 	return 0;
	}

	//
	// Not in memory. Try the database
	//
	try
	{
		device = OmnNew OmnDevice(deviceId);
	}

	catch (const OmnExcept &e)
	{
		//
		// error in retrieving an device from DB.
		//
		OmnString err = "Device not found: ";
	    err << deviceId;
		OmnTraceDb << err << endl;

		// 
		// Add to the black list
		//
		// Chen Ding, 06/01/2003, Change-2003-06-0003
		//
		OmnTraceCP << "Device not found. Add to black list: " 
			<< deviceId << endl;
		OmnDvBlackListSelf->add(deviceId);
		return 0;
	}

	//
	// Found the device from the database. Adds it to the hashtable
	//
	OmnTraceDb << "Found Device: " << deviceId << endl;

	addToTables(device);
	return device;
}


bool		
OmnDeviceTable::deviceAdded(const OmnSoDevicePtr &so)
{
	// 
	// Currently, we do nothing about it.
	//
	OmnTraceDb << "Device added: " << so->getDeviceId() << endl;
	OmnDvBlackListSelf->dvAdded(so->getDeviceId());
	return true;
}


bool		
OmnDeviceTable::deviceDeleted(const OmnSoDevicePtr &so)
{
	// 
	// An device has been deleted from the database. This
	// function checks whether the device is already in memory.
	// If not, do nothing. Otherwise, remove it from the hash table.
	// It then adds the device into the black list.
	//
	if (removeFromTables(so->getDeviceId()))
	{
		OmnTraceDb << "Device: " << so->getDeviceId()
			<< " removed from memory!" << endl;
	}
	else
	{
		OmnTraceDb << "To delete device: " << so->getDeviceId()
			<< " but not in memory!" << endl;
	}

	OmnDvBlackListSelf->dvDeleted(so->getDeviceId());
	return true;
}


bool		
OmnDeviceTable::deviceModified(const OmnString &oldDeviceId)
{
	// 
	// An device has been modified by another program. This
	// function checks whether the device is already in memory.
	// If not, do nothing. Otherwise, remove the device from 
	// the memory.
	//
	mTableLock->lock();
	OmnDevicePtr device;
    mHash.get(oldDeviceId, device, true);

	OmnDvBlackListSelf->dvModified(oldDeviceId);
	return true;
}

