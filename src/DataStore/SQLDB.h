////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 2014/02/06 Copied from DataStore by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_DataStore_SQLDB_h
#define Omn_DataStore_SQLDB_h

#include "Database/Ptrs.h"
#include "SQLDB/StoreType.h"
#include "SQLDB/StoreId.h"
#include "SQLDB/Ptrs.h"
#include "Debug/Rslt.h"
#include "Thread/Mutex.h"
#include "Util/RCObject.h"
#include "Util/String.h"
#include <list>
#include <string>



class AosSQLDB : virtual public OmnRCObject
{
	OmnDefineRCObject;

protected:
	OmnString		mDbType;
	OmnString		mDbName;
	OmnString		mHostname;
	int				mPort;
	OmnString		mUsername;
	OmnString		mPassword;

public:
	AosSQLDB(const AosString &db_type,
			const OmnString &name, 
			const OmnString &host,
			const int port, 
			const OmnString &username, 
			const OmnString &password);
	virtual ~AosSQLDB() {}

	virtual bool openDb(const AosRundataPtr &rdata);
	virtual bool closeDb(const AosRundataPtr &rdata);
	virtual bool runSQL(const AosRundataPtr &rdata, const OmnString &query);
	virtual bool tableExist(const AosRundataPtr &rdata, const OmnString &tablename);

	virtual bool query(		const AosRundataPtr &rdata, 
							const OmnString &query, 
							AosDataRecordPtr &r);

	virtual bool query(		const AosRundataPtr &rdata, 
							const OmnString &query, 
							AosDataTablePtr &table);

	virtual bool recordExist(
							const AosRundataPtr &rdata,
							const OmnString &fname, 
							const OmnString &value, 
							const OmnString &tname);

	virtual bool dropTable(	const AosRundataPtr &rdata, 
							const OmnString &tableName);

	virtual bool createTable(const AosRundataPtr &rdata, 
							const OmnString &stmt);

	virtual int	getAllTableNames(
							const AosRundataPtr &rdata, 
							vector<OmnString> &names);

	virtual int	getColumnNames(
							const AosRundataPtr &rdata, 
							const OmnString &tablename,
							vector<OmnString> &names);

	OmnString getName() const {return mDbName;}
};
#endif
