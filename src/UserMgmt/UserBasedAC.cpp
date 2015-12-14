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
#include "UserMgmt/UserBasedAC.h"

#if 0
#include "UserMgmt/Ptrs.h"
#include "UserMgmt/UserBasedAC.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Database/DbRecord.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Tracer/Tracer.h"
#include "Util/OmnNew.h"
#include <sys/types.h>
#include <regex.h>


AosUserBasedAC::AosUserBasedAC()
:
OmnObject(OmnClassId::eAosUserBasedAC),
OmnDbObj(OmnClassId::eAosUserBasedAC)
{
}

AosUserBasedAC::AosUserBasedAC(
			const OmnString &userName, 
			const OmnString &appName, 
			const OmnString &operation)
:
OmnObject(OmnClassId::eAosUserBasedAC),
OmnDbObj(OmnClassId::eAosUserBasedAC),
mUserName(userName),
mAppName(appName),
mOperation(operation)

{
}
AosUserBasedAC::AosUserBasedAC(
			const OmnString &userName, 
			const OmnString &appName, 
			const OmnString &operation, 
			const OmnString &resource)
:
OmnObject(OmnClassId::eAosUserBasedAC),
OmnDbObj(OmnClassId::eAosUserBasedAC),
mUserName(userName),
mAppName(appName),
mOperation(operation),
mResource(resource)
{
}


AosUserBasedAC::~AosUserBasedAC()
{
}


OmnClassId::E	
AosUserBasedAC::getClassId() const 
{
	return OmnClassId::eAosUserBasedAC;
}


int
AosUserBasedAC::objSize() const
{
	return sizeof(*this);
}


void
AosUserBasedAC::reset()
{
	// 
	// It resets all the member to its initial values
	//
    mUserName = "";
    mAppName = "";
    mOperation = "";
    mResource = "";

}


OmnString
AosUserBasedAC::toString() const
{
	OmnString str;
    str << "Class AosUserBasedAC:"
        << "    mUserName:   " << mUserName
        << "    mAppName:   " << mAppName
        << "    mOperation:   " << mOperation
        << "    mResource:   " << mResource;

	return str;
}


OmnDbObjPtr	
AosUserBasedAC::clone() const
{
    AosUserBasedACPtr obj = OmnNew AosUserBasedAC();
    obj.setDelFlag(false);
    obj->mUserName = mUserName;
    obj->mAppName = mAppName;
    obj->mOperation = mOperation;
    obj->mResource = mResource;

	return obj.getPtr();
}


OmnRslt
AosUserBasedAC::serializeFromDb()
{
	// 
	// It retrieves a user from the database.
	// 
    OmnString stmt = "select * from user_acc_ctrl where ";
    stmt << 
        "user_name='" << mUserName
        << "', app_name='" << mAppName
        << "', operation='" << mOperation;

	
	OmnTraceDb << "To retrieve user: " << stmt << endl;

	OmnDbRecordPtr record;
	OmnRslt rslt = retrieveRecord(stmt, record);
	if (!rslt)
	{
		return OmnAlarm << "Failed to retrieve AosUserBasedAC: " 
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
AosUserBasedAC::updateStmt() const
{
    OmnString stmt = "update user_acc_ctrl set ";
    stmt 
        << "user_name = '" << mUserName
        << "'app_name = '" << mAppName
        << "'operation = '" << mOperation
        << "'resource = '" << mResource;

	return stmt;
}


OmnString
AosUserBasedAC::removeStmt() const
{
    OmnString stmt = "delete from user_acc_ctrl where ";
    stmt 
        << "user_name = '" << mUserName
        << "'app_name = '" << mAppName
        << "'operation = '" << mOperation;

    return stmt;
}


OmnString
AosUserBasedAC::removeAllStmt() const
{
    OmnString stmt = "delete from user_acc_ctrl";

    return stmt;
}


OmnString
AosUserBasedAC::existStmt() const
{
    OmnString stmt = "select * from user_acc_ctrl where ";
    stmt 
        << "user_name = '" << mUserName
        << "'app_name = '" << mAppName
        << "'operation = '" << mOperation;

    return stmt;
}



OmnString
AosUserBasedAC::insertStmt() const
{
    //
    // It creates a SQL statement to insert a AosUserBasedAC 
	// into the database.
    //
    OmnString stmt = "insert into user_acc_ctrl ("
        "user_name, "
        "app_name, "
        "operation, "
        "resource) values (";

    stmt         << mUserName << "', '"
        << mAppName << "', '"
        << mOperation << "', '"
        << mResource  << "')";

    return stmt;
}


OmnRslt
AosUserBasedAC::serializeFromRecord(const OmnDbRecordPtr &record)
{
	if (!record)
	{
		// 
		// This should never happen
		//
		return OmnAlarm << "Null record passed in!" << enderr;
	}

	OmnRslt rslt(true);
	
    mUserName = record->getStr(0, "", rslt);
    mAppName = record->getStr(0, "", rslt);
    mOperation = record->getStr(0, "", rslt);
    mResource = record->getStr(0, "", rslt);

	return rslt;
}


AosUserBasedACPtr
AosUserBasedAC::retrieveFromDb(const OmnString &userName, const OmnString &appName, const OmnString &operation)
{
	AosUserBasedACPtr obj = OmnNew AosUserBasedAC(userName, appName, operation);
	if (obj->serializeFromDb()) return obj;

	OmnAlarm << "Failed to retrieve the object: " 
		<< " from database" << enderr;
	return 0;
}


bool
AosUserBasedAC::authorize(const AosAmRequest &req, AosAmResponse &response)
{
	// 
	// The request is correct. 
	//
	AosUserBasedAC ac(req.getStr(AosAmTagId::eUser), 
					  req.getStr(AosAmTagId::eApp),
					  req.getStr(AosAmTagId::eOpr),
					  req.getStr(AosAmTagId::eRsc));
	return ac.authorize(response);
}


bool
AosUserBasedAC::authorize(AosAmResponse &response)
{
	// 
	// Look up the table. 
	//
	OmnDbTablePtr table;
	if (!retrieveRecords(existStmt(), table))
	{
		OmnAlarm << "Failed to retrieve the table" << enderr;
		response.setRespCode(AosAmRespCode::eDenied);
		response.setReason("Failed to access the database");
		return false;
	}

	if (!table)
	{
		response.setRespCode(AosAmRespCode::eDenied);
		response.setReason("Failed to access the database");
		OmnAlarm << "Failed to retrieve the table" << enderr;
		return false;
	}

	table->reset();
	while (table->hasMore())
	{
		OmnRslt rslt;
		OmnDbRecordPtr record = table->next();
		OmnString rsc = record->getStr(3, "", rslt);
		if (!rslt)
		{
			OmnAlarm << "Failed to get the resource" << rslt.toString() << enderr;
			response.setRespCode(AosAmRespCode::eDenied);
			response.setReason("Failed to access the resource field");
			return false;
		}

		// 
		// Check whether it matches the resource field
		//
		regex_t reg;
		if (regcomp(&reg, rsc.data(), 0))
		{
			OmnAlarm << "Failed to calculate the regular expression" << enderr;
			continue;
		}

		if (regexec(&reg, mResource.data(), 0, 0, 0) == 0)
		{
			response.setRespCode(AosAmRespCode::eAllowed);
			return true;
		}

	}

	// 
	// Does not match. 
	//
	response.setRespCode(AosAmRespCode::eDenied);
	response.setReason("No match entry found");
	return true;
}

#endif
