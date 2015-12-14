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
// 03/18/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SEModules/LogMgr.h"

#include "Alarm/Alarm.h"
#include "Porting/TimeOfDay.h"
#include "DocClient/DocClient.h"
#include "SEUtil/DocTags.h"
#include "SEModules/ObjMgr.h"
#include "SEModules/LoginMgr.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SEUtilServer/UserDocMgr.h"
#include "SearchEngine/Ptrs.h"
#include "SearchEngine/DocServerCb.h"
#include "SEUtil/VersionDocMgr.h"
#include "SEServer/SeReqProc.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Util/StrSplit.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"


#define AOSLOGID_WHOVISITED		"_ztld_01"


OmnSingletonImpl(AosLogMgrSingleton,
                 AosLogMgr,
                 AosLogMgrSelf,
                "AosLogMgr");

const int sgIncsize = 0;
const int sgMaxFilesize = 100000000;
const int sgMaxHeaderPerFile = 5000000;
const int sgMaxDocfiles = 1000;
const int sgMaxHeaderfiles = 0;
static AosVersionDocMgr sgVersionDocMgr;


AosLogMgr::AosLogMgr()
:
mIsStopping(false)
{
}


AosLogMgr::~AosLogMgr()
{
}


bool      	
AosLogMgr::start()
{
	return true;
}

/*
bool      	
AosLogMgr::start(
		const OmnString &dirname, 
		const OmnString &fname)
{
	return sgVersionDocMgr.init( dirname, fname);
}

*/

bool        
AosLogMgr::stop()
{
	mIsStopping = true;
	sgVersionDocMgr.stop();
	return true;
}


bool
AosLogMgr::config(const AosXmlTagPtr &def)
{
	aos_assert_r(def, false);
	OmnString dirname = def->getAttrStr(AOSCONFIG_DIRNAME, "./");
	OmnString logfname = def->getAttrStr("logfilename", "log_");
	return sgVersionDocMgr.init(dirname, logfname);
}


/*
bool
AosLogMgr::getLog(
		const OmnString &siteid,
		const OmnString &logtype,
		const OmnString &logid, 
		const int pagesize,
		const bool reverse,
		OmnString &contents, 
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	// Given a logid, the log is stored in the IIL named:
	// 	AOSZTG_LOG + siteid + ":" + logtype + "_" + logid
	//
	// This function retrieves 'pagesize' number of entires from the 
	// log 
	OmnString iilname;
	createLogIILName(iilname, siteid, logtype, logid);
	AosIILPtr iil = AosIILClientObj::getIILClient()->getValues(startidx, Mgr::getSelf()->getIILPublic(iilname);
	if (!iil)
	{
		errcode = eAosXmlInt_General;
		errmsg = "Log not found: ";
		errmsg << siteid << ":" << logid;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (iil->getIILType() != eAosIILType_Hit)
	{
		errcode = eAosXmlInt_General;
		errmsg = "Internal error (001)";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	AosIILHitPtr hitiil = (AosIILHit *)iil.getPtr();
	int psize = pagesize; 
	if (psize <= 0) psize = eDftPagesize;
	int idx = (reverse)?hitiil->getNumDocs()-1:0;
	u64 docid;
	for (int i=0; i<psize; i++)
	{
		docid = hitiil->getNextEntryLocked(idx);
		if (docid == AOS_INVDID) break;

		u32 offset = (docid >> 32);
		u32 seqno = (u32)docid;
		AosXmlTagPtr doc = sgVersionDocMgr.getXmlDoc2(seqno, offset);
		if (doc)
		{
			AosQueryReq::createRecord(contents, 0, doc, errmsg, 0, 0, false);
		}

		idx += (reverse)?-1:1;
	}
	AosIILMgrObj::getIILMgr()->returnIILPublic(iil);
	return true;
}
*/


bool
AosLogMgr::rebuildLogEntry(
		const AosXmlTagPtr &logdoc, 
		const u32 seqno, 
		const u32 offset,
		const AosRundataPtr &rdata) 
{
	/*
	 * Chen Ding, 08/12/2011
	// This function is used to rebuild the log entry. The log entry
	// is defined in 'logdoc'. It was read from a log file. 
	aos_assert_r(logdoc, false);
	OmnString siteid = logdoc->getAttrStr(AOSTAG_SITEID);
	OmnString logtype = logdoc->getAttrStr(AOSTAG_LOGTYPE);
	OmnString logid = logdoc->getAttrStr(AOSTAG_LOGID);
	aos_assert_r(siteid != "", false);
	aos_assert_r(logtype != "", false);
	aos_assert_r(logid != "", false);

	// It adds a log entry to the log identified by 'logid'. 
	u64 docid = 0;
	docid = (((u64)offset) << 32) + seqno;

	OmnString iilname;
	createLogIILName(iilname, siteid, logtype, logid);

	u32 logtime = logdoc->getAttrU32(AOSTAG_LOGTIME, OmnGetSecond());
	AosIILClientObj::getIILClient()->addU64ValueDoc(iilname, logtime, docid, false, true, rdata);
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


bool
AosLogMgr::rebuildLogEntry(
		const AosXmlTagPtr &logdoc,
		const AosRundataPtr &rdata)
{
	/*
	 * Chen Ding, 08/12/2011
	// This function is used to rebuild the log entry. The log entry
	// is defined in 'logdoc'. It was read from a log file. 
	aos_assert_r(logdoc, false);
	OmnString siteid = logdoc->getAttrStr(AOSTAG_SITEID);
	OmnString logtype = logdoc->getAttrStr(AOSTAG_LOGTYPE);
	OmnString logid = logdoc->getAttrStr(AOSTAG_LOGID);
	aos_assert_r(siteid != "", false);
	aos_assert_r(logtype != "", false);
	aos_assert_r(logid != "", false);

	u32 seqno;
	u64 offset;
	OmnString docstr = logdoc->toString();
	bool rslt = sgVersionDocMgr.saveDoc(seqno, offset, docstr.length(), docstr.data());
	aos_assert_r(rslt, false);

	// It adds a log entry to the log identified by 'logid'. 
	OmnString iilname;
	u64 docid = 0;
	createLogIILName(iilname, siteid, logtype, logid);
	docid = (((u64)offset) << 32) + seqno;

	u32 logtime = logdoc->getAttrU32(AOSTAG_LOGTIME, OmnGetSecond());

	AosIILClientObj::getIILClient()->addU64ValueDoc(iilname, logtime, docid, false, true, rdata);
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


bool
AosLogMgr::addLogEntry(const AosXmlTagPtr &logdoc, const AosRundataPtr &rdata)
{
	// Logs are sequences of log entries, which are all xml docs. In most cases, 
	// logs are displayed as it is. To retrieve log entries, all we need to 
	// do is to specify the log id. 
	//
	// Logs are identified by logids. Given a logid, the log is an IIL whose
	// name is determined as:
	// 	AOSZTG_LOG + siteid + ":" + logid
	aos_assert_r(!mIsStopping, false);

	OmnString siteid = logdoc->getAttrStr(AOSTAG_SITEID);
	OmnString logtype = logdoc->getAttrStr(AOSTAG_LOGTYPE);
	OmnString logid = logdoc->getAttrStr(AOSTAG_LOGID);
	u32 logtime = OmnGetSecond();
	logdoc->setAttr(AOSTAG_LOGTIME, logtime);
	aos_assert_r(siteid != "", false);
	aos_assert_r(logtype != "", false);
	aos_assert_r(logid != "", false);
	return addLogEntry(siteid, logtype, logid, logdoc->toString(), rdata, logtime);
}


bool
AosLogMgr::addLogEntry(
		const OmnString &siteid, 
		const OmnString &logtype,
		const OmnString &logid, 
		const OmnString &docstr, 
		const AosRundataPtr &rdata,
		const u32 logtime)
{
	/*
	 * Chen Ding, 08/12/2011
	// It adds a log entry to the log identified by 'logid'. It first
	// appends the log into the log file. It then adds an entry
	// into the IIL:
	// 		[location]				(if !withtime), or
	// 		[crttime, location]		if (withtime)
	
	u64 docid = 0;
	u32 seqno;
	u64 offset;
	
	u32 logtime2 = logtime;
	if(logtime2 == 0)
	{
		logtime2 = OmnGetSecond();
	}

	bool rslt = sgVersionDocMgr.saveDoc(seqno, offset, docstr.length(), docstr.data());
	aos_assert_r(rslt, false);
	docid = (offset << 32) + seqno;

	OmnString iilname;
	createLogIILName(iilname, siteid, logtype, logid);
	AosIILClientObj::getIILClient()->addU64ValueDoc(iilname, logtime2, docid, false, true, rdata);
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


AosXmlTagPtr
AosLogMgr::getLogEntry(const u64 &docid)
{
	u32 offset = (docid >> 32);
	u32 seqno = (u32)docid;
	return sgVersionDocMgr.readDoc(seqno, offset);
}


bool
AosLogMgr::addSystemBackupLogEntry(
		const OmnString &siteid,
		const OmnString &type,
		const OmnString &fname,
		const u32 &starttime,
		const u32 &endtime,
		const OmnString &status, 
		const OmnString &command, 
		const AosRundataPtr &rdata)
{
	/*
	// System backup logs are grouped under the container whose objid
	// is calculated by AosObjid::composeLoginLogCtnr(siteid). 
	// In the current implementation, if the container has not 
	// been created yet, it will create it.
	aos_assert_r(!mIsStopping, false);

	AosRundataPtr supRdata = OmnNew AosRundata(siteid, AOSAPPNAME_SYSTEM);
	supRdata->resetForReuse(0);
	supRdata->setUserid(AosLoginMgr::getSuperUserDocid(siteid, supRdata));
	
	OmnString ctnr_objid = AosObjid::composeLoginLogCtnr(siteid);
	AosObjMgr::getSelf()->createLoginLogCtnr(supRdata);

	OmnString contents = "<entry ";
	contents << "stime=\"" << starttime << "\" "
		<< "etime=\"" << endtime << "\" "
		<< AOSTAG_OTYPE << "=\"" << AOSOTYPE_SYSBACK_LOG << "\" "
		<< AOSTAG_PARENTC << "=\"" << ctnr_objid << "\" "
		<< AOSTAG_SUBTYPE << "=\"" << type << "\">"
		<< "<fname>" << fname << "</fname>"
		<< "<status>" << status << "</status>"
		<< "<command><![CDATA[" << command << "]]></command>"
		<< "</entry>";

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(contents, "");
	aos_assert_r(root, false);
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, false);

	OmnScreen << "To add login log entry: " << child->toString() << endl;

	AosXmlTagPtr dd = AosDocServer::getSelf()->createDocSafe3(supRdata, child, 
		"", "", false, false, false, false, false, false, true);

	aos_assert_r(dd, false);
	return true;
	*/
	OmnNotImplementedYet;
	return 0;
}


bool
AosLogMgr::addInvalidReadEntry(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &doc)
{
	// Someone tried to retrieve something that are not authorized. This function
	// adds a log entry to the system. The log entry is added to the log container
	// whose objid is calculated by AosObjid::composeInvReadLogCtnr(). In addition,
	// the object's counter is incremented by one.
	//
	u64 requester = rdata->getUserid();
	OmnString siteid = rdata->getSiteid();

	aos_assert_r(!mIsStopping, false);

	// Modify the doc
	AosDocClientObj::getDocClient()->incrementValue(doc->getAttrU64(AOSTAG_DOCID, 0), 
			doc->getAttrStr(AOSTAG_OBJID), AOSTAG_INVREAD, 
			"0", false, true, __FILE__, __LINE__, rdata);

	OmnString ctnr_objid = AosObjid::composeInvReadLogCtnr(siteid);
	AosObjMgr::getSelf()->createInvReadLogCtnr(rdata);

	OmnString requester_cid = AosCloudidSvr::getCloudid(requester, rdata);
	u64 starttime = OmnGetSecond();
	OmnString contents = "<entry ";
	contents << AOSTAG_STARTTIME << "=\"" << starttime << "\" "
		<< AOSTAG_OTYPE << "=\"" << AOSOTYPE_LOG << "\" "
		<< AOSTAG_SUBTYPE << "=\"" << AOSSTYPE_INVREAD << "\" "
		<< AOSTAG_PARENTC << "=\"" << ctnr_objid << "\" "
		<< AOSTAG_ACCESSED_OBJID << "=\"" << doc->getAttrStr(AOSTAG_OBJID) << "\" "
		<< AOSTAG_ACCESSED_DOCID << "=\"" << doc->getAttrStr(AOSTAG_DOCID) << "\" "
		<< AOSTAG_ACCESSED_VER << "=\"" << doc->getAttrStr(AOSTAG_VERSION) << "\" "
		<< AOSTAG_ACCESSED_TYPE << "=\"" << doc->getAttrStr(AOSTAG_OTYPE) << "\" "
		<< AOSTAG_ACCESSED_STYPE << "=\"" << doc->getAttrStr(AOSTAG_SUBTYPE) << "\" "
		<< AOSTAG_REQUESTER << "=\"" << requester_cid << "\" "
		<< "/>";

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(contents, "");
	aos_assert_r(root, false);
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, false);

	OmnScreen << "To add login log entry: " << child->toString() << endl;

	AosXmlTagPtr received_doc = rdata->setReceivedDoc(child);
	u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
	OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	AosXmlTagPtr dd = AosDocClientObj::getDocClient()->createDocSafe3(rdata, child, 
		"", "", false, false,
		false, false, false, true, true);
	rdata->setUserid(userid);
	rdata->setAppname(appname);
	rdata->setReceivedDoc(received_doc);
	aos_assert_r(dd, false);
	return true;
}


bool
AosLogMgr::createWhoVisitedLogEntry(
		const AosRundataPtr &rdata,
		u32 &seqno,
		u64 &offset)
{
	// This function logs who visited which doc and when.
	
	// 1. Retrieve the smartDoc
	OmnString sdocObjid = rdata->getSmartdocObjid();
	AosXmlTagPtr sdoc = AosDocClientObj::getDocClient()->getDoc(rdata->getSiteid(), sdocObjid, rdata);
	aos_assert_r(sdoc, false);

	// 2. Retrieve UserDoc
	AosXmlTagPtr userdoc;
	const AosSessionPtr session = rdata->getSession();
	if(session)
	{
		userdoc = session->getUserDoc(rdata);
	}

	// 3. create logStr
	OmnString logstr = "<entry ";
	logstr << AOSTAG_SITEID << "=\"" << rdata->getSiteid() << "\" "
		<< AOSTAG_OBJID << "=\"" << rdata->getOrigObjid() << "\" ";

	
	OmnString whatToLog = sdoc->getAttrStr(AOSTAG_WHOREADME_LOG);
	OmnStrParser1 parser(whatToLog.data(), "|");
	OmnString log;
	while( (log = parser.nextWord()) != "" )
	{
		if(log == AOSTAG_TIME)
		{
			logstr << AOSTAG_TIME << "=\"" << OmnGetMDY() << "\" ";	
		}
		else if(log == AOSTAG_FULLTIME)
		{
			logstr << AOSTAG_FULLTIME << "=\"" << OmnGetTime(AosLocale::getDftLocale()) << "\" ";
		}
		else if(log == AOSTAG_CLOUDID)
		{
			OmnString cid = "";
			if(userdoc)
			{
				cid = userdoc->getAttrStr(AOSTAG_CLOUDID);
				if(cid == "") cid = "Unknown";
			}

			logstr << AOSTAG_CLOUDID << "=\"" << cid << "\" ";
		}
		else if(log == AOSTAG_REALNAME)
		{
			OmnString realname;
			if(userdoc)
			{
				realname = userdoc->getAttrStr(AOSTAG_REALNAME);
				if (realname == "") realname = "Unknown";
			}else
			{
				realname = "Visitor";
			}

			logstr << AOSTAG_REALNAME << "=\"" << realname << "\" ";
		}
		else if(log == AOSTAG_HPVPD)
		{
			OmnString hpvpd = "";
			if(userdoc)
			{
				hpvpd = userdoc->getAttrStr(AOSTAG_HPVPD);
			}

			logstr << AOSTAG_HPVPD << "=\"" << hpvpd << "\" ";
		}
		else if(log == AOSTAG_OBJIMAGE)
		{
			OmnString image = "";
			if(userdoc)
			{
				image = userdoc->getAttrStr(AOSTAG_OBJIMAGE);
			}

			logstr << AOSTAG_OBJIMAGE << "=\"" << image << "\" ";
		}
	}

	logstr << "/>";

	// Add the entry into the log file
	bool rslt = sgVersionDocMgr.saveDoc(seqno, offset, logstr.length(), logstr.data());
	return rslt;
}

#endif
