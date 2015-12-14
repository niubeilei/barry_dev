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
//
// Modification History:
// 2010/06/07	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Security_Session_h
#define Omn_Security_Session_h

#include "Config/ConfigEntity.h"
#include "Debug/Rslt.h"
#include "SearchEngine/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/SessionObj.h"
#include "Security/Ptrs.h"
#include "SEUtil/AcctType.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Locale.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"
#include "XmlParser/Ptrs.h"
#include "XmlInterface/XmlRc.h"
#include "Rundata/Ptrs.h"


class AosSession : virtual public AosSessionObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eBuffInitLength = 1000,
		eBuffIncLength = 1000,
		eDftTotalUsage = 10,
		eDftDuration = 3600,
		eLoginObjLen = 50000,
		// eSsidLen = 40,			Changed to 32, Chen Ding, 05/09/2012
		eSsidLen = 32,
	};

	enum Status
	{
		eIdle,
		eInvalid,

		eActive,
		eExpired

	};

private:
	AosSessionFileMgrPtr	mFileMgr;


	u32				mSeqno;
	u32				mRecordId;

	Status			mStatus;
	u32				mNumUsage;
	u32				mTotalUsage;
	u32				mStartTime;
	u32				mEndtime;
	u32				mDuration;
	u64				mUserid;
	u64				mSd;
	AosAcctType::E	mSessType;
	AosLocale::E	mLocale;

	AosUserAcctObjPtr	mUserAcct;
	u32				mSiteid;
	OmnString		mAppname;
	OmnString		mCid;
	OmnString		mSsid;
	OmnString		mHomeVpd;
	OmnString		mLoginVpd;
	OmnString		mErrmsg;
	OmnString 		mVerificationCode; //Linda, 2011/04/13

	AosXmlTagPtr	mLoginObj;
	AosXmlTagPtr    mAccessedObj;       // Chen Ding, 11/03/2010

	static u32		mDftTotalUsage;
	static u32		mDftDuration;
	bool 			mNeedLogin;
	AosXmlTagPtr	mUserdoc;
	OmnString		mLanguageType;	//Jackie 2011-08-04
	OmnString		mDatabase;	//Phil 2014-04-04
	AosDocClientObjPtr	mDocClient;
	AosObjMgrObjPtr	mObjMgr;
	AosSessionPtr	mMapPrev;
	AosSessionPtr	mMapNext;
	OmnString		mDbName;

public:
	AosSession(
		const AosSessionFileMgrPtr &filemgr, 
		const OmnString &ssid,
		const bool check = true);
	AosSession(const AosSessionFileMgrPtr &fm, const u32 seqno, const u32 recordid);
	AosSession(
		const AosSessionFileMgrPtr &filemgr,
		const u32 siteid,
		const OmnString &appname,
		const OmnString &requester_cid,
		const u64 &userid,
		const OmnString &loginvpd, 
		const OmnString &homevpd); 
	~AosSession();

	// Session Obj Interface
	virtual void			setDbName(const OmnString &db_name) { mDbName = db_name; }
	virtual OmnString		getDbName() { return mDbName; }
	virtual u64 			getUserid() const {return mUserid;}
	virtual OmnString		getCid() const {return mCid;}
	virtual OmnString 		getSsid() const {return mSsid;}
	virtual AosXmlTagPtr	getAccessedDoc() const;
	virtual u64				getAccessedDocUserid(const AosRundataPtr &rdata) const;
	virtual bool 			checkLogin( const AosXmlTagPtr &vpd,
								OmnString &hpvpd, 
								const AosRundataPtr &rdata);
	//Zky3233,Linda, 2011/04/13 Verification Code 
	virtual void			setVerificationCode(const OmnString &id);

	bool	logout();
	bool	isGood() const {return mStatus == eActive;}
	bool	isValid() const {return mStatus != eInvalid;}
	bool 	isSessionGood(OmnString &loginvpd, const AosRundataPtr &rdata);
	OmnString	getErrmsg() const {return mErrmsg;}
	AosUserAcctObjPtr getRequesterAcct(const AosRundataPtr &rdata);
	AosXmlTagPtr	getLoginObj();
	bool		modifyLoginObj(const AosXmlTagPtr &loginobj);
	void	
	setLocation(const u32 seqno, const u32 recordid)
	{
		mSeqno = seqno;
		mRecordId = recordid;
	}

	static AosSessionPtr retrieveSessionStatic(
				const AosSessionFileMgrPtr &filemgr, 
				const OmnString &ssid, 
				const bool check = true);

	bool 	setSession(
				const AosRundataPtr &rdata,
				const OmnString &ssid, 
				const u32 seqno, 
				const u32 recordid,
				const AosXmlTagPtr &userdoc);
	bool	saveToFile();
	void    setAccessedDoc(const AosXmlTagPtr &obj);
	AosLocale::E getLocale();
	// static bool	changeStatus(
	// 		const AosSessionFileMgrPtr &filemgr, 
	// 		const u32 &seqno, 
	// 		const u32 &recordid,
	// 		OmnString &sid, 
	// 		const Status status);
	//james get session status.
	Status getSessionStatus(){return mStatus;}

	//Zky2789
	u64 getSessionId()
	{
		u64 sid = mSeqno;
		sid = (sid << 32) + mRecordId;
		return sid;
	}
	u64 getSecretKey(){return mStartTime;};

	OmnString getVerificationCode() const
	{
		return mVerificationCode;
	}
	bool setSessionIdle();

	// Phil, 04/04/2014
	OmnString getCurDatabase();
	void	setCurDatabase(const AosRundataPtr &rdata, const OmnString &database);

	// Jackie, 08/02/2011
	OmnString getLanguageCode(const AosRundataPtr &rdata);
	void	setLanguageCode(const AosRundataPtr &rdata, const OmnString &langtype);
	OmnString toString() const
	{
		OmnString str;
		str << "Seqno: " << mSeqno << ", RecordId: " << mRecordId
			<< ", Status: " << mStatus
			<< ", UserID: " << mUserid
			<< ", CloudID: " << mCid
			<< ", SSID: " << mSsid;
		if (mUserdoc) str << ". UserDoc: " << mUserdoc->toString();
		return str;
	}

	AosSessionPtr getMapPrev() const {return mMapPrev;}
	AosSessionPtr getMapNext() const {return mMapNext;}
	void setMapPrev(const AosSessionPtr &p) {mMapPrev = p;}
	void setMapNext(const AosSessionPtr &n) {mMapNext = n;}

private:
	// bool	modifyNumUsage();
	bool	readSession1(const bool check);
	bool	readSessionFromFile();
};
#endif

