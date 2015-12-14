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
#ifndef Aos_AmProcFunc_UserAuth_h
#define Aos_AmProcFunc_UserAuth_h


#include "aosUtil/Types.h"
#include "Obj/ObjDb.h"
#include "Util/RCObjImp.h"
#include "AmProcFunc/Ptrs.h"
#include "Util/String.h"


class AosUserAuth : virtual public OmnDbObj
{
	OmnDefineRCObject;

private:

public:

private:
    u32         mUserId;
    OmnString   mUserName;
    OmnString   mUserPwd;


public:
    AosUserAuth();
    AosUserAuth(const u32 userId);
	AosUserAuth(const OmnString &strUserName, const OmnString &strUserPwd);
    ~AosUserAuth();

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
	virtual OmnString	authStmt() const;
	virtual OmnDbObjPtr	clone() const;
	OmnRslt				serializeFromRecord(const OmnDbRecordPtr &record);

	OmnString	toString() const;
	void		reset();

	static AosUserAuthPtr	retrieveFromDb(const u32 &userId);
	bool				modifyToDb() const;

	
    u32   getUserId() const {return mUserId;}
    OmnString   getUserName() const {return mUserName;}
    OmnString   getUserPwd() const {return mUserPwd;}


	
    void    setUserId(const u32 &userId) { mUserId = userId;}
    void    setUserName(const OmnString &userName) { mUserName = userName;}
    void    setUserPwd(const OmnString &userPwd) { mUserPwd = userPwd;}

	bool checkAccess(u16 &errcode, OmnString &errmsg);

private:

};
#endif

