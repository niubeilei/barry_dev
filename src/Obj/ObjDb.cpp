////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjDb.cpp
// Description:
//	A DbObj is an object that can be serialized to and from a database. 
//  The database can be any type, such as MySQL, Oracle, etc. 
//  
//  This class defines an interface. Any object that wants to be persistent
//  in databases should derive from this class. 
//
//	DbObj can be cached in memory. When such an object is cached, mTimeCreated
//  is set at the time of creation. Each time it is accessed, mTimeAccessed
//  is updated. A housekeeping thread may remove those that are too old and
//  not being accessed for long time to make more space.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Obj/ObjDb.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Database/DbRecord.h"
#include "Database/DbTable.h"
#include "DataStore/DataStore.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h"

bool
OmnDbObj::existInDb(const OmnString &stmt) const
{
	OmnDbRecordPtr record; 
	OmnRslt rslt = retrieveRecord(stmt, record);
	if (!rslt || !record)
	{
		return false;
	}
	return true;
}


OmnRslt
OmnDbObj::addToDb(const OmnString &stmt) const
{
	static OmnStoreMgr *slStoreMgr = OmnStoreMgr::getSelf();

	// 
	// Note: This function does not check whether the object is already
	// in the database. It is the caller's responsibility to make sure
	// of that. If the object is already in the database, the operation
	// will fail and an OmnRslt will be returned.
	//
    OmnDataStorePtr store = slStoreMgr->getStore(mStoreId);

	OmnCheckAReturn(!store.isNull(), 
		OmnString("Specified storage not available available: ") << mStoreId, false);

    OmnTrace << "Run SQL statement: \n" << stmt << endl;
    OmnRslt rslt = store->runSQL(stmt);
	if (rslt)
    {
		OmnTraceDb << "SQL statement Successful! " << endl;
	}
	else
	{
		OmnAlarm << "Failed to add to db. Statement: " 
			<< stmt << ". Errmsg: " << rslt.getErrmsg() << enderr;
	}
	return rslt;
}


OmnRslt
OmnDbObj::updateToDb(const OmnString &stmt) const
{
	static OmnStoreMgr *slStoreMgr = OmnStoreMgr::getSelf();

    OmnDataStorePtr store = slStoreMgr->getStore(mStoreId);
    OmnCheckAReturn(!store.isNull(),
        OmnString("Specified storage not available available: ") << mStoreId, false);

    OmnTrace << "Run SQL statement: \n" << stmt << endl;
    OmnRslt rslt = store->runSQL(stmt);
    if (rslt)
    {
        OmnTraceDb << "SQL statement Successful! " << endl;
    }

    return rslt;
}
	

OmnRslt
OmnDbObj::removeFromDb(const OmnString &stmt) const
{
	static OmnStoreMgr *lsStoreMgr = OmnStoreMgr::getSelf();

	// 
	// This function saves this suite entry into the database. 
	//
    OmnDataStorePtr store = lsStoreMgr->getStore(mStoreId);

	OmnCheckAReturn(!store.isNull(), 
		OmnString("Specified storage not available available: ") << mStoreId, false);

    OmnTraceDb << "Run SQL statement: \n" << stmt << endl;
    OmnRslt rslt = store->runSQL(stmt);
	if (rslt)
    {
		OmnTraceDb << "SQL statement Successful! " << endl;
	}

	return rslt;
}


OmnDataStorePtr
OmnDbObj::getStore() const
{
	static OmnStoreMgr *slStoreMgr = OmnStoreMgr::getSelf();

	return slStoreMgr->getStore(mStoreId);
}


OmnRslt
OmnDbObj::retrieveRecord(const OmnString &stmt, OmnDbRecordPtr &record) const
{
	// 
	// Retrieve the data store.
	//
    OmnDataStorePtr store = OmnStoreMgrSelf->getStore(mStoreId);
	if (store.isNull())
	{
		OmnString errmsg = OmnString("Store: ") << mStoreId << " not found!";
		OmnAlarm << errmsg << enderr;
		return OmnRslt(OmnErrId::eDbError, errmsg);
	}
		
    OmnTraceDb << "Query database: \n" << stmt 
		<< " from store: " << store->getName() << endl;

	// 
	// Query the database. 
	//
    OmnRslt rslt = store->query(stmt, record);
    if (!rslt)
	{
		// 
		// Failed to retrieve. 
		// 
		return rslt;
	}
	
	return true;
}


OmnRslt
OmnDbObj::retrieveRecords(const OmnString &stmt, OmnDbTablePtr &table) const
{
	// 
	// Retrieve the data store.
	//
    OmnDataStorePtr store = OmnStoreMgrSelf->getStore(mStoreId);
	if (store.isNull())
	{
		OmnString errmsg = OmnString("Store: ") << mStoreId << " not found!";
		OmnAlarm << errmsg << enderr;
		return OmnRslt(OmnErrId::eDbError, errmsg);
	}
		
    OmnTraceDb << "Query database: \n" << stmt 
		<< " from store: " << store->getName() << endl;

	// 
	// Query the database. 
	//
    OmnRslt rslt = store->query(stmt, table);
    if (!rslt || table.isNull())
    {
		// 
		// Didn't found the record. 
		//
		OmnRslt rslt(OmnErrId::eObjectNotFound, "Object not found!");
		return rslt;
    }

	return true;
}


bool
OmnDbObj::retrieveFromDb()
{
	OmnString stmt = retrieveStmt();
	return serializeFromDb(stmt);
}


OmnRslt
OmnDbObj::serializeFromDb()
{
	OmnString stmt = retrieveStmt();
	return serializeFromDb(stmt);
}


OmnRslt
OmnDbObj::serializeFromDb(const OmnString &stmt)
{
	// 
	// This function retrieves the object from database. 
	// The caller is responsible for constructing the query 
	// statement. The subclass should also implement the
	// 	serializeFromRecord(record);
	// member function.
	// 
	OmnTraceDb << "To retrieve: " << stmt << endl;

	OmnDbRecordPtr record;
	OmnRslt rslt = retrieveRecord(stmt, record);
	if (!rslt)
	{
		return OmnAlarm << "Failed to retrieve the entry from db: " 
			<< stmt << enderr;
	}

	if (!record)
	{
		// 
		// Didn't find the record
		//
		return OmnRslt(OmnErrId::eObjectNotFound);
	}

	// 
	// Found the record. 
	//
	return this->serializeFromRecord(record);
}


// Added by Chen Ding, 12/15/2008
OmnDbRecordPtr
OmnDbObj::getRecord(const OmnString &stmt)
{
	OmnTrace << "To retrieve record: " << stmt << endl;
	OmnDbTablePtr table;
    OmnRslt rslt = retrieveRecords(stmt, table);
    if (!rslt)
    {
        OmnAlarm << "Failed to retrieve records: "
            << stmt << " from db!" << enderr;
        return 0;
    }

    aos_assert_r(table, 0);
    if (table->entries() > 1)
    {
        OmnAlarm << "Found multiple records: " <<  stmt << enderr;
        return 0;
    }

    if (table->entries() == 0)
    {
        // Did not find the book
        return 0;
    }

    table->reset();
    return table->next();
}


OmnString
OmnDbObj::toString() const
{
	OmnString str;
	str << "\nOmnDbObj"
		<< "\n--------"
		<< "\n    StoreId: " << (int)mStoreId
		<< "\n    TimeCreated: " << (int)mTimeCreated
		<< "\n    TimeAccessed: " << (int)mTimeAccessed;
	return str;
}

OmnRslt		
OmnDbObj::serializeFromRecord(const OmnDbRecordPtr &record)
{
	OmnShouldNeverComeToThisPoint;
	return "";
}

OmnString	
OmnDbObj::insertStmt() const
{
	OmnShouldNeverComeToThisPoint;
	return "";
}

OmnString	
OmnDbObj::updateStmt() const
{
	OmnShouldNeverComeToThisPoint;
	return "";
}

OmnString	
OmnDbObj::removeStmt() const
{
	OmnShouldNeverComeToThisPoint;
	return "";
}

OmnString	
OmnDbObj::retrieveStmt() const
{
	OmnShouldNeverComeToThisPoint;
	return "";
}

OmnString	
OmnDbObj::removeAllStmt() const
{
	OmnShouldNeverComeToThisPoint;
	return "";
}

OmnString	
OmnDbObj::existStmt() const
{
	OmnShouldNeverComeToThisPoint;
	return "";
}

