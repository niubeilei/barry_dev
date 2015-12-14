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
//
// Modification History:
// 10/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
Chen Ding, 01/27/2012
This is not used anymore. 

#include "SeReqProc/ResumeReq.h"

#include "SEInterfaces/DocClientObj.h"
#include "EventMgr/EventMgr.h"
#include "SEServer/SeReqProc.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "SEModules/OnlineMgr.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"

static u32 sgCrtReqId = OmnGetSecond();
static AosXmlTagPtr sgPendingReqs[AosResumeReq::eMaxPendingReq];
static u32          sgPendingSecs[AosResumeReq::eMaxPendingReq];
static OmnString    sgPendingOprs[AosResumeReq::eMaxPendingReq];
static u32          sgPendingIds[AosResumeReq::eMaxPendingReq];
static OmnString    sgPendingLoginvpd[AosResumeReq::eMaxPendingReq];
static OmnMutex sgPendingReqLock;


AosResumeReq::AosResumeReq(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_RESUME_REQ, AosSeReqid::eResumeReq, rflag)
{
}


bool 
AosResumeReq::proc(const AosRundataPtr &rdata)
{
	// A user requests logging out. Logout means canceling the session.
	AOSLOG_ENTER_R(rdata, false);		
	AosWebRequestPtr req = rdata->getWebRequest();

	AosXmlTagPtr root = rdata->getRequestRoot();

	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	// It is to resume an interrupted request. 
	// 	<request>
	// 		...
	// 		<item name='zky_reqidx'><![CDATA[xxx]]></item>
	// 		<item name='zky_idx'><![CDATA[xxx]]></item>
	// 		<item name='zky_ctnr'><![CDATA[xxx]]></item>
	// 		<item name='zky_uname'><![CDATA[xxx]]></item>
	// 		<item name='zky_passwd'><![CDATA[xxx]]></item>
	// 	</request>
	//AosXmlRc errcode = eAosXmlInt_Denied;

	// Find the request
	u32 idx;
	AosXmlTagPtr newroot = getPendingReq(root, idx, rdata);
	if (!newroot)
	{
		rdata->setError() << "Failed to retrieve the pending request!";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString container = root->getChildTextByAttr("name", AOSTAG_CONTAINER);
	if (container == "")
	{
		rdata->setError() << "Missing container!";
		resendLoginReq(req, idx, rdata);
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString username = root->getChildTextByAttr("name", AOSTAG_USERNAME);
	if (username == "")
	{
		rdata->setError() << "Missing Username!";
		resendLoginReq(req, idx, rdata);
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString passwd = root->getChildTextByAttr("name", AOSTAG_PASSWD);
	if (passwd == "")
	{
		rdata->setError() << "Missing Passwd!";
		resendLoginReq(req, idx, rdata);
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr rcd;
	OmnString hpvpd;
	OmnNotImplementedYet;

	// The user has successfully logged in. Now, it is the time to 
	// resume the previous operation. Before resuming the previous, 
	// need to remove the pending request
	if (idx < eMaxPendingReq)
	{	
		sgPendingReqLock.lock();
		sgPendingReqs[idx] = 0;
		sgPendingSecs[idx] = 0;
		sgPendingIds[idx] = 0;
		sgPendingReqLock.unlock();
	}

	AosXmlTagPtr child = newroot->getFirstChild();
	if (!child)
	{
		// This should not happen
		rdata->setError() << "Failed to parse the request!";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	aos_assert_rr(mSeReqProc, rdata, false);
	AOSLOG_LEAVE(rdata);
	return mSeReqProc->procReq(req, newroot, child);
}


bool
AosResumeReq::resendLoginReq(
		const AosWebRequestPtr &req, 
		const u32 idx, 
		const AosRundataPtr &rdata) 
{
	// An invalid session was detected, and a relogin request was 
	// sent, the user responded with login name and password, but
	// it was not validated. Need to resend the login. This is what
	// this function does.
	//
	// a unique sequential number that is incremented each time.
	// If it fails to find an empty slot, it will kick off the 
	// one whose timestamp is the smallest (also the oldest). 
	if (idx >= eMaxPendingReq)
	{
		rdata->setError() << "Internal error(2384)";
		return false;
	}

	sgPendingReqLock.lock();
	OmnString loginvpd = sgPendingLoginvpd[idx];
	u32 reqid = sgPendingIds[idx];
	sgPendingReqLock.unlock();

	OmnString contents = "<Contents ";
	contents << AOSTAG_REQUESTINDEX << "=\"" << reqid << "\" "
		<< AOSTAG_INDEX << "=\"" << idx << "\">";

	// It requires relogin. 'loginvpd' should not be empty. 
	AosXmlTagPtr logindoc = AosDocClientObj::getDocClient()->getDocByObjid(loginvpd, rdata);
	if (logindoc)
	{
		// Need to return a response in the form:
		// 	<response
		// 		<status .../>
		// 		<Contents zky_reqidx="xxx" zky_idx="xxx">
		// 			<the login vpd .../>
		// 		</Contents>
		// 	</response>
		contents << (char *)logindoc->toString() << "</Contents>";
		rdata->setOk();
		rdata->setResults(contents); 
		return true;
	}

	rdata->setError() << "Access Denied!";
	return false;
}


AosXmlTagPtr
AosResumeReq::getPendingReq(
		const AosXmlTagPtr &root,
		u32 &idx,
		const AosRundataPtr &rdata) 
{
	OmnString errmsg;
	OmnString idxstr = root->getChildTextByAttr("name", AOSTAG_INDEX);
	if (idxstr == "")
	{
		errmsg = "Missing index!";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		return 0;
	}
	idx = atoll(idxstr.data());

	if (idx >= eMaxPendingReq)
	{
		errmsg = "Invalid index: ";
		errmsg << idx;
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		return 0;
	}

	OmnString reqidstr = root->getChildTextByAttr("name", AOSTAG_REQUESTINDEX);
	if (reqidstr == "")
	{
		errmsg = "Missing reqid!";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		return 0;
	}
	u32 reqid = atoll(reqidstr.data());

	sgPendingReqLock.lock();
	if (sgPendingIds[idx] == reqid)
	{
		AosXmlTagPtr root = sgPendingReqs[idx];
		// operation = sgPendingOprs[idx];
		rdata->setRecvedOperation(sgPendingOprs[idx]);
		sgPendingReqs[idx] = 0;
		sgPendingSecs[idx] = 0;
		sgPendingIds[idx] = 0;
		sgPendingReqLock.unlock();
		rdata->setOk();
		return root;
	}
	sgPendingReqLock.unlock();

	// This should not happen
	OmnAlarm << "Failed to find the pending request: " << reqid << enderr;
	rdata->setError()<< "Failed to find the pending request: " << reqid;
	return 0;
}


void
AosResumeReq::procInvalidSession(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &root,
		const OmnString &loginvpd, 
		const AosRundataPtr &rdata)
{
	// A invalid session is detected. When an invalid session
	// is detected, it should prompt the user for re-login (since
	// there is a session, it means the user should have been 
	// logged in before). If any error occurs, it should reset
	// the session.
	
	if (rdata->getErrcode() == eAosXmlInt_Ok)
	{
		// This is an error. 
		//errcode = eAosXmlInt_ResetSession;
		//errmsg = "Return code incorrect!";
		rdata->setError(eAosXmlInt_ResetSession) << "Return code incorrect!";
	}

	// Save the requests. Note that in the current implementation,
	// we use an array. This may not be efficient enough. Can be
	// improved in the future. Each pending request is assigned
	// a unique sequential number that is incremented each time.
	// If it fails to find an empty slot, it will kick off the 
	// one whose timestamp is the smallest (also the oldest). 
	sgPendingReqLock.lock();
	int idx = -1;
	u32 oldest = 0xffffffff;
	u32 newsec = OmnGetSecond();
	for (int i=0; i<eMaxPendingReq; i++)
	{
		if (!sgPendingReqs[i])
		{
			idx = i;
			break;
		}

		if (sgPendingSecs[i] < oldest)
		{
			idx = i;
			oldest = sgPendingSecs[i];
		}
	}

	if (idx == -1)
	{
		// This should never happen
		OmnAlarm << "Serious problem!" << enderr;
		idx = 0;
	}

	u32 reqid = sgCrtReqId++;
	sgPendingReqs[idx] = root;
	sgPendingSecs[idx] = newsec;
	sgPendingOprs[idx] = rdata->getRecvedOperation();
	sgPendingIds[idx] = reqid;
	sgPendingLoginvpd[idx] = loginvpd;
	sgPendingReqLock.unlock();

	OmnString contents = "<Contents ";
	contents << AOSTAG_REQUESTINDEX << "=\"" << reqid << "\" "
		<< AOSTAG_INDEX << "=\"" << idx << "\">";

	// If the session expires, normally it should return 
	// 'eAosXmlInt_Relogin' so that the user will be prompted 
	// for re-login. The login VPD is associated with the session.
	// If there is no session, it should be checked by (1) the system
	// (i.e., the Apache Server) at URL level; (2) by the operation.
	// It will not be checked at session level. In this case, 
	// it should return eAosXmlInt_Ok.
	if (rdata->getErrcode() == eAosXmlInt_Relogin)
	{
		// It requires relogin. 'loginvpd' should not be empty. 
		//AosXmlTagPtr logindoc = AosDocServerSelf->getDoc(mSiteid, loginvpd);
		AosXmlTagPtr logindoc = AosDocClientObj::getDocClient()->getDocByObjid(loginvpd, rdata);
		if (logindoc)
		{
			// Need to return a response in the form:
			// 	<response
			// 		<status .../>
			// 		<Contents zky_reqidx="xxx" zky_idx="xxx">
			// 			<the login vpd .../>
			// 		</Contents>
			// 	</response>
			// sendResp(req, errcode, errmsg, contents);
			contents << (char *)logindoc->getData() << "</Contents>";
			rdata->setResults(contents);
			return;
		}
	}

	// Session is not good but there is no login vpd. This is
	// normally an error. The error code should be 
	// 'eAosXmlInt_ResetSession'.
	if (rdata->getErrcode() != eAosXmlInt_ResetSession)
	{
		rdata->setError(eAosXmlInt_ResetSession) << "Errcode incorrect: ";
	}
	
	if (rdata->getErrmsg() == "")
	{
		//errmsg = "Session not valid!";
		rdata->setErrmsg("Session not valid!");
	}
	contents << "</Contents>";
	rdata->setResults(contents);
}
#endif
