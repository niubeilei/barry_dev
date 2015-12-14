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
// 07/01/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_SEInterfaces_TransSvrObj_h
#define Aos_SEInterfaces_TransSvrObj_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/TransId.h"
#include "SvrProxyUtil/Ptrs.h"
#include "UtilData/ModuleId.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

class AosTransSvrObj : virtual public OmnRCObject
{
private:
	static AosTransSvrObjPtr		smTransSvr;

public:
	virtual bool	config(const AosXmlTagPtr &conf) = 0;
	virtual bool 	start() = 0;
	virtual bool 	stop() = 0;

	virtual bool	recvMsg(const AosAppMsgPtr &trans) = 0;
	virtual bool 	finishTrans(vector<AosTransId> &trans_ids) = 0;
	virtual bool    finishTrans(const AosTransPtr &trans) = 0;
	virtual void 	resetCrtCacheSize(const u64 proced_msg_size) = 0;

	static void setTransSvr(const AosTransSvrObjPtr &d) {smTransSvr = d;}
	static AosTransSvrObjPtr getTransSvr() {return smTransSvr;}
};
#endif
