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
// 6/4/2007: Created by fchen
////////////////////////////////////////////////////////////////////////////
#include "AmProcFunc/UserBasedAC.h"

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
#include <regex.h>


AosUserBasedAC::AosUserBasedAC()
:
OmnObject(OmnClassId::eAosUserBasedAC),
OmnDbObj(OmnClassId::eAosUserBasedAC)
{
}

AosUserBasedAC::AosUserBasedAC(const u32 &userId, const OmnString &userName, const OmnString &app, const OmnString &opr, const OmnString &resource)
:
OmnObject(OmnClassId::eAosUserBasedAC),
OmnDbObj(OmnClassId::eAosUserBasedAC),
mUserId(userId),
mUserName(userName),
mApp(app),
mOpr(opr),
mResource(resource)

{
}


AosUserBasedAC::AosUserBasedAC(const OmnString &userName, const OmnString &app, const OmnString &opr, const OmnString &resource)
:
OmnObject(OmnClassId::eAosUserBasedAC),
OmnDbObj(OmnClassId::eAosUserBasedAC),
mUserId(0),
mUserName(userName),
mApp(app),
mOpr(opr),
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
    mUserId = 0;
    mUserName = "";
    mApp = "";
    mOpr = "";
    mResource = "";

}


OmnString
AosUserBasedAC::toString() const
{
	OmnString str;
    str << "Class AosUserBasedAC:"
        << "    mUserId:   " << mUserId
        << "    mUserName:   " << mUserName
        << "    mApp:   " << mApp
        << "    mOpr:   " << mOpr
        << "    mResource:   " << mResource;

	return str;
}


OmnDbObjPtr	
AosUserBasedAC::clone() const
{
    AosUserBasedACPtr obj = OmnNew AosUserBasedAC();
    obj.setDelFlag(false);
    obj->mUserId = mUserId;
    obj->mUserName = mUserName;
    obj->mApp = mApp;
    obj->mOpr = mOpr;
    obj->mResource = mResource;

	return obj.getPtr();
}


OmnRslt
AosUserBasedAC::serializeFromDb()
{
	// 
	// It retrieves a user from the database.
	// 
    OmnString stmt = "select * from user_based_acc where ";
    stmt << 
        "user_id=" << mUserId
        << ", user_name='" << mUserName
        << "', app='" << mApp
        << "', opr='" << mOpr
        << "', resource='" << mResource;

	
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
    OmnString stmt = "update user_based_acc set ";
    stmt 
        << "user_id = " << mUserId
        << "user_name = '" << mUserName
        << "'app = '" << mApp
        << "'opr = '" << mOpr
        << "'resource = '" << mResource;

	return stmt;
}


OmnString
AosUserBasedAC::removeStmt() const
{
    OmnString stmt = "delete from user_based_acc where ";
    stmt 
        << "user_id = " << mUserId
        << "user_name = '" << mUserName
        << "'app = '" << mApp
        << "'opr = '" << mOpr
        << "'resource = '" << mResource;

    return stmt;
}


OmnString
AosUserBasedAC::removeAllStmt() const
{
    OmnString stmt = "delete from user_based_acc";

    return stmt;
}


OmnString
AosUserBasedAC::existStmt() const
{
    OmnString stmt = "select * from user_based_acc where ";
    stmt 
        << "user_id = " << mUserId
        << "user_name = '" << mUserName
        << "'app = '" << mApp
        << "'opr = '" << mOpr
        << "'resource = '" << mResource;

    return stmt;
}



OmnString
AosUserBasedAC::insertStmt() const
{
    //
    // It creates a SQL statement to insert a AosUserBasedAC 
	// into the database.
    //
    OmnString stmt = "insert into user_based_acc ("
        "user_id, "
        "user_name, "
        "app, "
        "opr, "
        "resource) values (";

    stmt         << mUserId << ", '"
        << mUserName << "', '"
        << mApp << "', '"
        << mOpr << "', '"
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
	
    mUserId = record->getU32(0, 0, rslt);
    mUserName = record->getStr(0, "", rslt);
    mApp = record->getStr(0, "", rslt);
    mOpr = record->getStr(0, "", rslt);
    mResource = record->getStr(0, "", rslt);

	return rslt;
}


AosUserBasedACPtr
AosUserBasedAC::retrieveFromDb(const u32 &userId, const OmnString &userName, const OmnString &app, const OmnString &opr, const OmnString &resource)
{
	AosUserBasedACPtr obj = OmnNew AosUserBasedAC(userId, userName, app, opr, resource);
	if (obj->serializeFromDb()) return obj;

	OmnAlarm << "Failed to retrieve the object: " 
		<< " from database" << enderr;
	return 0;
}


bool
AosUserBasedAC::checkAccess(u16 &respCode, OmnString &errmsg)
{
	OmnString allow_deny;
    OmnString stmt = "select resource, allow_deny from user_based_acc where ";
    if(mUserId)
    {
	    stmt<< "(user_id='" << mUserId 
	    	<< "' or user_id=0)"
	        << " and app='" << mApp
	        << "' and (opr='" << mOpr
	        << "' or opr='*' or opr='any') "
	        << " order by priority asc, id desc ";
    }
    else
    {
	    stmt<< "(user_name='" << mUserName 
	    	<< "' or user_name='*')"
	        << " and app='" << mApp
	        << "' and (opr='" << mOpr
	        << "' or opr='*' or opr='any') "
	        << " order by priority asc, id desc ";
    }
	OmnTrace << "sql[" << stmt << "] " << endl;
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
		OmnString rsc = record->getStr(0, "", rslt);
		if (rsc == "")
		{
			respCode = AosAmRespCode::eInternalError;
			errmsg = "Internal error";
			OmnAlarm << "Failed to retrieve field" << enderr;
			return false;
		}
		else if(rsc == "*") // this "*" is one incorrect regular expression
		{
			allow_deny = record->getStr(1, "d", rslt);
			OmnTrace << "get allow_deny[" << allow_deny << "] " << endl;
			if(0 == strcmp("a", allow_deny.data()))
			{
				respCode = AosAmRespCode::eAllowed;
			}
			else
			{
				respCode = AosAmRespCode::eDenied;
			}
				
			errmsg = "";
			return true;
		}

		// 
		// Compare using the regular expression
		// 
		regex_t regex;
		if (regcomp(&regex, rsc.data(), REG_EXTENDED|REG_NOSUB))
		{
			OmnAlarm << "Database Regular expression incorrect" << enderr;
			// error 
			continue;
//			errcode = eAosRc_AmInternalError;
//			errmsg = "Internal error";
//			return false;
		}
	
OmnTrace << "get rsc[" << rsc << "] " << endl;
		if (regexec(&regex, mResource.data(), 0, 0, 0) == 0)
		{
			allow_deny = record->getStr(1, "d", rslt);
OmnTrace << "get allow_deny[" << allow_deny << "] " << endl;
			if(0 == strcmp("a", allow_deny.data()))
			{
				respCode = AosAmRespCode::eAllowed;
			}
			else
			{
				respCode = AosAmRespCode::eDenied;
			}
				
			errmsg = "";
			return true;
		}
		else
		{
			continue;
		}

	}

	respCode = AosAmRespCode::eDenied;
	errmsg = "Access denied";
	return false;
}

