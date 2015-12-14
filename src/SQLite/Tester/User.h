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
#ifndef Aos_SQLite_User_h
#define Aos_SQLite_User_h


#include "aosUtil/Types.h"
#include "Obj/ObjDb.h"
#include "Util/RCObjImp.h"
#include "UserMgmt/Ptrs.h"
#include "Util/String.h"


class AosUser : virtual public OmnDbObj
{
	OmnDefineRCObject;

private:

public:

private:
    u32         mUserId;
    OmnString   mUserName;
    OmnString   mFirstName;
    OmnString   mLastName;
    OmnString   mEmail;
    OmnString   mOfficePhone;
    OmnString   mHomePhone;
    OmnString   mDescription;
    char        mStatus;


public:
    AosUser();
    AosUser(const u32 id);
	AosUser(const u32 id, 
			const OmnString &name, 
			const OmnString &firstname,
			const OmnString &lastname,
			const OmnString &email, 
			const OmnString &officephone,
			const OmnString &homephone, 
			const char s);
    ~AosUser();

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

	static AosUserPtr	retrieveFromDb(const u32 &userId);
	bool				modifyToDb() const;
	OmnString			getUserName() const {return mUserName;}

	
    u32   getUserId() const {return mUserId;}
    OmnString   getDescription() const {return mDescription;}
    char   getStatus() const {return mStatus;}

    void    setUserId(const u32 &userId) { mUserId = userId;}
    void    setDescription(const OmnString &description) { mDescription = description;}
    void    setStatus(const char &status) { mStatus = status;}

private:

};
#endif

