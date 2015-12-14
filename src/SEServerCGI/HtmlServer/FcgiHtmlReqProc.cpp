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
// 10/16/2010: Created by Ketty
////////////////////////////////////////////////////////////////////////////
#include "SEServerCGI/HtmlServer/FcgiHtmlReqProc.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEServerCGI/FcgiConn.h"
#include "SEServer/SeReqProc.h"
#include "Util/OmnNew.h"
#include "UtilComm/ConnBuff.h"
#include "Util/StrParser.h"

AosFcgiHtmlReqProc::AosFcgiHtmlReqProc(const AosNetReqProcPtr &req)
:AosFcgiReqProc(req)
{
}


AosFcgiHtmlReqProc::~AosFcgiHtmlReqProc()
{
}

bool
AosFcgiHtmlReqProc::getHtmlRequest(FCGX_Request &request,char* &data, OmnString &trans_id, OmnString &htmlRequest)
{
	if(!data)
	{
		OmnString urlName = "http://";
		//urlName << FCGX_GetParam("SERVER_NAME", request.envp);
		urlName << FCGX_GetParam("SERVER_ADDR", request.envp);
		urlName << ":" << FCGX_GetParam("SERVER_PORT", request.envp);
		urlName << FCGX_GetParam("REQUEST_URI", request.envp);

		htmlRequest = "<request><item name='zky_retrieve'><![CDATA[4]]></item><item name='loginobj'><![CDATA[true]]></item><item name='url'><![CDATA[";
		htmlRequest << urlName << "]]></item>";
			
		//get Cookie
		OmnString cookies = FCGX_GetParam("HTTP_COOKIE", request.envp);		
		if(cookies)
		{
			htmlRequest << "<zky_cookies>";

			OmnStrParser parser(cookies, ";");
			OmnString subcookie;

			while( (subcookie = parser.nextWord()) !="" )
			{
				OmnStrParser subparser(subcookie, "=");
				OmnString cookieName = subparser.nextWord();
				OmnString cookieValue = subparser.nextWord();
				htmlRequest << "<cookie zky_name=\"" << cookieName << "\"><![CDATA[" << cookieValue << "]]></cookie>"; 
			}
			
			htmlRequest << "</zky_cookies>";
		}

		htmlRequest << "</request>";
		trans_id="true";
	}
	else
	{
		getRequest(request, data, trans_id, htmlRequest);
	}

	return true;
}


bool
AosFcgiHtmlReqProc::procRequest(FCGX_Request &request)
{
	//获取用户请求信息
	char * data = 0;
	int dataLength;
	bool rslt = readData(request, &data, dataLength);

 	OmnString htmlRequest, trans_id;
  	getHtmlRequest(request,data,trans_id, htmlRequest);

   	OmnString htmlResponse = "<response>";
    vector<OmnString> responseCookies;

	if(trans_id)
	{
		OmnConnBuffPtr buff = OmnNew OmnConnBuff(htmlRequest.data(), htmlRequest.length());
		AosFcgiConn *conn = OmnNew AosFcgiConn("","");
		buff->setConn(conn);
		mReqProc->procRequest(buff);
		responseCookies  = mReqProc->getCookie();
		OmnString responseData = conn->getResponse();
		
		if(trans_id != "true") htmlResponse << "<trans_id value=\"" << trans_id << "\"/>";
		htmlResponse << responseData;
		htmlResponse << "</response>";
	}else
	{
		htmlResponse << "<status error=\"true\" code=\"452\">Missing trans_id</status>";
		htmlResponse << "</response>";
	}

	FCGX_FPrintF(request.out, "Content-type: text/html; charset=utf-8\r\n");
	for (int i=0; i<(int)responseCookies.size(); i++)
	{
        OmnString cookie = "Set-Cookie:";
		cookie << responseCookies[i] <<"\r\n";
	    FCGX_FPrintF(request.out, cookie);
	}

	FCGX_FPrintF(request.out, "\r\n");
	FCGX_FPrintF(request.out, "%s", htmlResponse.data());

	FCGX_Finish_r(&request);
	
	if(rslt)	delete []data;
	return true;
}

