////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AppInfo.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "AppMgr/AppInfo.h"

#include "Alarm/Alarm.h"
#include "Database/DbRecord.h"
#include "Debug/Debug.h"


static OmnString sgTableName = "app_info";

OmnAppInfo::OmnAppInfo()
// :
// OmnObject(OmnClassId::eOmnAppInfo),
// OmnDbObj(OmnClassId::eOmnAppInfo)
{
}


OmnString
OmnAppInfo::getTableName()
{
	return sgTableName;
}


/*
OmnRslt
OmnAppInfo::serializeFromDb()
{
	OmnString stmt = "select * from ";
	stmt << sgTableName << " where "
		 << "app_name=" << mAppName;
	
	OmnTraceDb << "To run SQL: \n" << stmt << endl;

	return OmnDbObj::serializeFromDb(stmt);
}


OmnRslt
OmnAppInfo::serializeFromRecord(const OmnDbRecordPtr &record)
{
	if (!record)
	{
		// 
		// This should never happen
		//
		return OmnAlarm << "Null record passed in!" << enderr;
	}

	OmnRslt rslt(true);
	
	mAppName      = record->getStr( 0, "NoName", rslt);
	mSignalAddr   = record->getAddr(1, "0.0.0.0", rslt);
	mSignalPort	  = record->getInt( 2, -1, rslt);
	mStreamerAddr = record->getAddr(3, "0.0.0.0", rslt);
	mStreamerPort = record->getInt( 4, -1, rslt);
	return rslt;
}


OmnString
OmnAppInfo::insertStmt() const
{
    //
    // It creates a SQL statement to insert an endpoint into the SQL dataDiff
    //
    OmnString stmt = "insert into ";
	stmt << sgTableName 
		 << " values ( "
		 << mAppName << "','"
		 << mSignalAddr.toString() << "',"
		 << mSignalPort << ",'"
		 << mStreamerAddr.toString() << "',"
		 << mStreamerPort << ")";

    return stmt;
}


OmnString
OmnAppInfo::removeStmt() const
{
    //
    // Test case results are identified by:
	//
	//	(Test ID, Suite Name, Package Name, Test case Name).
	//
	// This function assumes the caller has set these values in the 
	// corresponding member data. 
    //
	OmnString stmt = "delete from ";
	stmt << sgTableName << " where app_name="
		 << mAppName;

    return stmt;
}
*/


OmnString
OmnAppInfo::toString() const
{
	OmnString str;
	str << "Class OmnAppInfo:"
		<< "\n    AppName:      " << mAppName
		<< "\n    SignalAddr:   " << mSignalAddr.toString()
		<< "\n    SignalPort:   " << mSignalPort
		<< "\n    StreamerAddr: " << mStreamerAddr.toString()
		<< "\n    StreamerPort: " << mStreamerPort;
	return str;
}


// OmnString
// OmnAppInfo::updateStmt() const
// {
	/*
//   OmnString stmt = "update ";
// 	stmt << sgTableName;

//    return stmt;
	*/

//	OmnNotImplementedYet;
//	return "";
//}


/*
OmnString
OmnAppInfo::existStmt() const
{
	OmnString stmt = "select count(image_id) from ";
	stmt << sgTableName << " where "
		 << "app_name=" << mAppName;
	return stmt;
}


bool
OmnAppInfo::existInDb(const OmnString &appName)
{
	mAppName = appName;
	return OmnDbObj::existInDb(existStmt());
}


OmnRslt		
OmnAppInfo::serializeFromDb(OmnDbObjPtr &objCreated)
{
	OmnNotImplementedYet;
	objCreated = 0;
	return false;
}
*/

