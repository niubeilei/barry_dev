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
// Each session is defined as a record that is stored in a file. The
// class manages a eMaxSessions number of sessions. When a new session
// is created, it allocates a new session. The class maintains a 
// list of idle sessions. When a session is needed, it uses one of the
// idle sessions. 
//
// There are a number of session docs. When creating a new session, 
// it appends the record to the current session doc. Session files are
// circularly used. 
//
// Session IDs are random letter strings of 16 characters, with the 
// following changes:
// 	session[eSeqnoByte] 	seqno
// 	session[eRecordId1]		the lower 5 bits
// 	session[eRecordId2]		the next 5 bits
// 	session[eRecordId3]		the next 5 bits (use only 15 bits)
//
// Modification History:
// 2010/06/07	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Security/SessionMgr.h"

#include "Alarm/Alarm.h"
#include "Debug/Error.h"
#include "IILUtil/IILId.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEModules/FrontEndSvr.h"
#include "Security/SessionFileMgr.h"
#include "Security/Session.h"
#include "SEBase/SecReq.h"
#include "Security/SimpleAuth.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Util/StrParser.h"
#include "Util/File.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
#include "XmlInterface/XmlRc.h"
#include "API/AosApi.h"


OmnSingletonImpl(AosSessionMgrSingleton, 
				 AosSessionMgr, 
				 AosSessionMgrSelf,
				"AosSessionMgr");

static OmnString sgSessionType_Timed 	= "timed";
static OmnString sgSessionType_Num  	= "num";
static OmnString sgSessionType_Norm  	= "norm";
static OmnString sgInitSessionId = "10";
static OmnString sgInitSeqno = "10";

static AosSessionFileMgrPtr	sgSessionFileMgr;

AosSessionMgr::AosSessionMgr()
:
mLock(OmnNew OmnMutex()),
mDftAcctDur(eDftAcctDur),
mMinLoginNum(eMinLoginNum),
mTimedUnit(eDftTimedUnit),
mStartSsid(0),
mEndSsid(0)	
{
	sgSessionFileMgr = OmnNew AosSessionFileMgr();
}


AosSessionMgr::~AosSessionMgr()
{
}


bool
AosSessionMgr::stop()
{
	return true;
}


bool
AosSessionMgr::start()
{
	return true;
}


bool
AosSessionMgr::config(const AosXmlTagPtr &conf)
{
	//
	// Call this function to configure this class. 
	// It assumes the following:
	//
	//	<config ...>
	//		<SessionMgr>
	//		</SessionMgr>
	//		...
	//	</config>
	aos_assert_r(conf, false);
    AosXmlTagPtr sec_config = conf->getFirstChild(AOSCONFIG_SESSIONMGR);
	if (!sec_config) return true;

	u32 max_sessions = sec_config->getAttrU32("max_cached_sessions", 0);
	if (max_sessions > 0)
	{
		mSessions.setMaxEntries(max_sessions);
	}

	mDftAcctDur = sec_config->getAttrInt(AOSCONFIG_DFTACCTDUR, mDftAcctDur);
	mMinLoginNum = sec_config->getAttrInt(AOSCONFIG_MINLOGIN_NUM, mMinLoginNum);
	mTimedUnit = sec_config->getAttrInt(AOSCONFIG_TIMED_UNIT, eDftTimedUnit);

	return true;
}


AosSessionPtr
AosSessionMgr::getSession1(
		const OmnString &ssid, 
		OmnString &hpvpd, 
		const AosRundataPtr &rdata)
{
	// 1. Check whether the session is in the hash table 'mSessions'. If yes,
	//    it retrieves the session.
	// 2. If not in the hash table, check whether it can be read from the 
	//    file. 
	
	// Check whether it is a legal ssid.
	// Ssids are random letter strings of AosSession::eSsidLen long that are
	// encoded with seqno, recordid, and sd.
	if (ssid.length() != AosSession::eSsidLen) 
	{
		return 0;
	}

	mLock->lock();
	AosSessionPtr session = mSessions.find(ssid);
	if (session)
	{
		// Found the session. Check whether the session is good.
		if (!session->isSessionGood(hpvpd, rdata))
		{
			// Session not good. Need to remove it.
			mSessions.erase(ssid);
			mLock->unlock();
			return 0;
		}
		mLock->unlock();

		// The session is good. Return it.
		return session;
	}

	// Session is not there. 
	session = AosSession::retrieveSessionStatic(sgSessionFileMgr, ssid);
	if (session)
	{
		// Retrieved the session from file.
		if (!session->isSessionGood(hpvpd, rdata))
		{
			// The session is not good.
			mLock->unlock();
			return 0;
		}
 
		// The session is good.
		mSessions.insert(ssid, session);
		mLock->unlock();
		return session;
	}
	mLock->unlock();
	return 0;
}


OmnString
AosSessionMgr::createSession(
	const AosRundataPtr &rdata,
	const AosXmlTagPtr &userdoc,
	const OmnString &loginvpd,
	const OmnString &hpvpd,
	AosSessionObjPtr &sion)
{
	// This function creates a new session.
	// modified by andy
	aos_assert_r(userdoc, "");
	sion = 0;
	OmnString appname = rdata->getAppname();
	u32 siteid = rdata->getSiteid();
	u64 userid = userdoc->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
	OmnString cid = AosCloudidSvr::getCloudid(userid, rdata);
	if (rdata->getUserid() ==0 ) rdata->setUserid(userid);
	
	u32 seqno, recordid;
	sgSessionFileMgr->getLocation(seqno, recordid);
	aos_assert_r(seqno < AosSessionFileMgr::eMaxSeqno, "");
	aos_assert_r(recordid > 0, "");

	mLock->lock();
	AosSessionPtr session;
	try
	{	
		session = OmnNew AosSession(sgSessionFileMgr, siteid, appname, cid,
			userid, loginvpd, hpvpd);
	}
	catch(...)
	{
		OmnAlarm << "Failed to create session!" << enderr;
		mLock->unlock();
		return "";
	}
	aos_assert_rl(session, mLock, "");
	OmnString ssid = getNextSsidLocked(rdata, seqno, recordid, userdoc, session);
	aos_assert_rl(ssid != "", mLock, "");
	
	//close older session  tank 2/12/2010
	// Chen Ding, 02/11/2012, We do not save sessions to IILs anymore.
	bool rs = setSessionToIIL(userid, seqno, recordid, rdata);
	aos_assert_rl(rs, mLock, "");
	mLock->unlock();
	sion = session;
	return ssid;
}


OmnString
AosSessionMgr::getNextSsidLocked(
		const AosRundataPtr &rdata,
		const u32 seqno, 
		const u32 recordid, 
		const AosXmlTagPtr &userdoc,
		const AosSessionPtr &session)
{
	// This function creates a new ssid. 
	
	if (AosGetSelfServerId()< 0)
	{
		OmnAlarm << "Session Manager is a service running on a front-end"
			<< " machine, but the application was not configured with "
			<< "server ID!" << enderr;
		return "";
	}

	OmnString ssid(AosSession::eSsidLen, ' ', true);
	char *data = (char *)ssid.data();

	// Chen Ding, 05/31/2012
	// AosRandomLetterStr(AosSession::eSsidLen, data);
	AosRandomLetterOnlyStr(AosSession::eSsidLen, data);
// Chen Ding, 05/09/2012
// Check whether there are spaces in 'data'
for (int i=0; i<AosSession::eSsidLen; i++)
{
	if (data[i] == ' ')
	{
		OmnAlarm << "There are spaces in sessionid: " << data << enderr;
	}
}

	u64 sd = AosSeIdGenMgr::getSelf()->nextSsid(rdata);
	sd = (sd << 16) + (u32)AosGetSelfServerId();
	encodeSsid1(ssid, seqno, recordid, sd);
// Chen Ding, 05/09/2012
// Check whether there are spaces in 'data'
for (int i=0; i<AosSession::eSsidLen; i++)
{
	if (data[i] == ' ')
	{
		OmnAlarm << "There are spaces in sessionid: " << ssid.data() 
			<< ":" << seqno << ":" << recordid << ":" << sd << enderr;
	}
}

	bool rslt = session->setSession(rdata, ssid, seqno, recordid, userdoc);
	aos_assert_r(rslt, "");
	rslt = session->saveToFile();
	aos_assert_r(rslt, "");
	if (session->isGood())
	{
		mSessions.insert(ssid, session);
		return ssid;
	}

	return "";
}


/* Chen Ding, 02/11/2012
 * We do not want to save sessions in IILs.*/
bool 
AosSessionMgr::setSessionToIIL(
		u64 &userid,
		const u32 seqno,
		const u32 recordid,
		const AosRundataPtr &rdata)
{
	// Sessions are saved in an IIL whose value portion is userid, docid portion
	// is [seqno, offset]. This function uses the IIL to retrieve the entry. 
	// If not found, it returns false. Otherwise, it removes the entry from 
	// the IIL, modifies the session as idle, and returns true.
	u64 did = 0;
	//OmnString iilname = AOSZTG_SESSIONIIL;
	u64 iilname = AOSZTG_SESSIONIILID;

	bool found, is_unique;
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, false);
	u64 key = (userid << 32) + rdata->getUrldocDocid();
	bool rs = iilclient->getDocid(iilname, key, did, 0, found, is_unique, rdata);
	if (!rs || !found)
	{
		// Not in the IIL yet. Add it.
		u64 dd = seqno;
		dd = (dd << 32) + recordid;
		rs = iilclient->addU64ValueDocToTable(iilname, key, dd, true, false, rdata);
		aos_assert_r(rs, false);
		return rs;
	}
	
	// Found it. Retrieve the record's position: [seqno, offset]
	try
	{
		u32 seq = (did >> 32);
		u32 rec = (u32)did;
		AosSessionPtr session = OmnNew AosSession(sgSessionFileMgr, seq, rec);
		session->setSessionIdle();
		mSessions.erase(session->getSsid());
	}

	catch (...)
	{
		OmnAlarm << "Failed to create session!" << enderr;
		return false;
	}

	// Remove the entry
	rs = iilclient->removeU64ValueDoc(iilname, key, did, rdata);
	if (!rs)
	{
		OmnAlarm << "Failed removing: " << userid<< ":" << did << enderr;
		return false;
	}

	// Add the entry
	u64 dd = seqno;
	dd = (dd << 32) + recordid;
	rs = iilclient->addU64ValueDocToTable(iilname, key, dd, true, false, rdata);
	return rs;
}


void 
AosSessionMgr::displaySession(const OmnString &ssid)
{
	AosSessionPtr session = AosSession::retrieveSessionStatic(sgSessionFileMgr, ssid, false);
	if (session)
	{
		OmnScreen << "Session: " << session->toString() << ":" << ssid << endl;
	}
	else
	{
		OmnScreen << "Session not found: " << ssid << endl;
	}
}

