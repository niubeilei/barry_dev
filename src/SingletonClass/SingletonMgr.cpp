////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SingletonMgr.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "SingletonClass/SingletonMgr.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "SingletonClass/SingletonObj.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlParser.h"

// OmnSingletonMgr * OmnSingletonMgrSelf = new OmnSingletonMgr();



OmnSingletonMgr::OmnSingletonMgr()
:
mLock(OmnNew OmnMutex())
{
}


OmnSingletonMgr::~OmnSingletonMgr()
{
}


OmnRslt			
OmnSingletonMgr::addSingleton(OmnSingletonObj *singleton)
{
	// 
	// SingletonMgr keeps a list of all singletons created so far.
	// This function checks whether the singleton is already
	// in the list. If yes, it returns errors. Otherwise, it adds
	// the singleton into the position in the list according to 
	// the singleton's ID.
	//
	mLock->lock();
	mSingletons.reset();
	while (mSingletons.hasMore())
	{
		if ((mSingletons.crtValue())->getSysObjId() == 
				singleton->getSysObjId())
		{
			// It is already in the list.
			mLock->unlock();
			return OmnAlarm << "To add a singleton creator but already there: " << enderr;
		}

		mSingletons.next();
	}

	//
	// It is not in the list. Need to add it.
	//
	mSingletons.reset();
	int index = 0;
	while (mSingletons.hasMore())
	{
		if (singleton->getSysObjId() < 
						(mSingletons.crtValue())->getSysObjId())
		{
			//
			// Need to insert the entry in front of the current one.
			//
			mSingletons.insertAt(singleton);
			mLock->unlock();
			return true;
		}

		index++;
		mSingletons.next();
	}

	mSingletons.append(singleton);

	mLock->unlock();
	return true;
}


OmnRslt			
OmnSingletonMgr::createSingleton()
{
	//
	// All singleton objects should have been inserted into the list. 
	//
	mLock->lock();
	mSingletons.reset();
	while (mSingletons.hasMore())
	{
		OmnTrace << "To create singleton object: " << endl;
		(mSingletons.crtValue())->createSingleton();	
		mSingletons.next();
	}

	mLock->unlock();
	return true;
}


OmnRslt
OmnSingletonMgr::configSingleton(const AosXmlTagPtr &def)
{
    //
    // All singleton objects should have been created.
    //          
    mLock->lock();
    mSingletons.reset();
    while (mSingletons.hasMore())
    {       
		OmnTrace << "Config singleton: " << endl;

        if (!((mSingletons.crtValue())->config(def)))
		{
			mLock->unlock();
			return OmnWarn << "Failed to configure singleton: " 
				<< (mSingletons.crtValue())->getSysObjName() << enderr;
		}

		mSingletons.next();
    }       
                
    mLock->unlock();
    return true;
}


OmnRslt
OmnSingletonMgr::startSingleton()
{
    //
    // All singleton objects should have been created.
    //
    mLock->lock();
    mSingletons.reset();

	//int index = 0;
    while (mSingletons.hasMore())
    {
		OmnTrace << "To start singleton: "
			<< (mSingletons.crtValue())->getSysObjName() << endl;
			
        if (!((mSingletons.crtValue())->createSingleton()))
        {
            mLock->unlock();
            return false;
        }

		mSingletons.next();
    }

    mLock->unlock();
    return true;
}


OmnRslt
OmnSingletonMgr::stopSingleton()
{
    //
    // All singleton objects should have been created.
    //
    mLock->lock();
    mSingletons.reset();

    while (mSingletons.hasMore())
    {
		OmnTrace << "To stop singleton: "
			<< (mSingletons.crtValue())->getSysObjName() << endl;
			
        (mSingletons.crtValue())->stopSingleton();
		mSingletons.next();
    }

    mLock->unlock();
    return true;
}



OmnRslt			
OmnSingletonMgr::deleteSingleton()	
{
	return false;
}


OmnRslt
OmnSingletonMgr::start(const AosXmlTagPtr &conf)
{
    //
    // Create the singleton classes
    //   
    OmnTrace << "Create singleton classes" << endl;
//    if (!createSingleton())
//    {   
//        OmnAlarm << "Failed to create singletons. Program Exit!!!" << enderr;
//        return false;
//    }
//        
    //
    // Configure singleton classes
    //
//    OmnTrace << "Configure singleton classes" << endl;
//   if (!configSingleton(conf))
//    {
//        OmnAlarm << "Failed to configure singletons. Program Exit!!!" << enderr;
//        return false;
//    }
//
    //
    // Start singletons
    // 
    OmnTrace << "Start singleton classes" << endl;
    if (!startSingleton())
    {
        OmnAlarm << "Failed to start singletons. Program Exit!!!" << enderr;
        return false;
    }

	return true;
}


OmnRslt
OmnSingletonMgr::startSingleton(const OmnSingletonObjId::E objId, 
								const AosXmlTagPtr &conf)
{
	// 
	// Retrieve the object from the list. 
	//
    mLock->lock();
    mSingletons.reset();
	OmnSingletonObj *singleton;
    while (mSingletons.hasMore())
    {
        singleton = mSingletons.next();
		if (singleton->getSysObjId() == objId)
		{
			// Found it. 
			mLock->unlock();
			OmnRslt rslt = singleton->createSingleton();
			if (!rslt)
			{
				// Need to raise an exception
				OmnString errmsg = rslt.toString();
				errmsg << ". Class Name: " 
					<< singleton->getSysObjName();
				OmnExcept e(OmnFileLine, rslt.getErrId(), errmsg);
				throw e;
			}

			return true;
		}
    }
    mLock->unlock();

	// Need to throw an exception
	OmnExcept e(OmnFileLine, OmnErrId::eProgError, OmnString("Singleton: ") << objId << " not found");
	throw e;

	return false;
}
