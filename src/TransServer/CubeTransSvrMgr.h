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
#ifndef Aos_TransServer_CubeTransSvrMgr_h
#define Aos_TransServer_CubeTransSvrMgr_h

#include "SEInterfaces/CubeTransSvrObj.h"
#include "TransServer/Ptrs.h"

class AosCubeTransSvrMgr: public AosCubeTransSvrObj
{
	OmnDefineRCObject;
	
private:
	OmnRwLockPtr    	mLock;
	AosCubeTransSvrPtr	mCrtTransSvr;	
	AosRundataPtr		mRdata;
	bool				mShowLog;

public:
	AosCubeTransSvrMgr();
	~AosCubeTransSvrMgr();

	// AosTransSvrObj Internfaces.
	virtual bool	config(const AosXmlTagPtr &conf);
	virtual bool 	start();
	virtual bool 	stop();
	
	virtual bool	recvMsg(const AosAppMsgPtr &msg);
	virtual bool 	finishTrans(vector<AosTransId> &trans_ids);
	virtual bool    finishTrans(const AosTransPtr &trans);
	virtual void    resetCrtCacheSize(const u64 proced_msg_size);

	// AosCubeTransSvrObj Internfaces.
	virtual AosFmtMgrObjPtr getFmtSvr();
	virtual bool 	readTransBySync(
						AosBuffPtr &trans_buff,
						int &read_id,
						bool &finish);
	virtual bool 	switchToMaster();
	//virtual bool 	switchToBkp();
	virtual bool	stopSendFmt();
	virtual bool 	reSwitchToMaster();
	virtual bool 	setNewMaster(const u32 cube_grp_id, const int new_master);
	//virtual int	getCrtMaster();
	virtual bool 	isSwitchToMFinish(bool &finish);
	
private:
	//bool 	triggerOldMasterSwitchToBkp();
	bool	triggerOldMasterStopSendFmt();
	bool 	sendSwitchToMasterFinish();

};
#endif
