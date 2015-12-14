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
// All requests are stored in mRequests. When there are too many, it will 
// stop adding requests to mRequests. When mRequests are empty, it checks
// whether there are additional requests from the log file. If yes, it
// reads in all the requests to mRequests. 
//
// Modification History:
// 2011/07/15: Created by Ken Lee
////////////////////////////////////////////////////////////////////////////

// Ketty 2013/03/22
#if 0
#include "IILTransServer/IILTransServer.h"

#include "IILUtil/IILFuncType.h"
#include "SEInterfaces/IILMgrObj.h"
#include "Rundata/Rundata.h"
#include "Thread/CondVar.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"


AosIILTransServer::AosIILTransServer()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar)
{
}


AosIILTransServer::~AosIILTransServer()
{
}


bool
AosIILTransServer::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosIILTransServer::start()
{
	OmnThreadedObjPtr thisPtr(this, false); 
	mProcThread = OmnNew OmnThread(thisPtr, "Thrd", 0, true, true, __FILE__, __LINE__);
	mProcThread->start();
	return true;
}


void
AosIILTransServer::addErrorResp(const AosRundataPtr &rdata)
{
	OmnString str;
	str << "<rsp rslt=\"false\">" << rdata->getErrmsg() << "</rsp>";
	rdata->setContents(str);
}


bool
AosIILTransServer::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	// Ketty 2013/02/22
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	while(state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if(mProcQueue.empty())
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}

		list<AosIILTransPtr>::iterator itr = mProcQueue.begin();
		aos_assert_rl(itr != mProcQueue.end(), mLock, false);

		AosIILTransPtr iil_trans = *itr;
		mProcQueue.erase(itr);
		mLock->unlock();

		rdata->setSiteid(iil_trans->getSiteid());
		bool rslt = procPriv(iil_trans, rdata);
		if(!rslt)
		{
			OmnAlarm << "proc iil_trans error!" << enderr;
			continue;
		}

		iil_trans->setFinishLater();
		if(iil_trans->isNeedResp())
		{
			sendResp(rdata, iil_trans);
		}
	}
	return true;
}


/*
bool
AosIILTransServer::proc(
		const AosXmlTransPtr &trans, 
		const AosXmlTagPtr &trans_doc, 
		const AosRundataPtr &rdata)
{
	AosIILTransPtr iiltrans = AosIILTrans::getTrans(trans, trans_doc, rdata);
	if (!iiltrans)
	{
		rdata->setError() << "Failed to get the trans!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		addErrorResp(rdata);
		return false;
	}

	bool rslt = proc(iiltrans, rdata);
	if(rslt)
	{
		trans->setFinishLater();
		iiltrans->setFinishLater();
	}
	return rslt;
}


bool
AosIILTransServer::proc(
		const AosTinyTransPtr &tiny_trans,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(tiny_trans, rdata, false);
	if (!tiny_trans->isIILTrans())
	{
		AosSetError(rdata, AosErrmsgId::eInternalError);
		OmnAlarm << rdata->getErrmsg() << enderr;
		addErrorResp(rdata);
		return false;
	}
	
	tiny_trans->setFinishLater();
	AosIILTransPtr iiltrans((AosIILTrans*)tiny_trans.getPtr(), false);
	return proc(iiltrans, rdata);
}
*/

bool
AosIILTransServer::proc(
		const AosBigTransPtr &trans,
		const AosBuffPtr &content,
		const AosRundataPtr &rdata)
{
	// Ketty 2013/02/22
	trans->setFinishLater();
	AosIILTransPtr iil_trans = AosIILTrans::getTrans(trans, content, rdata);
	if (!iil_trans)
	{
		rdata->setError() << "Failed to get the trans!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		addErrorResp(rdata);
		return false;
	}

	mLock->lock();
	mProcQueue.push_back(iil_trans);
	mCondVar->signal();
	mLock->unlock();

	return true;
}


bool
AosIILTransServer::proc(const AosIILTransPtr &iil_trans, const AosRundataPtr &rdata)
{
	iil_trans->setFinishLater();
	if (!iil_trans)
	{
		rdata->setError() << "Failed to get the trans!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		addErrorResp(rdata);
		return false;
	}

	// Ketty 2012/11/21
	mLock->lock();
	mProcQueue.push_back(iil_trans);
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool
//AosIILTransServer::proc(const AosIILTransPtr &iil_trans, const AosRundataPtr &rdata)
AosIILTransServer::procPriv(const AosIILTransPtr &iil_trans, const AosRundataPtr &rdata)
{
	if (!iil_trans)
	{
		rdata->setError() << "Failed to get the trans!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		addErrorResp(rdata);
		return false;
	}

	u32 siteid = iil_trans->getSiteid();
	if(siteid == 0)
	{
		rdata->setError() << "Failed to get the siteid!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		addErrorResp(rdata);
		return false;
	}
	
	AosIILFuncType::E opr = iil_trans->getOperation();
	aos_assert_r(AosIILFuncType::isValid(opr), false);

	bool rslt = true;
	if (opr == AosIILFuncType::eCreateIIL || opr == AosIILFuncType::eCreateIILByName)
	{
		rslt = iil_trans->proc(0, rdata);
	}
	else
	{
		u64 iilid = iil_trans->getIILID(rdata);
		if(iilid)
		{
			rslt = AosIILMgrObj::getIILMgr()->addTrans(
				iilid, siteid, iil_trans,
				iil_trans->isNeedResp(), rdata);
		}
		else
		{
			if(iil_trans->needCreateIIL())
			{
				rdata->setError() << "failed to get iilid!";
				OmnAlarm << rdata->getErrmsg() << enderr;
				rslt = false;
			}
			else
			{
				rslt = true;
				OmnString str = "<rsp rslt =\"false\" />";
				rdata->setContents(str);
			}
		}
	}

	if (!rslt)
	{
		addErrorResp(rdata);
	}
	return rslt;
}


void
AosIILTransServer::sendResp(
		const AosRundataPtr &rdata,
		const AosIILTransPtr &trans)
{
	OmnString resp_cont = rdata->getContents();
	AosBuffPtr resp_buff = OmnNew AosBuff(resp_cont.length() + 100, 0 AosMemoryCheckerArgs);
	resp_buff->setBuff(resp_cont.data(), resp_cont.length());

	AosIILMgrObj::getIILMgr()->sendResp(trans, resp_buff);
}

#endif
