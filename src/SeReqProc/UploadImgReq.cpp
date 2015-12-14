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
// The torturer is in SengTorturer/TesterUploadImgReqNew.cpp
//   
//
// Modification History:
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/UploadImgReq.h"

#include "EventMgr/EventMgr.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "SeLogClient/SeLogClient.h"
#include "SeReqProc/ReqidNames.h"
#include "SeReqProc/ResumeReq.h"
#include "SEModules/LoginMgr.h"
#include "SEModules/OnlineMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEUtil/Docid.h"
#include "SEUtil/UrlMgr.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "Util/File.h"
#include "Util/StrParser.h"
#include "VersionServer/VersionServer.h"
#include "XmlInterface/WebRequest.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"

static int sgUploadImgSeqno = AosGetUsec();
static OmnMutex sgLock;

AosUploadImgReq::AosUploadImgReq(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_UPLOADIMGREQ, 
		AosSeReqid::eUploadImgReq, rflag)
{
}


bool 
AosUploadImgReq::proc(const AosRundataPtr &rdata)
{
	// root
	// 	<request>
	// 		<objdef>
	// 			<userdoc .../>
	// 		</objdef>
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	sgLock.lock();
	int seqno = sgUploadImgSeqno++;
	sgLock.unlock();

	OmnString dirname = "aos_img_upload";
	dirname << seqno;

	// Remove the directory in case it exists
	OmnString cmd = "rm -rf ";
	cmd << dirname;
	system(cmd.data());

	cmd = "mkdir -m 777 /tmp/";
	cmd << dirname;
	system(cmd.data());

	// Chen Ding, 06/28/2010, Make sure the directory contains nothing
	cmd = "rm -f /tmp/";
	cmd << dirname << "/*";
	system(cmd.data());

	OmnString contents = "<Contents ";
	contents << AOSTAG_DIRNAME << "=\"" << "/tmp/" << dirname << "\"/>";

	rdata->setResults(contents);
	rdata->setOk();
	AOSSYSLOG_CREATE(true, AOSREQIDNAME_UPLOADIMGREQ, rdata);
	AOSLOG_LEAVE(rdata);
	return true;
}

