////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DataStoreMySQL.h
// Description:
//	This is the database for DataStoreDataStoreMySQL.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_MySQL_DataStoreMySQL_h
#define Omn_MySQL_DataStoreMySQL_h


#include "DataStore/DataStore.h"
#include "DataStore/Ptrs.h"
#include "Debug/Rslt.h"
#include "Porting/MySqlInclude.h"
#include "Thread/Ptrs.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class OmnDataStoreMySQL : public OmnDataStore
{
	OmnDefineRCObject;

private:
	bool			mIsOpened;
	MYSQL			mMySqlHandle;
	OmnString		mUserName;
	OmnString		mPassword;
	OmnString		mHostName;
	OmnString		mConfigFileName;
	int				mDbPort;
	OmnMutexPtr		mDbLock;

public:
	OmnDataStoreMySQL(const OmnString &userName,
             const OmnString &password,
             const OmnString &hostName,
			 const OmnString &dbName,
             const int dbPort,
             const OmnString &configFileName);
	virtual ~OmnDataStoreMySQL();

	virtual OmnRslt	openDb();
	virtual OmnRslt	closeDb();
	virtual OmnStoreType::E	getType() const;
	virtual OmnRslt	dropTable(const OmnString &tableName);
	virtual OmnRslt	runSQL(const OmnString &query);
	virtual OmnRslt	query(const OmnString &query, OmnDbRecordPtr &raw);
	virtual OmnRslt	query(const OmnString &query, OmnDbTablePtr &raw);
	virtual OmnRslt	query2(const OmnString &query, OmnDbTablePtr &raw);
	virtual u64 getNumOfRecord();

	virtual bool 		getDbId(
							const OmnString &idname,
							u64 &start, 
							const u32 size = 1);

	void			returnDbRslt(MYSQL_RES *dbrslt);
	static bool		startMySQL(int argc, char **argv);
	static bool		startMySQL(
						const OmnString &username, 
					   	const OmnString &passwd, 
						const OmnString &hostname,			
						const int dbPort,
					   	const OmnString &dbname);

	// Chen Ding, 02/20/2009
	virtual bool    getNextSeqno(const OmnString &tablename, u64 &seqno);
	void	lockDb();
	void	unlockDb();
	OmnRslt	insertBinary(
			const OmnString &query,
			u64 &transid,
			int &tt,
			int &xml_length,
			const OmnString &ss);

	OmnRslt	insertBinary1(
			const OmnString &query,
			int &length,
			const char *ss);

	inline OmnRslt	insertBinary1(
			const OmnString &query,
			int &length,
			const OmnString &ss)
	{
		return insertBinary1(query, length, ss.data());
	}
};
#endif
