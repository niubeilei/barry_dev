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
// 01/03/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEServer/SeReqProc.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "AppVar/WebAppMgr.h"
#include "SEInterfaces/DocClientObj.h"
#include "ErrorMgr/ErrmsgId.h"
#include "Microblog/Microblog.h"
#include "HealthCheck/HealthChecker.h"
#include "HealthCheck/HealthCheckObj.h"
#include "IdGen/U64IdGen.h"
#include "IdGen/IdGenMgr.h"
#include "EventMgr/EventMgr.h"
#include "Porting/TimeOfDay.h"
#include "QueryUtil/QrUtil.h"
#include "QueryClient/QueryClient.h"
#include "Rundata/Rundata.h"
#include "SearchEngine/Ptrs.h"
#include "SearchEngine/DocServerCb.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "SeAdmin/SeAdmin.h"
#include "SeReqProc/SeRequestProc.h"
#include "SeReqProc/ResumeReq.h"
#include "SEUtil/Ptrs.h"
#include "SEBase/SeUtil.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/UrlMgr.h"
#include "SEUtil/Objid.h"
#include "SEUtil/Docid.h"
#include "SEUtil/SysLogName.h"
#include "SEInterfaces/SeLogClientObj.h"
#include "SEUtil/IILName.h"
#include "SEModules/LoginMgr.h"
#include "SEUtilServer/UserDocMgr.h"
#include "SEModules/ObjMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEModules/OnlineMgr.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEModules/CodeComp.h"
#include "SEServer/CommonReq.h"
#include "Security/ScrtUtil.h"
#include "Security/Session.h"
#include "SEInterfaces/SecurityMgrObj.h"
#include "Security/SessionMgr.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "UserMgmt/UserAcct.h"
#include "Util/StrSplit.h"
#include "Util1/Time.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpClient.h"
#include "Util/StrParser.h"
#include "SmartDoc/SmartDoc.h"
#include "SmartDoc/SMDMgr.h"
#include "Actions/ActAddAttr.h"
#include "SEUtil/VersionDocMgr.h"
#include "SeSysLog/Ptrs.h"
#include "SeSysLog/SeSysLog.h"
#include "Actions/ActSeqno.h"
#include "VersionServer/VersionServer.h"
#include "XmlInterface/WebRequest.h"
#include "XmlInterface/XmlRc.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "MultiLang/LangTermIds.h"
#include <set>

#include "SmartDoc/SdocInlineSearch.h"
#include "API/AosApi.h"

extern int gAosLogLevel;
extern int gNeedBackup;
static OmnMutex sgLock;

AosImgProcPtr	AosSeReqProc::mImgProc;
bool			AosSeReqProc::mIsStopping = false;
OmnString		AosSeReqProc::mDomainAddr;
vector<OmnString>   AosSeReqProc::mMsgSvrUrl;

static AosU64IdGenPtr	sgCompIdGen;
static OmnString	sgImageTrashDir = "/home/AOS/Backup/ImageTrash";
static AosHealthCheckerPtr sgHealthChecker;
static AosSEClientPtr sgConn;
static set<int> sgUnResp;
static OmnTcpClientPtr	sgMsgConn[AosSeReqProc::eMaxConns];
static OmnString		sgMsgId[AosSeReqProc::eMaxConns];


AosSeReqProc::AosSeReqProc()
:
mNumProcessed(0),
mLock(OmnNew OmnMutex())
{
	AosSeReqProcPtr thisptr(this, false);
	mRundata = OmnApp::getRundata();
	aos_assert(mRundata);
	mRundata = mRundata->clone(AosMemoryCheckerArgsBegin);
	mRundata->setReqProc(thisptr);
}


// Chen Ding, 2013/03/07
AosSeReqProc::AosSeReqProc(const OmnConnBuffPtr &buff)
:
mNumProcessed(0),
mLock(OmnNew OmnMutex())
{
	AosSeReqProcPtr thisptr(this, false);
	mRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	mRundata->setReqProc(thisptr);

	OmnTcpClientPtr conn = buff->getConn();
	if (!conn)
	{
		OmnThrowException("missing_connection");
		return;
	}

	mWebRequest = OmnNew AosWebRequest(conn, buff);
}


AosSeReqProc::~AosSeReqProc()
{
}


AosNetReqProcPtr	
AosSeReqProc::clone()
{
	return OmnNew AosSeReqProc();
}


bool
AosSeReqProc::config(const AosXmlTagPtr &config)
{
	OmnString trashdir = config->getAttrStr(AOSCONFIG_IMAGE_TRASHDIR);
	if (trashdir != "") sgImageTrashDir = trashdir;
	sgHealthChecker = OmnNew AosHealthChecker(0, eHealthCheckFreq, 
			eHealthCheckMaxAges, eHealthCheckMaxEntries);
	if (!sgConn)
	{
		OmnString addr = config->getAttrStr(AOSCONFIG_REMOTE_ADDR, "127.0.0.1");
		int port = config->getAttrInt(AOSCONFIG_REMOTE_PORT, 16555);
		sgConn = OmnNew AosSEClient(addr, port);
	}
	return true;
}


bool
AosSeReqProc::stop()
{
	OmnScreen << "AosSeReqProc is stopping!" << endl;
	mIsStopping = true;
	if (mImgProc)
	{
		OmnScreen << "To stop image processor!" << endl;
		mImgProc->stop(); 
		mImgProc = 0;
	}
	return true;
}


bool
AosSeReqProc::procRequest(const OmnConnBuffPtr &buff)
{
	OmnTcpClientPtr conn = buff->getConn();
	aos_assert_r(conn, false);
	mWebRequest = OmnNew AosWebRequest(conn, buff);

	mRundata->reset();
	AosSeReqProcPtr thisptr(this, false);
	mRundata->setReqProc(thisptr);

	return procRequest();
}


bool
AosSeReqProc::procRequest()
{
	OmnApp::running();

	if (!mWebRequest)
	{
		AosSetErrorU(mRundata, "internal_error");
		sendResp(mRundata);
		mRundata->reset();
		return false;
	}

	u32 transid = mWebRequest->getTransId();
	if (gAosLogLevel >= 2)
	{
		OmnString request(mWebRequest->getData(), mWebRequest->getDataLen());
	 	cout << hex << "<0x" << pthread_self() << dec 
	 		<< ":" << __FILE__ << ":" << __LINE__
	 		<< "> Process request: (transid: " << mWebRequest->getTransId()
	 		<< ")\n" << request << endl;
	}
	
	sgLock.lock();
	sgUnResp.insert(mWebRequest->getTransId());
	sgLock.unlock();
	
	OmnString errmsg;
	char *data = mWebRequest->getData();

	OmnConnBuffPtr buff = mWebRequest->getConnData();
	if (!buff)
	{
		AosSetErrorU(mRundata, "internal_error");
		sendResp(mRundata);
		mRundata->reset();
		return false;
	}

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(buff, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child;

	mRundata->setRequest(mWebRequest);
	mRundata->setRequestRoot(root);
	
	if (mIsStopping)
	{
		OmnAlarm << mRundata->getErrmsg() << enderr;
		sendResp(mRundata);
		mRundata->reset();
		return false;
	}

	if (!root || !(child = root->getFirstChild()))
	{
		OmnAlarm << mRundata->getErrmsg() << ": " << data << enderr;
		sendResp(mRundata);
		mRundata->reset();
		return false;
	}

	mUrldocDocid = child->getChildU64ByAttr("name", "zkyurldocdid", 0);
	mRundata->setUrldocDocid(mUrldocDocid);

	mClientSsid = child->getChildTextByAttr("name", "sessionid");
	mTransId = child->getChildTextByAttr("name", "transid");
	mRecvCookie.clear();
	mUserid = atoll(child->getChildTextByAttr("name", AOSTAG_USERID).data());
	mAppid = child->getChildTextByAttr("name", AOSTAG_APPNAME);
	mNeedLoginObj = (child->getChildTextByAttr("name", AOSTAG_NEED_LOGINOBJ) == "true");
	mRundata->setIsEditing(
		(child->getChildTextByAttr("name", AOSTAG_EDITOR_FLAG) == "true") ||
		(child->getChildTextByAttr("name", "zky_isineditor") == "true"));
	if (mAppid == "") mAppid = AOSAPPNAME_UNKNOWN_CLIENT;
	OmnString siteid_str = child->getChildTextByAttr("name", AOSTAG_SITEID);

	if(siteid_str == "" || !siteid_str.isDigitStr())
	{
		mSiteid = 0;
	}
	else
	{
		mSiteid = atol(siteid_str.data());
	}

	if (gNeedBackup)
	{
		OmnString resp, errmsg;
		aos_assert_r(sgConn, false);	
		sgConn->procRequest(mSiteid, "", "", root->toString(), resp, errmsg);
	}

	// Retrieve cookies. Cookies are encoded by the following subtag:
	// 	<req ...>
	// 		<AOSTAG_COOKIES>
	// 			<cookie AOSTAG_NAME="cookie-name">the-cookie-value</cookie>
	// 			<cookie AOSTAG_NAME="cookie-name">the-cookie-value</cookie>
	// 			...
	// 		</AOSTAG_COOKIES>
	// 	</req>
	AosXmlTagPtr cookietag = child->getFirstChild(AOSTAG_COOKIES);
	if (cookietag)
	{
		AosXmlTagPtr cookie = cookietag->getFirstChild();
		while (cookie)
		{
			mRecvCookie[cookie->getAttrStr(AOSTAG_NAME)] = cookie->getNodeText();
			cookie = cookietag->getNextChild();
		}
	}
	
	mRundata->setSiteid(mSiteid);
	mRundata->setCookies(mRecvCookie);
	mRundata->setReceivedDoc(root, true);

	mSsid = child->getChildTextByAttr("name", AOSTAG_SESSIONID);
	if (mUrldocDocid != 0)
	{
		OmnString ssid_name = AosComposeSsidCookieName(mUrldocDocid);
		mSsid = child->getChildTextByAttr("name", ssid_name);
		if (mSsid == "")
		{
			map<OmnString, OmnString>::iterator iter = mRecvCookie.find(ssid_name);
			if(iter != mRecvCookie.end())
			{
				mSsid = iter->second;
			}
		}
	}

	if (mSsid == "undefined" || mSsid == "null") mSsid = "";

	OmnString loginvpd;
	mSession = AosSessionMgr::getSelf()->getSession1(mSsid, loginvpd, mRundata);
	if (mSession)
	{
		mUserid = mSession->getUserid();
		mRundata->setUserid(mUserid);
		mRundata->setCid(mSession->getCid());
		mRundata->setSession(mSession);
		mRundata->setCid(mSession->getCid());
	}
	else
	{
		if (mUrldocDocid)                                                                        
		{
			AosXmlTagPtr urldoc = AosDocClientObj::getDocClient()->getDocByDocid(mUrldocDocid, mRundata);
			if (urldoc && urldoc->getAttrBool(AOSTAG_USE_GUEST) == true && mSiteid)
			{
				// There is no session, which means it is not a logged in user.
				// Treat it as a guest. 
				AosXmlTagPtr guest_doc = AosLoginMgr::getGuestDoc(mSiteid, mRundata);
				if (guest_doc)
				{
					mUserid = guest_doc->getAttrU64(AOSTAG_DOCID, 0);
					mRundata->setCid(guest_doc->getAttrStr(AOSTAG_CLOUDID));
					mRundata->setUserid(mUserid);
				}
			}
		}
	}

	bool editor = (child->getChildTextByAttr("name", "editor") == "true" ||
				   child->getChildTextByAttr("name", "isineditor") == "true");
	if (!mSession && editor)
	{
		AosSetErrorU(mRundata, "re-login") << enderr;
	}
	mOperation = child->getChildTextByAttr("name", "operation");
	sgHealthChecker->addEntry(transid);

	bool rslt = procReq(mWebRequest, root, child);

	sgHealthChecker->removeEntry(transid);
	root = 0;
	mRundata->reset();
	return rslt;
}


bool
AosSeReqProc::procReq(
			const AosWebRequestPtr &req, 
			const AosXmlTagPtr &root,
			const AosXmlTagPtr &child)
{
	AOSLOG_ENTER_R(mRundata, false);

	if (mOperation == "modifyObj")
	{
		AOSLOG_LEAVE(mRundata);
		return modifyObjReq(req, child, root);
	}
	if (mOperation == "retrieve")
	{
		AOSLOG_LEAVE(mRundata);
		bool rslt = retrieveObjReq(req, root);
		return rslt;
	}
	if (mOperation == "delObject") 
	{
		AOSLOG_LEAVE(mRundata);
		return deleteObjReq(req, root, child);
	}
	if (mOperation == "delObjects")
	{
		AOSLOG_LEAVE(mRundata);
		return deleteObjReqs(req, root, child);
	}
	if (mOperation == "sendmsg")
	{
		AOSLOG_LEAVE(mRundata);
		return sendmsg(req, root);
	}
	if (mOperation == "regsvr")
	{
		AOSLOG_LEAVE(mRundata);
		return regServer(req, root);
	}
	if (mOperation == "serverreq")
	{
		AOSLOG_LEAVE(mRundata);
		return procServerReq(req, root);
	}
	if (mOperation == "serverCmd")
	{
		AOSLOG_LEAVE(mRundata);
		return procServerCmdReq(req, root);
	}

	if (mOperation == "rebuildDelObj")
	{
		AOSLOG_LEAVE(mRundata);
		return rebuildDelObjReq(req, root, child);
	}
	
	OmnString errmsg = "Unrecognized operation: ";
	errmsg << mOperation;
	OmnAlarm << errmsg << enderr;
	AosSetErrorU(mRundata, errmsg) << enderr;

	AOSLOG_LEAVE(mRundata);
	sendResp(mRundata);
	return true;
}


bool 
AosSeReqProc::procServerCmdReq(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &root)
{
	// The request is identifies by the arguments:

	// 'root' should be in the form:
	// 	<request ...>
	// 		<command>
	// 			<cmd opr="xxx">
	// 			</cmd>
	// 		</command>
	// 		<xmlobj.../>
	// 	</request>
	//
	AosXmlTagPtr rootchild = root->getFirstChild();
	if (!rootchild)
	{
		mRundata->setError() << "Request incorrect!";
		OmnAlarm << mRundata->getErrmsg() << enderr;
		sendResp(mRundata);
		return true;
	}

	AosXmlTagPtr child = rootchild->getFirstChild("command");
	if (!child || !(child = child->getFirstChild()))
	{
		mRundata->setError() << "Failed to retrieve the command!";
		OmnAlarm << mRundata->getErrmsg() << enderr;
		sendResp(mRundata);
		return true;
	}

	OmnString opr = child->getAttrStr("opr");
	if (opr == "")
	{
		AosSetErrorU(mRundata, "missing_operation");
		OmnAlarm << mRundata->getErrmsg() << enderr;
		sendResp(mRundata);
		return true;
	}

	AosSeRequestProcPtr proc = AosSeRequestProc::getProc(opr);
	if (proc)
	{
		proc->proc(mRundata);
		sendResp(mRundata);
		return true;
	}

	char c1 = opr.data()[0];
	switch (c1)
	{
	case 'd':
		 if (opr == "deleteobj")
		 {
			OmnString docid = child->getAttrStr(AOSTAG_DOCID);
			OmnString objid = child->getAttrStr(AOSTAG_OBJID);
			bool rslt = deleteObj(req, docid, objid);
			return rslt;
		 }
		 break;

	case 'g':
		 if (strcmp(opr.data(), "getFileNames") == 0)
		 {
			 bool rslt = AosCommonReq::getFileNames(this, req, child);
			 return rslt;
		 }
		 break;

	case 'm':
		 if (strcmp(opr.data(), "monitorreq") == 0)
		 {
			 OmnString contents = "<Contents>";
			 sgLock.lock();
			 set<int>::const_iterator itr = sgUnResp.begin();
			 for (; itr != sgUnResp.end(); ++itr)
			 {
				contents << "<trans>" << *itr << "</trans>";
			 }
			 sgLock.unlock();
			 contents << "</Contents>";
			 mRundata->setOk();
			 mRundata->setResults(contents);
		 }
		 break;

	case 'r':
		 if (strcmp(opr.data(), "retlist") == 0 || strcmp(opr.data(), "retmems") == 0)
		 {
			 bool rslt = getRecordsByCmd(req, child);
			 return rslt;
		 }
		 break;

	case 's':
		 if (opr == "saveobj")
		 {
			 bool rslt = modifyObjReq(req, rootchild, root);
			 return rslt;
		 }
		 break;
	}
	
	mRundata->setError() << "Unrecognized server request: " << opr;
	OmnAlarm << mRundata->getErrmsg() << enderr;
	sendResp(mRundata);
	AOSLOG_LEAVE(mRundata);
	return true;
}


bool
AosSeReqProc::prepareSaveas(const AosXmlTagPtr &xml)
{
	static OmnString lsValue;

	xml->removeAttr(AOSTAG_VERSION, 3, false);
	xml->removeAttr(AOSTAG_MTIME, 3, false);
	xml->removeAttr(AOSTAG_MT_EPOCH, 3, false);
	xml->removeAttr(AOSTAG_COUNTERCM);
	xml->removeAttr(AOSTAG_COUNTERDW);
	xml->removeAttr(AOSTAG_COUNTERLK);
	xml->removeAttr(AOSTAG_COUNTERRC);
	xml->removeAttr(AOSTAG_COUNTERWR);
	xml->removeAttr(AOSTAG_COUNTERUP);
	xml->removeAttr(AOSTAG_COUNTERRD);
	return true;
}


bool
AosSeReqProc::rebuildDelObjReq(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &root,
		const AosXmlTagPtr &child)
{
    AOSLOG_ENTER_R(mRundata, false);
	OmnString errmsg;

	OmnString objid = child->getChildTextByAttr("name", AOSTAG_OBJID);
	OmnString sdocid = child->getChildTextByAttr("name", AOSTAG_DOCID);
	if(sdocid == "" ||objid == "")
	{
		errmsg = "docid or objid is empty!";
		OmnAlarm << errmsg << enderr;
		mRundata->setError() << errmsg;
		sendResp(mRundata);
		AOSLOG_LEAVE(mRundata);
		return false;
		
	}
	u64 docid; 
	u32 len = sdocid.length();
	sdocid.parseU64(0, len, docid);

	// create delete iilname
	OmnString iilname = AosIILName::composeDeletedObjidIILName();
	bool rslt = AosIILClientObj::getIILClient()->addStrValueDocToTable(
			iilname, objid, docid, true, true, mRundata);
	if(!rslt)
	{
		errmsg = "Add to delete IIL Error!";
		OmnAlarm << errmsg << enderr;
		mRundata->setError() << errmsg;
		sendResp(mRundata);
		AOSLOG_LEAVE(mRundata);
		return false;
	}
	AOSLOG_LEAVE(mRundata);
	mRundata->setOk();
	mRundata->setResults("OK");
	sendResp(mRundata); 
	AOSLOG_LEAVE(mRundata);
	return true;
}


bool
AosSeReqProc::deleteObjReq(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &root,
		const AosXmlTagPtr &child)
{
	// It is to modify an object.
	// 'root' should be in the form:
	// 	<request >
	// 	</request>
	// where 'xmlobj' tag is the object to be saved. 
	AOSLOG_ENTER_R(mRundata, false);
	OmnString errmsg;
	OmnString docid = child->getChildTextByAttr("name", AOSTAG_DOCID);
	OmnString objid = child->getChildTextByAttr("name", "objid");
	OmnString origobj = child->getChildTextByAttr("name", AOSTAG_ORIG_CMTED);
	OmnString subopr = root->getChildTextByAttr("name", "subopr");

	u64 deldid;
	if (docid == "")
	{
		AosXmlTagPtr cmtdoc = AosDocClientObj::getDocClient()->getDocByObjid(objid, mRundata);
		if (cmtdoc)
			deldid = cmtdoc->getAttrU64(AOSTAG_DOCID, 0);
	}
	else
	{
		u32 len = docid.length();
		docid.parseU64(0, len, deldid);
	}

	OmnString sdocobjid= child->getChildTextByAttr("name", "sobjid");
	if (sdocobjid!= "")
	{
		mRundata->setReceivedDoc(child, false);
		AosSmartDocObj::procSmartdocsStatic(sdocobjid, mRundata);
		if (!mRundata->isOk())
		{
			sendResp(mRundata);
			AOSLOG_LEAVE(mRundata);
			return false;
		}
	}
		
	bool rslt = deleteObj(req, docid, objid);
	if (rslt) mRundata->setOk();
	sendResp(mRundata);
	AOSLOG_LEAVE(mRundata);
	return rslt;
}


bool
AosSeReqProc::deleteObjReqs(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &root,
		const AosXmlTagPtr &child)
{
	AOSLOG_ENTER_R(mRundata, false);
	OmnString errmsg;
	bool rslt;
	OmnString objidName = root->getChildTextByAttr("name", "objidName");
	OmnString docidName = root->getChildTextByAttr("name", "docidName");
	if(objidName == "" && docidName == "")
	{
		errmsg = "objidName and docidName are empty!";
		OmnAlarm << errmsg << enderr;
		mRundata->setError() << errmsg;
		sendResp(mRundata);
		AOSLOG_LEAVE(mRundata);
		return false;
	}

	OmnString data_record = root->getChildTextByAttr("name", "data");
	if(data_record == "")
	{
		errmsg = "data_record is empty!";
		OmnAlarm << errmsg << enderr;
		OmnScreen << data_record << endl;
		mRundata->setError() << errmsg;
		sendResp(mRundata);
		AOSLOG_LEAVE(mRundata);
		return false;
	}

	AosXmlParser parser;
	AosXmlTagPtr record_root = parser.parse(data_record, "" AosMemoryCheckerArgs);
	AosXmlTagPtr record_child;
	if (!record_root|| !(record_child = record_root->getFirstChild()))
	{
		errmsg = "Failed to parse the request!";
		OmnAlarm << errmsg << enderr;
		OmnScreen << data_record << endl;
		mRundata->setError() << errmsg;
		sendResp(mRundata);
		AOSLOG_LEAVE(mRundata);
		return false;
	}

	OmnString sdocobjid= child->getChildTextByAttr("name", "sobjid");
	if (sdocobjid!= "")
	{
		mRundata->setReceivedDoc(child, false);
		AosSmartDocObj::procSmartdocsStatic(sdocobjid, mRundata);
		if (!mRundata->isOk())
		{
			sendResp(mRundata);
			AOSLOG_LEAVE(mRundata);
			return false;
		}
	}

	AosXmlTagPtr record = record_root->getNextChild();
	while (record)
	{
		OmnString objid = record->getAttrStr(objidName);
		OmnString docid = record->getAttrStr(docidName);
		rslt = deleteObj(req,docid,objid);
		if(!rslt) break;
		record = record_root->getNextChild();
	}
	if(rslt) {
		mRundata->setOk();
		mRundata->setJqlMsg("delete successful");	
	}
	sendResp(mRundata);
	AOSLOG_LEAVE(mRundata);
	return rslt;
}


bool
AosSeReqProc::deleteObj(
		const AosWebRequestPtr &req, 
		const OmnString &docid,
		const OmnString &objid)
{
	AOSLOG_ENTER_R(mRundata, false);
	bool duplicated;
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDoc(mRundata, docid, objid, duplicated);
	if (!doc)
	{
		AosSetErrorU(mRundata, "failed_retrieve_doc") << ": " << docid 
			<< ":" << objid << enderr;
		AOSLOG_LEAVE(mRundata);
		return false;
	}

	// Check whether it is an image. If yes, need to remove the image files.
	if (doc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_IMAGE)
	{
		OmnString dirname = doc->getAttrStr(AOSTAG_IMGDIR);
		OmnString ofname = doc->getAttrStr(AOSTAG_ORIG_FNAME);
		if (dirname != "" && ofname != "")
		{
			// We are pretty much sure that this is an image doc. 
			// 'ofname' is in the form:
			// 		type + 'o' + nnn + '.' + extension
			// What we need to do is to remove all the files of the following format:
			// 		type + '*' + nnn + '.' + extension
			//
			// Chen Ding, 08/08/2010, ZKY-1019
			// In the current implementations, we will not remove the files. Instead
			// we will move the files into the trash directory.
			//
			// 		mv -f xxx <trashdir>
			//
			char *data = (char *)ofname.data();
			data[1] = '*';
			dirname << "/" << data;
			OmnString cmd = "mv -f ";
			cmd << dirname << " " << sgImageTrashDir;
			system(cmd.data());
		}
	}

	bool rslt = AosDocClientObj::getDocClient()->deleteObj(mRundata, docid, objid, "", true);
	if(rslt) {
		mRundata->setOk();
		mRundata->setJqlMsg("delete successful");	
	}
	AOSLOG_LEAVE(mRundata);
	return rslt;
}


bool
AosSeReqProc::retrieveObjReq(
		const AosWebRequestPtr &req,
		const AosXmlTagPtr &root)
{
	// This funciton retrieves an object that is identified by
	// either docid or objid. 
	// 	<request ...>
	// 		<item name="operation">retrieve</item>
	// 		<item name="siteid">xxx</item>
	// 		<item name="zky_ssid">xxx</item>
	// 		<item name="userid">xxx</item>
	// 		<item name="docname">xxx</item>
	// 		<item name="docid">xxx</item>
	// 		<item name="owner">xxx</item>
	// 		<item name="subopr">xxx</item>
	// 		<item name="appname">xxx</item>
	// 		<item name="objid">xxx</item>
	// 		...
	// 		<fields>
	// 			...
	// 		</fields>
	// 	</request>		
	// If "fnames" is missing, it retrieves the entire object. 
	// Otherwise, it retrieves an object that contains only
	// the named fields.
	AOSLOG_ENTER_R(mRundata, false);
	OmnString cid = AosCloudidSvr::getCloudid(mUserid, mRundata);

	OmnString objid = root->getChildTextByAttr("name", "objid");
	mRundata->setOrigObjid(objid);
	if (cid != "" && objid != "") 
	{
		OmnString errmsg;
		bool rslt = AosObjid::procObjid(objid, cid, errmsg);	
		if (!rslt)
		{
			AosSetError(mRundata, errmsg);
		}
	}

	AosEventMgr::getSelf()->procEvent(eAosHookReadAttempt, 
		eAosHkptReadAttempt1, objid, mRundata);

	OmnString docid = root->getChildTextByAttr("name", AOSTAG_DOCID);
	OmnString owner = root->getChildTextByAttr("name", "owner");
	OmnString isprivate = root->getChildTextByAttr("name", "private");
	OmnString ccc = cid;
	if (owner != "")
	{
		ccc = AosCloudidSvr::getCloudid(owner, mRundata);//?
		if (ccc == "")
		{
			AosEventMgr::getSelf()->procEvent(
				eAosHookReadAttemptFailed, eAosHkptReadAttemptFailed1, objid, mRundata);
			AosSetError(mRundata, AOSLT_FAILED_RETRIEVE_OWNER_CID);
			OmnAlarm << mRundata->getErrmsg() << enderr;
			sendResp(mRundata);
			AOSLOG_LEAVE(mRundata);
			return false;
		}
	}
	
	AosXmlTagPtr xml;
	AosMetaExtension::E meta_objid_type;
	if (objid != "")
	{
		if (AosObjid::isMetaObjid(objid, meta_objid_type))
		{
			// It is meta objid. Need to process it.
			if (!AosObjMgr::procMetaObjidOnRetObj(objid, meta_objid_type, mRundata))
			{
				// It is meta objid. Need to process it.
				AosEventMgr::getSelf()->procEvent(
					eAosHookReadAttemptFailed, 
					eAosHkptReadAttemptFailed2, objid, mRundata);
				sendResp(mRundata);
				AOSLOG_LEAVE(mRundata);
				return false;
			}
		}
		xml = mRundata->getCreatedDoc();
		if (!xml) xml = mRundata->getRetrievedDoc();
	}

	if (isprivate == "true")
	{
		AosObjid::attachCidAsNeeded(objid, ccc);
	}

	OmnString hpvpd;
	bool duplicated = false;
	OmnString subopr = root->getChildTextByAttr("name", "subopr");
	bool isVersionObj = false;
	if (subopr == "verobj")
	{
		isVersionObj = true;
		
		retrieveVersionObj(mRundata);
		xml = mRundata->getRetrievedDoc();
	}
	else if (!xml)
	{
		if (docid != "")
		{
			xml = AosDocClientObj::getDocClient()->getDoc(mRundata, docid, "", duplicated);
		}
		else if (objid != "")
		{
			xml = AosDocClientObj::getDocClient()->getDoc(mRundata, "", objid, duplicated);
		}
		else
		{
			AosSetError(mRundata, "missing_objid_001");
			AosEventMgr::getSelf()->procEvent(
				eAosHookReadAttemptFailed, eAosHkptReadAttemptFailed3, objid, mRundata);
			sendResp(mRundata);
			AOSLOG_LEAVE(mRundata);
			return true;
		}
	}

	if (!xml)
	{
		OmnString args = root->getChildTextByAttr("name", "args");
		if (args != "")
		{
			OmnString sdoc_objid;
			AosParseArgs(args, "smartdoc", sdoc_objid);
			if (sdoc_objid != "")
			{
				bool rslt = AosSmartDoc::runSmartdoc(sdoc_objid, mRundata);
				if (rslt)
				{
					xml = mRundata->getRetrievedDoc();
				}
			}
		}
	}

	if (!xml)
	{
		AosEventMgr::getSelf()->procEvent(
			eAosHookReadAttemptFailed, eAosHkptReadAttemptFailed4, objid, mRundata);
		AosSetError(mRundata, AOSLT_FAILED_RETRIEVE_DOC);
		sendResp(mRundata);
		AOSLOG_LEAVE(mRundata);
		return true;
	}

	// When it gets to this point, the doc has been retrieved.
	mRundata->setRetrievedDoc(xml, false);
	OmnString parents = xml->getAttrStr(AOSTAG_PARENTC);
	AosEventMgr::getSelf()->procEvent(eAosHookReadSuccess, 
			eAosHkptReadSuccess1, parents, mRundata);
	
	bool rslt = retrieveObjPublic(req, root, xml, isVersionObj, 0, 0, mRundata);

	if (!rslt)
	{
		sendResp(mRundata);
		AOSLOG_LEAVE(mRundata);
		return false;
	}

	mRundata->setOk();
	sendResp(mRundata);
	AOSLOG_LEAVE(mRundata);
	return true;
}


bool
AosSeReqProc::retrieveObjPublic(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &root, 
		const AosXmlTagPtr &docRetrieved, 
		const bool isVersionObj, 
		const AosXmlTagPtr &appobj,
		const AosXmlTagPtr &urldoc,
		const AosRundataPtr &rdata) 
{
	// IMPORTANT!!!!!!!!!!!!!!!! This function should not send response
	// Check whether it is allowed
	AOSLOG_ENTER_R(rdata, false);
	OmnString hpvpd;

	AosXmlTagPtr xml = docRetrieved;
	OmnString objid = xml->getAttrStr(AOSTAG_OBJID);
	u64 docid = xml->getAttrU64(AOSTAG_DOCID, 0);

	if (!AosDocClientObj::getDocClient()->isLog(docid) 
			&& !AosSecurityMgrObj::getSecurityMgr()->checkRead(xml, hpvpd, urldoc, rdata))
	{
		// The request did not pass the access checking. 
		// Chen Ding, 2013/07/27
		// Rlogin is turned off now. Need to re-work on it.
		// if (rdata->getErrcode() != eAosXmlInt_Relogin)
		// {
			// It did not pass the check and it is not relogin.
			// We need to reject the request.
			// Chen Ding, 2013/07/27
			// if (rdata->getErrcode() == eAosXmlInt_Ok)
			if (rdata->isOk())
			{
				AosSetErrorU(rdata, "access_denied") << enderr;
			}
			bool rslt = AosSeLogClientObj::getSelf()->addInvalidReadEntry(xml, rdata);
			if(!rslt)
			{
				rdata->setError();
				AOSLOG_LEAVE(rdata);
				return false;
			}
			AOSLOG_LEAVE(rdata);
			return false;
		//}
		AosSetError(rdata, "internal_error");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (!isVersionObj)
	{
		bool resp_sent = false;
		bool rslt = postReadProc(req, xml, resp_sent, rdata);
		if (resp_sent)
		{
			AOSLOG_LEAVE(rdata);
			return rslt;
		}

		// The request passed the access checking. Need to see whether
		// it is to retrieve a loginn vpd. If yes, instead of retrieving
		// the login vpd, it should retrieve the corresponding homepage
		// vpd, which is stored in 'hpvpd'. In the current implementation,
		// this is determined if 'hpvpd' is not empty.
		if (hpvpd != "" && hpvpd != objid && !rdata->isEditing())
		{
			// The retrieved is a login vpd, the session is valid, 
			// the homevpd is retrieved through 'hpvpd'. Instead
			// of retrieving the requested objid, it should retrieve
			// 'hpvpd'.
			objid = hpvpd;
			bool duplicated;
			xml = AosDocClientObj::getDocClient()->getDoc(rdata, "", objid, duplicated);

			if (!xml)
			{
				OmnString errmsg = "Failed to retrieve object, Siteid: "; 
				errmsg	<< rdata->getSiteid() << ", objid: " << objid;
				rdata->setError() << errmsg;
				AOSLOG_LEAVE(rdata);
				return false;
			}

			bool resp_sent = false;
			bool rslt = postReadProc(req, xml, resp_sent, rdata);
			if (resp_sent)
			{
				AOSLOG_LEAVE(rdata);
				return rslt;
			}
		}
	}

	AosXmlTagPtr fields = root->getFirstChild("fields");
	vector<AosQrUtil::FieldDef> fielddef;
	if (!AosQrUtil::parseFnames(rdata, fields, fielddef))
	{
		rdata->setError() << "Fields are incorrect: " << fields;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	OmnString contents = "<Contents ";
	if (urldoc)
	{
		contents << AOSTAG_URLDOC_DOCID << "=\"" << urldoc->getAttrStr(AOSTAG_DOCID) << "\"";
	}
	contents << ">";
	AosQrUtil::createRecord(rdata, contents, xml, fielddef);
	if (appobj)
	{
		contents << appobj->toString();
	}
	contents << "</Contents>";
	rdata->setResults(contents);
	AOSLOG_LEAVE(rdata);
	return true;
}


bool 
AosSeReqProc::modifyObjReq(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &rootchild, 
		const AosXmlTagPtr &root) 
{
	// It is to modify an object.
	// 'child' should be in the form:
	// 	<request ...>
	// 		<item name="rename">true|false|create</item>
	// 		<xmlobj ...>
	// 			<the object to be saved>
	// 		</xmlobj>
	// 		...
	// 	</request>
	// where 'xmlobj' tag is the object to be saved. 
	//
	AOSLOG_ENTER_R(mRundata, false);
	
	OmnString errmsg;

	OmnString renameflag = rootchild->getChildTextByAttr("name", "rename");
	if (renameflag == "undefined") renameflag = "create";
	AosXmlTagPtr child = rootchild->getFirstChild("xmlobj");
	if (!child)
	{
		// Missing the object to save. This is an error.
		errmsg = "Object incorrect!";
		OmnAlarm << errmsg << enderr;
	
		mRundata->setError() << errmsg;
		sendResp(mRundata);
		AOSLOG_LEAVE(mRundata);
		return false;
	}

	AosXmlTagPtr child1 = child->getFirstChild();
	if (!child1)
	{
		// Missing the object to save. This is an error.
		OmnScreen << (char *)rootchild->getData() << endl;
		errmsg = "Object incorrect!";
		OmnAlarm << errmsg << enderr;
		
		AOSLOG_LEAVE(mRundata);
		mRundata->setError() << errmsg;
		sendResp(mRundata);
		return false;
	}

	if (mSiteid == 0) 
	{
		OmnString siteid_str = child1->getAttrStr(AOSTAG_SITEID);
	
		if(siteid_str == "" || !siteid_str.isDigitStr())
		{
			mSiteid = 0;
		}
		else
		{
			mSiteid = atol(siteid_str.data());
		}
	}
	

	if (renameflag == "true")
	{
		prepareSaveas(child1);
	}
	bool rslt = AosDocClientObj::getDocClient()->modifyObj(mRundata, child1, renameflag, false);
	if(!rslt) {
		mRundata->setError();
	} else {
		mRundata->setJqlMsg("modify successfull");
	}
	
	sendResp(mRundata);
	AOSLOG_LEAVE(mRundata);
	return true;
}


bool
AosSeReqProc::getRecordsByCmd(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &childelem) 
{
	// This is the version that embed a command inside the request.
	// 'childelem' should be in the form:
	// 	<cmd opr="retlist'
	// 		psize="xxx"
	// 		tname="xxx"
	// 		ctnrs="xxx"
	// 		order="fname:true|false"
	// 		queryid="xxx"
	// 		qtype="xxx"
	// 		dir="xxx"		// either 'asc' or 'des', dft: 'asc'
	// 		startidx="xxx" (optional)>
	// 		<conds>
	// 			<.../>
	// 		</conds>
	// 		<tags>xxx</tags>
	// 		<words>xxx</words>
	// 		<fnames .../>
	// 	</cmd>
	// Refer to the function above for the definitions of these
	// attributes.
	//
	// For historical reasons, 'tname' is still used. It is equivalent
	// to 'ctnrs'. But if 'ctnrs' is used, 'tname' is ignored.
	//
	AOSLOG_ENTER_R(mRundata, false);
	AosXmlRc errcode = eAosXmlInt_General;
	OmnString errmsg;

	OmnString cid;
	if (mSession) cid = mSession->getCid();
	if (cid == "")
	{
		mRundata->setCid(AOSCLOUDID_GUEST);	
	}

	AosQueryReqObjPtr query = AosQueryClient::getSelf()->createQuery(childelem, mRundata);
	if (!query || !query->isGood())
	{
		if (errcode == eAosXmlInt_Ok)
		{
			errcode = eAosXmlInt_General;
		}

		sendResp(mRundata);
		AOSLOG_LEAVE(mRundata);
		return false;
	}

	OmnString contents;
	AOSLOG_LEAVE(mRundata);

	OmnString sdocprocobjid = childelem->getAttrStr("connecttoproc", ""); 
	if(sdocprocobjid != "")
	{
		AosSmartDocObj::procSmartdocsStatic(sdocprocobjid, mRundata);
	}

	bool rslt = query->procPublic(childelem, contents, mRundata);
	AOSSYSLOG_CREATE(rslt, AOSSYSLOGNAME_QUERY, mRundata);
	mRundata->setResults(contents);
	mRundata->setOk();
	sendResp(mRundata);
	AOSLOG_LEAVE(mRundata);
	return true;
}


void
AosSeReqProc::sendResp(const AosRundataPtr &rdata)
{
	OmnString errmsg;
	if(rdata->getErrmsg() != "")
	{
		errmsg << "<errors>";
		errmsg << rdata->getErrmsg();
		errmsg << "</errors>";
	}
	//arvin  2015.08.10
	//JIMODB-322
	OmnString contents = rdata->getResults();
	OmnString jql_msg = rdata->getJqlMsg();
	if(jql_msg == "" && contents == "")
	{
		jql_msg = "[ERR] : system error!";
	}
 	sendResp(
		rdata->getWebRequest(), 
		rdata->getErrmsg(), 
		contents,
		jql_msg);
}


void
AosSeReqProc::sendResp(
 		const AosWebRequestPtr &req, 
 		const OmnString &errmsg,
 		const OmnString &contents,
		const OmnString &jql_msg)
{
	OmnString resp = "<status ";

	double time = (OmnGetTimestamp() - mRundata->getStartTime())/(double)1000;
	resp << "proctime=\"" << time << "ms\" ";
	if (time > 2000)
	{
		// OmnAlarm << "Server proc timeout. " << time	<< enderr;
		OmnScreen << "Server proc timeout. " << time << ", msg len:" << contents.length() << endl;
	}

	if (mClientSsid != "")
	{
		resp << "session=\"" << mClientSsid<< "\" ";
	}

	bool added = false;
	if (mNeedLoginObj) 
	{
		AosXmlTagPtr loginobj;
		if (!mSession) mSession = mRundata->getSession();
		if (mSession) loginobj = mSession->getLoginObj();
		if (loginobj)
		{
			resp << "login=\"true\" ";
			resp << " error=\"";
			if (errmsg == "") resp << "false\" code=\"200\">";
			else resp << "true\" code=\"" << 400 << "\">" << errmsg;
			resp << loginobj->toString() << "</status>";
			added = true;
		}
	}

	OmnString user_opr = mRundata->getUserOperation();
	if (user_opr != "")
	{
		resp << " useropr=\"" << user_opr << "\"";
	}
	OmnString acc_opr = mRundata->getAccessOperation();
	if (acc_opr != "")
	{
		resp << " accopr=\"" << acc_opr << "\"";
	}

	if (!added)
	{
		resp << " error=\"";
		if (errmsg == "") resp << "false\" code=\"200\"/>";
		else resp << "true\" code=\"" << 400 << "\">" << errmsg << "</status>";
	}
	if (jql_msg != "") resp << "<message><![CDATA[" << jql_msg << "]]></message>";

	if (contents != "" ) resp << contents;

	if (gAosLogLevel >= 2)
	{
		OmnScreen << "Send response: (transid: " << req->getTransId()
			<< ")\n" << resp << "\n" <<  endl;
	}
	sgLock.lock();
	sgUnResp.erase(req->getTransId());
	sgLock.unlock();

	u32 browserId = 0;

	OmnString cookie;
	cookie << AOSTAG_BROWSERID << "=" << browserId 
		   << ";path=/;";
	mRundata->setSendCookie(cookie);
	
	resp <<	"<zky_cookies>";

	for(u32 i=0; i<mRundata->getSendCookies().size(); i++)
	{
		resp << "<cookie>" << mRundata->getSendCookies()[i] << "</cookie>";	
	}
	resp << "</zky_cookies>";
	req->sendResponse(resp);
	mSession = 0;
	mUrldocDocid = 0;
}


bool
AosSeReqProc::sendmsg(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &root)
{
	// 'root' should be in the form:
	// 	<request ...>
	// 		<item name="rcvr">xxx</item>
	// 		<item name="msg">xxx</item>
	// 	</request>
	//
	AOSLOG_ENTER_R(mRundata, false);
	OmnString contents; 
	OmnString errmsg;

	OmnString sender = root->getChildTextByAttr("name", "sender");
	OmnString receiver = root->getChildTextByAttr("name", "rcvr");
	OmnString msg;
	if (receiver == "")
	{
		errmsg = "Missing receiver!";
		OmnAlarm << errmsg << enderr;
		mRundata->setError() << errmsg;
		goto finished;
	}

	msg = root->getChildTextByAttr("name", "msg");
	if (msg == "")
	{
		errmsg = "Missing message to send!";
		OmnAlarm << errmsg << enderr;
		mRundata->setError() << errmsg;
		goto finished;
	}

	mLock->lock();
	for (int i=0; i<mNumReqs; i++)
	{
		if (mReqNames[i] == receiver)
		{
			// Found the receiver. 
			OmnString contents = "<Message><sender><![CDATA[";
			contents << sender << "]]></sender><contents><![CDATA["
				<< msg << "]]></contents></Message>";
			sendResp(mConns[i], "", contents, "");
			mLock->unlock();
			mRundata->setOk();
			goto finished;
		}
	}

	errmsg = "Receiver not online!";
	mLock->unlock();
	mRundata->setOk();
	contents = "";
	mRundata->setErrmsg(errmsg);
	goto finished;

finished:
	AOSLOG_LEAVE(mRundata);
	sendResp(mRundata);
	return true;
}


bool
AosSeReqProc::regServer(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &root)
{
	// 'root' should be in the form:
	// 	<request ...>
	// 		<item name="requester">xxx</item>
	// 		<item name="evt">xxx</item>
	// 	</request>
	//
	AOSLOG_ENTER_R(mRundata, false);
	OmnString errmsg;

	OmnString requester = root->getChildTextByAttr("name", "requester");
	if (requester == "")
	{
		errmsg = "Missing requester!";
		OmnAlarm << errmsg << enderr;
		mRundata->setError() << errmsg;
		sendResp(mRundata);
		AOSLOG_LEAVE(mRundata);
		return true;
	}

	OmnString evt = root->getChildTextByAttr("name", "evt");
	if (evt == "")
	{
		errmsg = "Missing message to send!";
		OmnAlarm << errmsg << enderr;
		mRundata->setError() << errmsg;
		sendResp(mRundata);
		AOSLOG_LEAVE(mRundata);
		return true;
	}

	i64 sec = OmnSystemRelativeSec1();
	bool needToNotify = true;
	bool found = false;

	// Search to see whether the user is alread online. 
	// The user is online if there is a record for it 
	// and its time is not expired yet.
	mLock->lock();
	for (int i=0; i<mNumReqs; i++)
	{
		if (mReqNames[i] == requester) 
		{
			if (mConns[i]) mConns[i]->closeConn();
			mConns[i] = req;
			found = true;
			if (mTime[i] + eTimeoutSec > sec)
			{
				// No need to notify
				needToNotify = false;
			}
			mTime[mNumReqs] = sec;
			break;
		}
	}

	if (found)
	{
		if (needToNotify)
		{
			notifyUsers(requester);
		}
		mLock->unlock();
		mRundata->setOk();
		goto finished;
	}

	// Find the one that has been expired
	for (int i=0; i<mNumReqs; i++)
	{
		if (mTime[i] + eTimeoutSec < sec)
		{
			mReqNames[i] = requester;
			if (mConns[i]) mConns[i]->closeConn();
			mConns[i] = req;
			mTime[mNumReqs] = sec;
			if (needToNotify)
			{
				notifyUsers(requester);
			}
			mLock->unlock();
			mRundata->setOk();
			
			goto finished;
		}
	}

	// Did not find it. Added it.
	mReqNames[mNumReqs] = requester;
	mConns[mNumReqs] = req;
	mTime[mNumReqs] = sec;
	mNumReqs++;
	if (needToNotify)
	{
		notifyUsers(requester);
	}

	mLock->unlock();
	mRundata->setOk();

finished:
	sendResp(mRundata);
	AOSLOG_LEAVE(mRundata);
	return true;
}


bool
AosSeReqProc::procServerReq(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &root)
{
	//<request reqid="xxxx">
	//	...
	//</request>
	static AosSeReqProcPtr lsThisPtr(this, false);
	AOSLOG_ENTER_R(mRundata, false);
	OmnString reqid = root->getChildTextByAttr("name", "reqid");
	if (reqid == "")
	{
		mRundata->setError() << "Missing reqid!";
		OmnAlarm << mRundata->getErrmsg() << enderr;
		sendResp(mRundata);
		AOSLOG_LEAVE(mRundata);
		return true;
	}

	AosSeRequestProcPtr proc = AosSeRequestProc::getProc(reqid);
	if (proc)
	{
		proc->setSeReqProc(lsThisPtr);
		proc->proc(mRundata);
		sendResp(mRundata);
		AOSLOG_LEAVE(mRundata);
		return true;
	}

	//Jozhi OmnNotImplementedYet, please add this feathure to the src/SeReqProc/
	//don't use template code
	/*
	if (reqid == "ris")
	{
		mRundata->setUserid(AosObjMgr::getSysRootDocid(mSiteid, mRundata));
		mRundata->setCid(AosObjMgr::getSysRootCid(mSiteid, mRundata));
		AosXmlTagPtr reqobj = root->getFirstChild("objdef"); 
		aos_assert_r(reqobj, false);

		OmnString sdocobjid = reqobj->getAttrStr("sdocobjid", "");
		OmnString ctnrobjid = reqobj->getAttrStr("ctnrobjid", "");
			
		AosSdocInlineSearch::reBuild(sdocobjid, ctnrobjid, mRundata);
		sendResp(mRundata);
		return true;
	}
	*/

	mRundata->getErrmsg() << reqid;
	mRundata->setError() << "Unrecognized request id!";
	OmnAlarm << mRundata->getErrmsg() << enderr;
	sendResp(mRundata);
	AOSLOG_LEAVE(mRundata);
	return true;
}


bool
AosSeReqProc::notifyUsers(const OmnString &username)
{
	// The caller should have locked the class.
	OmnString notify = "<status error=\"false\" code=\"200\"/><notify type=\"useronline\"><![CDATA[";
	notify << username << "]]></notify>";
	i64 sec = OmnSystemRelativeSec1();
	for (int i=0; i<mNumReqs; i++)
	{
		if (mReqNames[i] != username) 
		{
			if (mTime[i] + eTimeoutSec < sec)
			{
				if (mConns[i]) mConns[i]->closeConn();
				mConns[i] = 0;
				mReqNames[i] = "";
			}
			else
			{
				sendResp(mConns[i], "", notify, "");
			}
		}
	}

	return true;
}


bool
AosSeReqProc::retrieveVersionObj(const AosRundataPtr &rdata)
{
	// It is to retrieve the version object. The object id 
	// is specified in 'objid' and the version is specified
	// in 'vernum'. 
	AosXmlTagPtr root = rdata->getReceivedDoc();
	OmnString objid = root->getChildTextByAttr("name", "objid");
	if (objid == "")
	{
		AosSetError(rdata, "missing_objid");
		AosEventMgr::getSelf()->procEvent(
			eAosHookReadAttemptFailed, eAosHkptReadAttemptFailed5, 
			rdata->getOrigObjid(), rdata);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString owner = root->getChildTextByAttr("name", "owner");
	if (owner != "")
	{
		// It is the docid to which the object belongs.
		OmnString cid = AosCloudidSvr::getCloudid(owner, mRundata);
		if (cid == "")
		{
			AosSetError(rdata, "missing_objid");
			AosEventMgr::getSelf()->procEvent(
				eAosHookReadAttemptFailed, 
				eAosHkptReadAttemptFailed6, rdata->getOrigObjid(), rdata);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		objid = AosObjid::compose(objid, cid);
	}

	// The 'objid' is in the form:
	// 		<objid>:<version>[:<deleted_flag>]
	char *data = (char *)objid.data();
	char *ptr = strchr(data, ':');
	OmnString vernum;
	OmnString deleted_flag;
	if (!ptr)
	{
		// This is considered retrieving the original doc.
		vernum = "1.0";
	}
	else
	{
		ptr++;

		// Check whether it has 'deleted_flag'
		char *pp = strchr(ptr, ':');
		if (pp) 
		{
			pp++;
			vernum.assign(ptr, pp-ptr-1);
			deleted_flag = pp;
		}
		else
		{
			vernum = ptr;
		}
		int objidlen = ptr - data - 1;
		objid.setLength(objidlen);
	}

	if (deleted_flag == "ver_deleted")
	{
		// It is to retrieve a deleted object
		AosEventMgr::getSelf()->procEvent(
			eAosHookRetDelObjAttempt, 
			eAosHkptRetDelObjAttempt1, objid, rdata);
		AosXmlTagPtr retrievedDoc = AosDocClientObj::getDocClient()->getDeletedDoc(
				objid, 0, mRundata);
		if (!retrievedDoc)
		{
			AosSetError(rdata, "failed_ret_delobj") << ": " << objid;
			AosEventMgr::getSelf()->procEvent(
				eAosHookRetDelObjFailed, 
				eAosHkptRetDelObjAttemptFailed1, objid, rdata);
			return false;
		}

		// Retrieve the deleted object success
		rdata->setRetrievedDoc(retrievedDoc, false);
		rdata->setOk();
		AosEventMgr::getSelf()->procEvent(
			eAosHookRetDelObjSuccess, 
			eAosHkptRetDelObjSuccess1, objid, rdata);
		return true;
	}
	
	AosEventMgr::getSelf()->procEvent(
			eAosHookRetVerAttempt, 
			eAosHkptRetVerAttempt1, objid, rdata);
	if (vernum == "")
	{
		AosEventMgr::getSelf()->procEvent(
			eAosHookRetVerAttemptFailed, 
			eAosHkptRetVerAttemptFailed1, objid, rdata);
		AosSetError(rdata, "missing_version_num") << ": " << objid;
		return false;
	}

	OmnString errmsg;
	AosXmlTagPtr retrievedDoc = AosVersionServer::getSelf()->getVersionObj(
			objid, vernum, mRundata);
	if (!retrievedDoc)
	{
		// Check whether it is the current object.
		bool duplicated;
		retrievedDoc = AosDocClientObj::getDocClient()->getDoc(mRundata, "", objid, duplicated);

		if (!retrievedDoc)
		{
			AosSetError(mRundata, "failed_ret_version") << ": " << objid << ":" << vernum;
			return false;
		}
	}

	rdata->setRetrievedDoc(retrievedDoc, false);
	rdata->setOk();
	return true;
}


bool
AosSeReqProc::isValidVoteType(const OmnString &type)
{
	if (type == AOSTAG_VOTEUP || type == AOSTAG_VOTEDOWN) return true;
	return false;
}


bool
AosSeReqProc::postReadProc(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &doc, 
		bool &resp_sent, 
		const AosRundataPtr &rdata)
{
	resp_sent = false;
	u64 did = doc->getAttrU64(AOSTAG_DOCID, 0);
	if (did == 0) return true;
	
	AosSessionObjPtr session = rdata->getSession();
	if (doc->isSmartDoc())
	{
		OmnString sdoc_objid = doc->getAttrStr(AOSTAG_OBJID); 
		OmnString run_doc= doc->getAttrStr(AOSTAG_RUN);
		if (run_doc != "yes")
		{
			return true;
		}
		if (session)
		{
			session->setAccessedDoc(doc);
		}

		AosSmartDocObj::procSmartdocsStatic(sdoc_objid, rdata);
		return true;
	}

	OmnString sdoc_objids = doc->getAttrStr(AOSTAG_COUNTERS);
	if (sdoc_objids != "")
	{
		if (session) session->setAccessedDoc(doc);
		AosSmartDocObj::procSmartdocsStatic(sdoc_objids, rdata);
		return true;
	}

	OmnString sdoc_objid = doc->getAttrStr(AOSZTG_COUNTDOWN);
	if (sdoc_objid != "")
	{
		if (session) session->setAccessedDoc(doc);
   		AosSmartDocObj::procSmartdocsStatic(sdoc_objid, rdata);
		return true;
	}

	return true;
}


AosRundataPtr
AosSeReqProc::getRundata(const AosWebRequestPtr &req, const AosXmlTagPtr &doc)
{
	return mRundata;
}


void
AosSeReqProc::sendResp()
{
	sendResp(mRundata);
}
