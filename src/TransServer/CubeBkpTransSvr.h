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
#ifndef Aos_TransServer_CubeBkpTransSvr_h
#define Aos_TransServer_CubeBkpTransSvr_h

#include "TransServer/CubeTransSvr.h"
#include "FmtMgr/BkpFmtSvr.h"

class AosCubeBkpTransSvr: public AosCubeTransSvr
{

private:
	int					mCrtMaster;
	OmnMutexPtr         mSyncLock;
	bool				mSyncFinish;
	AosBkpFmtSvr*		mFmtSvrRaw;

	map<u64, AosSyncInfoPtr> mSyncInfo;
	
public:
	AosCubeBkpTransSvr(const int crt_master);
	~AosCubeBkpTransSvr();
	
	virtual bool start();
	virtual bool switchFrom(const AosCubeTransSvrPtr &from);
	
	virtual bool setNewMaster(const int new_master);
	virtual int  getCrtMaster(){ return mCrtMaster; };

private:
	virtual bool recvTrans(const AosTransPtr &trans);

	bool 	storageDiskCheck();
	bool 	cleanDfmLog();
	bool 	syncData();
	
	bool 	syncTrans();
	bool 	initSyncBeg();
	int 	getNextSyncSvr(const int crt_sid);
	bool 	syncTransFromSvr(
				const AosRundataPtr &rdata,
				const int sync_sid,
				bool &succ);
	bool 	recvTransBySync(const AosBuffPtr &cont);
	bool 	checkTransCanSync(const AosTransId &trans_id, bool &can_sync);
	AosSyncInfoPtr getSyncInfo(const u64 clt_key);

	bool 	mergeNewerFiles();

	bool 	isRecvedBySync(const AosTransPtr &trans);
	bool 	recvNewerTrans(const AosTransPtr &trans);

};
#endif
