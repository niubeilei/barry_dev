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
#ifndef Aos_TransServer_CubeTransSvr_h
#define Aos_TransServer_CubeTransSvr_h

#include "TransServer/TransServer.h"
#include "FmtMgr/Ptrs.h"

class AosCubeTransSvr: public AosTransServer
{
	OmnDefineRCObject;

private:
	bool				mIsMaster;
	
protected:
	AosFmtSvrPtr		mFmtSvr;
	bool				mStartFinish;
	AosRundataPtr		mRdata;

public:
	AosCubeTransSvr(const bool is_master);
	~AosCubeTransSvr();
	
	static AosCubeTransSvrPtr getTransSvrStatic(const AosXmlTagPtr &conf);
	static AosCubeTransSvrPtr switchFromStatic(const AosCubeTransSvrPtr &from);

	// AosTransSvrObj Interfaces.
	virtual bool config(const AosXmlTagPtr &conf);
	virtual bool start() = 0;
	virtual bool recvMsg(const AosAppMsgPtr &msg);

	//	CubeTransSvr Interface.
	virtual bool switchFrom(const AosCubeTransSvrPtr &from);
	//virtual bool reSwitch();
	virtual bool stopSendFmt();
	virtual bool setNewMaster(const int new_master);
	virtual int getCrtMaster() = 0;

	bool 	init();

	bool 	isMaster(){ return mIsMaster; };

	bool	readTransBySync(
				AosBuffPtr &trans_buff,
				int &read_id,
				bool &finish);

	bool	isStartFinished(){ return mStartFinish; }
	
	AosFmtSvrPtr	getFmtSvr();

protected:
	bool 	startVfsMgr();
	bool 	startIpcClt();

private:
	virtual bool recvTrans(const AosTransPtr &trans) = 0;

};
#endif
