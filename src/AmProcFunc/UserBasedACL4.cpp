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
// 7/15/2007: Created by fchen
////////////////////////////////////////////////////////////////////////////
#include "AmProcFunc/UserBasedACL4.h"

#include "AmUtil/AmRespCode.h"
#include "AmUtil/ReturnCode.h"


#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Database/DbRecord.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Tracer/Tracer.h"
#include "Util/OmnNew.h"



AosUserBasedACL4::AosUserBasedACL4()
:
OmnObject(OmnClassId::eAosUserBasedACL4),
OmnDbObj(OmnClassId::eAosUserBasedACL4)
{
}

AosUserBasedACL4::AosUserBasedACL4(const u32 &userId, const OmnString &userName, const OmnIpAddr &ipAddr, const u16 &port)
:
OmnObject(OmnClassId::eAosUserBasedACL4),
OmnDbObj(OmnClassId::eAosUserBasedACL4),
mUserId(userId),
mUserName(userName),
mIpAddr(ipAddr),
mPort(port)

{
}

AosUserBasedACL4::AosUserBasedACL4(const OmnString &userName, const OmnIpAddr &ipAddr, const u16 &port)
:
OmnObject(OmnClassId::eAosUserBasedACL4),
OmnDbObj(OmnClassId::eAosUserBasedACL4),
mUserId(0),
mUserName(userName),
mIpAddr(ipAddr),
mPort(port)

{
}


AosUserBasedACL4::~AosUserBasedACL4()
{
}


OmnClassId::E	
AosUserBasedACL4::getClassId() const 
{
	return OmnClassId::eAosUserBasedACL4;
}


int
AosUserBasedACL4::objSize() const
{
	return sizeof(*this);
}


void
AosUserBasedACL4::reset()
{
	// 
	// It resets all the member to its initial values
	//
    mUserId = 0;
    mUserName = "";
    mIpAddr = OmnIpAddr(0);
    mPort = 0;

}


OmnString
AosUserBasedACL4::toString() const
{
	OmnString str;
    str << "Class AosUserBasedACL4:"
        << "    mUserId:   " << mUserId
        << "    mUserName:   " << mUserName
        << "    mIpAddr:   " << mIpAddr.toString()
        << "    mPort:   " << mPort;

	return str;
}


OmnDbObjPtr	
AosUserBasedACL4::clone() const
{
    AosUserBasedACL4Ptr obj = OmnNew AosUserBasedACL4();
    obj.setDelFlag(false);
    obj->mUserId = mUserId;
    obj->mUserName = mUserName;
    obj->mIpAddr = mIpAddr;
    obj->mPort = mPort;

	return obj.getPtr();
}


OmnRslt
AosUserBasedACL4::serializeFromDb()
{
	// 
	// It retrieves a user from the database.
	// 
    OmnString stmt = "select * from user_based_acc_L4 where ";
    stmt << 
        "user_id=" << mUserId
        << ", user_name='" << mUserName
        << "', ip=" << mIpAddr.getIPv4()
        << ", port=" << mPort;

	
	OmnTraceDb << "To retrieve user: " << stmt << endl;

	OmnDbRecordPtr record;
	OmnRslt rslt = retrieveRecord(stmt, record);
	if (!rslt)
	{
		return OmnAlarm << "Failed to retrieve AosUserBasedACL4: " 
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
AosUserBasedACL4::updateStmt() const
{
    OmnString stmt = "update user_based_acc_L4 set ";
    stmt 
        << "user_id = " << mUserId
        << "user_name = '" << mUserName
        << "'ip = " << mIpAddr.getIPv4()
        << "port = " << mPort;

	return stmt;
}


OmnString
AosUserBasedACL4::removeStmt() const
{
    OmnString stmt = "delete from user_based_acc_L4 where ";
    stmt 
        << "user_id = " << mUserId
        << "user_name = '" << mUserName
        << "'ip = " << mIpAddr.getIPv4()
        << "port = " << mPort;

    return stmt;
}


OmnString
AosUserBasedACL4::removeAllStmt() const
{
    OmnString stmt = "delete from user_based_acc_L4";

    return stmt;
}


OmnString
AosUserBasedACL4::existStmt() const
{
    OmnString stmt = "select * from user_based_acc_L4 where ";
    stmt 
        << "user_id = " << mUserId
        << "user_name = '" << mUserName
        << "'ip = " << mIpAddr.getIPv4()
        << "port = " << mPort;

    return stmt;
}



OmnString
AosUserBasedACL4::insertStmt() const
{
    //
    // It creates a SQL statement to insert a AosUserBasedACL4 
	// into the database.
    //
    OmnString stmt = "insert into user_based_acc_L4 ("
        "user_id, "
        "user_name, "
        "ip, "
        "port) values (";

    stmt << mUserId << ", '"
        << mUserName << "', "
        << mIpAddr.getIPv4() << ", "
        << mPort  << ")";

    return stmt;
}


OmnRslt
AosUserBasedACL4::serializeFromRecord(const OmnDbRecordPtr &record)
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
    mIpAddr = OmnIpAddr(record->getU32(0, 0, rslt));
    mPort = record->getU32(0, 0, rslt);

	return rslt;
}


AosUserBasedACL4Ptr
AosUserBasedACL4::retrieveFromDb(const u32 &userId, const OmnString &userName, const OmnIpAddr &ipAddr, const u16 &port)
{
	AosUserBasedACL4Ptr obj = OmnNew AosUserBasedACL4(userId, userName, ipAddr, port);
	if (obj->serializeFromDb()) return obj;

	OmnAlarm << "Failed to retrieve the object: " 
		<< " from database" << enderr;
	return 0;
}


bool
AosUserBasedACL4::checkAccess(u16 &respCode, OmnString &errmsg)
{
	OmnString allow_deny;
    OmnString stmt = "select user_id, allow_deny from user_based_acc_L4 where ";
    if(mUserId)
    {
	    stmt<< "(user_id='" << mUserId 
	    	<< "' or user_id=0)"
	        << " and (ip=" << mIpAddr.getIPv4()
	        << " or ip=0) and (port=" << mPort
	        << " or port=0) order by priority asc ";
    }
    else
    {
	    stmt<< "(user_name='" << mUserName.data() 
	    	<< "' or user_name='*')"
	        << " and (ip=" << mIpAddr.getIPv4()
	        << " or ip=0) and (port=" << mPort
	        << " or port=0) order by priority asc ";
    }
	OmnDbTablePtr table;
OmnTrace << "sql [" << stmt << "]" << endl;
	if (!retrieveRecords(stmt, table))
	{
		// 
		// No record is found
		// 
		respCode = AosAmRespCode::eDenied;
		errmsg << "Layer 4 Access denied. usr [" 
			   << mUserName.data() << "] [" << mIpAddr.toString() << "]" ;
		return false;
	}

	table->reset();
	OmnRslt rslt;

	while (table->hasMore())
	{
		OmnDbRecordPtr record = table->next();
		allow_deny = record->getStr(1, "d", rslt);
		if(0 == strcmp("a", allow_deny.data()))
		{
			respCode = AosAmRespCode::eAllowed;
			return true;
		}
		else
		{
			respCode = AosAmRespCode::eDenied;
			errmsg << "Layer 4 Access denied. usr [" 
				   << mUserName.data() << "] [" << mIpAddr.toString() << "]" ;
			return false;
		}
	}

	respCode = AosAmRespCode::eDenied;
	errmsg << "Layer 4 Access denied. usr [" 
		   << mUserName.data() << "] [" << mIpAddr.toString() << "]" ;
	return false;
}


