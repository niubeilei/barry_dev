////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DataStoreMySQL.cpp
// Description:
//	This is the database for DataStoreMySQL.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "MySQL/DataStoreMySQL.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataStore/StoreMgr.h"
#include "MySQL/DbTableMySql.h"
#include "MySQL/DbRcdMySql.h"
#include "Debug/Debug.h"
#include "Debug/Error.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"


static OmnDataStoreMySQLPtr sgMySql;

OmnDataStoreMySQL::OmnDataStoreMySQL(const OmnString &userName,
				   const OmnString &password,
				   const OmnString &hostName,
				   const OmnString &dbName,
				   const int dbPort,
				   const OmnString &configFileName)
:
OmnDataStore(dbName),
mIsOpened(false),
mUserName(userName),
mPassword(password),
mHostName(hostName),
mConfigFileName(configFileName),
mDbPort(dbPort),
mDbLock(OmnNew OmnMutex())
{
}


OmnDataStoreMySQL::~OmnDataStoreMySQL()
{
	closeDb();
}


bool
OmnDataStoreMySQL::startMySQL(int argc, char **argv)
{
	OmnString dbname, dbusername, dbpasswd;
	OmnString dbaddr = "127.0.0.1";
	int dbport = 3306;

	int index = 1;
	while (index < argc)
	{
		if (strcmp(argv[index], "-dbname") == 0 && index < argc-1)
		{
			dbname = argv[index+1];
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-dbuname") == 0 && index < argc-1)
		{
			dbusername = argv[index+1];
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-dbpasswd") == 0 && index < argc-1)
		{
			dbpasswd = argv[index+1];
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-dbaddr") == 0 && index < argc-1)
		{
			dbaddr = argv[index+1];
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-dbport") == 0 && index < argc-1)
		{
			dbport = atoi(argv[index+1]);
			index += 2;
			continue;
		}
		index ++;
	}

	// The database may be Not crreated!
	/*
	if (dbname == "")
	{
		OmnAlarm << "Missing database name" << enderr;
		exit(-1);
	}
	*/

	if (dbusername == "")
	{
		OmnAlarm << "Missing database username" << enderr;
		//exit(-1);
	}

	if (dbpasswd == "")
	{
		OmnAlarm << "Missing database passwd" << enderr;
		//exit(-1);
	}

	bool rslt = startMySQL(dbusername, dbpasswd, dbaddr, dbport, dbname);
	if (!rslt)
	{
		OmnAlarm << "Failed connecting to the database: "
			<< "Username: " << dbusername
			<< ", password: " << dbpasswd
			<< ", addr: " << dbaddr
			<< ", port: " << dbport
			<< ", database name: " << dbname << enderr;
		//exit(-1);
	}

	return true;
}


bool
OmnDataStoreMySQL::startMySQL(
		const OmnString &username, 
		const OmnString &passwd, 
		const OmnString &hostname,			
		const int dbPort,
		const OmnString &dbname)
{
	static OmnStoreMgr * slStoreMgr = OmnStoreMgr::getSelf();

	// 
	// This function is called in main.cpp to add MySQL to OmnStoreMgr.
	// 
	// OmnString hostname = "localhost";
	// int dbPort = 3306;
	OmnString dbConfigFileName = "";
	sgMySql = OmnNew OmnDataStoreMySQL(
		username, passwd, hostname, dbname, 
		dbPort, dbConfigFileName);
	sgMySql->supportAll();
	slStoreMgr->addStore(sgMySql.getPtr());
	sgMySql->lockDb();
	OmnRslt rslt = sgMySql->openDb();
	sgMySql->unlockDb();
	return rslt;
}


void
OmnDataStoreMySQL::lockDb()
{
	mDbLock->lock();
}

void
OmnDataStoreMySQL::unlockDb()
{
	mDbLock->unlock();
}


OmnRslt
OmnDataStoreMySQL::openDb()
{
	//
	// 1. Check whether the database has been opened. If yes, return true.
	// 2. If not, open the database. If successful, set mIsOpened to true
	//    and return true. Otherwise, report problems and return false.
	//
	// !!! IMPORTANT: The caller should have locked the db.

	//
	// Connect to the database
	//
	mysql_init(&mMySqlHandle);
	mMySqlHandle.options.compress = 0;		// no compress
	mMySqlHandle.options.connect_timeout = 60; 	// timeout after 60 seconds

	// Set reconnect option
	char value = 1;
	mysql_options(&mMySqlHandle, MYSQL_OPT_RECONNECT, (char *)&value);
			 
	//
	// Use 'configFileName' to configure the database connection
	//
	//mysql_options(&mMySqlHandle, MYSQL_READ_DEFAULT_FILE, mConfigFileName);
	mysql_options(&mMySqlHandle, MYSQL_READ_DEFAULT_FILE, mConfigFileName.data());
//	mysql_options(&mMySqlHandle, MYSQL_SET_CHARSET_NAME, "utf8");
    
	//
	// Ready to connect to the database
	//
OmnTrace << "To reconnect db: " << mHostName 
	<< ":" << mUserName << ":" << mPassword << ":" << mDbName 
	<< ":" << mDbPort << endl;

	if (mysql_real_connect(&mMySqlHandle, 
			   	//	HostName, mUserName, mPassword, NULL, 
				//  mHostName.data(), mUserName.data(), mPassword.data(), mDbName.data(), 
					mHostName.data(), mUserName.data(), mPassword.data(), NULL,
					mDbPort, 0, 0))
    {
		//
		// Database connected successfully
		//
		if(!mysql_set_character_set(&mMySqlHandle, "utf8"))
		{
		    printf("New client character set: %s\n",
		               mysql_character_set_name(&mMySqlHandle));
		}

		if (mDbName != "")
		{
			OmnString cmd = "use ";
			cmd << mDbName;
			if (mysql_query(&mMySqlHandle, cmd.data())==0)
			{
        		mIsOpened = true;
				return true;
			}
			else 
			{
				cmd = "create database ";
				cmd << mDbName;
				if (mysql_query(&mMySqlHandle, cmd.data())==0)
				{
					mIsOpened = true;
					return true;
				}
			}
		}
    }
	//
	// Failed to connect to the database. This is a serious problem.
	//
	mIsOpened = false;
	OmnString error = mysql_error(&mMySqlHandle);
	return OmnAlarm << "Failed to connect to database: " 
		<< "hostname: " << mHostName << ", UseName: " << mUserName
		<< ", Password: " << mPassword << ", DbName: " << mDbName
		<< ", DbPort: " << mDbPort
		<< ", ERROR: " << error << enderr;
}


OmnRslt		
OmnDataStoreMySQL::closeDb()
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
	mysql_close(&mMySqlHandle);

	mIsOpened = false;
	
	mDbLock->unlock();
	return true;
}


OmnRslt
OmnDataStoreMySQL::dropTable(const OmnString &tableName)
{
	//
	// 1. Use the tablename to drop the table. If successful, return true.
	//    Otherwise, report problem and return false.
	//
	return runSQL(OmnString("drop table ") << tableName);
}



OmnRslt
OmnDataStoreMySQL::runSQL(const OmnString &query)
{
	mDbLock->lock();
const char *data = query.data();
	//int rslt = mysql_query(&mMySqlHandle, data);
	int rslt = mysql_real_query(&mMySqlHandle, data, query.length());
    if (rslt)
    {
        //
        // Failed to query the database.
        //
        mDbLock->unlock();
        return OmnAlarm << "Failed To Run SQL: " << query << enderr;
    }

	//
	// Otherwise, it ran the SQL successfully
	//
	mDbLock->unlock();
	return true;
}


OmnRslt
OmnDataStoreMySQL::query2(const OmnString &query, OmnDbTablePtr &raw)
{
	mDbLock->lock();
	int rslt = mysql_query(&mMySqlHandle, query.data()); 
	if (rslt)
	{
		//
		// Failed to query the database.  
		//
		/*
		OmnAlarm << "Failed database query. Trying to reconnect" << enderr;
		bool rr = openDb();
		if (!rr || !mysql_query(&mMySqlHandle, query))
		{
			mDbLock->unlock();
			return OmnAlarm << "Failed to query database: " 
				<< query <<":"<<mysql_error(&mMySqlHandle) <<enderr;
		}
		OmnTrace << "Reconnected!" << endl;
		*/
		mDbLock->unlock();
		OmnAlarm << "Failed database query: " << query << enderr;
		return false;
	}

	//
	// Query successful
	//

	MYSQL_RES *dbrslt = mysql_use_result(&mMySqlHandle);
	mDbLock->unlock();
	OmnDbTableMySql *raws  = OmnNew OmnDbTableMySql(dbrslt);
	raws->setMysqlHandle(&mMySqlHandle);
	raw = raws;
	return true;
}


OmnRslt
OmnDataStoreMySQL::query(const OmnString &query, OmnDbTablePtr &raw)
{
	//
	// Query the database. If successful, return the results in 
	// 'dbrslt' and return true. If failed, report the problem and
	// return false.
	//
	// IMPORTANT: the caller must delete the memory after finishing
	// using the data to avoid memory leak.
	//
	
	mDbLock->lock();
	int rslt = mysql_query(&mMySqlHandle, query.data());
	if (rslt)
	{
		//
		// Failed to query the database.  
		//
		/*
		OmnAlarm << "Failed database query. Trying to reconnect" << enderr;
		bool rr = openDb();
		if (!rr || !mysql_query(&mMySqlHandle, query))
		{
			mDbLock->unlock();
			return OmnAlarm << "Failed to query database: " 
				<< query <<":"<<mysql_error(&mMySqlHandle) <<enderr;
		}
		OmnTrace << "Reconnected!" << endl;
		*/
		mDbLock->unlock();
		OmnAlarm << "Failed database query: " << query << enderr;
		return false;
	}

	//
	// Query successful
	//
	MYSQL_RES *dbrslt = mysql_store_result(&mMySqlHandle);
	mDbLock->unlock();
	raw = OmnNew OmnDbTableMySql(dbrslt);
	return true;
}


OmnRslt
OmnDataStoreMySQL::query(const OmnString &query, OmnDbRecordPtr &record)
{
    //
    // Query the database. If successful, return the results in
    // 'dbrslt' and return true. If failed, report the problem and
    // return false.
    //
    // IMPORTANT: the caller must delete the memory after finishing
    // using the data to avoid memory leak.
    //

    mDbLock->lock();
    int rslt = mysql_query(&mMySqlHandle, query.data());
    if (rslt)
    {
        //
        // Failed to query the database.
        //
		/*
		OmnAlarm << "Failed database query. Trying to reconnect" << enderr;
		bool rr = openDb();
		if (!rr || !mysql_query(&mMySqlHandle, query))
		{
        	mDbLock->unlock();
        	return OmnAlarm << "Failed To Query DB: " 
				<< query << ":" << mysql_error(&mMySqlHandle) << enderr;
		}
		OmnTrace << "Reconnected" << endl;
		*/
		mDbLock->unlock();
		OmnAlarm << "Failed database query: " << query << enderr;
		return false;
    }

    //
    // Query successful
    //
    MYSQL_RES *dbrslt = mysql_store_result(&mMySqlHandle);
    mDbLock->unlock();

    OmnDbTablePtr r = OmnNew OmnDbTableMySql(dbrslt);

	r->reset();
	if (r->hasMore())
	{
		record = r->next();

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
	// Otherwise, the object is not in the database. Note that this doesn't mean
	// errors. As far as the database is concerned, it is perfect ok.
	//
	return true;
}

void
OmnDataStoreMySQL::returnDbRslt(MYSQL_RES *dbrslt)
{
	//
	// Delete the memory. After calling this function, using
	// 'dbrslt' will cause program core dump!!!
	//
	mysql_free_result(dbrslt);
}


OmnStoreType::E	
OmnDataStoreMySQL::getType() const
{
	return OmnStoreType::eMySQL;
}


bool    
OmnDataStoreMySQL::getNextSeqno(const OmnString &tablename, u64 &seqno)
{
	// It assumes that the table does not have a primary key. Or in other
	// word, a null record can be inserted into the table by the following:
	// 		insert into <tablename> () values ();
	// In addition, it assumes there is an auto_incremented field. 
	// After running the above statement, it will run:
	// 		select last_insert_id();
	// It shall return the last seqno.
	OmnString stmt = "insert into ";
	stmt << tablename << " () values ()";

	aos_assert_r(runSQL(stmt), false);
	stmt = "select last_insert_id()";
	OmnDbRecordPtr rcd;
	aos_assert_r(query(stmt, rcd), false);
	aos_assert_r(rcd, false);
	OmnRslt rslt;
	seqno = rcd->getU64(0, 0, rslt);
	aos_assert_r(rslt, false);
	return true;
}


bool
OmnDataStoreMySQL::getDbId(
		const OmnString &idname,
		u64 &start, 
		const u32 size)
{
	// It assumes there is a table named "systemids" that has two fields:
	// 		"id" and "name"
	// Each record in the table defines a global integer. This function
	// will increment the corresponding "id" field whose "name" matches 
	// "idname". Each time this function increment 'size'. 
	//
	OmnString stmt = "update systemids set id=last_insert_id(id+";
	stmt << size << ") where " << "name='" << idname << "'";
	OmnRslt rslt = runSQL(stmt);
	aos_assert_r(rslt, "");
	OmnDbRecordPtr record;
	stmt = "select last_insert_id()";
	rslt = query(stmt, record);
	aos_assert_r(rslt, "");
	aos_assert_r(record, "");
	u64 end = record->getU64(0, 0, rslt);
	aos_assert_r(end > 0, false);
	start = end - size + 1;
	return true;
}

OmnRslt
OmnDataStoreMySQL::insertBinary(
			const OmnString &query,
			u64 &transid,
			int &tt,
			int &xml_length,
			const OmnString &ss)
{
	//http://dev.mysql.com/doc/refman/5.0/en/mysql-stmt-execute.html
	MYSQL_STMT    *stmt;
	MYSQL_BIND	 bind[4];
	mDbLock->lock();
	stmt = mysql_stmt_init(&mMySqlHandle);
	if (!stmt)
	{
        mDbLock->unlock();
        return OmnAlarm << "Failed To Run SQL: " << query << enderr;
	}

	int rslt  = mysql_stmt_prepare(stmt, query.data(), query.length());
	if (rslt)
	{
        mDbLock->unlock();
        return OmnAlarm << "mysql_stmt_prepare(), INSERT failed. " 
			<< mysql_stmt_error(stmt)<< enderr;
	}

	int param_count = mysql_stmt_param_count(stmt);
	if (param_count != 4)
	{
		mDbLock->unlock();
		return OmnAlarm << "invalid parameter count returned by MySQL" << enderr;
	}
	
	//my_bool is_null = 1;
	memset(bind, 0, sizeof(bind));
	bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
	bind[0].buffer= (char *) &transid;
	bind[0].is_null= 0;//&is_null;
	bind[0].is_unsigned = 1;//&is_null;
	bind[0].length= 0;

	bind[1].buffer_type = MYSQL_TYPE_LONG;
	bind[1].buffer= (char *) &tt;
	bind[1].is_null= 0;//&is_null;
	bind[1].length= 0;

	bind[2].buffer_type = MYSQL_TYPE_LONG;
	bind[2].buffer= &xml_length;
	bind[2].is_null= 0;//&is_null;
	bind[2].length= 0;

	unsigned long ss_length = xml_length;
	bind[3].buffer_type = MYSQL_TYPE_BLOB;
	bind[3].buffer = (char *)ss.data();
	bind[3].buffer_length = xml_length;
	bind[3].is_null= 0;//&is_null;
//	bind[3].length= (unsigned long *)&xml_length;
	bind[3].length= &ss_length;
	
	rslt = mysql_stmt_bind_param(stmt, bind);
	if (rslt)
	{
        mDbLock->unlock();
        return OmnAlarm << "Failed To Run SQL: " << query << enderr;
	}

	rslt = mysql_stmt_execute(stmt);
	if (rslt)
	{
        mDbLock->unlock();
        OmnAlarm << "mysql_stmt_execute(), 1 failed. " 
			<< mysql_stmt_error(stmt)<< enderr;
		return false;
	}

	if (mysql_stmt_close(stmt))
	{
		mDbLock->unlock();
		OmnAlarm << "failed while closing the statement ."
			<< mysql_stmt_error(stmt) << enderr;
		return false;
	}
	mDbLock->unlock();
	return true;
}

OmnRslt
OmnDataStoreMySQL::insertBinary1(
			const OmnString &query,
			int &length,
			const char *ss)
{
	MYSQL_STMT    *stmt;
	MYSQL_BIND	 bind[2];
	mDbLock->lock();
	stmt = mysql_stmt_init(&mMySqlHandle);
	if (!stmt)
	{
        mDbLock->unlock();
        return OmnAlarm << "Failed To Run SQL: " << query << enderr;
	}

	int rslt  = mysql_stmt_prepare(stmt, query.data(), query.length());
	if (rslt)
	{
        mDbLock->unlock();
        return OmnAlarm << "mysql_stmt_prepare(), INSERT failed. " 
			<< mysql_stmt_error(stmt)<< enderr;
	}

	int param_count = mysql_stmt_param_count(stmt);
	if (param_count != 2) 
	{
		mDbLock->unlock();
		return OmnAlarm << "invalid parameter count returned by MySQL" << enderr;
	}
	
	memset(bind, 0, sizeof(bind));

	bind[0].buffer_type = MYSQL_TYPE_LONG;
	bind[0].buffer= &length;
	bind[0].is_null= 0;
	bind[0].length= 0;

	unsigned long str_length = length;
	bind[1].buffer_type = MYSQL_TYPE_BLOB;
	// bind[1].buffer = (char *)ss.data();
	bind[1].buffer = (char *)ss;
	bind[1].buffer_length = length; 
	bind[1].is_null= 0;
	//bind[1].length= (unsigned long )&length;
	bind[1].length= &str_length;
	
	rslt = mysql_stmt_bind_param(stmt, bind);
	if (rslt)
	{
        mDbLock->unlock();
        return OmnAlarm << "Failed To Run SQL: " << query << enderr;
	}

	rslt = mysql_stmt_execute(stmt);
	if (rslt)
	{
        mDbLock->unlock();
        OmnAlarm << "mysql_stmt_execute(), 1 failed. " 
			<< mysql_stmt_error(stmt)<< enderr;
		return false;
	}

	if (mysql_stmt_close(stmt))
	{
		mDbLock->unlock();
		OmnAlarm << "failed while closing the statement ."
			<< mysql_stmt_error(stmt) << enderr;
		return false;
	}
	mDbLock->unlock();
	return true;
}


u64
OmnDataStoreMySQL::getNumOfRecord()
{
	MYSQL_RES *dbrslt = mysql_store_result(&mMySqlHandle);
	return mysql_num_fields(dbrslt);            
}

