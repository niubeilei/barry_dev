////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: NatTable.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "DataServer/NatTable.h"

#include "Alarm/Alarm.h"
#include "Database/Database.h"
#include "DataServer/DataMgr.h"
#include "Debug/Except.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SysObj/SoNat.h"
#include "Thread/Mutex.h"


OmnSingletonImpl(OmnNatTableSingleton,
				 OmnNatTable,
				 OmnNatTableSelf,
				"OmnNatTable");

OmnNatTable::OmnNatTable()
:
mLock(OmnNew OmnMutex())
{
}


OmnNatTable::~OmnNatTable()
{
}


bool
OmnNatTable::removeNat(const int bubbleId, const OmnIpAddr &addr)
{
	OmnTraceDb << "To remove NAT: " 
		<< addr.toString() << ":" << bubbleId << endl;

	mLock->lock();
	OmnSoNatPtr nat = mHash.get(addr, bubbleId, true);
	mLock->unlock();

	// 
	// Remove it from the database
	//
	OmnSoNat so;
	so.removeNat(addr, bubbleId);
	return true;
}


OmnSoNatPtr
OmnNatTable::getNat(const int bubbleId, const OmnIpAddr &addr)
{
	OmnTraceDb << "To retrieve NAT: " 
		<< addr.toString() << ":" << bubbleId << endl;

	mLock->lock();
	OmnSoNatPtr nat = mHash.get(bubbleId, addr.getIPv4(), false);
	if (!nat.isNull())
	{
		mLock->unlock();
		OmnTraceDb << "Found NAT" << endl;
		return nat;
	}


	//
	// Not found in memory. Check the database. 
	//
	mLock->unlock();
	OmnSoNat so;
	nat = so.getNat(addr, bubbleId);
	if (nat.isNull())
	{
		//
		// This means the endpoints is not defined in the system.
		//	
		OmnString err = "NAT not found: ";
			err << "Network ID: " << bubbleId
			<< ", Address: " << addr.toString();
		OmnTraceDb << err << endl;
		return 0;
	}

	//
	// Retrieved the NAT. Add it to the hash table
	//
	OmnTraceDb << "Retrieved the NAT!" << endl;
	mLock->lock();
	mHash.add(addr, bubbleId, nat);
	mLock->unlock();
	return nat;
}


bool
OmnNatTable::isNat(const int bubbleId, const OmnIpAddr &addr)
{
	return !getNat(bubbleId, addr).isNull();
}


bool		
OmnNatTable::addNat(const int bubbleId, const OmnIpAddr &addr)
{
	OmnSoNatPtr so = getNat(bubbleId, addr);
	if (!so.isNull())
	{
		return true;
	}

	so = OmnNew OmnSoNat(bubbleId, addr);
	so->serializeToDb();

	mLock->lock();
	mHash.add(addr, bubbleId, so);
	mLock->unlock();

	return true;
}


OmnRslt         
OmnNatTable::config(const OmnSysObjPtr &def)
{
	return false;
}

