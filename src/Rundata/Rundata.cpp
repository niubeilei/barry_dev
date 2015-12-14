////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 01/19/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Rundata/Rundata.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "ErrorMgr/ErrmsgId.h"
#include "LogUtil/XmlLogEntry.h"
#include "Message/BuffMsg.h"
#include "Porting/GetTime.h"
#include "Proggie/ReqDistr/NetReqProc.h"
#include "SEServer/SeReqProc.h"
#include "SEInterfaces/SessionObj.h"
#include "SEInterfaces/LangDictObj.h"
#include "SEInterfaces/SeLogClientObj.h"
#include "SEUtil/SysLogName.h"
#include "SEBase/SeUtil.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Util/Buff.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/AccessRcd.h"
#include "XmlInterface/WebRequest.h"
#include "SEInterfaces/UserAcctObj.h"

static AosLangDictObjPtr sgDictionary;
// static u32 sgBrowserId = OmnGetSecond();
static OmnMutex sgLock;
static int sgNumInstances = 0;

OmnString 				AosRundata::smSysSdocEventObjid;
OmnString     			AosRundata::smModuleId;
OmnMutex      			AosRundata::smLock;
list<AosRundataPtr>  	AosRundata::smRundatas;
AosRundataPtr			AosRundata::smRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
AosRdataErrEntry 		AosRundata::smRdataError;

AosRundata::AosRundata(const AosSeReqProcPtr &proc AosMemoryCheckDecl)
:
mReqProc(proc),
mSiteid(0),
mLocale(AosLocale::eUSA),
mStartTime(OmnGetTimestamp()),
mUserid(0),
mIsInternalCall(false),
mSnapshotId(0),
mCmpLen(0)
{
	sgLock.lock();
	sgNumInstances++;
	sgLock.unlock();

	//for debug only
	//OmnScreen << "Total rundata is: " << sgNumInstances << endl;

	mJQLUpdateCount = 0;
	AosMemoryCheckerObjCreated(AosClassName::eAosRundata);
}


AosRundata::AosRundata(AosMemoryCheckDeclBegin)
:
mSiteid(0),
mLocale(AosLocale::eUSA),
mStartTime(OmnGetTimestamp()),
mUserid(0),
mIsInternalCall(false),
mSnapshotId(0),
mCmpLen(0)
{
	sgLock.lock();
	sgNumInstances++;
	sgLock.unlock();
	mJQLUpdateCount = 0;

	//for debug only
	//OmnScreen << "Total rundata is: " << sgNumInstances << endl;

	AosMemoryCheckerObjCreated(AosClassName::eAosRundata);
}


AosRundata::AosRundata(
		const u32 siteid, 
		const OmnString &app_name, 
		const OmnString &ssid, 
		const OmnString &cloudid, 
		const u64 userid AosMemoryCheckDecl) 
:
mSiteid(siteid),
mAppname(app_name),
mSsid(ssid),
mCid(cloudid), 
mUserid(userid),
mIsInternalCall(false),
mSnapshotId(0),
mCmpLen(0)
{
	sgLock.lock();
	sgNumInstances++;
	sgLock.unlock();

	//for debug only
	//OmnScreen << "Total rundata is: " << sgNumInstances << endl;
	
	AosMemoryCheckerObjCreated(AosClassName::eAosRundata);
}


AosRundata::~AosRundata()
{
	sgLock.lock();
	sgNumInstances--;
	sgLock.unlock();

	//for debug only
	//OmnScreen << "Total rundata is: " << sgNumInstances << endl;

	mSiteid = 12345;
	AosMemoryCheckerObjDeleted(AosClassName::eAosRundata);
}


bool
AosRundata::reset()
{
	mReqProc = 0;
	mReq = NULL;
	mErrmsg = "";
	mSiteid = 0;
	mSession = 0;
	mSsid = "";
	mIsEditing = false;
	mContinueFlag = false;
	mReceivedDoc = 0;
	mAppname = "";
	mReqRoot = 0;
	mCid = "";
	mLocale = AosLocale::eUSA;
	mOldDoc = 0;
	mNewDoc = 0;
	mResults = "";
	mWorkingDoc = 0;
	mReceivedCookies.clear();
	mSendCookies.clear();
	mMonitorLog = "";
	mStartTime = OmnGetTimestamp();
	mUserid = 0;
	mResults = "";
	mOrigObjid = "";
	mArgs.clear();
	mTimerCaller = 0;

	mSdocReturnStaus = "";
	mEventStr = "";
	mVarDocs.clear();
	mUrldocDocid = 0;
	mUserOperation = "";
	mAccessOperation = "";
	mSecReq.reset();	
	mIsEditing = false;

	mCharPtrs.clear();			
	mCharPtrLens.clear();	
	mU64Values.clear();	
	mInt64Values.clear();
	mOmnStrs.clear();	

	mValueU64 = 0;
	mValueInt64 = 0;
	mValueChar = 0;
	mValueStr = "";
	mErrEntry.reset();
	mOverrideFlag1 = false;
	mJimo = 0;	
	mValueRslt.reset();
	mTimeTracker.reset();
	mIsInternalCall = false;

	for (int i=0; i<AosRundataParm::eMaxType; i++) mParms[i] = 0; 

	return true;
}


bool
AosRundata::copyFrom(AosRundata *rdata)
{
	mReqProc			= rdata->mReqProc;
	mReq				= rdata->mReq;
	mErrmsg				= rdata->mErrmsg;
	mSiteid				= rdata->mSiteid;
	mIsEditing			= rdata->mIsEditing;
	mContinueFlag		= rdata->mContinueFlag;
	mAppname			= rdata->mAppname;
	mSession			= rdata->mSession;
	mSsid				= rdata->mSsid;
	mCid				= rdata->mCid;
	mLocale				= rdata->mLocale;
	mReqRoot			= rdata->mReqRoot;
	mReceivedDoc		= rdata->mReceivedDoc;
	mWorkingDoc			= rdata->mWorkingDoc;
	mOldDoc				= rdata->mOldDoc;
	mNewDoc				= rdata->mNewDoc;
	mVarDocs			= rdata->mVarDocs;
	mResults			= rdata->mResults;
	mOrigObjid			= rdata->mOrigObjid;
	mSendCookies		= rdata->mSendCookies;
	mReceivedCookies	= rdata->mReceivedCookies;
	mMonitorLog			= rdata->mMonitorLog;
	mStartTime			= rdata->mStartTime;
	mUserid				= rdata->mUserid;
	mAccessOperation	= rdata->mAccessOperation;
	mUserOperation		= rdata->mUserOperation;
	mArgs				= rdata->mArgs;
	mSdocReturnStaus	= rdata->mSdocReturnStaus; 		
	mEventStr			= rdata->mEventStr;			
	mUrldocDocid		= rdata->mUrldocDocid;	
	mTimerCaller		= rdata->mTimerCaller;
	mSecReq				= rdata->mSecReq;
	mCharPtrs			= rdata->mCharPtrs;	
	mCharPtrLens		= rdata->mCharPtrLens;
	mU64Values			= rdata->mU64Values;		
	mInt64Values		= rdata->mInt64Values;
	mOmnStrs			= rdata->mOmnStrs;	
	mValueU64			= rdata->mValueU64;
	mValueInt64			= rdata->mValueInt64;
	mValueChar			= rdata->mValueChar;
	mValueStr			= rdata->mValueStr;
	mErrEntry			= rdata->mErrEntry;
	mOverrideFlag1		= rdata->mOverrideFlag1;
	mJimo				= rdata->mJimo;	
	mValueRslt			= rdata->mValueRslt;	
	mTimeTracker 		= rdata->mTimeTracker;
	mIsInternalCall		= rdata->mIsInternalCall;

	for (int i=0; i<AosRundataParm::eMaxType; i++) mParms[i] = rdata->mParms[i];

	return true;
}


bool 
AosRundata::copyCtlrInfo(AosRundata *rdata)
{
	mSiteid			= rdata->mSiteid;
	mAppname		= rdata->mAppname;
	mSession		= rdata->mSession;
	mCid			= rdata->mCid;
	mUserid			= rdata->mUserid;
	mUrldocDocid	= rdata->mUrldocDocid;
	mTimeTracker	= rdata->mTimeTracker;
	return true;
}


void
AosRundata::setRequest(const AosWebRequestPtr &req) 
{
	mReq = req;
}


int
AosRundata::getTransid() const
{
	if (mReq) return mReq->getTransId();
	return -1;
}


u64				
AosRundata::getUserid() const
{
	if (mUserid) return mUserid;
	if (!mSession) return 0;
	return mSession->getUserid();
}


OmnString		
AosRundata::toString(const int level)
{
	return "";
}


AosWebRequestPtr
AosRundata::getWebRequest() const
{
	return mReq;
}


AosSessionObjPtr
AosRundata::getSession() const
{
 	return mSession;
}


/*
void
AosRundata::returnRundata(const AosRundataPtr &rdata)
{
	smRundatas.push_back(rdata);
}
*/


OmnString
AosRundata::getSsid() const
{
	if (mSession) return mSession->getSsid();
	return mSsid;
}


void
AosRundata::setSdoc(const AosXmlTagPtr &sdoc)
{
	aos_assert(sdoc->isRootTag());
	mVarDocs[AOSARG_SMARTDOC] = sdoc;
}


AosRundataPtr
AosRundata::clone(AosMemoryCheckDeclBegin)
{
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerFileLineBegin);
	rdata->copyFrom(this);
	return rdata;
}


// Chen Ding, 2013/09/21
OmnString
AosRundata::getUserArg(const OmnString &name)
{
	// User args are the args received from clients.
	aos_assert_r(mReqRoot, "");
	OmnString args = mReqRoot->getChildTextByAttr("name", "args");
	if (args =="") return "";
	return AosParseArgs(args, name);
}


OmnString
AosRundata::getCookie(const OmnString &cookie_name)
{
	map<OmnString, OmnString>::iterator itr = mReceivedCookies.find(cookie_name);
	if (itr == mReceivedCookies.end()) return "";
	return itr->second;
}


AosSeReqProcPtr
AosRundata::getReqProc() const
{
	return mReqProc;
}


AosRdataErrEntry &
AosRundata::pushError(	
		const OmnString &errmsg_id, 
		const OmnString &name, 
		const OmnString &errmsg, 
		const bool is_user_error,
		const char *fname, 
		const int line)
{
	// smLock.lock();
 	// mErrorSeqno = smErrorSeqno++;
	// smLock.unlock();
	OmnString msg;
	if (!sgDictionary) sgDictionary = AosLangDictObj::getDictionary();
	if (sgDictionary)
	{
		msg = sgDictionary->getTerm(errmsg_id, this);
	}
	mErrEntry.setError(fname, line, errmsg_id, msg, name, errmsg, is_user_error, this);
	
	return mErrEntry;
}



AosRdataErrEntry &		
AosRundata::setError(
		const OmnString &errmsg_id, 
		const char *fname, 
		const int line,
		const bool is_user_error)
{
	// smLock.lock();
 	// mErrorSeqno = smErrorSeqno++;
	// smLock.unlock();
	OmnString msg;
	if (!sgDictionary) sgDictionary = AosLangDictObj::getDictionary();
	if (sgDictionary)
	{
		msg = sgDictionary->getTerm(errmsg_id, this);
	}
	mErrEntry.setError(fname, line, errmsg_id, msg, "", "", is_user_error, this);
	
	return mErrEntry;
}



bool
AosRundata::setVerificationCode(const OmnString &code)
{
	aos_assert_r(mSession, false);
	mSession->setVerificationCode(code);
	return true;
}


OmnString
AosRundata::getVerificationCode() const
{
	if (!mSession) return "";
	return mSession->getVerificationCode();
}


OmnString		
AosRundata::getRequesterDomains() const
{
	return mSecReq.getRequesterDomains();
}


OmnString		
AosRundata::getRequesterUserRoles() const
{
	return mSecReq.getRequesterUserRoles();
}


OmnString		
AosRundata::getRequesterUserGroups() const
{
	return mSecReq.getRequesterUserGroups();
}


OmnString 
AosRundata::getAccessedCid() const
{
	OmnNotImplementedYet;
	return "";
}


u64 
AosRundata::getAccessedUserid() const
{
	OmnNotImplementedYet;
	return 0;
}


OmnString 
AosRundata::getAccessedUsername() const
{
	OmnNotImplementedYet;
	return "";
}

AosXmlTagPtr
AosRundata::getRequesterAcctDoc() const
{
	//AosUserAcctObjPtr acct = getRequesterAcct();
	AosUserAcctObjPtr acct = getRequesterAcct();
	if (!acct) return 0;
	return acct->getDoc();
}


OmnString
AosRundata::getRequesterCid() const
{
	AosXmlTagPtr doc = getRequesterAcctDoc();
	if (!doc) return "";
	aos_assert_r(doc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_USERACCT, "");
	return doc->getAttrStr(AOSTAG_CLOUDID);
}


u64
AosRundata::getRequesterUserid() const
{
	AosXmlTagPtr doc = getRequesterAcctDoc();
	if (!doc) return 0; 
	aos_assert_r(doc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_USERACCT, 0);
	return doc->getAttrU64(AOSTAG_DOCID, 0);
}


OmnString
AosRundata::getRequesterUsername() const
{
	AosXmlTagPtr doc = getRequesterAcctDoc();
	if (!doc) return "";
	aos_assert_r(doc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_USERACCT, "");
	return doc->getAttrStr(AOSTAG_USERNAME);
}


AosXmlTagPtr 
AosRundata::getDocByVar(const OmnString &varname)
{
	DocMapItr_t itr = mVarDocs.find(varname);
	if (itr == mVarDocs.end()) return 0;
	return itr->second;
}


bool 
AosRundata::setDocByVar(
		const OmnString &varname, 
		const AosXmlTagPtr &doc, 
		const bool checkDoc)
{
	if (checkDoc && doc) aos_assert_r(doc->isRootTag(), false);
	mVarDocs[varname] = doc;
	return true;
}


void		
AosRundata::setNeedLoginObj(const bool needloginobj)
{
	aos_assert(mReqProc);
	mReqProc->setNeedLoginObj(needloginobj);
}


void 
AosRundata::setReqProc(const AosSeReqProcPtr &req) 
{
	mReqProc = req;
}


AosXmlTagPtr 
AosRundata::setDocByVarWithRet(
		const OmnString &name, 
		const AosXmlTagPtr &doc, 
		const bool checkDoc)
{
	if (checkDoc && doc)
	{
		aos_assert_r(doc->isRootTag(), 0);
	}
	DocMapItr_t itr = mVarDocs.find(AOSARG_SOURCE_DOC);
	AosXmlTagPtr dd;
	if (itr != mVarDocs.end())
	{
		dd = itr->second;
	}
	mVarDocs[name] = doc;
	return dd;
}


AosXmlTagPtr	
AosRundata::setReceivedDoc(const AosXmlTagPtr &doc, const bool checkdoc)
{
	if (checkdoc && doc) aos_assert_r(doc->isRootTag(), 0);
	AosXmlTagPtr dd = mReceivedDoc;
	mReceivedDoc = doc;
	return dd;
}


AosXmlTagPtr 
AosRundata::setWorkingDoc(const AosXmlTagPtr &doc, const bool checkdoc)
{
	aos_assert_r(doc, 0);
	if (checkdoc) aos_assert_r(doc->isRootTag(), 0);
	AosXmlTagPtr dd = mWorkingDoc;
	mWorkingDoc = doc;
	return dd;
}


OmnString
AosRundata::getParmValue(const OmnString &parmname)
{
	aos_assert_r(parmname != "", "");
	if (!mReqRoot) return "";
	AosXmlTagPtr child = mReqRoot->getFirstChild();
	if (!child) return "";
	return child->getChildTextByAttr("name", parmname);
}


// Ketty 2012/05/04
void
AosRundata::addToBuff(const AosBuffPtr &buff)	
{
	buff->setOmnStr(mCid);
	buff->setU64(mUserid);
	buff->setU32(mSiteid);
}


// Ketty 2012/05/04	
AosRundataPtr
AosRundata::getRdataByBuff(const AosBuffPtr &buff AosMemoryCheckDecl)
{
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerFileLineBegin);

	rdata->mCid = buff->getOmnStr("");
	rdata->mUserid = buff->getU64(0);
	rdata->mSiteid = buff->getU32(0);
	return rdata;
}


// Chen Ding, 06/19/2012
void
AosRundata::addToXml(const AosXmlTagPtr &xml)	
{
	xml->setAttr(AOSTAG_CLOUDID, mCid);
	xml->setAttr(AOSTAG_USERID, mUserid);
	xml->setAttr(AOSTAG_SITEID, mSiteid);
}


// Chen Ding, 06/19/2012
AosRundataPtr
AosRundata::getRdataFromXml(const AosXmlTagPtr& xml AosMemoryCheckDecl)
{
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerFileLineBegin);

	rdata->mCid = xml->getAttrStr(AOSTAG_CLOUDID);
	rdata->mUserid = xml->getAttrU64(AOSTAG_USERID, 0);
	rdata->mSiteid = xml->getAttrU64(AOSTAG_SITEID, 0);
	return rdata;
}


bool
AosRundata::setValue(const AosValueRslt &value, const AosXmlTagPtr &sdoc)
{
	aos_assert_r(sdoc, false);
	OmnString varname = sdoc->getAttrStr(AOSTAG_RUNDATA_VAR_NAME);
	if (varname != "")
	{
		OmnAlarm << "Var name is empty: " << sdoc->toString() << enderr;
		return false;
	}

	// It is to set the value 'value' to 'varname'
	OmnString vv = value.getStr();
	mArgs[varname] = vv;
	return true;
}


bool
AosRundata::setDoc(const AosXmlTagPtr &doc, const AosXmlTagPtr &sdoc)
{
	aos_assert_r(sdoc, false);
	OmnString varname = sdoc->getAttrStr(AOSTAG_RUNDATA_VAR_NAME);
	if (varname != "")
	{
		OmnAlarm << "Var name is empty: " << sdoc->toString() << enderr;
		return false;
	}

	mVarDocs[varname] = doc;
	return true;
}


bool
AosRundata::setErrorEntry(
		const bool is_user_error,
		const OmnString &errmsg, 
		const char *fname, 
		const int line)
{
	mErrmsg << errmsg;
//	if (is_user_error) return true;
	OmnAlarmEntryBegin(fname, line) << OmnErrId::eAlarm << errmsg << enderr;
	return true;
}


// Chen Ding, 2013/02/14
bool
AosRundata::setForSending(const AosXmlTagPtr &xml)
{
	// This function sets some of the member data to 'xml. 
	// This is called when one wants to send a message to 
	// another side.
	aos_assert_r(xml, false);

	xml->setAttr(AOSTAG_SITEID, mSiteid);
	xml->setAttr(AOSTAG_APPLICATION_NAME, mAppname);
	xml->setAttr(AOSTAG_SESSION_ID, mSsid);
	xml->setAttr(AOSTAG_CLOUDID, mCid);
	xml->setAttr(AOSTAG_USER_ID, mUserid);
	return true;
}


// Chen Ding, 2013/02/14
AosRundataPtr
AosRundata::serializeFromMsg(const AosXmlTagPtr &xml AosMemoryCheckDecl)
{
	// This function is called when a message is received and
	// we want to recover the rdata.
	aos_assert_r(xml, 0);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	u32 siteid = xml->getAttrU32(AOSTAG_SITEID, 0);
	aos_assert_r(siteid > 0, 0);
	rdata->setSiteid(siteid);
	rdata->setAppname(xml->getAttrStr(AOSTAG_APPLICATION_NAME, ""));
	rdata->setSsid(xml->getAttrStr(AOSTAG_SESSION_ID, ""));
	rdata->setCid(xml->getAttrStr(AOSTAG_CLOUDID, ""));
	rdata->setUserid(xml->getAttrU64(AOSTAG_USER_ID, 0));
	return rdata;
}


// Chen Ding, 2013/03/05
bool
AosRundata::setForSending(AosBuffMsg &msg)
{
	// This function sets some of the member data to 'xml. 
	// This is called when one wants to send a message to 
	// another side.
	msg.setFieldU32(AosMsgFieldId::eSiteId, mSiteid);
	msg.setFieldStr(AosMsgFieldId::eAppname, mAppname);
	msg.setFieldStr(AosMsgFieldId::eSSID, mSsid);
	msg.setFieldStr(AosMsgFieldId::eCloudId, mCid);
	msg.setFieldU64(AosMsgFieldId::eUserId, mUserid);
	return true;
}


// Chen Ding, 2013/03/05
AosRundataPtr
AosRundata::serializeFromMsg(AosBuffMsg &msg AosMemoryCheckDecl)
{
	// This function is called when a message is received and
	// we want to recover the rdata.
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	u32 siteid = msg.getFieldU32(AosMsgFieldId::eSiteId, 0);
	aos_assert_r(siteid > 0, 0);
	rdata->setSiteid(siteid);
	rdata->setAppname(msg.getFieldStr(AosMsgFieldId::eAppname, ""));
	rdata->setSsid(msg.getFieldStr(AosMsgFieldId::eSSID, ""));
	rdata->setCid(msg.getFieldStr(AosMsgFieldId::eCloudId, ""));
	rdata->setUserid(msg.getFieldU64(AosMsgFieldId::eUserId, 0));
	return rdata;
}


void 
AosRundata::setJimo(const AosJimoPtr &jimo)
{
	mJimo = jimo;
}


AosUserAcctObjPtr	
AosRundata::getAccessedAcct() const 
{
	return mSecReq.getAccessedAcct();
}


AosUserAcctObjPtr	
AosRundata::getRequesterAcct() const 
{
	return mSecReq.getRequesterAcct();
}


const OmnString 
AosRundata::getArg1(
		const OmnString &name, 
		const bool remove_flag, 
		const OmnString &dft)
{
	aos_assert_r(name != "", dft);
	map<OmnString, OmnString>::iterator itr;
	itr = mArgs.find(name);
	if (itr == mArgs.end()) return dft;
	OmnString ss = itr->second;
	if (remove_flag)
	{
		mArgs.erase(itr);
	}
	return ss;
}
	

OmnString &
AosRundata::getMonitorLog()
{
	if (mMonitorLog.length() > eMaxMonitorLogLength)
	{
		mMonitorLog = "";
	}

	return mMonitorLog;
}


int
AosRundata::getNumInstances()
{
	sgLock.lock();
	int nn = sgNumInstances;
	sgLock.unlock();
	return nn;
}


OmnString
AosRundata::getErrmsg() 
{
	OmnString errmsg = mErrEntry.createErrmsg();
	mErrmsg << errmsg;
	return mErrmsg;
}


void			
AosRundata::appendErrors(const OmnString &m)
{
	mErrmsg << m;
}


// Chen Ding, 2014/11/09
AosRdataErrEntry &
AosRundata::pushError(	
		const OmnString &errmsg_id, 
		const bool is_user_error,
		const char *fname, 
		const int line)
{
	mErrEntry.setError(fname, line, errmsg_id, is_user_error, this);
	return mErrEntry;
}


// Chen Ding, 2014/11/09
AosRdataErrEntry &
AosRundata::pushError(	
		const AosErrmsgId::E errmsg_id, 
		const bool is_user_error,
		const char *fname, 
		const int line)
{
	mErrEntry.setError(fname, line, errmsg_id, is_user_error, this);
	return mErrEntry;
}


bool
AosRundata::serializeToBuffForJimoCall(AosBuff *buff)
{
    buff->appendU64(mSiteid);
	buff->appendU64(mUserid);
	buff->appendU64(mJPID);
	buff->appendU64(mJSID);
	buff->appendU64(mSnapshotId);
	buff->appendOmnStr(mAppname);
	buff->appendOmnStr(mSsid);
	buff->appendOmnStr(mCid);
	return true;
}


AosRundataPtr
AosRundata::serializeFromBuffForJimoCall(AosBuff *buff)
{
	u32 siteid = buff->getU64(0);
	u64 userid = buff->getU64(0);
	u64 jpid = buff->getU64(0);
	u64 jsid = buff->getU64(0);
	u64 snap_id = buff->getU64(0);
	OmnString app_name = buff->getOmnStr("");
    OmnString ssid = buff->getOmnStr("");
	OmnString cid = buff->getOmnStr("");
		 
	AosRundataPtr rdata = OmnNew AosRundata(
		siteid, app_name, ssid, cid, userid AosMemoryCheckerArgs);
	rdata->setJPID(jpid);
	rdata->setJSID(jsid);
	rdata->setSnapshotId(snap_id);
	return rdata;
}            

