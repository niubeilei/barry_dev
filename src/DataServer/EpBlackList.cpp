////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: EpBlackList.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "DataServer/EpBlackList.h"

#include "Alarm/Alarm.h"
#include "Database/Database.h"
#include "DataServer/DataMgr.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SysObj/SoMgcpEndpoint.h"
#include "Thread/Mutex.h"
#include "Util/Integer.h"
#include "Util1/Time.h"


OmnSingletonImpl(OmnEpBlackListSingleton,
				 OmnEpBlackList,
				 OmnEpBlackListSelf,
				"OmnEpBlackList");


extern OmnTime *	OmnTimeSelf;


OmnEpBlackList::OmnEpBlackList()
:
mTableLock(OmnNew OmnMutex())
{
}


OmnEpBlackList::~OmnEpBlackList()
{
}


bool
OmnEpBlackList::epAdded(const OmnSoMgcpEndpoint &so)
{
	// 
	// Remove the endpoint from the hash tables, if it is 
	// in the hash tables.
	//
	mTableLock->lock();
	OmnIntegerPtr tick;
	mPubHash.get(so.getPublicEid(), tick, true);
	mPriHash.get(so.getPrivateEid(), tick, true);
	mTableLock->unlock();
	return true;
}


bool
OmnEpBlackList::epModified(const OmnSoMgcpEndpoint &so)
{
	// 
	// If an endpoint wasn't defined (so it got added to this
	// black list) and now it is modified in such a way that
	// that 'endpoint' should be defined. Therefore, we need
	// to remove the endpoint from this black list.
	//
	OmnTraceDb << "Endpoint removed from black list: " 
		<< so.getPublicEid() << endl;
	removeEp(so);
	return true;
}


bool
OmnEpBlackList::epDeleted(const OmnSoMgcpEndpoint &so)
{
	// 
	// Chen Ding, 06/01/2003, Change-0001
	// 
	// mTableLock->lock();
	// OmnIntegerPtr tick = new OmnInteger(OmnTimeSelf->getSecTick());
	// mPubHash.add(so.getPublicEid(), tick);
	// mPriHash.add(so.getPrivateEid(), tick);
	// mTableLock->unlock();	
	return true;
}


void
OmnEpBlackList::addPri(const OmnString &priEid)
{
 	OmnIntegerPtr tick = OmnNew OmnInteger(OmnTimeSelf->getSecTick());
		
	mTableLock->lock();
	mPriHash.add(priEid, tick);
	mTableLock->unlock();
	return;
}


void
OmnEpBlackList::addPub(const OmnString &pubEid)
{
	mTableLock->lock();
	OmnIntegerPtr tick = OmnNew OmnInteger(OmnTimeSelf->getSecTick());
	mPubHash.add(pubEid, tick);
	mTableLock->unlock();
}


bool
OmnEpBlackList::isPriOnList(const OmnString &priEid)
{
	OmnIntegerPtr tick;
	return mPriHash.get(priEid, tick, false);
}


bool
OmnEpBlackList::isPubOnList(const OmnString &pubEid)
{
	OmnIntegerPtr tick;
	return mPubHash.get(pubEid, tick, false);
}


void
OmnEpBlackList::removeEp(const OmnSoMgcpEndpoint &so)
{
	OmnIntegerPtr tick;
	mTableLock->lock();
	mPubHash.get(so.getPublicEid(), tick, true);
	mPriHash.get(so.getPrivateEid(), tick, true);
	mTableLock->unlock();
}

