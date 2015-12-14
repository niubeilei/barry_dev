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
#ifndef Aos_UserMgmt_Role_h
#define Aos_UserMgmt_Role_h


#include "aosUtil/Types.h"
#include "Obj/ObjDb.h"
#include "Util/RCObjImp.h"
#include "UserMgmt/Ptrs.h"
#include "Util/String.h"


class AosRole : virtual public OmnDbObj
{
	OmnDefineRCObject;

private:

public:

private:
    u32         mRoleId;
    OmnString   mRoleName;
    OmnString   mDescription;
    char        mStatus;


public:
    AosRole();
	AosRole(const u32 &roleId);
    ~AosRole();

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

	static AosRolePtr	retrieveFromDb(const u32 &roleId);
	bool				modifyToDb() const;

	
    u32   getRoleId() const {return mRoleId;}
    OmnString   getRoleName() const {return mRoleName;}
    OmnString   getDescription() const {return mDescription;}
    char   getStatus() const {return mStatus;}


	
    void    setRoleId(const u32 &roleId) { mRoleId = roleId;}
    void    setRoleName(const OmnString &roleName) { mRoleName = roleName;}
    void    setDescription(const OmnString &description) { mDescription = description;}
    void    setStatus(const char &status) { mStatus = status;}


private:

};
#endif

