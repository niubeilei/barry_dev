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
// 06/03/2011: Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_TransServer_CubeMasterTransSvr_h
#define Aos_TransServer_CubeMasterTransSvr_h

#include "TransServer/CubeTransSvr.h"
#include "FmtMgr/MasterFmtSvr.h"

class AosCubeMasterTransSvr: public AosCubeTransSvr
{
private:
	bool				mStartFinish;
	bool				mMergeFinish;
	OmnMutexPtr         mMergeLock;
	OmnMutexPtr         mLock;
	OmnCondVarPtr       mCondVar;
	
	map<u64, u64>		mFirstRecvRespSeq;
	AosMasterFmtSvr*	mFmtSvrRaw;
	
	deque<AosTransPtr> mQueueTest;
	
	//map<u64, AosTransPtr>	mResendedTrans;

public:
	AosCubeMasterTransSvr();
	~AosCubeMasterTransSvr();

	virtual bool start();
	virtual bool switchFrom(const AosCubeTransSvrPtr &from);
	//virtual bool reSwitch();
	
	virtual bool stopSendFmt();
	virtual int getCrtMaster();

private:
	virtual bool recvTrans(const AosTransPtr &trans);
	virtual bool contPushTransToQueue();
	
	bool	recoverTrans();
	bool	recoverPriv(int &read_id);
	//bool 	procRecoveredTransBuffs(const AosBuffPtr &cont);
	bool 	procRecoveredTrans(vector<AosTransPtr> &v_trans);
	
	bool 	setFirstRecvTrans(const AosTransPtr &trans);
	bool 	procResendTrans(const AosTransPtr &trans);

	bool 	recvNewerTrans(const AosTransPtr &trans);

	bool 	createTrans();
	bool 	recoverTest1();
	bool 	recoverTest2();
	bool 	recoverTest3();
	bool 	recoverTest4();
	bool 	recoverTest5();
	bool 	recoverTest6();
	bool 	recoverTesttt(deque<AosTransPtr> &my_que);
	void 	recoverTestPriv(
				const AosAppMsgPtr &msg,
				deque<AosAppMsgPtr> &my_que);

};
#endif
