////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/05/09 Created by Ketty 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_SEInterfaces_IpcCltObj_h
#define Aos_SEInterfaces_IpcCltObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SvrProxyUtil/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosIpcCltObj : virtual public OmnRCObject
{
protected:
	static AosIpcCltObjPtr		smIpcClt;

public:
	virtual bool config(const AosXmlTagPtr &app_conf) = 0;
	virtual bool start() = 0;
	virtual bool stop() = 0;
	virtual bool connFailed() = 0;
	
	virtual bool sendMsg(const AosAppMsgPtr &msg) = 0;
	virtual bool sendTrans(const AosTransPtr &trans) = 0;

	static void setIpcClt(const AosIpcCltObjPtr &tc) {smIpcClt = tc;}
	static AosIpcCltObjPtr getIpcClt() {return smIpcClt;}
};

#endif
