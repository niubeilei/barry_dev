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
// 2011/07/15: Created by Ken Lee
////////////////////////////////////////////////////////////////////////////

// Ketty 2013/03/22
/*
#ifndef Aos_IILTransServer_IILTransServer_h
#define Aos_IILTransServer_IILTransServer_h 

#include "Rundata/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "TransServer/TransProc.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"

class AosIILTransServer : public AosTransProc,
						public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	OmnThreadPtr    mProcThread;        // Ketty 2012/11/21
	OmnMutexPtr     mLock;
	OmnCondVarPtr   mCondVar;                              
	list<AosIILTransPtr>    mProcQueue;

public:

	AosIILTransServer();
	~AosIILTransServer();

	bool			start();

	static bool		config(const AosXmlTagPtr &config);
	virtual bool 	proc(const AosIILTransPtr &trans, const AosRundataPtr &rdata);
	virtual bool 	proc(const AosBigTransPtr &trans,
						const AosBuffPtr &content,	
						const AosRundataPtr &rdata);

	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const{ return true;};
	virtual bool    signal(const int threadLogicId){ return true; };

private:
	//bool			start(const AosXmlTagPtr &config);
	void			addErrorResp(const AosRundataPtr &rdata);
	
	bool        procPriv(const AosIILTransPtr &iil_trans, const AosRundataPtr &rdata);
	void        sendResp( const AosRundataPtr &rdata, const AosIILTransPtr &trans);
};
#endif

*/

