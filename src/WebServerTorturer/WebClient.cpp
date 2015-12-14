////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 	Created: 05/09/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "WebServerTorturer/WebClient.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Util/File.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "sys/stat.h"
#include "UtilComm/TcpClient.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"


OmnSingletonImpl(AosWebClientSingleton,
                 AosWebClient,
                 AosWebClientSelf,
                "AosWebClient");


static OmnString sgWebPath;
const int sgGroupSize = 1000;
const int sgSleepLength = 10;

AosWebClient::AosWebClient()
:
mLock(OmnNew OmnMutex())
{
}


AosWebClient::~AosWebClient()
{
}


bool      	
AosWebClient::start()
{
	return true;
}


bool        
AosWebClient::stop()
{
	return true;
}


OmnRslt     
AosWebClient::config(const OmnXmlParserPtr &def)
{
	return true;
}


bool
AosWebClient::start(const AosXmlTagPtr &config)
{
	/////////////////////////////////////////////test
	/*
	OmnString uri = "http://192.168.99.98/lps-4.7.2/prod/Ext/GICs/gic_container.js";
	OmnString method = "GET";
	OmnString parms = "trans_id=2&objid=sc_search_vpd&zky_siteid=100&operation=retrieve&username=nonameyet&zky_retrieve=2&zky_vpdid=10002";
	OmnString cookies = "ys-cute_editor_west=o%3Acollapsed%3Db%253A0; ys-cute_editor_east=o%3Acollapsed%3Db%253A0";
	OmnString mRemoteAddr = config->getAttrStr(AOSCONFIG_REMOTE_ADDR);
	int mRemotePort = config->getAttrInt(AOSCONFIG_REMOTE_PORT, -1);
	int mNumPorts = 1;
	OmnTcpClientPtr conn = OmnNew OmnTcpClient("test", mRemoteAddr, mRemotePort, mNumPorts, eAosTLT_NoLengthIndicator);
	OmnConnBuffPtr buff = OmnNew OmnConnBuff();
	sendRequestPublic(uri, method, parms, cookies, conn, buff);
	OmnScreen <<"end Response: \r\n" << buff->getBuffer() <<endl;
	*/
	return true;
}

bool	
AosWebClient::sendRequestPublic(
		const OmnString &uri,
		const OmnString	&method,
		const OmnString	&parms,
		const OmnString	&cookies,
		OmnTcpClientPtr &conn,
		OmnConnBuffPtr &buff)
{
	OmnString errmsg;
	OmnRslt rslt;
	if (!conn->isConnGood())
	{
		conn->closeConn();
		rslt = conn->connect(errmsg);
		if (!conn->isConnGood())
		{
			OmnAlarm << "Failed to process connection" << rslt.getErrmsg() << enderr;
		}
	}

	sendRequest(uri, method, parms, cookies, conn);
	OmnString response;
	while (1)
	{
		bool timeout = false;
		bool connBroken;
		bool ok = conn->readFrom(buff, eReadTimeoutSec, timeout, connBroken, true);
		if (!ok || timeout)
		{
			break;
		}
		if (connBroken)
		{
			break;
		}
	}
	return true;
}

void
AosWebClient::createRequest(
		OmnString &request,
		const OmnString &uri, 
		const OmnString &method, 
		const OmnString &parms,
		const OmnString &cookies,
		const int connType)
{
	OmnString hostname;
	OmnString path;
	OmnString port;
	OmnString protocol = "http://";
	int hostFirstIdx = (uri.substr(0, protocol.length()-1) == protocol)?protocol.length():0;
	int pathFirstIdx = uri.findSubString("/", hostFirstIdx);
	int portFirstIdx = uri.findSubString(":", hostFirstIdx);
	if (portFirstIdx > 0)
	{
		port = uri.substr(portFirstIdx + 1, pathFirstIdx-1);
	}
	else
	{
		port = "80";
	}
	if (pathFirstIdx > 0)
	{
		hostname = uri.substr(hostFirstIdx, pathFirstIdx - 1);
		path = uri.substr(pathFirstIdx, uri.length()-1);
	}
	else
	{
		hostname = uri.substr(hostFirstIdx, uri.length()-1);
		path = "/";
	}

	request << method << " " << path << " HTTP/1.0\r\n";
	request << "Host: "<< hostname << "\r\n";
	request << "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9.2.10) Gecko/20100914 Firefox/3.6.10\r\n";
	request << "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n";
	request << "Accept-Language: zh-cn,zh;q=0.5\r\n";
	//request << "Accept-Encoding: gzip,deflate\r\n";
	request << "Accept-Encoding: utf-8\r\n";
	request << "Accept-Charset: GB2312,utf-8;q=0.7,*;q=0.7\r\n";
	if (connType == 1)
	{
		request << "Keep-Alive: 115\r\n";
		request << "Connection: keep-alive\r\n";
	}
	else
	{
		request << "Connection: colse\r\n";
	}
	if (method == "POST")
	{
		request << "Content-Type: application/x-www-form-urlencoded;charset=UTF-8\r\n";
		request << "X-Requested-With: Ext.basex\r\n";
		request << "Referer: " << uri <<"\r\n";
		request << "Content-Length: " << strlen(parms) << "\r\n";
		request << "Pragma: no-cache\r\n";
		request << "Cache-Control: no-cache\r\n";
	}
	request << "Cookie: " << cookies << "\r\n";
	request << "\r\n";
	if (method == "POST")
	{
		request << parms;
	}
}


bool	
AosWebClient::sendRequest(
		const OmnString &uri,
		const OmnString &method,
		const OmnString &parms, 
		const OmnString &cookies,
		const OmnTcpClientPtr &conn)
{
	int connType = 1;
	OmnString request ;
	createRequest(request, uri, method, parms, cookies, connType);
	conn->writeToSock(request.getBuffer(), request.length());
	return true;
}

