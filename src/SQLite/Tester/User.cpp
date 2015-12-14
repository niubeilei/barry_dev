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
// This file is generated automatically by the ProgramAid facility. 
//
// Modification History:
// 3/23/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#include "SQLite/Tester//User.h"


#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Database/DbRecord.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Tracer/Tracer.h"
#include "Util/OmnNew.h"



AosUser::AosUser()
:
OmnObject(OmnClassId::eAosUser),
OmnDbObj(OmnClassId::eAosUser),
mUserId(0)
{
}

AosUser::AosUser(const u32 userId)
:
OmnObject(OmnClassId::eAosUser),
OmnDbObj(OmnClassId::eAosUser),
mUserId(userId)
{
}


AosUser::AosUser(const u32 id, 
			const OmnString &name, 
			const OmnString &firstname,
			const OmnString &lastname,
			const OmnString &email, 
			const OmnString &officephone,
			const OmnString &homephone, 
			const char s)
:
OmnObject(OmnClassId::eAosUser),
OmnDbObj(OmnClassId::eAosUser),
mUserId(id),
mUserName(name),
mFirstName(firstname),
mLastName(lastname),
mEmail(email),
mOfficePhone(officephone),
mHomePhone(homephone),
mStatus(s)
{
}


AosUser::~AosUser()
{
}


OmnClassId::E	
AosUser::getClassId() const 
{
	return OmnClassId::eAosUser;
}


int
AosUser::objSize() const
{
	return sizeof(*this);
}


void
AosUser::reset()
{
	// 
	// It resets all the member to its initial values
	//
    mUserId = 0;
    mStatus = 'U';

}


OmnString
AosUser::toString() const
{
	OmnString str;
    str << "Class AosUser:"
        << "    mUserId:   " << mUserId
        << "    mUserName:   " << mUserName
        << "    mDescription:   " << mDescription
        << "    mStatus:   " << mStatus;

	return str;
}


OmnRslt
AosUser::serializeFromDb()
{
	// 
	// It retrieves a user from the database.
	// 
    OmnString stmt = "select * from user where ";
    stmt << "user_id=" << mUserId;

	
	OmnTrace << "To retrieve user: " << stmt << endl;

	OmnDbRecordPtr record;
	OmnRslt rslt = retrieveRecord(stmt, record);
	if (!rslt)
	{
		return OmnAlarm << "Failed to retrieve AosUser: " 
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
AosUser::updateStmt() const
{
    OmnString stmt = "update user set ";
    stmt 
        << "user_id = " << mUserId
        << "user_name = '" << mUserName
        << "'desc = '" << mDescription
        << "'status = " << mStatus;

	return stmt;
}


OmnString
AosUser::removeStmt() const
{
    OmnString stmt = "delete from user where ";
    stmt << "user_id = " << mUserId;

    return stmt;
}


OmnString
AosUser::removeAllStmt() const
{
    OmnString stmt = "delete from user";

    return stmt;
}


OmnString
AosUser::existStmt() const
{
    OmnString stmt = "select * from user where ";
    stmt 
        << "user_id = " << mUserId;

    return stmt;
}



OmnString
AosUser::insertStmt() const
{
    //
    // It creates a SQL statement to insert a AosUser 
	// into the database.
    //
    OmnString stmt = "insert into user ("
        "user_id, "
        "user_name, "
        "desc, "
        "status) values (";

    stmt         << mUserId << ", '"
        << mUserName << "', '"
        << mDescription << "', '"
        << mStatus  << "')";

    return stmt;
}


OmnRslt
AosUser::serializeFromRecord(const OmnDbRecordPtr &record)
{
	if (!record)
	{
		// 
		// This should never happen
		//
		return OmnAlarm << "Null record passed in!" << enderr;
	}

	OmnRslt rslt(true);
	
    mUserId 	 = record->getU32(0, 0, rslt);
    mUserName 	 = record->getStr(1, "NoName", rslt);
    mFirstName 	 = record->getStr(2, "NoFirstname", rslt);
    mLastName 	 = record->getStr(3, "NoLastname", rslt);
    mEmail 		 = record->getStr(4, "NoEmail", rslt);
    mOfficePhone = record->getStr(5, "NoOfficePhone", rslt);
    mHomePhone 	 = record->getStr(6, "NoHomePhone", rslt);
    mDescription = record->getStr(7, "NoDescription", rslt);
    mStatus 	 = record->getChar(8, 'U', rslt);

	return rslt;
}


AosUserPtr
AosUser::retrieveFromDb(const u32 &roleId)
{
	AosUserPtr obj = OmnNew AosUser(roleId);
	if (obj->serializeFromDb()) return obj;

	OmnAlarm << "Failed to retrieve the object: " 
		<< " from database" << enderr;
	return 0;
}


OmnDbObjPtr	
AosUser::clone() const
{
	return 0;
}


