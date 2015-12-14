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
// 2014/11/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoCall/JimoCallDialer.h"

#include "JimoAPI/Jimo.h"
#include "JimoCall/DialerManual.h"
#include "JimoCall/DialerRoundRobin.h"
#include "JimoCall/DialerAuto.h"
#include "JimoCall/DialerAsync.h"
#include "JimoCall/JimoCall.h"
#include "JimoCall/JimoFunction.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/JimoCaller.h"
#include "UtilData/FN.h"
#include "Thread/Mutex.h"
#include "Util/Buff.h"


static bool sgInited = false;
static OmnMutex sgLock;


AosJimoCallDialer *	AosJimoCallDialer::smIILManualDialer = 0;
AosJimoCallDialer *	AosJimoCallDialer::smDocManualDialer = 0;
AosJimoCallDialer *	AosJimoCallDialer::smRoundRobinDialer = 0;


AosJimoCallDialer::AosJimoCallDialer(
		AosRundata *rdata,
		const Type type)
:
mLock(OmnNew OmnMutex()),
mLockRaw(mLock.getPtr()),
mType(type),
mSelfEPID(0)
{
}


AosJimoCallDialer::~AosJimoCallDialer()
{
}


bool
AosJimoCallDialer::init(AosRundata *rdata)
{
	sgLock.lock();
	if (sgInited)
	{
		sgLock.unlock();
		return true;
	}

	int d_id = eFirstDialerID;

	AosXmlTagPtr app_conf = OmnApp::getAppConfig();
	aos_assert_r(app_conf, false);

	AosXmlTagPtr iil_cfg = app_conf->getFirstChild("iil_dialer_manual");
	aos_assert_r(iil_cfg, false);

	AosXmlTagPtr doc_cfg = app_conf->getFirstChild("doc_dialer_manual");
	aos_assert_r(doc_cfg, false);

	try
	{
		smIILManualDialer = OmnNew AosDialerManual(rdata, iil_cfg, d_id++);
		smDocManualDialer = OmnNew AosDialerManual(rdata, doc_cfg, d_id++);
		//smRoundRobinDialer = OmnNew AosDialerRoundRobin(rdata, dialer_id++);
	}
	catch(...)
	{
		OmnAlarm << "error" << enderr;
		sgLock.unlock();
		return false;
	}

	sgInited = true;
	sgLock.unlock();
	return true;
}


bool 
AosJimoCallDialer::msgRead(const OmnConnBuffPtr &connbuff)
{
	// A message is read from the network. This function decodes
	// the message and calls the function.
	AosBuffPtr buff = OmnNew AosBuff(connbuff AosMemoryCheckerArgs);

	AosJimoCall jimo_call;
	jimo_call.setConnBuff(connbuff);
	jimo_call.setBSON(buff);
	jimo_call.setFromEPInfo(connbuff->getRemoteAddr(), connbuff->getRemotePort());
	
	AosRundataPtr rdataptr = AosRundata::serializeFromBuffForJimoCall(buff.getPtr());
	aos_assert_r(rdataptr, false);	
	AosRundata *rdata = rdataptr.getPtrNoLock();

	bool rslt = jimo_call.msgRead(rdata, buff.getPtr());
	aos_assert_r(rslt, false);

	if (jimo_call.getMsgID() != OmnMsgId::eJimoCallResp)
	{
		AosLogError(rdata, false, AosErrmsgId::eInvalidMessage)
			<< AosFN::eValue1 << jimo_call.getMsgID()
			<< AosFN::eValue2 << jimo_call.getJimoCallID() << enderr;
		return false;
	}

	// u64 call_id = jimo_call.getJimoCallID();
	//OmnScreen << "call_id: " << call_id << endl;

	mLock->lock();
	callitr_t call_itr = mPendingCalls.find(jimo_call.getJimoCallID());
	if (call_itr == mPendingCalls.end())
	{
		mLock->unlock();
		AosLogError(rdata, false, AosErrmsgId::eUnrecognizedJimoCallID)
			<< AosFN::eValue1 << jimo_call.getMsgID()
			<< AosFN::eValue2 << jimo_call.getJimoCallID() << enderr;
		return true;
	}

	AosJimoCallPtr old_jimo_call = call_itr->second;
	mLockRaw->unlock();

	aos_assert_r(old_jimo_call, false);
	old_jimo_call->callResponded(rdata, jimo_call);

	return true;
}


bool
AosJimoCallDialer::logError(
		AosRundata *rdata, 
		const AosErrmsgId::E errid,
		const u32 from_epid,
		const u32 to_epid,
		const u64 jimocall_id,
		const u32 msg_id,
		const int func,
		const int package)
{
	// AosLog(rdata, tJimoCallLog, AosErrmsgId::eInvalidFunction)
	// 	<< AosFN::eFromEPID << from_epid
	// 	<< AosFN::eToEPID << to_epid
	// 	<< AosFN::eJimoCallID << jimocall_id
	// 	<< AosFN::eMessageId << mesasge_id
	// 	<< AosFN::eFunction << func
	// 	<< AosFN::ePackage << package << endlog;
	OmnAlarm << "Error" << enderr;
	return true;
}


OmnString 
AosJimoCallDialer::getCommListenerName() const
{
	return "JimoCallDialer";
}


void 		
AosJimoCallDialer::readingFailed()
{
	OmnNotImplementedYet;
	return;
}


AosJimoCallDialer *
AosJimoCallDialer::getIILManualDialer(AosRundata *rdata)
{
	if (!sgInited) init(rdata);
	return smIILManualDialer;
}


AosJimoCallDialer *
AosJimoCallDialer::getDocManualDialer(AosRundata *rdata)
{
	if (!sgInited) init(rdata);
	return smDocManualDialer;
}


AosJimoCallDialer *
AosJimoCallDialer::getRoundRobinDialer(AosRundata *rdata)
{
	if (!sgInited) init(rdata);
	return smRoundRobinDialer;
}


void
AosJimoCallDialer::eraseJimoCall(const u64 &jimo_callid)
{
	mLockRaw->lock();
	if (mPendingCalls.count(jimo_callid) > 0)
	{
		mPendingCalls.erase(jimo_callid);
	}
	else
	{
		OmnScreen << "not find jimo_call: " << jimo_callid << endl;
	}
	mLockRaw->unlock();
}


AosJimoCallDialerPtr 
AosJimoCallDialer::createDialer(AosRundata *rdata, const AosXmlTagPtr &conf)
{
	// This function creates a dialer based on 'conf':
	// 	<conf type="xxx" .../>
	aos_assert_rr(conf, rdata, 0);

	OmnString typestr = conf->getAttrStr("type");
	try
	{
		if (typestr == "manual")
		{
			return OmnNew AosDialerManual(rdata, conf, rand());
		}
		else if (typestr == "roundrobin")
		{
			return OmnNew AosDialerRoundRobin(rdata, conf, rand());
		}
		else if (typestr == "auto")
		{
			return OmnNew AosDialerAuto(rdata, conf, rand());
		}
		else if (typestr == "async")
		{
			return OmnNew AosDialerAsync(rdata, conf, rand());
		}
		else
		{
			AosLogError(rdata, true, "unrecognized_dialer_type")
				<< AosFN("Type") << typestr << enderr;
			return 0;
		}
	}

	catch (...)
	{
		AosLogError(rdata, false, "internal_error") << enderr;	
	}

	return 0;
}

