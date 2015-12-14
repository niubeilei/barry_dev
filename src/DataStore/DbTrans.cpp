////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//   
//
// Modification History:
// 	Created: 12/17/2008 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataStore/DbTrans.h"

#include "alarm_c/alarm.h"
#include "Database/DbRecord.h"
#include "Debug/Debug.h"


const OmnString sgTablename = "dbtrans";

OmnString
AosDbTrans::retrieveStmt() const
{
	OmnString stmt = "select * from ";
	stmt << sgTablename << " where status = 'i'";
	return stmt;
}


OmnRslt     
AosDbTrans::serializeFromRecord(const OmnDbRecordPtr &record)
{
	aos_assert_r(record, false);
	OmnRslt rslt(true);
	mId = record->getU32(0, 0, rslt);
	mStatus = record->getChar(1, 'i', rslt);
	mData = record->getStr(2, "", rslt);
	return true;
}


OmnString   
AosDbTrans::insertStmt() const
{
	OmnString stmt = "insert into ";
	stmt << sgTablename
		<< " (status, data) values ('"
		<< mStatus << "', '"
		<< mData << "')";
	return stmt;
}


OmnString   
AosDbTrans::updateStmt() const
{
	OmnString stmt = "update ";
	stmt << sgTablename << " set status='a', data='" << mData << "'"
		<< " where id=" << mId;
	return stmt;
}


OmnString   
AosDbTrans::removeStmt() const
{
	OmnString stmt = "delete from ";
	stmt << sgTablename << " where id=" << mId;
	return stmt;
}


OmnString   
AosDbTrans::removeAllStmt() const
{
	OmnString stmt = "delete from ";
	stmt << sgTablename;
	return stmt;
}


OmnString   
AosDbTrans::existStmt() const
{
	OmnString stmt = "select * from ";
	stmt << sgTablename << " where status='i'";
	return stmt;
}


bool
AosDbTrans::storeTransData(const OmnString &data, u32 &transId)
{
OmnTrace << "To store trans data: " << data << endl;
	OmnString stmt = existStmt();
	OmnDbRecordPtr record;
	retrieveRecord(stmt, record);
	if (!record)
	{
		// No more idle records. Need to insert one
		mStatus = 'i';
		mData = "";
		addToDb();
	}
	retrieveRecord(stmt, record);
	aos_assert_r(record, false);
	serializeFromRecord(record);
	transId = mId;
	mData = data;
	stmt = updateStmt();
OmnTrace << "------ to store error: " << stmt << endl;
OmnTrace << "------ " << mId << endl;
	if (updateToDb(stmt)) return true;
	return false;
}


bool
AosDbTrans::releaseTrans(const u32 transId)
{
	mId = transId;
	OmnString stmt = "update ";
	stmt << sgTablename << " set status='i', data='' where id=" << mId;
	if (updateToDb(stmt)) return true;
	return false;
}

