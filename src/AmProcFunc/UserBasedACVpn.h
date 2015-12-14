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
#ifndef Aos_AmProcFunc_UserBasedACVpn_h
#define Aos_AmProcFunc_UserBasedACVpn_h


#include "aosUtil/Types.h"
#include "Obj/ObjDb.h"
#include "Util/RCObjImp.h"
#include "AmProcFunc/Ptrs.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Database/DbTable.h"


typedef struct aos_am_vpn_acl
{
	int 		mId;
	int 		mWeekday;
	char  		mSTime[6];
	char  		mETime[6];
	char 		mUser[16];
	u32 		mUid;
	u32 		mSip;
	u32 		mSMask;
	u16 		mSSPort;
	u16 		mSEPort;
	u32 		mDip;
	u32 		mDMask;
	u16 		mDSPort;
	u16 		mDEPort;
	u16 		mProto;
	char 		mAllow_deny;
	u16 		mPriority;
}aos_am_vpn_acl_t;


class AosUserBasedACVpn : virtual public OmnDbObj
{
	OmnDefineRCObject;

private:

public:

private:
    u32         mUserId;
    OmnString   mUserName;
    OmnIpAddr   mSIpAddr;
    u16         mSPort;
    OmnIpAddr   mDIpAddr;
    u16         mDPort;
    u8          mProto;


public:
    AosUserBasedACVpn();
	AosUserBasedACVpn(const u32 &userId, const OmnString &userName, const OmnIpAddr &sIpAddr, const u16 &sPort, const OmnIpAddr &dIpAddr, const u16 &dPort, const u8 &proto);
	AosUserBasedACVpn(const OmnString &userName, const OmnIpAddr &sIpAddr, const u16 &sPort, const OmnIpAddr &dIpAddr, const u16 &dPort, const u8 &proto);
    ~AosUserBasedACVpn();

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

	static AosUserBasedACVpnPtr	retrieveFromDb(const u32 &userId, const OmnString &userName, const OmnIpAddr &sIpAddr, const u16 &sPort, const OmnIpAddr &dIpAddr, const u16 &dPort, const u8 &proto);
	bool				modifyToDb() const;

	
    u32   getUserId() const {return mUserId;}
    OmnString   getUserName() const {return mUserName;}
    OmnIpAddr   getSIpAddr() const {return mSIpAddr;}
    u16   getSPort() const {return mSPort;}
    OmnIpAddr   getDIpAddr() const {return mDIpAddr;}
    u16   getDPort() const {return mDPort;}
    u8   getProto() const {return mProto;}


	
    void    setUserId(const u32 &userId) { mUserId = userId;}
    void    setUserName(const OmnString &userName) { mUserName = userName;}
    void    setSIpAddr(const OmnIpAddr &sIpAddr) { mSIpAddr = sIpAddr;}
    void    setSPort(const u16 &sPort) { mSPort = sPort;}
    void    setDIpAddr(const OmnIpAddr &dIpAddr) { mDIpAddr = dIpAddr;}
    void    setDPort(const u16 &dPort) { mDPort = dPort;}
    void    setProto(const u8 &proto) { mProto = proto;}

	bool    checkAccess(u16 &respCode, OmnString &errmsg);
	bool    getAllAcl(OmnDbTablePtr & table);

private:

};
#endif

