////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2013/03/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_CloudCube_CubeMsgRouter_h
#define AOS_CloudCube_CubeMsgRouter_h

#include "CloudCube/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "TransClient/Ptrs.h"
#include "TransClient/TransModuleClt.h"
#include "TransUtil/Ptrs.h"
#include "TransServer/TransProc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include <queue>
using namespace std;


class AosCubeMsgRouter : public AosTransProc, public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	typedef hash_map<const u64, AosCubeCallerPtr, u64_hash, u64_cmp> cmap_t;
	typedef hash_map<const u64, AosCubeCallerPtr, u64_hash, u64_cmp>::iterator cmapitr_t;

	OmnMutexPtr				mLock;
	OmnCondVarPtr			mCondVar;
	queue<AosTrans1Ptr>		mTransQueue;
	cmap_t					mCallers;
	AosCubeMsgProcPtr		mMsgProcs[OmnMsgId::eLastValidEntry];
	AosTransModuleCltPtr	mTransClient;
	AosTransModuleSvrPtr	mTransServer;

	static int			smPhysicalId;

public:
	AosCubeMsgRouter();
	~AosCubeMsgRouter();

    bool start();
    bool stop();
    bool config(const AosXmlTagPtr &def);

	// OmnThreadedObj interface
	virtual bool threadFunc(OmnThrdStatus::E &, const OmnThreadPtr &);
	virtual bool signal(const int threadLogicId);
	virtual bool checkThread(OmnString &errmsg, const int tid) const;
	
	// TransProc Interface
	virtual bool proc( 	const AosXmlTransPtr &trans,
						const AosXmlTagPtr &trans_doc,
						const AosRundataPtr &rdata);
	virtual bool proc(const AosTinyTransPtr &trans, const AosRundataPtr &rdata);
	virtual bool proc(const AosBuffTransPtr &trans, const AosRundataPtr &rdata);

	AosTrans1Ptr sendMsg(
						const AosRundataPtr &rdata,
						const u32 cube_id,
						const OmnString &docstr,
						const AosCubeCallerPtr &caller);
	AosTrans1Ptr sendMsg(
						const AosRundataPtr &rdata,
						const u32 cube_id,
						const AosBuffPtr &msg,
						const AosCubeCallerPtr &caller);
	bool registerMsgProc(const AosCubeMsgProcPtr &proc);

private:
	bool addCaller(
			const AosRundataPtr &rdata, 
			const AosTrans1Ptr &trans, 
			const AosCubeCallerPtr &caller);
	bool procOneTrans(const AosTrans1Ptr &trans);
	bool procXmlTrans(const AosXmlTransPtr &trans);
	bool procBuffTrans(const AosBuffTransPtr &trans);
	bool procTinyTrans(const AosTinyTransPtr &trans);
	bool logError(
			const AosRundataPtr &rdata, 
			const OmnString &errmsg, 
			const char *filename, 
			const int line);
};
#endif

