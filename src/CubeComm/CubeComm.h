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
// Modification History:
// 2014/11/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CubeComm_CubeComm_h
#define Aos_CubeComm_CubeComm_h

#include "Alarm/Alarm.h"
#include "CubeComm/EndPointInfo.h"
#include "CubeComm/Ptrs.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SEUtil/LogFieldNames.h"
#include "Thread/ThreadedObj.h"
#include "Util/IpAddr.h"
#include "Util/OmnNew.h"
#include "Util/Ptrs.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "UtilComm/CommListener.h"
#include "UtilComm/Ptrs.h"


class OmnMutex;
class OmnCondVar;


class AosCubeComm : public OmnCommListener,
				    public OmnThreadedObj
{
	OmnDefineRCObject;

protected:
	OmnString           	mName;
	OmnCommListenerPtr		mCaller;

	OmnMutexPtr				mLock;
	OmnMutex *		        mLockRaw;
	OmnCondVarPtr	        mCondVar;
	OmnCondVar *	        mCondVarRaw;
	queue<OmnConnBuffPtr>	mQueue;
	OmnThreadPtr            mThread;

public:
	AosCubeComm(const OmnString &name, const OmnCommListenerPtr &caller);
	~AosCubeComm();

	virtual bool	connect(AosRundata *rdata) = 0;
	virtual bool	close() = 0;

	virtual bool	startReading(AosRundata *rdata, const OmnCommListenerPtr &caller) {return true;}
	virtual bool	stopReading() {return true;}
	virtual bool	sendTo(AosRundata *rdata, const AosEndPointInfo &remote_epinfo, AosBuff *buff) = 0;

	// CommListener interface
	virtual bool	msgRead(const OmnConnBuffPtr &connbuff);
	virtual OmnString getCommListenerName() const;
	virtual void	readingFailed();
	virtual void	sendingFailed();

	// OmnThreadedObj interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);

private:
	virtual bool	proc(const OmnConnBuffPtr &data) = 0;

};

#endif

