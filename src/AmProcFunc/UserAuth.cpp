////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// This file is generated automatically by the ProgramAid facility. 
//
// Modification History:
// 7/22/2007: Created by fchen
////////////////////////////////////////////////////////////////////////////
#include "AmProcFunc/UserAuth.h"

#include "AmUtil/AmRespCode.h"
#include "AmUtil/ReturnCode.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Database/DbRecord.h"
#include "Database/DbTable.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Tracer/Tracer.h"
#include "Util/OmnNew.h"



AosUserAuth::AosUserAuth()
:
OmnObject(OmnClassId::eAosUserAuth),
OmnDbObj(OmnClassId::eAosUserAuth)
{
}

AosUserAuth::AosUserAuth(const OmnString &strUserName, const OmnString &strUserPwd)
:
OmnObject(OmnClassId::eAosUserAuth),
OmnDbObj(OmnClassId::eAosUserAuth),
mUserName(strUserName),
mUserPwd(strUserPwd)
{
}


AosUserAuth::AosUserAuth(const u32 userId)
:
OmnObject(OmnClassId::eAosUserAuth),
OmnDbObj(OmnClassId::eAosUserAuth),
mUserId(userId)
{
}


AosUserAuth::~AosUserAuth()
{
}


OmnClassId::E	
AosUserAuth::getClassId() const 
{
	return OmnClassId::eAosUserAuth;
}


int
AosUserAuth::objSize() const
{
	return sizeof(*this);
}


void
AosUserAuth::reset()
{
	// 
	// It resets all the member to its initial values
	//
    mUserId = 0;
    mUserName = "";
    mUserPwd = "";

}


OmnString
AosUserAuth::toString() const
{
	OmnString str;
    str << "Class AosUserAuth:"
        << "    mUserId:   " << mUserId
        << "    mUserName:   " << mUserName
        << "    mUserPwd:   " << mUserPwd;

	return str;
}


OmnDbObjPtr	
AosUserAuth::clone() const
{
    AosUserAuthPtr obj = OmnNew AosUserAuth();
    obj.setDelFlag(false);
    obj->mUserId = mUserId;
    obj->mUserName = mUserName;
    obj->mUserPwd = mUserPwd;

	return obj.getPtr();
}


OmnRslt
AosUserAuth::serializeFromDb()
{
	// 
	// It retrieves a user from the database.
	// 
    OmnString stmt = "select * from user_auth where ";
    stmt << 
        "user_id=" << mUserId;

	
	OmnTraceDb << "To retrieve user: " << stmt << endl;

	OmnDbRecordPtr record;
	OmnRslt rslt = retrieveRecord(stmt, record);
	if (!rslt)
	{
		return OmnAlarm << "Failed to retrieve AosUserAuth: " 
			<< toString() << " from db!" << enderr;
	}

	if (!record)
	{
		// 
		// Didn't find the record
		//
		rslt.setErrmsg("Didn't find the object");
		OmnTraceDb << "Didn't find the object: " << toString() << endl;
		return OmnRslt(OmnErrId::eObjectNotFound, "");
	}

	// 
	// Found the record. 
	//
	return serializeFromRecord(record);
}


OmnString
AosUserAuth::updateStmt() const
{
    OmnString stmt = "update user_auth set ";
    stmt 
        << "user_id = " << mUserId
        << ", user_name = '" << mUserName
        << "', user_pwd = '" << mUserPwd
        << "' ";

	return stmt;
}


OmnString
AosUserAuth::removeStmt() const
{
    OmnString stmt = "delete from user_auth where ";
    stmt 
        << "user_id = " << mUserId;

    return stmt;
}


OmnString
AosUserAuth::removeAllStmt() const
{
    OmnString stmt = "delete from user_auth";

    return stmt;
}


OmnString
AosUserAuth::existStmt() const
{
    OmnString stmt = "select * from user_auth where ";
    stmt 
        << "user_id = " << mUserId;

    return stmt;
}


OmnString
AosUserAuth::authStmt() const
{
    OmnString stmt = "select * from user_auth where ";
    stmt 
        << "user_name = '" << mUserName
        << "' and user_pwd = '" << mUserPwd
        << "'";

    return stmt;
}


OmnString
AosUserAuth::insertStmt() const
{
    //
    // It creates a SQL statement to insert a AosUserAuth 
	// into the database.
    //
    OmnString stmt = "insert into user_auth ("
        "user_id, "
        "user_name, "
        "user_pwd) values (";

    stmt         << mUserId << ", '"
        << mUserName << "', '"
        << mUserPwd  << "')";

    return stmt;
}


OmnRslt
AosUserAuth::serializeFromRecord(const OmnDbRecordPtr &record)
{
	if (!record)
	{
		// 
		// This should never happen
		//
		return OmnAlarm << "Null record passed in!" << enderr;
	}

	OmnRslt rslt(true);
	
    mUserId = record->getU32(0, 0, rslt);
    mUserName = record->getStr(0, "", rslt);
    mUserPwd = record->getStr(0, "", rslt);

	return rslt;
}


AosUserAuthPtr
AosUserAuth::retrieveFromDb(const u32 &userId)
{
	AosUserAuthPtr obj = OmnNew AosUserAuth(userId);
	if (obj->serializeFromDb()) return obj;

	OmnAlarm << "Failed to retrieve the object: " 
		<< " from database" << enderr;
	return 0;
}




bool
AosUserAuth::checkAccess(u16 &respCode, OmnString &errmsg)
{
    OmnString stmt = "select user_id from user_auth where ";
    stmt 
        << "user_name = '" << mUserName
        << "' and user_pwd = '" << mUserPwd
        << "'";

	OmnDbTablePtr table;
	if (!retrieveRecords(stmt, table))
	{
		// 
		// No record is found
		// 
		respCode = AosAmRespCode::eDenied;
		errmsg = "Access denied.";
		return false;
	}

	table->reset();
	OmnRslt rslt;

	while (table->hasMore())
	{
		OmnDbRecordPtr record = table->next();
		mUserId = atoi(record->getStr(0, "", rslt));
		respCode = AosAmRespCode::eAllowed;
		errmsg = "";
		return true;
	}

	respCode = AosAmRespCode::eDenied;
	errmsg = "Access denied";
	return false;
}


