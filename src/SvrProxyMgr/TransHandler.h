////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// Created: 04/17/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SvrProxyMgr_TransHandler_h
#define AOS_SvrProxyMgr_TransHandler_h

#include "SvrProxyUtil/Ptrs.h"
#include "SvrProxyMgr/Ptrs.h"
#include "Thread/Ptrs.h"
#include "TransClient/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

#include <set>
#include <map>
#include <queue>
#include <vector>
using namespace std;

class AosTransHandler : public OmnRCObject 
{
	OmnDefineRCObject;

private:
	OmnMutexPtr		mLock;
	AosSvrProxyPtr	mSvrProxy;
	OmnFilePtr		mMasterFile;
	map<u32, int>	mCrtMaster;
	map<u32, AosBuffPtr>	mWaitMasterMap;
	
	bool			mShowLog;

public:
	AosTransHandler(
			const AosSvrProxyPtr &svr_proxy,
			const bool &show_log);
	~AosTransHandler();

	bool 	getMasters(const AosBuffPtr &master_buff);
	bool 	getMastersFromFile(const AosBuffPtr &master_buff);
	bool 	contInitCrtMasters();
	
	bool 	recvMasters(const AosBuffPtr &master_buff);
	bool 	recvMastersFromOtherSvrFile(
				const int from_sid,
				const AosBuffPtr &master_buff,
				bool &collect_finish);

	bool 	setNewMaster(const u32 cube_grp_id, const int svr_id);
	int 	getCrtMaster(const u32 cube_grp_id);
	
	bool 	procTrans(const AosTransPtr &trans);
	
private:
	bool	config(const AosXmlTagPtr &app_conf);
	bool 	saveMasterBuffToFile(const AosBuffPtr master_buff);
	OmnFilePtr getMasterFile();

	bool 	initCrtMasters();
	
	bool 	procCubeTrans(const AosCubicTransPtr &trans);
	bool 	sendTransPriv(const AosTransPtr &trans);

	bool 	pushToWaitMasterMapPriv(const int from_sid, const AosBuffPtr &master_buff);
	bool 	getMastersFromWaitMap(AosBuffPtr &master_buff);

	bool 	serializeMasterBuff(const AosBuffPtr &master_buff);
	
};
#endif
