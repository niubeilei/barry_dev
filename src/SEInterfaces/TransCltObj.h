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
// 2013/03/11 Created by Ketty 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_SEInterfaces_TransCltObj_h
#define Aos_SEInterfaces_TransCltObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SvrProxyUtil/Ptrs.h"
#include "SysMsg/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "Util/TransId.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"


class AosTransCltObj : virtual public OmnRCObject
{
protected:
	static AosTransCltObjPtr		smTransClt;

public:
	virtual bool config(const AosXmlTagPtr &app_conf) = 0;
	virtual bool start() = 0;
	virtual bool stop() = 0;

	virtual bool sendTransAsyncResp(
					const AosRundataPtr &rdata,
					const AosTransPtr &trans) = 0;
	virtual bool sendTransSyncResp(
					const AosRundataPtr &rdata,
					vector<AosTransPtr> vt_trans) = 0;

	virtual bool sendTrans(
					const AosRundataPtr &rdata,
					const AosTransPtr &trans,
					AosBuffPtr &resp,
					bool &svr_death) = 0;
	virtual bool sendTrans(
					const AosRundataPtr &rdata,
					const AosTransPtr &trans,
					bool &svr_death) = 0;

	virtual bool recvResp(
					const AosTransId &trans_id,
					const AosBuffPtr &resp,
					const int from_sid) = 0;
	virtual bool recvAck(
					const AosTransId &trans_id,
					const int from_sid) = 0;
	virtual bool recvSvrDeath(
					const AosTransId &trans_id,
					const int death_sid) = 0;
	virtual bool triggerResend(const AosTriggerResendMsgPtr &msg) = 0;
 	
	static void setTransClt(const AosTransCltObjPtr &tc) {smTransClt = tc;}
	static AosTransCltObjPtr getTransClt() {return smTransClt;}

};

#endif
