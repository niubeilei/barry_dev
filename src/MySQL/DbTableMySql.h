////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DbTableMySql.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_MySQL_DbTableMySql_h
#define Omn_MySQL_DbTableMySql_h

#include "Database/DbTable.h"
#include "Util/RCObjImp.h"
#include "Util/BasicTypes.h"
#include "Porting/MySqlInclude.h"


class OmnDbTableMySql : public OmnDbTable
{
	OmnDefineRCObject;

private:
	MYSQL_RES			*mDbRslt;
	MYSQL           	*mMySql;
	uint				 mCrtIndex;
	uint				 mNumFields;
	bool				 mDeleteFlag;

public:
	OmnDbTableMySql(MYSQL_RES *dbrslt);
	virtual ~OmnDbTableMySql();

    virtual void            reset();
    virtual bool            hasMore() const;
    virtual OmnDbRecordPtr  next();
    virtual OmnDbRecordPtr  next2();
    virtual OmnDbRecordPtr  getRecord(const int idx);
    virtual int             entries() const;
	void					setFlag(const bool b) {mDeleteFlag = b;}
	bool readDataFinish();

	bool setMysqlHandle(MYSQL *mysql)
	{
		mMySql = mysql;
		return true;
	}

};
#endif

