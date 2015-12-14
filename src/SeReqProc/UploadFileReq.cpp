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
// The torturer is in SengTorturer/TesterUploadFileReqNew.cpp
//   
//
// Modification History:
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/UploadFileReq.h"

#include "SEInterfaces/DocClientObj.h"
#include "EventMgr/EventMgr.h"
#include "Security/Session.h"
#include "SEInterfaces/SecurityMgrObj.h"
#include "SeLogClient/SeLogClient.h"
#include "SearchEngine/Ptrs.h"
#include "SEInterfaces/DocSvrCbObj.h"
#include "SeReqProc/ReqidNames.h"
#include "SeReqProc/ResumeReq.h"
#include "SEInterfaces/LoginMgrObj.h"
#include "SEModules/OnlineMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEUtil/Docid.h"
#include "SEUtil/UrlMgr.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "Util/File.h"
#include "Util/StrParser.h"
#include "VersionServer/VersionServer.h"
#include "XmlInterface/WebRequest.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosUploadFileReq::AosUploadFileReq(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_UPLOADFILEREQ, 
		AosSeReqid::eUploadFileReq, rflag)
{
}


bool 
AosUploadFileReq::proc(const AosRundataPtr &rdata)
{
	// The client requests uploading a file. The file size and 
	// file name are specified in 'args', which is in the format:
	// 	"size=xxx,fname=xxx" 
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString args = root->getChildTextByAttr("name", "args");
	AosXmlTagPtr objdef = root->getFirstChild("objdef");

	if (!objdef || !(objdef = objdef->getFirstChild()))
	{
		OmnString errmsg = "Missing the resource document!";
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	

	OmnString cid = AosCloudidSvr::getCloudid(rdata->getUserid(), rdata);
	if (cid == "")
	{
		AosSetError(rdata, "missing_cloudid");
		AOSLOG_LEAVE(rdata);
		return false;
	}

	// 'args' is in the form:
	// 		name=value,name=value
	// where 'name' can be either 'folder' or 'size'.
	OmnString ctnr_objid, size, orig_fname, is_public;
	AosParseArgs(args, "folder", ctnr_objid, "size", size, "fname", 
		orig_fname, "public", is_public);

	int filesize = (size == "")?-1:atoi(size.data());
	if (orig_fname == "")
	{
		AosSetError(rdata, AOSLT_MISSING_FILENAME);
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString dirname;
	if (ctnr_objid != "")
	{
		// It is to upload a file into a pre-defined folder. 
		OmnString errmsg;
		if (!AosObjid::procObjid(ctnr_objid, cid, errmsg))
		{
			AosSetError(rdata, AOSLT_INTERNAL_ERROR);
			OmnAlarm << rdata->getErrmsg() << ": " << errmsg << enderr;
			AOSLOG_LEAVE(rdata);
			return false;
		}

		AosXmlTagPtr folderdoc = AosDocClientObj::getDocClient()->getDocByObjid(ctnr_objid, rdata);
		if (!folderdoc)
		{
			AosSetError(rdata, AOSLT_CONTAINER_NOT_FOUND);
			OmnAlarm << rdata->getErrmsg() << ". Container: " << ctnr_objid << enderr;
			AOSLOG_LEAVE(rdata);
			return false;
		}

		if (!AosSecurityMgrObj::getSecurityMgr()->checkUploadToPubFolder(ctnr_objid, filesize, rdata))
		{
			AOSLOG_LEAVE(rdata);
			return false;
		}

		// It is allowed. Get the next seqno and updates it.
		OmnString fno = AosDocClientObj::getDocClient()->incrementValue(
				folderdoc->getAttrU64(AOSTAG_DOCID, 0), 
				folderdoc->getAttrStr(AOSTAG_OBJID), 
				AOSZTG_FSEQNO_PFOLDER, 
				"1", true, true, rdata);
		dirname = ctnr_objid;
		dirname << "/" << fno;
	}
	else
	{
		// It should be put into the user's home directory. Retrieve
		// the user's doc.
		u64 userid = rdata->getUserid();
		AosUserAcctObjPtr user_acct = AosDocClientObj::getDocClient()->getUserAcct(userid, rdata);
		if (!user_acct)
		{
			AosSetError(rdata, AOSLT_USER_NOT_FOUND);
			OmnAlarm << rdata->getErrmsg() << ": " << userid << enderr;
			AOSLOG_LEAVE(rdata);
			return false;
		}

		AosXmlTagPtr userdoc = user_acct->getDoc();
		if (!userdoc)
		{
			AosSetError(rdata, AOSLT_INTERNAL_ERROR);
			OmnAlarm << rdata->getErrmsg() << ": " << userid << enderr;
			AOSLOG_LEAVE(rdata);
			return false;
		}

		// Get the new filename. It is stored in the iil: 
		// 	AOSZTG_FILE_SEQNO
		dirname = userdoc->getAttrStr(AOSTAG_USER_RSCDIR);
		u64 ctnr_docid = 0;
		if (dirname == "")
		{
			OmnString ctnrobjid = userdoc->getAttrStr(AOSTAG_PARENTC);
			if (ctnrobjid == "")
			{
				AosSetError(rdata, AOSLT_MISSING_PARENT_CONTAINER);
				OmnAlarm << rdata->getErrmsg() << ": " << userdoc->toString() << enderr;
				AOSLOG_LEAVE(rdata);
				return false;
			}

			ctnr_docid = AosDocClientObj::getDocClient()->getDocidByObjid(ctnrobjid, rdata);
			if (!ctnr_docid)
			{
				AosSetError(rdata, AOSLT_CONTAINER_NOT_FOUND);
				OmnAlarm << rdata->getErrmsg() << ": " << ctnrobjid << enderr;
				AOSLOG_LEAVE(rdata);
				return false;
			}

		}

		dirname = userdoc->getAttrStr(AOSTAG_USER_RSCDIR);
		if (dirname == "")
		{
			if (!AosCreateUserDirs(ctnr_docid, userdoc, rdata))
			{
				AOSLOG_LEAVE(rdata);
				return false;
			}

			dirname = userdoc->getAttrStr(AOSTAG_USER_RSCDIR);
			if (dirname == "")
			{
				AosSetError(rdata, AOSLT_FAILED_CREATE_USERDIR);
				OmnAlarm << rdata->getErrmsg() << ": " << ctnr_docid << enderr;
				AOSLOG_LEAVE(rdata);
				return false;
			}
		}

		//James 2011/02/15
		if (!AosSecurityMgrObj::getSecurityMgr()->checkUploadFile(filesize, dirname, rdata))
		{		
			AOSLOG_LEAVE(rdata);	
			return false;
		}

		OmnString fno = AosDocClientObj::getDocClient()->incrementValue(
				userdoc->getAttrU64(AOSTAG_DOCID, 0), 
				userdoc->getAttrStr(AOSTAG_OBJID),
				AOSZTG_FSEQNO_USER, "1", true, true, rdata);
		dirname << "/" << fno;
	}

	// Now the user request has been granted and the directory into
	// which the file should be uploaded is in 'fno'. Note that to
	// prevent duplicated names, in the current implementations, 
	// all files are put in a unique subdirectory (just for the file).
	// The file directory is 'dirname'. 
	
	// Create the directory
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
		//fdir << "/" << dir << "/" << dirname;
		fdir << dir << "/" << dirname;
	}

	//OmnString cmd = "mkdir -p ";
	OmnString cmd = "mkdir -m 777 -p ";
	cmd << fdir;
	system(cmd.data());

	// Create the resource object
	objdef->setAttr(AOSTAG_RSC_FNAME, orig_fname);
	objdef->setAttr(AOSTAG_RSCDIR, dirname);
	objdef->setAttr(AOSTAG_OTYPE, "uploadfile");
	objdef->removeAttr(AOSTAG_DOCID);
	objdef->removeAttr(AOSTAG_OBJID);

	// Modify the objid
	OmnString objid = objdef->getAttrStr(AOSTAG_OBJID);
	if (objid != "")
	{
		AosObjid::attachCidAsNeeded(objid, cid);
	}

	//AosXmlTagPtr dd = AosDocClientObj::getDocClient()->createDocSafe3(
	//		rdata, objdef, "", "", (is_public == "true"), 
	//		true, false, false, true, true, true);
	bool rslt = AosDocClientObj::getDocClient()->createDoc1(
			rdata, 0, "", objdef, false, 0, 0, false);
	if (!rslt)
	{
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString contents = "<Contents ";
	contents << AOSTAG_DOCID << "=\"" << objdef->getAttrStr(AOSTAG_DOCID)
		<< "\" " << AOSTAG_OBJID << "=\"" << objdef->getAttrStr(AOSTAG_OBJID)
		<< "\" " << AOSTAG_DIRNAME << "=\"" << fdir
		<< "\"/>";

	rdata->setResults(contents);
	rdata->setOk();
	AOSSYSLOG_CREATE(true, AOSREQIDNAME_UPLOADFILEREQ, rdata);
	AOSLOG_LEAVE(rdata);
	return true;
}

