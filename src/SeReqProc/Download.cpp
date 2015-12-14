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
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/Download.h"

#include "SEInterfaces/DocClientObj.h"
#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SeLogClient/SeLogClient.h"
#include "SeReqProc/ReqidNames.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SEServer/SeReqProc.h"
#include "SEModules/OnlineMgr.h"
#include "SEUtil/Docid.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "Util/File.h"
AosDownload::AosDownload(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_DOWNLOAD, 
		AosSeReqid::eDownload, rflag)
{
}


bool 
AosDownload::proc(const AosRundataPtr &rdata)
{
	// It is to download a resource. The resource id (either docid
	//  or objid) is specified through the 'args' node.
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString errmsg;
	OmnString args = root->getChildTextByAttr("name", "args");
	if (args == "")
	{
		errmsg = "Missing the resource ID to download!";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

// felicia, 2013/08/16 for document
//	OmnString cid = AosCloudidSvr::getCloudid(rdata->getUserid(), rdata);
//	if (cid == "")
//	{
//		AosSetError(rdata, "missing_cloudid");
//		AOSLOG_LEAVE(rdata);
//		return false;
//	}

	OmnString objid, docid, owner;
	AosParseArgs(args, "objid", objid, "docid", docid, "owner", owner);

	AosXmlTagPtr doc;
	if (objid != "")
	{
		// Ketty 2012/03/03
		//OmnString ccc = cid;
		//if (owner != "") 
		//{
		//	ccc = AosCloudidSvr::getCloudid(owner, rdata);
		//	if (ccc == "")
		//	{
		//		AosSetError(rdata, "missing_cloudid");
		//		AOSLOG_LEAVE(rdata);
		//		return false;
		//	}
		//}
		//AosObjid::attachCidAsNeeded(objid, ccc);
		doc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	}
	else if (docid != "")
	{
		u64 did = AosDocid::convertToU64(docid);
		doc = AosDocClientObj::getDocClient()->getDocByDocid(did, rdata);
	}
	else
	{
		OmnString errmsg = "Unrecognized resource args: ";
		errmsg << args;
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	if (!doc)
	{
		errmsg = "Resource not found!";
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	// Check whether the requester has the right
	if (!AosSecurityMgrSelf->checkRead(doc, rdata))
	{
		rdata->setError();
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString fname = doc->getAttrStr(AOSTAG_RSC_FNAME);
	OmnString dirname = doc->getAttrStr(AOSTAG_RSCDIR);
	if (fname == "" || dirname == "")
	{
		errmsg = "Failed to find the resource!";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	//OmnString dir = OmnApp::getAppConfig()->getAttrStr("user_basedir");
	//dir << "/" << dirname;
	OmnString dir = OmnApp::getAppConfig()->getAttrStr("user_basedir", "User");
	OmnString fdir;
	if (dir[0] == '/')
	{
		dir << "/" << dirname;
		fdir = dir;
	}
	else
	{
		fdir = OmnApp::getAppBaseDir();
		fdir << "/" << dir << "/" << dirname;
	}

	u64 binary_docid = doc->getAttrU64(AOSTAG_DOCID, 0);
	OmnString otype = doc->getAttrStr(AOSTAG_OTYPE);
	if (otype == "zky_binarydoc")
	{
		AosBuffPtr buff;                                                                                            
		AosXmlTagPtr binary_doc = AosDocClientObj::getDocClient()->retrieveBinaryDoc(binary_docid, buff, rdata);
		aos_assert_r(binary_doc, false);

		OmnString file_name;
		file_name << binary_docid << "_" << fname;
		fname = file_name;

		fdir = "/tmp";
		file_name = fdir;
		file_name << "/" << fname;
		OmnFilePtr file = OmnNew OmnFile(file_name, OmnFile::eCreate AosMemoryCheckerArgs);
		aos_assert_r(file && file->isGood(), false);
		
		file->put(0, buff->data(), buff->dataLen(), true);
	}

	OmnString contents = "<Contents ";
	contents << AOSTAG_DOCID << "=\"" << binary_docid
		<< "\" " << AOSTAG_OBJID << "=\"" << doc->getAttrStr(AOSTAG_OBJID)
		<< "\" zky_filedir=\"" << fdir
		<< "\" filename =\"" << fname 
		<< "\"/>";

	rdata->setResults(contents);
	rdata->setOk();
	AOSSYSLOG_CREATE(true, AOSREQIDNAME_DOWNLOAD, rdata);
	AOSLOG_LEAVE(rdata);
	return true;
}
