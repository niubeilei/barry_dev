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
#ifndef Aos_AccessManager_UserBasedAC_h
#define Aos_AccessManager_UserBasedAC_h

#if 0

#include "aosUtil/Types.h"
#include "Database/DbTable.h"
#include "Obj/ObjDb.h"
#include "Util/RCObjImp.h"
#include "AccessManager/Ptrs.h"
#include "Util/String.h"

class AosAmRequest;
class AosAmResponse;

class AosUserBasedAC : virtual public OmnDbObj
{
	OmnDefineRCObject;

private:

public:

private:
    OmnString   mUserName;
    OmnString   mAppName;
    OmnString   mOperation;
    OmnString   mResource;


public:
    AosUserBasedAC();
	AosUserBasedAC(const OmnString &userName, 
				   const OmnString &appName, 
				   const OmnString &operation);
	AosUserBasedAC(const OmnString &userName, 
				   const OmnString &appName, 
				   const OmnString &operation, 
				   const OmnString &resource);
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

	static AosUserBasedACPtr	retrieveFromDb(const OmnString &userName, const OmnString &appName, const OmnString &operation);
	bool				modifyToDb() const;

	
    OmnString   getUserName() const {return mUserName;}
    OmnString   getAppName() const {return mAppName;}
    OmnString   getOperation() const {return mOperation;}
    OmnString   getResource() const {return mResource;}


	
    void    setUserName(const OmnString &userName) { mUserName = userName;}
    void    setAppName(const OmnString &appName) { mAppName = appName;}
    void    setOperation(const OmnString &operation) { mOperation = operation;}
    void    setResource(const OmnString &resource) { mResource = resource;}

	bool	authorize(AosAmResponse &resp);
	static bool authorize(const AosAmRequest &request, AosAmResponse &response);

private:
};
#endif

#endif
