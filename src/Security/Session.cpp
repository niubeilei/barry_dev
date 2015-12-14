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
// 10/02/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Security/Session.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Error.h"
#include "Debug/Except.h"
#include "SEUtil/ValueDefs.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/ObjMgrObj.h"
#include "SEInterfaces/UserAcctObj.h"
#include "Security/SecurityMgr.h"
#include "Security/SessionMgr.h"
#include "Security/SessionFileMgr.h"
#include "Security/SimpleAuth.h"
#include "Thread/Mutex.h"
#include "SEBase/SecReq.h"
#include "Util/File.h"
#include "Util/StrParser.h"
#include "Util/Buff.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlInterface/XmlRc.h"


u32 AosSession::mDftTotalUsage = AosSession::eDftTotalUsage;
u32 AosSession::mDftDuration = AosSession::eDftDuration;
const u32 sgPoison = 0x36dae453;


AosSession::AosSession(
		const AosSessionFileMgrPtr &filemgr, 
		const OmnString &ssid, 
		const bool check)
:
mFileMgr(filemgr),
mStatus(eIdle),
mUserid(0),
mSsid(ssid),
mNeedLogin(false),
mObjMgr(AosSecurityMgr::getSelf()->getObjMgr())
{
	if (!readSession1(check))
	{
		mStatus = eInvalid;
	}
}


AosSession::AosSession(
		const AosSessionFileMgrPtr &filemgr,
		const u32 siteid,
		const OmnString &appname,
		const OmnString &requester_cid,
		const u64 &userid,
		const OmnString &loginvpd, 
		const OmnString &homevpd) 
:
mFileMgr(filemgr),
mStatus(eIdle),
mUserid(userid),
mSiteid(siteid),
mAppname(appname),
mCid(requester_cid),
mHomeVpd(homevpd),
mLoginVpd(loginvpd),
mNeedLogin(false),
mObjMgr(AosSecurityMgr::getSelf()->getObjMgr())
{
}


AosSession::AosSession(
		const AosSessionFileMgrPtr &filemgr, 
		const u32 seqno, 
		const u32 recordid)
:
mFileMgr(filemgr), 
mSeqno(seqno),
mRecordId(recordid),
mStatus(eIdle),
mUserid(0),
mNeedLogin(false),
mObjMgr(AosSecurityMgr::getSelf()->getObjMgr())
{
	OmnFilePtr file = filemgr->getFile(seqno);
	if (!file)
	{
		OmnString errmsg = "Invalid seqno: ";
		errmsg << seqno << ":" << recordid;
		OmnExcept e(__FILE__, __LINE__, errmsg);
		throw e;
	}
	if (!readSessionFromFile())
	{
		OmnString errmsg = "Session not found: ";
		errmsg << seqno << ":" << recordid;
		OmnExcept e(__FILE__, __LINE__, errmsg);
		throw e;
	}
}


AosSession::~AosSession()
{
}


bool
AosSession::setSession(
		const AosRundataPtr &rdata,
		const OmnString &ssid,
		const u32 seqno, 
		const u32 recordid,
		const AosXmlTagPtr &userdoc)
{
	// It creates a new session. 
	// This function assumes that the user must have a valid account.
	//
	// <session 
	// 		AOSOBJID=AOSOBJID_SSID + seqno 
	// 		AOSZTG_SESSIONID="ssid"		
	// 		zky_otype=AOSOTYPE_SESSION
	// 		zky_ssid="xxx"
	// 		zky_loginvpd="xxx"
	// 		zky_hpvpd="xxx"
	// 		zky_type="xxx"/>
	// where AOSZTG_SESSIONID is a system reserved attribute name for 
	// session status. Allowed values are: 1 for active, 2 for idle. 
	//
	// If it failed to create a session, it returns 0, which means
	// that will be no session. Otherwise, it returns a u64 that
	// uniquely identifies the session.
	
	mSsid = ssid;
	mSeqno = seqno;
	mRecordId = recordid;
	mStartTime = OmnGetSecond();
	mLocale = AosLocale::toEnum(userdoc->getAttrStr(AOSTAG_LOCALE));

	// Set the session type based on the account type
	mSessType = AosAcctType::toEnum(
			userdoc->getAttrStr(AOSTAG_ACCT_TYPE, AOSVALUE_ACCTTYPE_NORM));
	switch (mSessType)
	{
	case AosAcctType::eNormal:
	 	 mStatus = eActive;
		 rdata->setOk();
		 return true;
	
	case AosAcctType::eTimed:
		 // The account is time-based. If the account has not been 
		 // activated, activate it now. Otherwise, there should be
		 // an endtime.
		 mEndtime = userdoc->getAttrU64(AOSTAG_ENDTIME, 0);
		 if (mEndtime == 0)
		 {
			// Not activated yet. Need to activate it. The time duration is
			// defined in the attribute AOSTAG_ACCT_DUR. 
			mDuration = userdoc->getAttrU64(AOSTAG_ACCT_DUR, 0); 

			aos_assert_rr(mObjMgr, rdata, false);
			u64 userid = rdata->setUserid(mObjMgr->getSuperUserDocid());
			OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
			if (mDuration == 0)
			{
				// Account duration is not set. Set to default
				mDuration = mDftDuration;
				OmnString duration;
				duration << mDuration;
				AosModifyDocAttrStr(rdata, userdoc, AOSTAG_ACCT_DUR, duration); 
			}

			// Calculate the endtime
			mEndtime = mStartTime + mDuration;
			OmnString endtime;
			endtime << mEndtime;

			// Chen Ding, 2013/01/19
			// aos_assert_rr(mDocClient, rdata, false);
			// mDocClient->modifyAttrStr1(rdata,
			// 		userdoc->getAttrU64(AOSTAG_DOCID, 0),
			// 		userdoc->getAttrStr(AOSTAG_OBJID),
			// 		AOSTAG_ENDTIME, endtime, "0", false, true, true);
			AosModifyDocAttrStr(rdata, userdoc, AOSTAG_ENDTIME, endtime);
			rdata->setUserid(userid);
			rdata->setAppname(appname);
		 }

		 // Chen Ding, 05/09/2011
		 // if (mEndtime <= mStartTime)
		 if (mStartTime >= mEndtime)
		 {
			// The account expired.
			OmnString status = userdoc->getAttrStr(AOSTAG_USERSTATUS);
			mErrmsg = "Account Expired!";
			if (status != AOSUSRSTATUS_INVALID)
			{
				aos_assert_rr(mObjMgr, rdata, false);
				u64 userid = rdata->setUserid(mObjMgr->getSuperUserDocid());
				OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);

				// Chen Ding, 2013/01/19
				// aos_assert_rr(mDocClient, rdata, false);
				// mDocClient->modifyAttrStr1(rdata,
				// 	userdoc->getAttrU64(AOSTAG_DOCID, 0), 
				// 	userdoc->getAttrStr(AOSTAG_OBJID),
				// 	AOSTAG_USERSTATUS, AOSUSRSTATUS_INVALID, 0, 
				// 	true, false, true);
				AosModifyDocAttrStr(rdata, userdoc, AOSTAG_USERSTATUS, AOSUSRSTATUS_INVALID);
				rdata->setAppname(appname);
				rdata->setUserid(userid);
			}
			mStatus = eExpired;
			rdata->setError() << mErrmsg;
			return false; 
		 }
		 mStatus = eActive;
		 rdata->setOk();
		 return true;
	
	case AosAcctType::eUsage:
		 {
		 mNumUsage = userdoc->getAttrU32(AOSTAG_NUMUSAGE, 0);
		 mTotalUsage = userdoc->getAttrInt(AOSTAG_TOTALUSAGE, mDftTotalUsage);
		 if (mTotalUsage <= 0)
		 {
			mTotalUsage = mDftTotalUsage;
			OmnString totalusage;
			totalusage << mTotalUsage;

			// Chen Ding, 2013/01/19
			// aos_assert_rr(mDocClient, rdata, false);
			// mDocClient->modifyAttrStr1(rdata,
			// 	userdoc->getAttrU64(AOSTAG_DOCID, 0),
			// 	userdoc->getAttrStr(AOSTAG_OBJID),
			// 	AOSTAG_TOTALUSAGE, totalusage, "0", false, true, true);
			AosModifyDocAttrStr(rdata, userdoc, AOSTAG_TOTALUSAGE, totalusage);
		 }

		 if (mNumUsage < 0) mNumUsage = 0;
		 
		 mNumUsage++;
		 if (mNumUsage > mTotalUsage) 
		 {
			// It exceeds the number.
			OmnString status = userdoc->getAttrStr(AOSTAG_USERSTATUS);
			if (status != AOSUSRSTATUS_INVALID)
			{
				aos_assert_rr(mObjMgr, rdata, false);
				u64 userid = rdata->setUserid(mObjMgr->getSuperUserDocid());
				OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);

				// Chen Ding, 2013/01/19
				// aos_assert_rr(mDocClient, rdata, false);
				// rslt = mDocClient->modifyAttrStr1(rdata,
				// 		userdoc->getAttrU64(AOSTAG_DOCID, 0), 
				// 		userdoc->getAttrStr(AOSTAG_OBJID),
				// 		AOSTAG_USERSTATUS, 
				// 		AOSUSRSTATUS_INVALID, 0, true, false, true);
				AosModifyDocAttrStr(rdata, userdoc, AOSTAG_USERSTATUS, AOSUSRSTATUS_INVALID);
				rdata->setAppname(appname);
				rdata->setUserid(userid);
			}
			mStatus = eExpired;
			mErrmsg = "Account Expired!";
			rdata->setError() << mErrmsg;
			return false; 
		 }

		 mStatus = eActive;
		 OmnString numusage;
		 numusage << mNumUsage;

		 // Chen Ding, 2013/01/19
		 // aos_assert_rr(mDocClient, rdata, false);
		 // rslt = mDocClient->modifyAttrStr1(rdata,
		 // 			userdoc->getAttrU64(AOSTAG_DOCID, 0),
		 // 			userdoc->getAttrStr(AOSTAG_OBJID),
		 // 			AOSTAG_NUMUSAGE, numusage, "0", false, true, true);
		 if (!AosModifyDocAttrStr(rdata, userdoc, AOSTAG_NUMUSAGE, numusage))
		 {
			 mStatus = eInvalid;
			 return false;
		 }
		 rdata->setOk();
		 return true;
		 }

	case AosAcctType::eInvalid:
	default:
		 mStatus = eInvalid;
		 mSessType = AosAcctType::eInvalid;
		 mErrmsg = "Account Invalid!";
		 rdata->setError() << mErrmsg;
		 return false;
	}

	mStatus = eInvalid;
	mErrmsg = "Internal Error!";
	rdata->setError() << mErrmsg;
	OmnShouldNeverComeHere;
	return false;
}


bool	
AosSession::logout()
{
	mStatus = eIdle;
	return true;
}


bool
AosSession::isSessionGood(OmnString &loginvpd, const AosRundataPtr &rdata)
{
	// It checks whether the sesion is good.  A session is good if:
	// (1). AOSTAG_STATUS is active
	// (2). If session type is timed, it is not expired
	//      If session type is number of times, the number 
	//      is not exceeded.
	// If a session is not good, it will return false. If the session
	// has the login vpd, it will return the login vpd and the
	// errcode is set to 'eAosXmlInt_Relogin'. Otherwise, it is an 
	// error, meaning that the session is not good but no login 
	// vpd is found or there are internal errors. In this case, it
	// sets errcode = 'eAosXmlInt_ResetSession' and returns false.
	OmnString type;
	if (mStatus != eActive) 
	{
		AosSetError(rdata, "access_denied_003");
		goto getlogin;
	}

	// It is active.
	if (mSessType == AosAcctType::eTimed)
	{
		// It is time-based. 
		if (mEndtime == 0)
		{
			// This should not happen. Will reset the session.
			AosSetError(rdata, "access_denied");
			OmnAlarm << rdata->getErrmsg() << ": Missing end time!" << enderr;
			goto getlogin;
		}

		u32 crttime = OmnGetSecond();
		if (crttime >= mEndtime) 
		{
			// Session expired. Need to relogin
			AosSetError(rdata, "access_denied_003");
			goto getlogin;
		}
		return true;
	}

	if (mSessType == AosAcctType::eUsage)
	{
		// It is number of times based. This is managed by the login
		// process. Simply return true.
		return true;
	}

	if (mSessType == AosAcctType::eNormal) return true;

	AosSetError(rdata, "access_denied_004");
	OmnAlarm << rdata->getErrmsg() << ": Invalid session type: " << type << enderr;
	goto getlogin;

getlogin:
	loginvpd = mLoginVpd;

	// Chen Ding, 01/27/2012
	// errcode = eAosXmlInt_Ok;
	// if (loginvpd != "") errcode = eAosXmlInt_Relogin;
	return false;
}


bool 
AosSession::checkLogin(
		const AosXmlTagPtr &vpd,
		OmnString &hpvpd, 
		const AosRundataPtr &rdata)
{
	// This function checks whether the retrieved object ('objid') is
	// the session's login vpd. If yes, whether the hpvpd is present. 
	// If yes, it sets the vpd name to 'hpvpd'.
	//
	// It returns true if and only if the session is correct and 
	// good, the object 'objid' is the login vpd, and the home page
	// vpd is not empty.
	//
	// The function returns false if it failed the checking. It
	// can fail in several ways:
	// 1. Login sesion expired
	// 2. Access denied
	//
	// a. If it requires re-login, errcode = eAosXmlInt_Relogin,
	// b. If errors, errcode = eAosXmlInt_Denied,
	// c. Otherwise, errcode = eAosXmlInt_Ok
	
	// aos_assert_r(sdoc->isRoot(), false);

	// Note that this function assumes the session is good. 
	// IT WILL NOT RE-CHECK SESSION
	// Check whether it 'objid' is the loginvpd. If it is 
	// a login vpd, it will retrieve the home vpd. The caller
	// should retrieve the home vpd instead of the login vpd.
	
	//James 2011/02/17
	//check the user AOSTAG_PARENTC is same as the loginvpd container AOSTAG_PARENTC.
	AosUserAcctObjPtr user_acct = getRequesterAcct(rdata);
	if (user_acct)
	{
		OmnString uctn = user_acct->getAttrStr(AOSTAG_PARENTC);
		OmnString lgnctn = vpd->getAttrStr(AOSTAG_PARENTC);
		if (lgnctn != "" && mSiteid != 0)
		{
			AosXmlTagPtr lgnctndoc = AosGetDocByObjid(lgnctn, rdata);
			if (lgnctndoc)
			{
				OmnString uaccount = lgnctndoc->getAttrStr(AOSTAG_CTNR_USER);
				if (uaccount!="" && uctn.indexOf(uaccount, 0) == -1) 
				{
					//set the flag for get the loginobj.
					mNeedLogin = true;
					return true;
				}
			}
		}
	}
	if (mNeedLogin) mNeedLogin = false;
		
	// Zky-2281 Ice Yu 2011/1/7
	hpvpd = mHomeVpd;
	if (hpvpd == "")
	{
		hpvpd = vpd->getAttrStr(AOSTAG_AFTLGI_VPD, "");
	}

	if (hpvpd != "")
	{
		// It is a login attempt. If the session type is 
		// sgSessionType_Num, it will increment the number.
		// If it exceeds the maximum, the session becomes
		// invalid.
		if (mSessType == AosAcctType::eUsage)
		{
			if (mTotalUsage < 0) mTotalUsage = mDftTotalUsage;
			if (mNumUsage < 1) mNumUsage = 1;
			else mNumUsage++;
			if (mNumUsage > mTotalUsage) 
			{
				// This means that the session is no longer valid.
				// Need to prompt the user for login.

				// Chen Ding, 01/27/2012
				// rdata->setError(eAosXmlInt_Relogin) << "Relogin";
				AosSetErrorUser(rdata, "access_denied") << enderr;
				return false;
			}
			// modifyNumUsage();
			saveToFile();
		}

		// Zky-2281 Ice Yu 2011/1/7
		if (hpvpd != "")
		{
			// It is a valid login session and there is the home vpd.
			return true;
		}

		// It is a login session but there is no home vpd. This is 
		// normally an error.
		OmnAlarm << "Failed to retrieve home vpd!" << enderr;

		// Do not treat it as login. Proceed as normal reading.
		rdata->setOk();
		return false;
	}

	// It is not a login session or the login session does not have
	// a homepage vpd. Treat it as a normal readying.
	rdata->setOk();
	return false;
}


bool
AosSession::saveToFile()
{
	OmnFilePtr file = mFileMgr->getFile(mSeqno);
	aos_assert_r(file, false);
	u64 start = mFileMgr->getStart(mRecordId);
	aos_assert_r(start > 0, false);

	// In the current implementations, we simply write every field
	// separately. This may be an area for performance 
	// improvements.
	//AosBuff buff(eBuffInitLength, eBuffIncLength AosMemoryCheckerArgs);
	//felicia, 2012/09/26
	AosBuff buff(eBuffInitLength AosMemoryCheckerArgs);
	buff.setU32(sgPoison);
	buff.setU32((u32)mStatus);
	buff.setU32(mNumUsage);
	buff.setU32(mTotalUsage);
	buff.setU32(mStartTime);
	buff.setU32(mEndtime);
	buff.setU32(mDuration);
	buff.setU64(mUserid);
	buff.setU64(mSd);
	buff.setU32((u32)mSessType);
	buff.setU32(mSiteid);
	buff.setOmnStr(mAppname);
	buff.setOmnStr(mCid);
	buff.setOmnStr(mSsid);
	buff.setOmnStr(mHomeVpd);
	buff.setOmnStr(mLoginVpd);
	if (mLoginObj)
	{
		OmnString docstr = mLoginObj->toString();
		if (docstr.length() < eLoginObjLen)
		{
			buff.setChar('y');
			buff.setOmnStr(docstr);
		}
		else
		{
			buff.setChar('n');
		}
	}
	else
	{
		buff.setChar('n');
	}
	file->setU32(start, (u32)buff.dataLen(), false);
	file->put(start+sizeof(u32), buff.data(), buff.dataLen(), true);
	return true;
}


bool
AosSession::readSession1(const bool check)
{
	if (!AosSessionMgr::decodeSsid1(mSsid, mSeqno, mRecordId, mSd)) return false;
	
	OmnString ssid = mSsid;
	bool rslt = readSessionFromFile();
	if (!rslt)
	{
		// The session is not good
		return false;
	}

	if (!check) return true;

	if (ssid != mSsid)
	{
		OmnScreen << "SSID incorrect: " << ssid << ":" << mSsid << endl;
		AosSessionMgr::displaySession(ssid);
		OmnScreen << toString() << endl;
		return false;
	}
	return true;
}


bool
AosSession::readSessionFromFile()
{
	if (mRecordId >= AosSessionFileMgr::eMaxRecordId || 
		mSeqno >= AosSessionFileMgr::eMaxSeqno) return false;
	OmnFilePtr file = mFileMgr->getFile(mSeqno);
	if (!file) return false;
	u64 start = mFileMgr->getStart(mRecordId);
	aos_assert_r(start > 0, false);

	// Record format is:
	// 		length		(four bytes)
	// 		data		(variable)
	u32 len = file->readBinaryU32(start, 0);
	if (!(len > 0 && len < AosSessionFileMgr::eRecordSize))
	{
		// The session is not good. 
		return false;
	}

	AosBuff buff(len+10, 0 AosMemoryCheckerArgs);
	// int num = file->readToBuff(start + sizeof(u32) + sizeof(u32), len, buff.data());
	int num = file->readToBuff(start + sizeof(u32), len, buff.data());
	aos_assert_r(num >= 0 && (u32)num == len, false);
	buff.setDataLen(len);

	u32 poison 		= buff.getU32(0); 
	aos_assert_r(poison == sgPoison, false);
	mStatus 		= (Status)buff.getU32(0);
	mNumUsage 		= buff.getU32(0);
	mTotalUsage 	= buff.getU32(0);
	mStartTime 		= buff.getU32(0);
	mEndtime 		= buff.getU32(0);
	mDuration 		= buff.getU32(0);
	mUserid 		= buff.getU64(0);
	mSd 			= buff.getU64(0);
	mSessType 		= (AosAcctType::E)buff.getU32(0);
	mSiteid 		= buff.getU32(0);
	mAppname 		= buff.getOmnStr("");
	mCid 			= buff.getOmnStr("");
	mSsid			= buff.getOmnStr("");
	mHomeVpd 		= buff.getOmnStr("");
	mLoginVpd 		= buff.getOmnStr("");
	char ss 		= buff.getChar('n');
	if (ss == 'y')
	{
		OmnString docstr = buff.getOmnStr("");
		if (docstr.length() > 0)
		{
			AosXmlParser parser;
			mLoginObj= parser.parse(docstr, "" AosMemoryCheckerArgs);
		}
	}

	return true;
}


AosSessionPtr
AosSession::retrieveSessionStatic(
		const AosSessionFileMgrPtr &filemgr,
		const OmnString &ssid, 
		const bool check)
{
	// This is a static member function. It tries to read the session
	// identified by 'ssid'. If found, it creates the session based on 
	// the data stored in file. Otherwise, it creates a new instance.
	try
	{
		AosSessionPtr session = OmnNew AosSession(filemgr, ssid, check);
		aos_assert_r(session, 0);
		if (!session->isValid()) return 0;
		return session;
	}

	catch (...)
	{
		OmnAlarm << "Faild to create session!" << enderr;
		return 0;
	}
}


AosXmlTagPtr
AosSession::getLoginObj() 
{
	if (!mLoginObj)
	{
		OmnString docstr = "<";
		docstr << AOSTAG_LOGIN_OBJ << "></" << AOSTAG_LOGIN_OBJ << ">";
		AosXmlParser parser;
		mLoginObj = parser.parse(docstr, "" AosMemoryCheckerArgs);
	}
	//James 2010/02/18
	if (mNeedLogin)
	{
		return 0;
	}
	return mLoginObj;
}


bool
AosSession::modifyLoginObj(const AosXmlTagPtr &loginobj)
{
	mLoginObj = loginobj;
	return saveToFile();
}


AosUserAcctObjPtr
AosSession::getRequesterAcct(const AosRundataPtr &rdata) 
{
	if (!mUserAcct)
	{
		aos_assert_rr(mUserid, rdata, NULL);
		mUserAcct = AosGetUserAcct(mUserid, rdata);	
		aos_assert_r(mUserAcct, NULL);
	}
	return mUserAcct;
}


void
AosSession::setAccessedDoc(const AosXmlTagPtr &obj)
{
	mAccessedObj = obj;
}


AosXmlTagPtr
AosSession::getAccessedDoc() const
{
	if (mAccessedObj)
		return mAccessedObj;
	return NULL;
}


AosLocale::E 
AosSession::getLocale()
{
	return mLocale;
}


bool
AosSession::setSessionIdle()
{
	mStatus = eIdle;
	return saveToFile();
}

//Phil, 04/04/2014
OmnString
AosSession::getCurDatabase()
{
	return mDatabase;
}

void
AosSession::setCurDatabase(const AosRundataPtr &rdata, const OmnString &database)
{
	if (database == "")
	{
		OmnString errmsg = "Got empty database name";
		AosSetError(rdata, "missing_database") << errmsg << enderr;
		return;
	}

	mDatabase = database;
}
	
// Jackie, 08/02/2011
OmnString 
AosSession::getLanguageCode(const AosRundataPtr &rdata)
{
	if(mLanguageType != "")
	{
		return mLanguageType;
	}
	if (!mUserdoc)
	{
		if (!mUserid) return "";
		mUserdoc = AosGetDocByDocid(mUserid, rdata);
		if (!mUserdoc) return "";
	}
	mLanguageType = mUserdoc->getAttrStr(AOSTAG_LANGUAGECODE);
	return mLanguageType;
}


void 
AosSession::setLanguageCode(const AosRundataPtr &rdata, const OmnString &langtype)
{
	if(langtype == "")
	{
		OmnString errmsg = "Language Code is Null: ";
		AosSetError(rdata, "missing_dictionary") << errmsg << enderr;
		return;
	}
	mLanguageType = langtype;
}


u64
AosSession::getAccessedDocUserid(const AosRundataPtr &rdata) const
{
	return AosSecReq::getAccessedDocUserid(mAccessedObj, rdata);
}


void
AosSession::setVerificationCode(const OmnString &code)
{
	mVerificationCode = code;
}

