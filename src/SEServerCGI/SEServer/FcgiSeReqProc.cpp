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
#include "SEServerCGI/SEServer/FcgiSeReqProc.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEServerCGI/FcgiConn.h"
#include "SEServer/SeReqProc.h"
#include "Util/OmnNew.h"
#include "UtilComm/ConnBuff.h"
#include "Util/StrParser.h"

AosFcgiSeReqProc::AosFcgiSeReqProc(const AosNetReqProcPtr &req)
:AosFcgiReqProc(req)
{
}


AosFcgiSeReqProc::~AosFcgiSeReqProc()
{
}


bool
AosFcgiSeReqProc::procRequest(FCGX_Request &request)
{
	//获取用户请求信息
	char * data = 0;
	int dataLength;
	bool rslt = readData(request, &data, dataLength);

	if(!rslt) return false; //No Data;
	
	OmnString content_type = FCGX_GetParam("CONTENT_TYPE", request.envp);
	if(content_type == "application/x-www-form-urlencoded")
	{
		OmnString seRequest;
		OmnString trans_id;
	    getRequest(request, data, trans_id, seRequest);

		OmnString seResponse = "<response>";
		vector<OmnString> responseCookies;
		if(trans_id)
		{
			OmnConnBuffPtr buff = OmnNew OmnConnBuff(seRequest.data(), seRequest.length());
			AosFcgiConn *conn = OmnNew AosFcgiConn("","");
			buff->setConn(conn);
			mReqProc->procRequest(buff);
			responseCookies  = mReqProc->getCookie();
		    OmnString responseData = conn->getResponse();

			seResponse << "<trans_id value=\"" << trans_id << "\"/>";
			seResponse << responseData;
			seResponse << "</response>";
		}
		else
		{
			seResponse << "<status error=\"true\" code=\"452\">Missing trans_id</status>";
			seResponse << "</response>";
		}

		OmnString downloadPath;
		
		/*OmnString downloadData = responseCookies[0];
		OmnStrParser downloadParser(downloadData, "=");

		OmnString downloadPath;
		if(downloadParser.nextWord() == "DownloadPath")
		{
			downloadPath = downloadParser.nextWord();
		}*/

		if(downloadPath == "")
		{

		    FCGX_FPrintF(request.out, "Content-type: text/html; charset=utf-8\r\n");
			for (int i=0; i<(int)responseCookies.size(); i++)
			{
				OmnString cookie = "Set-Cookie:";
		        cookie << responseCookies[i] <<"\r\n";
		        FCGX_FPrintF(request.out, cookie);
		    }

		    FCGX_FPrintF(request.out, "\r\n");
		    FCGX_FPrintF(request.out, "%s", seResponse.data());

		    FCGX_Finish_r(&request);
		}
		else
		{
			download(request, downloadPath);
		}
	}
	else
	{
		upload(data,dataLength, content_type);
	}
	
	delete []data;
	return true;
}

