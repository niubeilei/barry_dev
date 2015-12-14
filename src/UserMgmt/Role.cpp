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
#include "UserMgmt/Role.h"


#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Database/DbRecord.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Tracer/Tracer.h"
#include "Util/OmnNew.h"



AosRole::AosRole()
:
OmnObject(OmnClassId::eAosRole),
OmnDbObj(OmnClassId::eAosRole)
{
}

AosRole::AosRole(const u32 &roleId)
:
OmnObject(OmnClassId::eAosRole),
OmnDbObj(OmnClassId::eAosRole),
mRoleId(roleId)
{
}


AosRole::~AosRole()
{
}


OmnClassId::E	
AosRole::getClassId() const 
{
	return OmnClassId::eAosRole;
}


int
AosRole::objSize() const
{
	return sizeof(*this);
}


void
AosRole::reset()
{
	// 
	// It resets all the member to its initial values
	//
    mRoleId = 0;
    mRoleName = "";
    mDescription = "";
    mStatus = 'U';

}


OmnString
AosRole::toString() const
{
	OmnString str;
    str << "Class AosRole:"
        << "    mRoleId:   " << mRoleId
        << "    mRoleName:   " << mRoleName
        << "    mDescription:   " << mDescription
        << "    mStatus:   " << mStatus;

	return str;
}


OmnDbObjPtr	
AosRole::clone() const
{
    AosRolePtr obj = OmnNew AosRole();
    obj.setDelFlag(false);
    obj->mRoleId = mRoleId;
    obj->mRoleName = mRoleName;
    obj->mDescription = mDescription;
    obj->mStatus = mStatus;

	return obj.getPtr();
}


OmnRslt
AosRole::serializeFromDb()
{
	// 
	// It retrieves a user from the database.
	// 
    OmnString stmt = "select * from role where ";
    stmt << 
        "role_id=" << mRoleId;

	
	OmnTraceDb << "To retrieve user: " << stmt << endl;

	OmnDbRecordPtr record;
	OmnRslt rslt = retrieveRecord(stmt, record);
	if (!rslt)
	{
		return OmnAlarm << "Failed to retrieve AosRole: " 
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
AosRole::updateStmt() const
{
    OmnString stmt = "update role set ";
    stmt 
        << "role_id = " << mRoleId
        << "role_name = '" << mRoleName
        << "'desc = '" << mDescription
        << "'status = " << mStatus;

	return stmt;
}


OmnString
AosRole::removeStmt() const
{
    OmnString stmt = "delete from role where ";
    stmt 
        << "role_id = " << mRoleId;

    return stmt;
}


OmnString
AosRole::removeAllStmt() const
{
    OmnString stmt = "delete from role";

    return stmt;
}


OmnString
AosRole::existStmt() const
{
    OmnString stmt = "select * from role where ";
    stmt 
        << "role_id = " << mRoleId;

    return stmt;
}



OmnString
AosRole::insertStmt() const
{
    //
    // It creates a SQL statement to insert a AosRole 
	// into the database.
    //
    OmnString stmt = "insert into role ("
        "role_id, "
        "role_name, "
        "desc, "
        "status) values (";

    stmt         << mRoleId << ", '"
        << mRoleName << "', '"
        << mDescription << "', "
        << mStatus  << ")";

    return stmt;
}


OmnRslt
AosRole::serializeFromRecord(const OmnDbRecordPtr &record)
{
	if (!record)
	{
		// 
		// This should never happen
		//
		return OmnAlarm << "Null record passed in!" << enderr;
	}

	OmnRslt rslt(true);
	
    mRoleId = record->getU32(0, 0, rslt);
    mRoleName = record->getStr(0, "", rslt);
    mDescription = record->getStr(0, "", rslt);
    mStatus = record->getChar(0, 'U', rslt);

	return rslt;
}


AosRolePtr
AosRole::retrieveFromDb(const u32 &roleId)
{
	AosRolePtr obj = OmnNew AosRole(roleId);
	if (obj->serializeFromDb()) return obj;

	OmnAlarm << "Failed to retrieve the object: " 
		<< " from database" << enderr;
	return 0;
}


