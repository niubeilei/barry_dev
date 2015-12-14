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
// 10/20/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SeSysLog/SeSysLog.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
//#include "DocProc/DocProc.h"
#include "SEInterfaces/DocClientObj.h"
#include "Thread/Mutex.h"
#include "AppMgr/App.h"
#include "Util/OmnNew.h"
#include "Porting/TimeOfDay.h"
#include "Rundata/Rundata.h"
#include "Porting/GetTime.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/SeTypes.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SearchEngine/Ptrs.h"
#include "DataSync/DataSyncClt.h"
#include "VersionServer/VersionServer.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlDoc.h"
#include "SEInterfaces/IILClientObj.h"
#include "API/AosApi.h"

#include <dirent.h>
#include <sys/types.h>

#if 0
AosSeSysLog::AosSeSysLog()
:
mLock(OmnNew OmnMutex()),
mBuffs(0),
mGlobalLogId(1000)
{
}


AosSeSysLog::~AosSeSysLog()
{
}

bool
AosSeSysLog::config(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);
	AosXmlTagPtr def = config->getFirstChild("NewDs");   
	aos_assert_r(def, false);
	
	AosTransDistributorPtr roundRobin = OmnNew AosRoundRobin();
	mTransClient = OmnNew AosTransClient(def, roundRobin);
	return true;
}


bool
AosSeSysLog::addCreateLog(const u64 userid, const AosXmlTagPtr &doc, const AosRundataPtr &rdata)
{
	// Call this function when the server adds a create-doc request
	// to its queue.
	// <log operation="create" userid="" crttime="">
	// 		<dfdfdfdfdfddfd>
	// </log>

	if (!OmnApp::isRunning()) return true;
	u32 logid = getLogId();
	u64 time = OmnGetSecond();
	OmnString log = "<log operation=\"create\" ";
	log << "logid=\"" << logid << "\" "; 
	log << "userid=\"" << userid << "\" crttime=\"" << time << "\">"; 
	log << doc->toString() << "</log>";

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(log, "" AosMemoryCheckerArgs);
	aos_assert_rr(root, rdata, false);

	AosXmlTagPtr doc_xml = root->getFirstChild();
	aos_assert_rr(doc_xml, rdata, false);

	//AosXmlTagPtr resp;
	aos_assert_r(mTransClient, false);
	mTransClient->addTrans(doc_xml, rdata);
		
	return true;
}


bool
AosSeSysLog::addDeleteLog(
		const u64 &userid,
		const OmnString &appname,
		const AosXmlTagPtr &doc, 
		const AosRundataPtr &rdata)
{
	//<log operation="delete_obj" userid="" crttime="" siteid="" appname="">
	//     dfdfdfdfdfddfd
	//</log>

	if (!OmnApp::isRunning()) return true;
	u64 time = OmnGetSecond();
	OmnString strDoc = doc->toString();
	strDoc.removeNonprintables();

	u32 logid = getLogId();
	OmnString log = "<log operation=\"delete_obj\" ";
	log << "logid=\"" << logid << "\" "; 
	log << "userid=\"" << userid << "\" crttime=\"" << time << "\" ";
	log << "appname=\"" << appname << "\">";
	log << strDoc << "</log>";

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(log, "" AosMemoryCheckerArgs);
	aos_assert_rr(root, rdata, false);

	AosXmlTagPtr doc_xml = root->getFirstChild();
	aos_assert_rr(doc_xml, rdata, false);

	//AosXmlTagPtr resp;
	aos_assert_rr(mTransClient, rdata, false);
	mTransClient->addTrans(doc_xml, rdata); 
	
	return true;
}


bool
AosSeSysLog::addModifyLog(
		const u64 userid,
		const AosXmlTagPtr &newdoc,
		const AosXmlTagPtr &orgdoc,
		const bool synobj, 
		const AosRundataPtr &rdata)
{
	// <log operation="modify_obj" userid="" crttime="">
	// 		<newdoc>dfdfddfd</newdoc>
	// 		<orgdoc>dfdfddfd</orgdoc>
	// </log>

	if (!OmnApp::isRunning()) return true;
	u32 logid = getLogId();
	u64 time = OmnGetSecond();
	OmnString newdoc_str = newdoc->toString();
	newdoc_str.removeNonprintables();
	
	OmnString orgdoc_str = orgdoc->toString();
	orgdoc_str.removeNonprintables();
	
	OmnString log = "<log operation=\"modify_obj\" ";
	log << "logid=\"" << logid << "\" "; 
	log << "userid=\"" << userid << "\" crttime=\"" 
		<< time << "\" synobj=\"" << synobj << "\">";
	log << "<newdoc>" << newdoc_str << "</newdoc>";
	log << "<orgdoc>" << orgdoc_str << "</orgdoc>";
	log << "</log>";

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(log, "" AosMemoryCheckerArgs);
	aos_assert_rr(root, rdata, false);

	AosXmlTagPtr doc_xml = root->getFirstChild();
	aos_assert_r(doc_xml, false);

	aos_assert_rr(mTransClient, rdata, false);
	mTransClient->addTrans(doc_xml, rdata); 
	return true;
}


bool
AosSeSysLog::addModifyAttrLog(
		const OmnString &attrname,
		const OmnString &oldvalue,
		const OmnString &newvalue,
		const bool exist,
		const bool value_unique,
		const bool docid_unique,
		const u64 &docid, 
		const AosRundataPtr &rdata)
{
	// This system log entry is for modifying an attribute
	// <log operation="modify_attr" userid="" crttime=""
	// 		attrname="" oldvalue="" 
	// 		newvalue="" exist="" value_unique="" docid_unique="" docid="" />
	// </log>
	
	if (!OmnApp::isRunning()) return true;
	u32 logid = getLogId();
	u64 time = OmnGetSecond();
	OmnString log = "<log operation=\"modify_attr_str\" ";
	log << "logid=\"" << logid << "\" "; 
	log	<< "crttime=\"" << time 
		<< "\" attrname=\"" << attrname 
		<< "\" oldvalue=\"" << oldvalue
		<< "\" newvalue=\"" << newvalue 
		<< "\" exist=\"" << exist
		<< "\" value_unique=\"" << value_unique 
		<< "\" docid_unique=\"" << docid_unique 
		<< "\" docid=\"" << docid << "\"/>";

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(log, "" AosMemoryCheckerArgs);
	aos_assert_rr(root, rdata, false);

	AosXmlTagPtr doc_xml = root->getFirstChild();
	aos_assert_rr(doc_xml, rdata, false);

	//AosXmlTagPtr resp;
	aos_assert_rr(mTransClient, rdata, false);
	mTransClient->addTrans(doc_xml, rdata); 
	return true;
}


bool
AosSeSysLog::addModifyAttrLog(
		const OmnString &attrname,
		const u64 &oldvalue,
		const u64 &newvalue,
		const bool exist,
		const bool value_unique,
		const bool docid_unique,
		const u64 &docid, 
		const AosRundataPtr &rdata)
{
	// This system log entry is for modifying an attribute
	// <log operation="modify_attr" userid="" crttime=""
	// 		attrname="" oldvalue="" 
	// 		newvalue="" exist="" value_unique="" docid_unique="" docid="" />
	// </log>
	
	if (!OmnApp::isRunning()) return true;
	u32 logid = getLogId();
	u64 time = OmnGetSecond();
	OmnString log = "<log operation=\"modify_attr_u64\" ";
	log << "logid=\"" << logid << "\" "; 
	log	<< "crttime=\"" << time 
		<< "\" attrname=\"" << attrname 
		<< "\" oldvalue=\"" << oldvalue
		<< "\" newvalue=\"" << newvalue 
		<< "\" exist=\"" << exist
		<< "\" value_unique=\"" << value_unique 
		<< "\" docid_unique=\"" << docid_unique 
		<< "\" docid=\"" << docid << "\"/>";

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(log, "" AosMemoryCheckerArgs);
	aos_assert_rr(root, rdata, false);

	AosXmlTagPtr doc_xml = root->getFirstChild();
	aos_assert_rr(doc_xml, rdata, false);

	aos_assert_rr(mTransClient, rdata, false);
	mTransClient->addTrans(doc_xml, rdata); 
	return true;
}


bool
AosSeSysLog::addAddMemberLog(
			const AosXmlTagPtr &doc,
			const OmnString &ctnr_objid,
			const u64 &userid, 
			const AosRundataPtr &rdata)
{
	// <log operation="modify_obj" userid="" crttime="">
	// 		<newdoc>dfdfddfd</newdoc>
	// 		<orgdoc>dfdfddfd</orgdoc>
	// </log>

	if (!OmnApp::isRunning()) return true;
	u32 logid = getLogId();
	u64 time = OmnGetSecond();
	OmnString doc_str = doc->toString();
	doc_str.removeNonprintables();
	
	OmnString log = "<log operation=\"addAddMember\" ";
	log << "logid=\"" << logid << "\" "; 
	log << "userid=\"" << userid << "\" crttime=\"" << time 
		<< "\" ctnr_objid=\"" << ctnr_objid << "\">";
	log << "<doc>" << doc_str << "</doc>";
	log << "</log>";

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(log, "" AosMemoryCheckerArgs);
	aos_assert_rr(root, rdata, false);

	AosXmlTagPtr doc_xml = root->getFirstChild();
	aos_assert_rr(doc_xml, rdata, false);

	aos_assert_rr(mTransClient, rdata, false);
	mTransClient->addTrans(doc_xml, rdata); 
	return true;
}

	

bool	
AosSeSysLog::addRemoveMemberLog(
			const AosXmlTagPtr &doc,
			const OmnString &ctnr_objid, 
			const AosRundataPtr &rdata)
{
	// <log operation="modify_obj" userid="" crttime="">
	// 		<newdoc>dfdfddfd</newdoc>
	// 		<orgdoc>dfdfddfd</orgdoc>
	// </log>

	if (!OmnApp::isRunning()) return true;
	u32 logid = getLogId();
	u64 time = OmnGetSecond();
	OmnString doc_str = doc->toString();
	doc_str.removeNonprintables();
	
	OmnString log = "<log operation=\"addAddMember\" ";
	log << "logid=\"" << logid << "\" "; 
	log << "crttime=\"" << time << "\" ctnr_objid=\"" << ctnr_objid << "\">";
		log << "<doc>" << doc_str << "</doc>";
	log << "</log>";

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(log, "" AosMemoryCheckerArgs);
	aos_assert_rr(root, rdata, false);

	AosXmlTagPtr doc_xml = root->getFirstChild();
	aos_assert_rr(doc_xml, rdata, false);

	aos_assert_rr(mTransClient, rdata, false);
	mTransClient->addTrans(doc_xml, rdata); 
	return true;
}



OmnFilePtr
AosSeSysLog::getFile(bool readOnly)
{
	// There is a file named:
	// 		mDirName << "/" + mAdminFname
	// that is used to record the last seqno used. 
	// When creating a new file, it checks the admin file for the next
	// seqno. If the admin file does not exist, it creates it. 
	// When creating a new file, it updates the admin file (saving 
	// the last seqno used).
	
	if (mFile)
	{
		if (readOnly || mFileSize < mMaxFileSize)
			return mFile;
	}

	u32 crtSeqno = getSeqno();
	OmnString fname = mDirName;
	fname << "/" << mFileName << "_" << crtSeqno; 
	mFile = OmnNew OmnFile(fname, OmnFile::eReadWrite);
	if (!mFile->isGood())
	{
		// The file has not been created yet. Create it.
		mFile = OmnNew OmnFile(fname, OmnFile::eCreate);
		aos_assert_r(mFile->isGood(), NULL);
	}

	mFileSize = mFile->getLength();
	if(mFileSize >= mMaxFileSize)
	{
		fname = mDirName;
		fname << "/" << mFileName << "_" << (crtSeqno + 1); 
		mFile = OmnNew OmnFile(fname, OmnFile::eCreate);
		aos_assert_r(mFile->isGood(), NULL);
		setSeqno(crtSeqno + 1);
	}
	mFileSize = mFile->getLength();

	return mFile;
}


bool
AosSeSysLog::addCreateRequest(
		const AosXmlTagPtr &root, 
		const AosXmlTagPtr &log,
		const AosRundataPtr &rdata)
{
	// <log operation="create" userid="" crttime="">
	// 		<dfdfdfdfdfddfd>
	// </log>
	
	AosXmlTagPtr doc = log->getNextChild();

	// Retrieve 'userid'
	u64 userid = log->getAttrU64("userid", AOS_INVDID);
	aos_assert_rr(userid != AOS_INVDID, rdata, false);

	// Retrieve 'siteid'
	OmnString siteid = doc->getAttrStr(AOSTAG_SITEID);
	aos_assert_r(siteid != "", false);

	// Retrieve 'objid'
	OmnString objid = doc->getAttrStr(AOSTAG_OBJID);
	aos_assert_rr(objid != "", rdata, false);

	// Retrieve 'docid'
	u64 docid = doc->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
	aos_assert_rr(docid != AOS_INVDID, rdata, false);

	// Retrieve 'cloudid'
	OmnString cid = AosCloudidSvr::getCloudid(userid, rdata);

	// Bind the objid
	OmnString iilname = AosIILName::composeObjidListingName(siteid);
	AosIILClientPbj::getIILCLient()->addStrValueDoc(iilname, false, objid, docid, true, true, rdata);

	//save to file
	bool rslt = false; //= AosXmlDoc::saveToFile(docid, doc, true);
	if (!rslt)
	{
		rdata->setError() << "Failed saving to file: " << docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// AosDocProc::getSelf()->addCreateRequest(root, userid, doc, 0, 0, "", "", rdata);
	return true;
}


bool
AosSeSysLog::addModifyObjRequest(
		const AosXmlTagPtr &root, 
		const AosXmlTagPtr &log, 
		const AosRundataPtr &rdata)
{
	// 	<log ...>
	// 		<newdoc ...>
	// 		<orgdoc ...>
	// 	</log>

	AosXmlTagPtr newdoc = log->getNextChild("newdoc")->getFirstChild();
	AosXmlTagPtr orgdoc = log->getNextChild("orgdoc")->getFirstChild();
	//convert to root
	OmnString orgdoc1 = orgdoc->toString();
	AosXmlParser parser;
	orgdoc = parser.parse(orgdoc1, "" AosMemoryCheckerArgs);

	if(!newdoc || !orgdoc)
	{
		OmnAlarm << "The xml doc is wrong" << enderr;
		return false;
	}
	// Retrieve userid
	u64 userid = log->getAttrU64("userid", AOS_INVDID);

	// Retrieve cloudid
	OmnString cid = AosCloudidSvr::getCloudid(userid, rdata);

	// Retrieve sysobj 
	int temp = -2;
	temp = log->getAttrInt("synobj", -1);
	aos_assert_r(temp!= -1, false);
	bool sysobj = intToBool(temp);
	
	// Retrieve 'siteid'
	OmnString siteid = newdoc->getAttrStr(AOSTAG_SITEID);
	aos_assert_r(siteid != "", false);

	//check new objid exist
	OmnString newobjid = newdoc->getAttrStr(AOSTAG_OBJID);
	u64 docid = newdoc->getAttrU64(AOSTAG_DOCID, 0);
	OmnString oldobjid = orgdoc->getAttrStr(AOSTAG_OBJID);
	if(!orgdoc)
	{
		OmnAlarm << "The xml doc is wrong" << enderr;
	}
	if (newobjid != oldobjid)
	{
		if (AosDocClientObj::getDocClient()->doesObjidExist(siteid, newobjid, rdata) == AOS_INVDID)
		{
			OmnString iilname = AosIILName::composeObjidListingName(siteid);
			AosIILClientObj::getIILClient()->modifyValueDoc(iilname, oldobjid, newobjid, true, true, docid, rdata);
		}
	}

	//AosDocServer::getSelf()->checkModifying(orgdoc, newdoc, cid, rdata);
	if(!orgdoc)
	{
		OmnAlarm << "The xml doc is wrong" << enderr;
	}
	
	// Chen Ding, 10/12/2011
	// if (!AosVersionServer::getSelf()->addVersionDoc(docid, orgdoc, newdoc, rdata))
	// {
	//	OmnAlarm << "Failed creating the version obj: " << newobjid << enderr;
	//}
	if(!orgdoc)
	{
		OmnAlarm << "The xml doc is wrong" << enderr;
	}
	
	//AosDocMgr::getSelf()->addCopy(docid, newdoc);
	AosDocClientObj::getDocClient()->addCopy(docid, newdoc);
	bool rslt = false;// = AosXmlDoc::saveToFile(docid, newdoc, false);
	if (!rslt)
	{
		rdata->setError() << "Failed saving to file: " << docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	u64 doc_transid = 0;
	AosDocProc::getSelf()->addModifyRequest(root, userid, newdoc, orgdoc, sysobj, rdata, doc_transid);
	
	return true;
}


bool
AosSeSysLog::addModifyAttrRequest_str(
		const AosXmlTagPtr &root, 
		const AosXmlTagPtr &log, 
		const AosRundataPtr &rdata)
{
	// Retrieve aname
	OmnString aname = log->getAttrStr("attrname");
	aos_assert_rr(aname != "", rdata, false);

	// Retrieve oldvalue
	OmnString oldvalue = log->getAttrStr("oldvalue");
	aos_assert_rr(oldvalue != "", rdata, false);
	
	// Retrieve newvalue
	OmnString newvalue = log->getAttrStr("newvalue");
	aos_assert_rr(newvalue != "", rdata, false);
	
	// Retrieve exist
	int temp = -2;
	temp = log->getAttrInt("exist", -1);
	aos_assert_rr(temp!= -1, rdata, false);
	bool exist = intToBool(temp);

	// Retrieve value_unique 
	temp = log->getAttrInt("value_unique", -1);
	aos_assert_rr(temp!= -1, rdata, false);
	bool value_unique = intToBool(temp);
	
	// Retrieve docid_unique
	temp = log->getAttrInt("docid_unique", -1);
	aos_assert_rr(temp!= -1, rdata, false);
	bool docid_unique = intToBool(temp);

	// Retrieve 'docid'
	u64 docid = log->getAttrU64("docid", AOS_INVDID);
	aos_assert_rr(docid != AOS_INVDID, rdata, false);
	
	u64 doc_transid = 0;
	AosDocProc::getSelf()->addModifyAttrRequest(aname, oldvalue, newvalue,
			exist, value_unique, docid_unique, docid, __FILE__, __LINE__, rdata, doc_transid);
	
	//need to save
	//AosXmlTagPtr doc = AosDocMgr::getSelf()->getDoc(docid, "", rdata);
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
	if (doc)
	{
		doc->setAttr(aname, newvalue);
		bool rslt = false;// = AosXmlDoc::saveToFile(docid, doc, false);
		if (!rslt)
		{
			rdata->setError() << "Failed saving to file: " << docid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}
	return true;
}

	
bool
AosSeSysLog::addModifyAttrRequest_u64(
		const AosXmlTagPtr &root, 
		const AosXmlTagPtr &log, 
		const AosRundataPtr &rdata)
{
	// Retrieve aname
	OmnString aname = log->getAttrStr("attrname");
	aos_assert_rr(aname != "", rdata, false);

	// Retrieve oldvalue
	u64 oldvalue = log->getAttrU64("oldvalue", AOS_INVDID);
	aos_assert_rr(oldvalue != AOS_INVDID, rdata, false);
	
	// Retrieve newvalue
	u64 newvalue = log->getAttrU64("newvalue", AOS_INVDID);
	aos_assert_rr(newvalue != AOS_INVDID, rdata, false);
	
	// Retrieve exist
	int temp = -2;
	temp = log->getAttrInt("exist", -1);
	aos_assert_rr(temp!= -1, rdata, false);
	bool exist = intToBool(temp);

	// Retrieve value_unique 
	temp = log->getAttrInt("value_unique", -1);
	aos_assert_rr(temp!= -1, rdata, false);
	bool value_unique = intToBool(temp);
	
	// Retrieve docid_unique
	temp = log->getAttrInt("docid_unique", -1);
	aos_assert_rr(temp!= -1, rdata, false);
	bool docid_unique = intToBool(temp);

	// Retrieve 'docid'
	u64 docid = log->getAttrU64("docid", AOS_INVDID);
	aos_assert_rr(docid != AOS_INVDID, rdata, false);
	
	//AosDocProc::getSelf()->addModifyAttrRequest(aname, oldvalue, newvalue,
	//		exist, value_unique, docid_unique, docid, rdata);
	
	//need to save
	//AosXmlTagPtr doc = AosDocMgr::getSelf()->getDoc(docid, "", rdata);
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
	if (doc) 
	{
		bool rslt = false;// = AosXmlDoc::saveToFile(docid, doc, false);
		if (!rslt)
		{
			rdata->setError() << "Failed saving to file: " << docid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}
	return true;
}


bool
AosSeSysLog::addDeleteObjRequest(
		const AosXmlTagPtr &root, 
		const AosXmlTagPtr &log,
		const AosRundataPtr &rdata)
{
	//<log operation="delete_obj" userid="" crttime="" siteid="" appname="">
	//     dfdfdfdfdfddfd
	//</log>
	
	AosXmlTagPtr deletedoc = log->getNextChild();

	// Retrieve 'objid'
	OmnString objid = deletedoc->getAttrStr(AOSTAG_OBJID);
	aos_assert_rr(objid!= "", rdata, false);

	// Retrieve 'did'
	u64 did = deletedoc->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
	aos_assert_rr(did!= AOS_INVDID, rdata, false);
	
	// Retrieve 'userid'
	u64 userid = log->getAttrU64("userid", AOS_INVDID);
	aos_assert_rr(userid != AOS_INVDID, rdata, false);

	// Retrieve 'siteid'
	OmnString siteid = deletedoc->getAttrStr(AOSTAG_SITEID);
	aos_assert_rr(siteid != "", rdata, false);
	
	// Retrieve 'iappname'
	OmnString appname = log->getAttrStr("appname");

	OmnString iilname = AosIILName::composeObjidListingName(siteid);
	// bool rslt1 = AosIILClientObj::getIILClient()->removeValueDoc(iilname, objid, did, rdata);
	// bool rslt2 = AosDocProc::getSelf()->addDeleteRequest(
	// 		root, deletedoc, userid, appname, "", "", rdata);
	
	// return rslt1 & rslt2;
	return false;
}


bool
AosSeSysLog::setSeqno(u32 value)
{
	if (!mSeqnoFile) openSeqnoFile();
	aos_assert_r(mSeqnoFile, false);

	u32 start = eSeqnoStartAddr;
	char *buff = mBuffs;
	if(!buff)
	{
    	buff = OmnNew char[eSeqnoNums+10];
	  	mBuffs = buff;
	}
	*(u32 *)&buff[eStartFlagOffset] = eStartFlag;
	*(u32 *)&buff[eValueOffset] = value;
	*(u32 *)&buff[eEndFlagOffset] = eEndFlag;

	mSeqnoFile->put(start, buff, eSeqnoNums, true);
	return true;
}


u32
AosSeSysLog::getSeqno()
{
	if (!mSeqnoFile) openSeqnoFile();
	aos_assert_r(mSeqnoFile, false);

	u32 start = eSeqnoStartAddr;

	char *buff = mBuffs;
	if(!buff)
	{
		buff = OmnNew char[eSeqnoNums+10];
		mBuffs = buff;
	}
	mSeqnoFile->readToBuff(start, eSeqnoNums, buff);

	u32 shead = *(u32 *)&buff[eStartFlagOffset];
	u32 value = *(u32 *)&buff[eValueOffset];
	u32 send = *(u32 *)&buff[eEndFlagOffset];

	if(shead!= eStartFlag && send!= eEndFlag)
	{
		OmnAlarm << "fail to read the right value!" << enderr;
		return 0;
	}

	else return value;
}


bool
AosSeSysLog::openSeqnoFile()
{
	if (mSeqnoFile) return true;

	OmnString fname = mDirName;
	fname << "/" << mSeqnoFileName;

	mSeqnoFile = OmnNew OmnFile(fname, OmnFile::eReadWrite);
	aos_assert_r(mSeqnoFile, false);

	if (!mSeqnoFile->isGood())
	{
		mSeqnoFile = OmnNew OmnFile(fname, OmnFile::eCreate);
		aos_assert_r(mSeqnoFile, false);

		u32 start = eSeqnoStartAddr;
		char *buff = mBuffs;
		if(!buff)
		{
    		buff = OmnNew char[eSeqnoNums+10];
	    	mBuffs = buff;
		}
		*(u32 *)&buff[eStartFlagOffset] = eStartFlag;
		*(u32 *)&buff[eValueOffset] = 0;
		*(u32 *)&buff[eEndFlagOffset] = eEndFlag;

		mSeqnoFile->put(start, buff, eSeqnoNums, true);
	}
	aos_assert_r(mSeqnoFile->isGood(), false);

	return true;
}

bool
AosSeSysLog::intToBool(int value)
{
	if(value == 0) return false;
	if(value == 1) return true;
	return false;
}

	
bool
AosSeSysLog::recoverSystem(
		const AosXmlTagPtr &root, 
		const AosXmlTagPtr &log, 
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr doc = log;
	if (!doc)
	{
		rdata->setError() << "Missing the object to be dataSync";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString opr = doc->getAttrStr("operation");
	const char *copr = opr.data();
	switch(copr[0])
	{
	case 'c':
		 if(opr=="create")
		 {
		 	bool rslt = addCreateRequest(root, doc, rdata);
			aos_assert_rr(rslt, rdata, false);
		 	return true;
		 }
		 break;

	case 'm':
		 if (opr=="modify_obj")
		 {
			aos_assert_r(addModifyObjRequest(root, doc, rdata), false);
			return true;
		 }

		 if (opr=="modify_attr_str")
		 {
			aos_assert_r(addModifyAttrRequest_str(root, doc, rdata), false);
			return true;
		 }

		 if (opr=="modify_attr_u64")
		 {
			aos_assert_r(addModifyAttrRequest_u64(root, doc, rdata), false);
			return true;
		 }
		 break;

	case 'd':
		 if (opr=="delete_obj")
		 {
			aos_assert_r(addDeleteObjRequest(root, doc, rdata), false);
			return true;
		 }
		 break;

	default:
		 break;
	}

	OmnAlarm << "Invalid operation: " << opr <<enderr;
	return false;
}

u32 
AosSeSysLog::getLogId()
{
	mLock->lock();
	u32 logid = mMaxLogId++;
	mLock->unlock();
	
	return logid;
}


OmnFilePtr
AosSeSysLog::readFile(u32 seqno)  
{
	OmnString fname = mDirName;
	fname << "/" << mFileName << "_" << seqno;
	mFile = OmnNew OmnFile(fname, OmnFile::eReadOnly);

	return mFile;
}
#endif
