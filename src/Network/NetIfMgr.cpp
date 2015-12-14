////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: NetIfMgr.cpp
// Description:
//	This is a singleton class that manages all network interfaces. 
//  Whenever OmnNetInterface instances are created, they should
//  register with this class (by calling addNetInterface(...). 
//  Similarly, when a network interface instance is deleted, 
//  it should remove the instance from this class.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Network/NetIfMgr.h"

#include "Alarm/Alarm.h"
#include "Network/NetIf.h"
#include "Thread/Mutex.h"
#include "Util/IpAddr.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"

OmnNetIfMgr* OmnNetIfMgrSelf = NULL;

OmnNetIfMgr::OmnNetIfMgr()
:
mLock(OmnNew OmnMutex())
{
}


OmnNetIfMgr::~OmnNetIfMgr()
{
}


OmnRslt
OmnNetIfMgr::config(const OmnXmlItemPtr &def)
{
	//
	// It assumes
	//
	//	<NetInterfaceDefs>
	//		<NetInterfaceDef>
	//			<NIID>
	//			<IpAddr>
	//		</NetInterfaceDef>
	//		<NetInterfaceDef>
	//			<NIID>
	//			<IpAddr>
	//		</NetInterfaceDef>
	//		...
	//
	// If it fails to configure, it throws an exception.
	//

	def->reset();
	while (def->hasMore())
	{
		OmnXmlItemPtr item = def->next();
		OmnNetIfPtr net = OmnNew OmnNetIf(item);
		addNetInterface(net);
	}

	return true;
}


bool
OmnNetIfMgr::isNiidDefined(const int niid) const
{
	mLock->lock();
	for (unsigned int i=0; i<mNetIfs.size(); i++)
	{
		if (mNetIfs[i]->getNiid() == niid)
		{
			mLock->unlock();
			return true;
		}
	}

	mLock->unlock();
	return false;
}


OmnNetIfPtr	
OmnNetIfMgr::getNetInterface(const int niid) const
{
	mLock->lock();
	for (unsigned int i=0; i<mNetIfs.size(); i++)
	{
		if (mNetIfs[i]->getNiid() == niid)
		{
			OmnNetIfPtr nif = mNetIfs[i];
			mLock->unlock();
			return nif;
		}
	}

	mLock->unlock();
	return 0;
}


OmnIpAddr
OmnNetIfMgr::getIpAddr(const int niid) const
{
	OmnNetIfPtr itfc = getNetInterface(niid);
	if (itfc.isNull())
	{
		OmnAlarm << "Interface: " << niid << " not found!" << enderr;
		return OmnIpAddr::eInvalidIpAddr;
	}

	return itfc->getIpAddr();
}


OmnRslt					
OmnNetIfMgr::removeNetInterface(const int niid)
{
	mLock->lock();
	for (unsigned int i=0; i<mNetIfs.size(); i++)
	{
		if (mNetIfs[i]->getNiid() == niid)
		{
			//
			// Need to remove the item
			//
			if (i == mNetIfs.size()-1)
			{
				//
				// It is the last element.
				//
				mNetIfs.pop_back();
			}
			else
			{
				//
				// Swap with the last one and then delete the last one.
				//
				mNetIfs[i] = mNetIfs[mNetIfs.size()-1];
				mNetIfs.pop_back();
			}
			mLock->unlock();
			return true;
		}
	}

	mLock->unlock();
	return OmnAlarm << OmnErrId::eAlarmNetworkError
		<< "Network Interface Not Found: " << niid << enderr;
}


OmnRslt					
OmnNetIfMgr::addNetInterface(const OmnNetIfPtr &net)
{
	mLock->lock();
	mNetIfs.push_back(net);
	mLock->unlock();
	return true;
}

