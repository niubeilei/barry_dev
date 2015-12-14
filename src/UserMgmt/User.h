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
#ifndef Aos_UserMgmt_User_h
#define Aos_UserMgmt_User_h


#include "aosUtil/Types.h"
#include "Obj/ObjDb.h"
#include "Proggie/ProggieUtil/TcpRequest.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObjImp.h"
#include "UserMgmt/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include "XmlInterface/XmlRc.h"


class TiXmlNode;
class TiXmlElement;

class AosUser : virtual public OmnDbObj
{
	OmnDefineRCObject;

private:

public:

private:
    OmnString   mObjid;
    OmnString   mUsername;
    OmnString   mPasswd;
    OmnString   mEngname;
    OmnString   mFirstName;
    OmnString   mLastName;
    OmnString   mEmail;
    OmnString   mWorkEmail;
    OmnString   mOtherEmail;
    OmnString   mCell;
    OmnString   mHomePhone;
    OmnString   mWorkPhone;
    OmnString   mAddress;
    OmnString   mCountry;
    OmnString   mState;
    OmnString   mCity;
    OmnString   mZip;
    OmnString   mQQ;
    OmnString   mMSN;
    OmnString   mNickname;
    OmnString   mProfession;
    char		mSex;
    OmnString   mBirthday;
    OmnString   mDescription;
    char		mStatus;
    OmnString   mXml;

public:
    AosUser();
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

	static AosUserPtr	retrieveFromDb(const OmnString &username);
	bool				modifyToDb() const;

	
    OmnString   getUserName() const {return mUsername;}
    OmnString   getFirstName() const {return mFirstName;}
    OmnString   getLastName() const {return mLastName;}
    OmnString   getEmail() const {return mEmail;}
    OmnString   getHomePhone() const {return mHomePhone;}
    OmnString   getDescription() const {return mDescription;}

    void    setUserName(const OmnString &userName) { mUsername = userName;}
    void    setFirstName(const OmnString &firstName) { mFirstName = firstName;}
    void    setLastName(const OmnString &lastName) { mLastName = lastName;}
    void    setEmail(const OmnString &email) { mEmail = email;}
    void    setDescription(const OmnString &description) { mDescription = description;}

	static bool checkLogin(const OmnString &name, 
					const OmnString &passwd, 
					OmnString &errmsg);
	static bool checkLogin(const OmnString &name, 
					const OmnString &passwd, 
					const OmnString &container,
					const OmnString &tname,
					OmnString &dataid,
					AosXmlTagPtr &xml,
					OmnString &errmsg);
	bool 	createUser(
					TiXmlElement *userdef,
					AosXmlRc &errcode, 
					OmnString &errmsg);
	static AosXmlTagPtr getUserRcd(
					const OmnString &name, 
					const OmnString &container, 
					OmnString &dataid, 
					OmnString &errmsg);
	static OmnString	getUserRealname(const OmnString &name, const OmnString &container);

};
#endif

