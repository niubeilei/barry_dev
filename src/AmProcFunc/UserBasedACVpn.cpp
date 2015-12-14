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
#include "AmProcFunc/UserBasedACVpn.h"

#include "AmUtil/AmRespCode.h"
#include "AmUtil/ReturnCode.h"


#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Database/DbRecord.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Tracer/Tracer.h"
#include "Util/OmnNew.h"



AosUserBasedACVpn::AosUserBasedACVpn()
:
OmnObject(OmnClassId::eAosUserBasedACVpn),
OmnDbObj(OmnClassId::eAosUserBasedACVpn)
{
}

AosUserBasedACVpn::AosUserBasedACVpn(const u32 &userId, const OmnString &userName, const OmnIpAddr &sIpAddr, const u16 &sPort, const OmnIpAddr &dIpAddr, const u16 &dPort, const u8 &proto)
:
OmnObject(OmnClassId::eAosUserBasedACVpn),
OmnDbObj(OmnClassId::eAosUserBasedACVpn),
mUserId(userId),
mUserName(userName),
mSIpAddr(sIpAddr),
mSPort(sPort),
mDIpAddr(dIpAddr),
mDPort(dPort),
mProto(proto)

{
}

AosUserBasedACVpn::AosUserBasedACVpn(const OmnString &userName, const OmnIpAddr &sIpAddr, const u16 &sPort, const OmnIpAddr &dIpAddr, const u16 &dPort, const u8 &proto)
:
OmnObject(OmnClassId::eAosUserBasedACVpn),
OmnDbObj(OmnClassId::eAosUserBasedACVpn),
mUserId(0),
mUserName(userName),
mSIpAddr(sIpAddr),
mSPort(sPort),
mDIpAddr(dIpAddr),
mDPort(dPort),
mProto(proto)

{
}

AosUserBasedACVpn::~AosUserBasedACVpn()
{
}


OmnClassId::E	
AosUserBasedACVpn::getClassId() const 
{
	return OmnClassId::eAosUserBasedACVpn;
}


int
AosUserBasedACVpn::objSize() const
{
	return sizeof(*this);
}


void
AosUserBasedACVpn::reset()
{
	// 
	// It resets all the member to its initial values
	//
    mUserId = 0;
    mUserName = "";
    mSIpAddr = OmnIpAddr(0);
    mSPort = 0;
    mDIpAddr = OmnIpAddr(0);
    mDPort = 0;
    mProto = 0;

}


OmnString
AosUserBasedACVpn::toString() const
{
	OmnString str;
    str << "Class AosUserBasedACVpn:"
        << "    mUserId:   " << mUserId
        << "    mUserName:   " << mUserName
        << "    mSIpAddr:   " << mSIpAddr.toString()
        << "    mSPort:   " << mSPort
        << "    mDIpAddr:   " << mDIpAddr.toString()
        << "    mDPort:   " << mDPort
        << "    mProto:   " << mProto;

	return str;
}


OmnDbObjPtr	
AosUserBasedACVpn::clone() const
{
    AosUserBasedACVpnPtr obj = OmnNew AosUserBasedACVpn();
    obj.setDelFlag(false);
    obj->mUserId = mUserId;
    obj->mUserName = mUserName;
    obj->mSIpAddr = mSIpAddr;
    obj->mSPort = mSPort;
    obj->mDIpAddr = mDIpAddr;
    obj->mDPort = mDPort;
    obj->mProto = mProto;

	return obj.getPtr();
}


OmnRslt
AosUserBasedACVpn::serializeFromDb()
{
	// 
	// It retrieves a user from the database.
	// 
    OmnString stmt = "select * from user_based_acc_vpn where ";
    stmt << 
        "user_id=" << mUserId
        << ", user_name='" << mUserName
        << "', sip=" << mSIpAddr.getIPv4()
        << ", sport=" << mSPort
        << ", dip=" << mDIpAddr.getIPv4()
        << ", dport=" << mDPort
        << ", proto=" << mProto;

	
	OmnTraceDb << "To retrieve user: " << stmt << endl;

	OmnDbRecordPtr record;
	OmnRslt rslt = retrieveRecord(stmt, record);
	if (!rslt)
	{
		return OmnAlarm << "Failed to retrieve AosUserBasedACVpn: " 
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
AosUserBasedACVpn::updateStmt() const
{
    OmnString stmt = "update user_based_acc_vpn set ";
    stmt 
        << "user_id = " << mUserId
        << "user_name = '" << mUserName
        << "'sip = " << mSIpAddr.getIPv4()
        << "sport = " << mSPort
        << "dip = " << mDIpAddr.getIPv4()
        << "dport = " << mDPort
        << "proto = " << mProto;

	return stmt;
}


OmnString
AosUserBasedACVpn::removeStmt() const
{
    OmnString stmt = "delete from user_based_acc_vpn where ";
    stmt 
        << "user_id = " << mUserId
        << "user_name = '" << mUserName
        << "'sip = " << mSIpAddr.getIPv4()
        << "sport = " << mSPort
        << "dip = " << mDIpAddr.getIPv4()
        << "dport = " << mDPort
        << "proto = " << mProto;

    return stmt;
}


OmnString
AosUserBasedACVpn::removeAllStmt() const
{
    OmnString stmt = "delete from user_based_acc_vpn";

    return stmt;
}


OmnString
AosUserBasedACVpn::existStmt() const
{
    OmnString stmt = "select * from user_based_acc_vpn where ";
    stmt 
        << "user_id = " << mUserId
        << "user_name = '" << mUserName
        << "'sip = " << mSIpAddr.getIPv4()
        << "sport = " << mSPort
        << "dip = " << mDIpAddr.getIPv4()
        << "dport = " << mDPort
        << "proto = " << mProto;

    return stmt;
}



OmnString
AosUserBasedACVpn::insertStmt() const
{
    //
    // It creates a SQL statement to insert a AosUserBasedACVpn 
	// into the database.
    //
    OmnString stmt = "insert into user_based_acc_vpn ("
        "user_id, "
        "user_name, "
        "sip, "
        "sport, "
        "dip, "
        "dport, "
        "proto) values (";

    stmt         << mUserId << ", '"
        << mUserName << "', "
        << mSIpAddr.getIPv4() << ", "
        << mSPort << ", "
        << mDIpAddr.getIPv4() << ", "
        << mDPort << ", "
        << mProto  << ")";

    return stmt;
}


OmnRslt
AosUserBasedACVpn::serializeFromRecord(const OmnDbRecordPtr &record)
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
    mSIpAddr = OmnIpAddr(record->getU32(0, 0, rslt));
    mSPort = record->getU32(0, 0, rslt);
    mDIpAddr = OmnIpAddr(record->getU32(0, 0, rslt));
    mDPort = record->getU32(0, 0, rslt);
    mProto = record->getU32(0, 0, rslt);

	return rslt;
}


AosUserBasedACVpnPtr
AosUserBasedACVpn::retrieveFromDb(const u32 &userId, const OmnString &userName, const OmnIpAddr &sIpAddr, const u16 &sPort, const OmnIpAddr &dIpAddr, const u16 &dPort, const u8 &proto)
{
	AosUserBasedACVpnPtr obj = OmnNew AosUserBasedACVpn(userId, userName, sIpAddr, sPort, dIpAddr, dPort, proto);
	if (obj->serializeFromDb()) return obj;

	OmnAlarm << "Failed to retrieve the object: " 
		<< " from database" << enderr;
	return 0;
}


bool
AosUserBasedACVpn::checkAccess(u16 &respCode, OmnString &errmsg)
{
	OmnString allow_deny;
    OmnString stmt = "select user_name from user_based_acc_vpn where ";
    if(mUserId)
    {
	    stmt<< "(user_id='" << mUserId 
	    	<< "' or user_id=0)"
	        << " and sip=" << mSIpAddr.getIPv4()
	        << " and sport=" << mSPort
	        << " and dip=" << mDIpAddr.getIPv4()
	        << " and dport=" << mDPort
	        << " and proto = '" << mProto
	        << "' "
	        << " order by priority asc ";
    }
    else
    {
	    stmt<< "(user_name='" << mUserName.data() 
	    	<< "' or user_name='*')"
	        << " and sip=" << mSIpAddr.getIPv4()
	        << " and sport=" << mSPort
	        << " and dip=" << mDIpAddr.getIPv4()
	        << " and dport=" << mDPort
	        << " and proto = '" << mProto
	        << "' "
	        << " order by priority asc ";
    }
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
		allow_deny = record->getStr(1, "d", rslt);
		if(0 == strcmp("a", allow_deny.data()))
		{
			respCode = AosAmRespCode::eAllowed;
			return true;
		}
		else
		{
			respCode = AosAmRespCode::eDenied;
			return false;
		}
	}

	respCode = AosAmRespCode::eDenied;
	errmsg = "Access denied";
	return false;
}


bool
AosUserBasedACVpn::getAllAcl(OmnDbTablePtr & table)
{
    OmnString stmt = "select id, user_id, user_name, weekday, stime, etime, "
					 "sip, smask, ssport, seport, dip, dmask, dsport, deport, proto, "
					 "allow_deny, priority "
					 "from user_based_acc_vpn order by priority asc";
	OmnTrace << stmt << endl;
	if (!retrieveRecords(stmt, table))
	{
		// 
		// No record is found
		// 
		return false;
	}

	return true;
}

