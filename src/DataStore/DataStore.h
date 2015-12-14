////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DataStore.h
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

#ifndef Omn_DataStore_DataStore_h
#define Omn_DataStore_DataStore_h

#include "Database/Ptrs.h"
#include "DataStore/StoreType.h"
#include "DataStore/StoreId.h"
#include "DataStore/Ptrs.h"
#include "Debug/Rslt.h"
#include "Thread/Mutex.h"
#include "Util/RCObject.h"
#include "Util/String.h"
#include <list>
#include <string>



class OmnDataStore : virtual public OmnRCObject
{
public:
	enum
	{
		eSystemSeqnoBatchSize = 10,
		eImageSeqnoBatchSize = 10
	};

protected:
	u64				mSystemSeqno;
	int				mNumIds;
	u64				mImageSeqno;
	int				mImgNumIds;
	OmnMutex		mIdLock;
	OmnString		mDbName;
	bool			mSupportedStoreId[OmnStoreId::eLastStoreId];

public:
	OmnDataStore(const OmnString &name);
	virtual ~OmnDataStore() {}

	virtual OmnStoreType::E	getType() const = 0;
	virtual OmnRslt		openDb() = 0;
	virtual OmnRslt		closeDb() = 0;

	virtual OmnRslt		query(const OmnString &query, OmnDbRecordPtr &r) = 0;
	virtual OmnRslt		query(const OmnString &query, OmnDbTablePtr &table) = 0;
	virtual OmnRslt		runSQL(const OmnString &query) = 0;
	virtual bool 		getDbId(
							const OmnString &idname,
							u64 &start, 
							const u32 size = 1) = 0;

	// Chen Ding, 02/20/2009
	virtual bool	getNextSeqno(const OmnString &tablename, u64 &seqno) = 0;
	virtual bool	insertEmptyRecord(const OmnString &tablename, u64 &seqno);

	// Chen Ding, 02-19-2009
	virtual bool	tableExist(const OmnString &tablename);
	bool			recordExist(const OmnString &fname, 
						const OmnString &value, 
						const OmnString &tname);

	virtual OmnRslt	dropTable(const OmnString &tableName);
	bool			createTable(const OmnString &stmt);
	int				getAllTableNames(std::list<std::string> &names);
	int				getColumnNames(const OmnString &tablename,
						std::list<std::string> &names);

	bool			supportStore(const OmnStoreId::E storeId) const;
	bool			setSupportedStoreId(const OmnStoreId::E storeId, const bool);
	void			supportAll();
	OmnString		getName() const {return mDbName;}
	u64				getTransId();
	u64				getSystemSeqno();
	u64				getImageSeqno();
	bool			storeTransData(const OmnString &data, u32 &transId);
	virtual OmnRslt insertBinary(const OmnString &query, u64 &transid, int &tt, int &xml_length, const OmnString &ss) = 0;
	virtual OmnRslt insertBinary1(const OmnString &query, int &length, const OmnString &ss) = 0;
};
#endif
