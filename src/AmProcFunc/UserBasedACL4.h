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
#ifndef Aos_AmProcFunc_UserBasedACL4_h
#define Aos_AmProcFunc_UserBasedACL4_h

#include "aosUtil/Types.h"
#include "Obj/ObjDb.h"
#include "Util/RCObjImp.h"
#include "AmProcFunc/Ptrs.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Database/DbTable.h"


class AosUserBasedACL4 : virtual public OmnDbObj
{
	OmnDefineRCObject;

private:

public:

private:
    u32         mUserId;
    OmnString   mUserName;
    OmnIpAddr   mIpAddr;
    u16         mPort;


public:
    AosUserBasedACL4();
	AosUserBasedACL4(const u32 &userId, const OmnString &userName, const OmnIpAddr &ipAddr, const u16 &port);
	AosUserBasedACL4(const OmnString &userName, const OmnIpAddr &ipAddr, const u16 &port);
    ~AosUserBasedACL4();

	// 
	// OmnObject interface
	//
	virtual int 	objSize() const;
	OmnClassId::E	getClassId() const;

	// 
	// OmnDbObj interface
	//
    virtual OmnRslt		serializeFromDb();
	virtual OmnString	insertStmt() const;
	virtual OmnString	removeStmt() const;
	virtual OmnString	removeAllStmt() const;
	virtual OmnString	updateStmt() const;
	virtual OmnString	existStmt() const;
	virtual OmnDbObjPtr	clone() const;
	OmnRslt				serializeFromRecord(const OmnDbRecordPtr &record);

	OmnString	toString() const;
	void		reset();

	static AosUserBasedACL4Ptr	retrieveFromDb(const u32 &userId, const OmnString &userName, const OmnIpAddr &ipAddr, const u16 &port);
	bool				modifyToDb() const;

	
    u32   getUserId() const {return mUserId;}
    OmnString   getUserName() const {return mUserName;}
    OmnIpAddr   getIpAddr() const {return mIpAddr;}
    u16   getPort() const {return mPort;}


	
    void    setUserId(const u32 &userId) { mUserId = userId;}
    void    setUserName(const OmnString &userName) { mUserName = userName;}
    void    setIpAddr(const OmnIpAddr &ipAddr) { mIpAddr = ipAddr;}
    void    setPort(const u16 &port) { mPort = port;}

	bool    checkAccess(u16 &respCode, OmnString &errmsg);

private:

};
#endif

