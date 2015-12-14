////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DataStoreSQLite.cpp
// Description:
//	This is the database for DataStoreSQLite.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "SQLite/DataStoreSQLite.h"

#include "Alarm/Alarm.h"
#include "DataStore/StoreMgr.h"
#include "SQLite/DbTableSQLite.h"
#include "SQLite/DbRcdSQLite.h"
#include "SQLite/Ptrs.h"
#include "Debug/Debug.h"
#include "Debug/Error.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"

OmnDataStoreSQLite::OmnDataStoreSQLite(
				   const OmnString &dbName,
				   const OmnString &configFileName)
:
OmnDataStore(dbName),
mIsOpened(false),
mConfigFileName(configFileName),
mDbLock(OmnNew OmnMutex())
{
}


OmnDataStoreSQLite::~OmnDataStoreSQLite()
{
	closeDb();
}


OmnRslt	
OmnDataStoreSQLite::startSQLite(const OmnString &dbname)
{
	static OmnStoreMgr * slStoreMgr = OmnStoreMgr::getSelf();

	// 
	// This function is called in main.cpp to add SQLite to OmnStoreMgr.
	// 
	OmnString dbConfigFileName = "";
	OmnDataStoreSQLitePtr sqlite = OmnNew OmnDataStoreSQLite(dbname, dbConfigFileName);
	sqlite->supportAll();
	slStoreMgr->addStore(sqlite.getPtr());
	OmnRslt rslt = sqlite->openDb();
	if (!rslt)
	{
		OmnAlarm << "Failed to open the database: " << dbname << enderr;
	}

	return rslt;
}


OmnRslt
OmnDataStoreSQLite::openDb()
{
	//
	// 1. Check whether the database has been opened. If yes, return true.
	// 2. If not, open the database. If successful, set mIsOpened to true
	//    and return true. Otherwise, report problems and return false.
	//
OmnTrace << "=================== Open SQLite: " << mDbName << endl;	
	//
	// Connect to the database
	//
	int rc = sqlite3_open( mDbName.data(), &mSQLiteHandle);
	if (rc) 
	{
	    mIsOpened = false;
		OmnAlarm << "Failed to open SQLite database: " << rc 
			<< ". DBName: " << mDbName << enderr;
		return false;
	}
	else 
	{
	    mIsOpened = true;
	}
	//
	// Database initialized successfully
	//
	return true;
}


OmnRslt		
OmnDataStoreSQLite::closeDb()
{
	//
	// 1. If database is not opened, report error and return false.
	// 2. Close the databse. If unsuccessful, return false. Otherwise,
	//    set mIsOpened to false. and return true.
	//
	
	mDbLock->lock();
	if (!mIsOpened)
	{
	    //
	    // Database is not opened
	    //
	    mDbLock->unlock();
	    return true;
	}
	
	//
	// Close the database
	//
	sqlite3_close(mSQLiteHandle);
	mIsOpened = false;
	
	mDbLock->unlock();
	return true;
}


OmnRslt
OmnDataStoreSQLite::dropTable(const OmnString &tableName)
{
	//
	// 1. Use the tablename to drop the table. If successful, return true.
	//    Otherwise, report problem and return false.
	//
	return runSQL(OmnString("drop table ") << tableName);
}



OmnRslt
OmnDataStoreSQLite::runSQL(const OmnString &query)
{
	char * errMsg = 0;
	mDbLock->lock();
	int rslt = sqlite3_exec(mSQLiteHandle, query, 0, 0, &errMsg);
	if (rslt)
	{
	    //
	    // Failed to query the database.
	    //
	    mDbLock->unlock();
	    return false;
	}
	
	//
	// Otherwise, it ran the SQL successfully
	//
	mDbLock->unlock();
	return true;
}


OmnRslt
OmnDataStoreSQLite::query(const OmnString &query, OmnDbTablePtr &raw)
{
	//
	// Query the database. If successful, return the results in 
	// 'dbrslt' and return true. If failed, report the problem and
	// return false.
	//
	// IMPORTANT: the caller must delete the memory after finishing
	// using the data to avoid memory leak.
	//
	
	char **dbrslt = 0;
	int  numRows = 0;
	int  numFields = 0;
	char *errMsg = 0;
	
	mDbLock->lock();
	int rslt = sqlite3_get_table(mSQLiteHandle, query.data(), &dbrslt, &numRows, &numFields, &errMsg);

	/*
	int  i = 0, j=0, k=0;
	if (numRows) {
	    printf("numRows=%i, numFields=%i\n",numRows, numFields);
	    for (i=0; i<=numRows; i++) {
	        for (j=0; j<numFields; j++) {
	            printf("%s|",dbrslt[k++]);
	        }
	        printf("\n");
	    }
	}*/
	if (rslt)
	{
	    //
	    // Failed to query the database.  
	    //
	    mDbLock->unlock();
	    return OmnAlarm << OmnErrId::eAlarmDatabaseError
	        << "Failed to query database: "
	        << query << ". Rslt: " << rslt << enderr;
	}
	
	//
	// Query successful
	//
	mDbLock->unlock();
	
OmnTrace << "numRows[" << numRows << "] numFields[" << numFields << "]" << " mIsOpen[" << mIsOpened << "]" << endl;
	raw = OmnNew OmnDbTableSQLite(dbrslt, numRows, numFields);
	
	return true;

}


OmnRslt
OmnDataStoreSQLite::query(const OmnString &query, OmnDbRecordPtr &record)
{
	
	//
	// Query the database. If successful, return the results in
	// 'dbrslt' and return true. If failed, report the problem and
	// return false.
	//
	// IMPORTANT: the caller must delete the memory after finishing
	// using the data to avoid memory leak.
	//
	char **dbrslt = 0;
	int  numRows = 0;
	int  numFields = 0;
	char *errMsg = 0;
	
	mDbLock->lock();
	int rslt = sqlite3_get_table(mSQLiteHandle, query, &dbrslt, &numRows, &numFields, &errMsg);
	
	if (rslt)
	{   
	    //
	    // Failed to query the database.
	    //
	    mDbLock->unlock();
	    OmnAlarm << OmnErrId::eFailedToQueryDB
	       << "Failed to query table. " << enderr;
	    return false;
	}
	
	//
	// Query successful
	//
	mDbLock->unlock();
	
	OmnDbTablePtr r = OmnNew OmnDbTableSQLite(dbrslt, numRows, numFields);
	
	r->reset();
	if (r->hasMore())
	{   
	    record = r->next();
		record->setFlag(true);
		r->setFlag(false);
	    
	    //
	    // It used to be a problem here since once we return, 'r' goes out of
	    // scope, which will delete the table. If the table is deleted, 
	    // the record is deleted, too. To prevent from this happening, 
	    // the record not keeps a pointer to the table. 
	    // So it is safe to return here.
	    //
	    return true;
	}
	
	//
	// Otherwise, it is an error. 
	//
	//return OmnErr << "Failed to retrieve anything from: " 
	//  << query << enderr;
	return false;
}

void
OmnDataStoreSQLite::returnDbRslt(char **dbrslt)
{
	//
	// Delete the memory. After calling this function, using
	// 'dbrslt' will cause program core dump!!!
	//
    sqlite3_free_table(dbrslt);
}


OmnStoreType::E	
OmnDataStoreSQLite::getType() const
{
	return OmnStoreType::eSQLite;
}
