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
// 12/17/2009: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SiteMgr/SiteReqProc.h"

#include "Alarm/Alarm.h"
#include "Book/BookMgr.h"
#include "Book/Container.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "DataService/DataService.h"
#include "DataStore/DataStore.h"
#include "DataStore/StoreMgr.h"
#include "Proggie/TaggedData/TaggedData.h"
#include "Proggie/ProggieUtil/TcpRequest.h"
#include "SEUtil/SeXmlParser.h"
#include "SEUtil/XmlTag.h"
#include "SearchEngine/DocServer.h"
#include "TinyXml/TinyXml.h"
#include "TorturerWrappers/ObjectWrapper.h"
#include "TorturerWrappers/ImgConverter.h"
#include "UserMgmt/User.h"
#include "util_c/strutil.h"
#include "Util/OmnNew.h"
#include "Util/StrSplit.h"
#include "Util/File.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/Ptrs.h"
#include "XmlInterface/WebRequest.h"
#include "XmlInterface/Passwd.h"
#include "XmlInterface/XmlRc.h"
#include "XmlInterface/Server/Ptrs.h"
#include "XmlInterface/Server/XmlObj.h"
#include "SiteMgr/SyncServer.h"
#include <iostream>
#include <fstream>
#include <sstream>


AosSiteReqProc::AosSiteReqProc()
{
}


AosSiteReqProc::~AosSiteReqProc()
{
}


bool			
AosSiteReqProc::procRequest(const OmnConnBuffPtr &buff)
{
	// <request syncopr="xxx" .../>
	OmnTcpClientPtr conn = buff->getConn();
	aos_assert_r(conn, false);

	AosWebRequestPtr req = OmnNew AosWebRequest(conn, buff);
	OmnTrace << "To process request: " << req->getData() << endl;
	AosXmlRc errcode = eAosXmlInt_General;
	OmnString errmsg;
	char *data = req->getData();

	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(data, "");
	if (!xml || !(xml=xml->getFirstChild()))
	{
		errmsg = "Failed to retrieve the XML!";
		OmnAlarm << errmsg << enderr;
		sendResp(req, errcode, errmsg, "");
		return false;
	}

	OmnString opr = xml->getAttrStr("syncopr");
	data = (char *)opr.data();
	bool rslt = false;
	OmnString contents;
	switch (data[0])
	{
	case eGetDataid:
		 rslt = getDataid(req, xml, contents, errcode, errmsg);
		 break;
	
	case eXmlObjCreated:
		 rslt = xmlObjCreated(req, xml, contents, errcode, errmsg);
		 break;

	default:
		 errmsg = "Unrecognized operator code: ";
		 errmsg << opr;
		 OmnAlarm << errmsg << enderr;
	}

	sendResp(req, errcode, errmsg, contents);
	return rslt;
}


bool
AosSiteReqProc::getDataid(
		const AosNetRequestPtr &req, 
		const AosXmlTagPtr &cmd,
		OmnString &contents,
		AosXmlRc &errcode,
		OmnString &errmsg)
{
	// It requests a block of dataid. It will send a response 
	// <response startid='xxx' num='xxx'/>
	errcode = eAosXmlInt_General;
	int num;
	OmnString startid = AosSyncServerSelf->getDataidBlock(num);
	aos_assert_r(startid != "" && num > 0, false);
	contents = "<response startid=\"";
	contents << startid << "\" num=\"" << num << "\"/>";
	errcode = eAosXmlInt_Ok;
	return true;
}


bool
AosSiteReqProc::xmlObjCreated(
		const AosNetRequestPtr &req, 
		const AosXmlTagPtr &cmd,
		OmnString &contents,
		AosXmlRc &errcode,
		OmnString &errmsg)
{
	return false;
}


void
AosSiteReqProc::sendResp(
		const AosNetRequestPtr &req, 
		const AosXmlRc errcode,
		const OmnString &errmsg,
		const OmnString &contents)
{
	OmnString resp = "<status error=\"";
	if (errcode == eAosXmlInt_Ok) resp << "false\" code=\"200\"/>";
	else resp << "true\" code=\"" << errcode << "\">" << errmsg << "</status>";

	if (contents != "" ) resp << contents;

OmnTrace << "Send response: " << resp << endl;
	req->sendResponse(resp);
}
#endif
