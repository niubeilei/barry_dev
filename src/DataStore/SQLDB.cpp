////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DataStore.cpp
// Description:
//	This class defines Data Store. Data store
//  is an abstraction of storage, such as relational 
//  database, files, shared memory, remote virtual store, etc. To the
//  rest of the system, only OmnDataStore is visible. This class is
//  intended to hide the actual implementation, such as MySQL database,
//  SQL Server, etc.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "DataStore/DataStore.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "DataStore/DbTrans.h"
#include "Debug/Debug.h"
#include "Tracer/Tracer.h"



OmnDataStore::OmnDataStore(const OmnString &name)
:
mSystemSeqno(0),
mNumIds(-1),
mDbName(name)
{
	for (int i=0; i<OmnStoreId::eLastStoreId; i++)
	{
		mSupportedStoreId[i] = false;
	}
}


bool
OmnDataStore::supportStore(const OmnStoreId::E storeId) const
{
	OmnCheckAReturn(storeId > OmnStoreId::eFirstStoreId && 
		storeId < OmnStoreId::eLastStoreId,
		OmnString("Invalid store Id: ") << storeId, false);

	return mSupportedStoreId[storeId];
}


bool
OmnDataStore::setSupportedStoreId(const OmnStoreId::E storeId, const bool flag)
{
	OmnCheckAReturn(storeId > OmnStoreId::eFirstStoreId && 
		storeId < OmnStoreId::eLastStoreId,
		OmnString("Invalid store Id: ") << storeId, false);

	OmnTrace << "Change store support: " << mSupportedStoreId[storeId]
		<< ". New value is: " << flag << endl;
	mSupportedStoreId[storeId] = flag;

	return true;
}


void
OmnDataStore::supportAll()
{
	for (int i=0; i<OmnStoreId::eLastStoreId; i++)
	{
		mSupportedStoreId[i] = true;
	}
}


// Added by Chen Ding, 12/15/2008
bool
OmnDataStore::createTable(const OmnString &stmt)
{
	OmnTrace << "To create table: " << stmt << endl;
	OmnRslt rslt = runSQL(stmt);
	if (rslt) return true;
	return false;
}


// Added by Chen Ding, 12/15/2008
OmnRslt
OmnDataStore::dropTable(const OmnString &tablename)
{
	OmnString stmt = "drop table ";
	stmt << tablename;
	OmnTrace << "To drop table: " << stmt << endl;
	return runSQL(stmt);
}


// Chen Ding, 02/19/2009
bool    
OmnDataStore::tableExist(const OmnString &tablename)
{
	// It checks whether the table exists. 
	OmnString stmt = "select table_name from information_schema.tables where "
		"table_schema = '";
	stmt << mDbName << "' AND table_name = '" << tablename << "'";

	OmnDbTablePtr table;
	OmnRslt rslt = query(stmt, table);

	if (!rslt || table.isNull())
	{
		OmnAlarm << "Failed to retrieve table names. Most likely, the "
			<< "database name is not correct: " << mDbName << enderr;
		return false;
	}

	return (table->entries() > 0);
}


int
OmnDataStore::getAllTableNames(std::list<std::string> &names)
{
	OmnString stmt = "select table_name from information_schema.tables where "
		"table_schema ='";
	stmt << mDbName << "'";

	OmnDbTablePtr table;
	OmnRslt rslt = query(stmt, table);

	if (!rslt || table.isNull())
	{
		OmnAlarm << "Failed to retrieve table names. Most likely, the "
			<< "database name is not correct: " << mDbName << enderr;
		return -1;
	}

	table->reset();
	OmnDbRecordPtr record;
	while (table->hasMore())
	{
		record = table->next();
		names.push_back(record->getStr(0, "", rslt).data());
	}

	return table->entries();
}


int
OmnDataStore::getColumnNames(const OmnString &tablename, 
		std::list<std::string> &names)
{
	OmnString stmt = "select COLUMN_NAME from information_schema.columns ";
	stmt << "where table_schema = '"
		<< mDbName 
		<< "' and table_name = '"
		<< tablename 
		<< "'";
	OmnDbTablePtr table;
	OmnRslt rslt = query(stmt, table);

	if (!rslt || table.isNull())
	{
		OmnAlarm << "Failed to retrieve column names. It can be either "
			<< "the database name (" << mDbName 
			<< ") is incorrect or the table name("
			<< tablename << ") is incorrect" << enderr;
		return -1;
	}

	table->reset();
	OmnDbRecordPtr record;
	while(table->hasMore())
	{
		record = table->next();
		names.push_back(record->getStr(0, "", rslt).data());
		OmnScreen << "column: " << record->getStr(0, "", rslt) << endl;
	}
	return table->entries();
}


u64
OmnDataStore::getTransId()
{
	// This function returns a new transaction ID. It is guaranteed
	// that each time this function call, it will return a different
	// transaction ID. The last transaction ID is stored in the table
	// 'systemids', in the field "trans_id". 
	u64 transid;
	getDbId("trans_id", transid);
	return transid;
}	


u64
OmnDataStore::getImageSeqno()
{
	mIdLock.lock();
	if (mImageSeqno == 0 || mImgNumIds <= 0)
	{
		if (!getDbId("imgseqno", mImageSeqno, 
				eImageSeqnoBatchSize))
		{
			mIdLock.unlock();
			OmnAlarm << "Failed to retrieve image seqno!" << enderr;
			return 0;
		}
		mImgNumIds = eImageSeqnoBatchSize;
	}

	u64 id = mImageSeqno++;
	mImgNumIds--;
	mIdLock.unlock();
	return id;
}

u64
OmnDataStore::getSystemSeqno()
{
	mIdLock.lock();
	if (mSystemSeqno == 0 || mNumIds <= 0)
	{
		if (!getDbId("system_seqno", mSystemSeqno, 
				eSystemSeqnoBatchSize))
		{
			mIdLock.unlock();
			OmnAlarm << "Failed to retrieve system seqno!" << enderr;
			return 0;
		}
		mNumIds = eSystemSeqnoBatchSize;
	}

	u64 id = mSystemSeqno++;
	mNumIds--;
	mIdLock.unlock();
	return id;
}


bool
OmnDataStore::storeTransData(const OmnString &data, u32 &transId)
{
	AosDbTrans trans;
	return trans.storeTransData(data, transId);
}


bool			
OmnDataStore::recordExist(
		const OmnString &fname, 
		const OmnString &value, 
		const OmnString &tname)
{
	// This function assumes 'fieldname' is a primary key. It checks whether
	// the record identified by 'value' exists. 
	OmnString stmt = "select ";
	stmt << fname << " from " << tname << " where "
		<< fname << " = '" << value << "'";

	OmnDbRecordPtr rcd;
	query(stmt, rcd);
	if (rcd) return true;
	return false;
}


bool
OmnDataStore::insertEmptyRecord(const OmnString &tablename, u64 &seqno) 
{
	// This function assumes the table has a field whose values is 
	// auto incremented bigint field. All other fields can be
	// null. This function will insert an empty record into the table
	// and retrieve the seqno for the record. 
	OmnString stmt = "insert into ";
	stmt << tablename << " () values ()";
	aos_assert_r(runSQL(stmt), false);

	stmt = "select last_insert_id()";
	OmnDbRecordPtr record;
	OmnRslt rslt = query(stmt, record);
	aos_assert_r(rslt, false);
	aos_assert_r(record, false);
	seqno = record->getU64(0, 0, rslt);
	return true;
}

