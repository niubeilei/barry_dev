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
// The torturer is in SengTorturer/TesterCreateEcgImgNew.cpp
//   
//
// Modification History:
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/CreateEcgImg.h"

#include "Actions/ActSeqno.h"
#include "SearchEngine/DocServerCb.h"
#include "EventMgr/EventMgr.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "SeLogClient/SeLogClient.h"
#include "SeReqProc/ReqidNames.h"
#include "SeReqProc/ResumeReq.h"
#include "SEModules/ImgProc.h"
#include "SEModules/LoginMgr.h"
#include "SEModules/OnlineMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEBase/SeUtil.h"
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
#include "Base64/Base64.h"
#include "API/AosApiC.h"


AosCreateEcgImg::AosCreateEcgImg(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_CREATEECGIMG, 
		AosSeReqid::eCreateEcgImg, rflag)
{
}


bool 
AosCreateEcgImg::proc(const AosRundataPtr &rdata)
{
	// 'root' should be in the form:
	// 	<request>
	// 		<command>
	// 			<cmd opr="create_ecg_img"/>
	// 		</command>
	// 		<item name="siteid">100</item>
	// 		<item name="operation">serverCmd</item>
	// 		<item name="trans_id">10000</item>
	// 		<item name="args">data</item>
	// 	</request>
	//
	// If success, it returns the following:
	// 	<Contents objid="xxx"/>
	//
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString errmsg;

	if (AosSeReqProc::getIsStopping())
	{
		rdata->setError() << "Server is stopping";
		AOSLOG_LEAVE(rdata);
		return true;
	}

	OmnString decoded = root->getChildTextByAttr("name", "args");
	if (decoded == "") 
	{
		errmsg = "Missing the args";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	OmnString session = rdata->getSsid();
	if (session == "") 
	{
		errmsg = "Missing the session";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString mkcmd = "mkdir  -p ";
	mkcmd << AosImgProc::getImgDir() << "/EcgImgPdf/" << session;
	OmnScreen << "---------------- To mkdir file: " << mkcmd << endl;
	system(mkcmd.data());

	AosBuff buff(decoded.length(), 0 AosMemoryCheckerArgs);
	int len = DecodeBase64((unsigned char*)decoded.data(), (unsigned char*)buff.data(), decoded.length());

	OmnString filename = AosImgProc::getImgDir() << "/EcgImgPdf/";
		filename << session << "/ecg.jpg";
	OmnFilePtr file = OmnNew OmnFile(filename, OmnFile::eCreate AosMemoryCheckerArgs);
	aos_assert_r(file->isGood(), false);
	file->put(0, buff.data(), len, true);	


	//upload img
	
	AosXmlTagPtr doc;
	OmnString imgdoc = "";
	imgdoc << "<embedobj zky_pctrs=\"images\" fname=\"ecg.png\" zky_otype=\"image\" "
		   << "zky_imgdir=\"" << AosImgProc::getImgDir() << "/EcgImgPdf/" << session << "\" zky_cat=\"image\"/>";
	doc = AosXmlParser::parse(imgdoc AosMemoryCheckerArgs);
	aos_assert_rr(doc, rdata, false);

	AosImgProcPtr mImgProc = AosSeReqProc::getImgProc();
	aos_assert_r(!mImgProc, false);
	AosXmlTagPtr cmd;
	OmnString contents;
	bool rslt = mImgProc->uploadImage(rdata, doc, cmd, contents);
	
	//remove img
	OmnString rmcmd = "rm -rf ";
	rmcmd << AosImgProc::getImgDir() << "/EcgImgPdf/" << session;
	OmnScreen << "---------------- To remove file: " << rmcmd << endl;
	system(rmcmd.data());

	if(!rslt) 
	{
		AOSLOG_LEAVE(rdata);
		return false;
	}else
	{
		rdata->setResults(contents);
		rdata->setOk();
		AOSLOG_LEAVE(rdata);
		return true;
	}
	AOSLOG_LEAVE(rdata);
	return true;
}


