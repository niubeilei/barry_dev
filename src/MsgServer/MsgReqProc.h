////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
// Description:
// Modification History:
// 06/15/2011 Created by Jozhi
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_MsgServer_MsgReqProc_h
#define Aos_MsgServer_MsgReqProc_h

#include "XmlUtil/SeXmlParser.h"
#include "MsgServer/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Security/Session.h"
#include "Security/SessionMgr.h"
#include "MsgServer/SysUser.h"

class AosMsgReqProc : public AosNetReqProc
{
	OmnDefineRCObject;
public:

private:
	OmnMutexPtr					mLock;
	bool						mIsStopping;
	int 						mCurrentId;
	int 						mEndId;
	AosRundataPtr				mRundata;
	OmnString 					mSsid;
	OmnString 					mOperation;

public:
	AosMsgReqProc();
	~AosMsgReqProc();
	static bool 	config(const AosXmlTagPtr &config);

	bool 	stop();

	// NetProc Interface 
	AosNetReqProcPtr	clone();
	bool				procRequest(const OmnConnBuffPtr &buff);
	bool				conClosed(const OmnTcpClientPtr &conn);
	
	static bool	sendClientResp(
			const OmnTcpClientPtr &conn,
			const OmnString &callback,
			const OmnString &contents);
	
private:
	bool	procHttpReq(
			const AosWebRequestPtr &req,
			const OmnString &httpreq);	        

	bool	parseHttpReq(
			OmnString &method,
			map<OmnString, OmnString> &parms,
			const OmnString &hhead);

	static char		charToInt(char ch);
	static char* 	decodeUrl(const char *data);

	static string 	encodeUrl(const char *src);
	static string 	char2hex(char dec);
};
#endif
