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
// 2014/11/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoCallServer_JimoCallServer_h
#define Aos_JimoCallServer_JimoCallServer_h

#include "CubeComm/CubeComm.h"
#include "ErrorMgr/ErrmsgId.h"
#include "JimoCall/Ptrs.h"
#include "JimoCallServer/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/Buff.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/HashUtil.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/CommListener.h"

#include <queue>
using namespace std;


class OmnMutex;
class OmnCondVar;
class AosJimoCall;
class AosJimoCallPackage;

class AosJimoCallServer : public OmnCommListener,
						  public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	typedef hash_map<const OmnString, AosJimoCallPackagePtr, Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, AosJimoCallPackagePtr, Omn_Str_hash, compare_str>::iterator itr_t;
	enum
	{
		eDftMaxQueueSize = 10000
	};

	AosEndPointInfo			mEndPointInfo;
	OmnString				mConnType;
	AosCubeCommPtr			mCubeComm;
	AosCubeComm *	        mCubeCommRaw;
	OmnMutexPtr				mLock;
	OmnMutex *		        mLockRaw;
	OmnCondVarPtr	        mCondVar;
	OmnCondVar *	        mCondVarRaw;

	u32				        mMaxQueueSize;
	map_t			        mPackages;
	queue<AosJimoCallPtr>	mQueue;
	vector<OmnThreadPtr>	mThreads;

public:
	AosJimoCallServer(AosRundata *rdata);
	~AosJimoCallServer();

	// OmnCommListener Interface
	virtual bool		msgRead(const OmnConnBuffPtr &buff);
	virtual OmnString	getCommListenerName() const;
	virtual void 		readingFailed();

	// OmnThreadedObj interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);


	bool			JimoCallBack(
						AosRundata *rdata, 
						AosJimoCall &jimo_call,
						AosBuff *buff);

	//getters and setters
	u32 getEndpointId() { return mEndPointInfo.getEndpointId(); } 

	//set and get the singleton
	static bool setSelf(AosJimoCallServer *server);
	static AosJimoCallServer *getSelf();

private:
	bool			config(AosRundata *rdata);
	//bool			proc(const OmnConnBuffPtr &data);
	bool			proc(const AosJimoCallPtr &jimo_call);
	AosJimoCallPackage * getJimoCallPackage(
						AosRundata *rdata,
						const AosJimoCallPtr &jimo_call);
	bool			sendJimoCallFailed(AosRundata *rdata, const AosErrmsgId::E errid);
	OmnString		composeJimoCallPackageName(const int package_id);
};

#endif
