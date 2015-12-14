////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StoreMgr.cpp
// Description:
//	This class (a singleton class) manages all abstract stores. 
//  Each instance of stores is identified by a name (similar to 
//  domain name). When one wants to store data into a store, 
//  it should obtain the store by the name. In order for a store
//  to be accessible through this manager, after the store is
//  created (an instance of OmnAbstStore), it should add the 
//  instance to this class. 
//  
//  It is possible that one store is identified by multiple names.
//  
//  In order to make this class independent of the actual abstract
//  stores, this class does not create stores. The only thing it
//  sees is OmnAbstStore. If an application needs an OmnMySQL store
//  and an OmnRemoteFileStore, it should either create them in main
//  and add them into this class or create a singleton class that 
//  creates these stores and to start the singleton class in main.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "DataStore/StoreMgr.h"

#include "DataStore/DataStore.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "XmlUtil/XmlTag.h"
#include <iostream>

OmnSingletonImpl(OmnStoreMgrSingleton,
                 OmnStoreMgr,
                 OmnStoreMgrSelf,
                "OmnStoreMgr");



OmnStoreMgr::OmnStoreMgr()
:
mLock(OmnNew OmnMutex())
{
}


OmnStoreMgr::~OmnStoreMgr()
{
}


bool			
OmnStoreMgr::start()
{
	return true;
}


bool			
OmnStoreMgr::stop()
{
	return true;
}


bool
OmnStoreMgr::config(const AosXmlTagPtr &def)
{
	return true;
}


OmnDataStorePtr	
OmnStoreMgr::getStore(const OmnStoreId::E storeId)
{
	mLock.lock();
	mStores.reset();
	OmnDataStorePtr store;
	while (mStores.hasMore())
	{
		store = mStores.crtValue();
		mStores.next();

		if (store->supportStore(storeId))
		{
			// 
			// The store does support the store 'storeId'
			//
			mLock.unlock();
			return store;
		}
	}

	mLock.unlock();
	
	// 
	// Didn't find it.
	//
	return 0;
}


OmnRslt
OmnStoreMgr::addStore(const OmnDataStorePtr &store)
{
	mStores.append(store);
	return true;
}

