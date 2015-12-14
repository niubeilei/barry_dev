////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TestAgentInfo.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestServer_TestAgentInfo_h
#define Omn_TestServer_TestAgentInfo_h

#include "Debug/Rslt.h"
#include "Message/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Ptrs.h"


class OmnTestAgentInfo : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnString		mAgentName;
	OmnTcpClientPtr	mConn;
	
public:
	OmnTestAgentInfo(const OmnString &name, const OmnTcpClientPtr &conn);
	virtual ~OmnTestAgentInfo();

	OmnString		getName() const {return mAgentName;}
	bool			isConnGood();
	OmnRslt			sendMsg(const OmnMsgPtr &msg);
	bool			setConn(const OmnTcpClientPtr &conn);

private:

};
#endif

