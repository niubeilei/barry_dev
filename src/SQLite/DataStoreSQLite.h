////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DataStoreSQLite.h
// Description:
//	This is the database for DataStoreDataStoreSQLite.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_SQLite_DataStoreSQLite_h
#define Omn_SQLite_DataStoreSQLite_h


#include "DataStore/DataStore.h"
#include "DataStore/Ptrs.h"
#include "Debug/Rslt.h"
#include "Porting/SQLiteInclude.h"
#include "Thread/Ptrs.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class OmnDataStoreSQLite : public OmnDataStore
{
	OmnDefineRCObject;

private:
	bool			mIsOpened;
	sqlite3			*mSQLiteHandle;
	OmnString		mUserName;
	OmnString		mPassword;
	OmnString		mHostName;
	OmnString		mConfigFileName;
	int				mDbPort;
	OmnMutexPtr		mDbLock;

public:
	OmnDataStoreSQLite(const OmnString &dbName,
             const OmnString &configFileName);
	virtual ~OmnDataStoreSQLite();

	virtual OmnRslt	openDb();
	virtual OmnRslt	closeDb();
	virtual OmnStoreType::E	getType() const;
	virtual OmnRslt	dropTable(const OmnString &tableName);
	virtual OmnRslt	runSQL(const OmnString &query);
	virtual OmnRslt	query(const OmnString &query, OmnDbRecordPtr &raw);
	virtual OmnRslt	query(const OmnString &query, OmnDbTablePtr &raw);

	void			returnDbRslt(char **dbrslt);
	static OmnRslt	startSQLite(const OmnString &dbname);
};
#endif
