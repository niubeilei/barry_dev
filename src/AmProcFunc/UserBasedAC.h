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
#ifndef Aos_AmProcFunc_UserBasedAC_h
#define Aos_AmProcFunc_UserBasedAC_h


#include "aosUtil/Types.h"
#include "Obj/ObjDb.h"
#include "Util/RCObjImp.h"
#include "AmProcFunc/Ptrs.h"
#include "Util/String.h"


class AosUserBasedAC : virtual public OmnDbObj
{
	OmnDefineRCObject;

private:

public:

private:
    u32         mUserId;
    OmnString   mUserName;
    OmnString   mApp;
    OmnString   mOpr;
    OmnString   mResource;


public:
    AosUserBasedAC();
	AosUserBasedAC(const u32 &userId, const OmnString &userName, const OmnString &app, const OmnString &opr, const OmnString &resource);
	AosUserBasedAC(const OmnString &userName, const OmnString &app, const OmnString &opr, const OmnString &resource);
    ~AosUserBasedAC();

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

	static AosUserBasedACPtr	retrieveFromDb(const u32 &userId, const OmnString &userName, const OmnString &app, const OmnString &opr, const OmnString &resource);
	bool				modifyToDb() const;

	
    u32   getUserId() const {return mUserId;}
    OmnString   getUserName() const {return mUserName;}
    OmnString   getApp() const {return mApp;}
    OmnString   getOpr() const {return mOpr;}
    OmnString   getResource() const {return mResource;}


	
    void    setUserId(const u32 &userId) { mUserId = userId;}
    void    setUserName(const OmnString &userName) { mUserName = userName;}
    void    setApp(const OmnString &app) { mApp = app;}
    void    setOpr(const OmnString &opr) { mOpr = opr;}
    void    setResource(const OmnString &resource) { mResource = resource;}

	bool checkAccess(u16 &errcode, OmnString &errmsg);

private:

};
#endif // Aos_AmProcFunc_UserBasedAC_h

