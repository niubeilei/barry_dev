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
// 05/19/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CounterServer_PhyCtnrSvr_h
#define Aos_CounterServer_PhyCtnrSvr_h

#include "Proggie/ReqDistr/NetReqProc.h"
#include "Proggie/ProggieUtil/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/ThreadedObj.h"
#include "Thread/CondVar.h"
#include "TransUtil/Ptrs.h"
#include "UtilComm/Ptrs.h"
#include "Util1/Ptrs.h"
#include "Rundata/Rundata.h"
#include "CounterUtil/Ptrs.h"
#include "CounterServer/Ptrs.h"
#include "CounterUtil/CounterTrans.h"
#include "SEInterfaces/CounterSvrObj.h"
#include "TransServer/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlInterface/WebRequest.h"
#include "XmlInterface/Ptrs.h"
#include <queue>


class AosPhyCtnrSvr : public AosCounterSvrObj 
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxVirSvrNum = 100
	};
	OmnString 			mClientSsid;
	OmnString 			mTransId;
	OmnString 			mOperation;
	static AosXmlTagPtr mVirConfig;
	static int			mNumVirtuals;

private:
	class procThrd : public OmnThrdShellProc
	{
		OmnDefineRCObject;
	private:
		AosVirCtnrSvrPtr 		mVserver;
		AosCounterTransPtr		mTrans;
		AosRundataPtr			mRdata;

	public:
		//procThrd(const AosVirCtnrSvrPtr &vserver, 
		//		const AosBuffTransPtr &trans, 
		//		const AosRundataPtr &rdata)
		procThrd(const AosVirCtnrSvrPtr &vserver, 
				const AosCounterTransPtr &trans,
				const AosRundataPtr &rdata)
		:
		OmnThrdShellProc("phyctnrsvr"),
		mVserver(vserver)
		{
			mTrans = trans;
			mRdata = rdata;
			//mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
		}

		~procThrd(){};

		virtual bool    run();
		virtual bool    procFinished();
		virtual bool    waitUntilFinished(){return true;}
	};

public:
	AosPhyCtnrSvr();
	//AosPhyCtnrSvr(const AosXmlTagPtr &theconfig);
	~AosPhyCtnrSvr();


	bool	config(const AosXmlTagPtr &config);
	bool 	proc(const u32 vid,
				const AosBuffPtr &content,
				const AosRundataPtr &rdata);

	static  int 	getNumVirtuals() {return mNumVirtuals;};
	static  AosVirCtnrSvrPtr getVirtualServer(const u32 id);
	static  bool	resetVirtualServers();
	static  AosVirCtnrSvrPtr getVirtualServer(const OmnString &cname);

	virtual bool	addTrans(const AosCounterTransPtr &trans,
				const AosRundataPtr &rdata);

	bool 	stop();
	
	bool 	start();
};
#endif
