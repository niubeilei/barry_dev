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
#ifndef AOS_CloudCube_CubeMgr_h
#define AOS_CloudCube_CubeMgr_h

#include "CloudCube/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "TransServer/TransProc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include <queue>
using namespace std;

OmnDefineSingletonClass(AosCubeMgrSingleton,
						AosCubeMgr,
						AosCubeMgrSelf,
						OmnSingletonObjId::eCubeMgr,
						"CubeMgr");

class AosCubeMgr : public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	enum
	{
		eDftBuffLen = 10000
	};

	AosCubeMsgRouterPtr	mCubeMsgRouter;
	AosCubeMapPtr		mCubeMap;

public:
	AosCubeMgr();
	~AosCubeMgr();

    // Singleton class interface
    static AosCubeMgr *    	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	// OmnThreadedObj interface
	virtual bool threadFunc(OmnThrdStatus::E &, const OmnThreadPtr &);
	virtual bool signal(const int threadLogicId);
	virtual bool checkThread(OmnString &errmsg, const int tid) const;

	virtual int cubeId2Physicalid(const u32 cubeid);

	AosTrans1Ptr sendMsgByCubeId(
						const AosRundataPtr &rdata,
						const u32 cubeid,
						const OmnString &docstr,
						const AosCubeCallerPtr &caller = 0);
	AosTrans1Ptr sendMsgByPhysicalId(
						const AosRundataPtr &rdata,
						const int physical_id,
						const OmnString &docstr,
						const AosCubeCallerPtr &caller = 0);
	AosTrans1Ptr sendMsgByCubeId(
						const AosRundataPtr &rdata,
						const u32 cubeid,
						const AosBuffMsgPtr &msg,
						const AosCubeCallerPtr &caller = 0);
	AosTrans1Ptr sendMsgByPhysicalId(
						const AosRundataPtr &rdata,
						const int physical_id,
						const AosBuffMsgPtr &msg,
						const AosCubeCallerPtr &caller = 0);

	bool registerMsgProc(const AosCubeMsgProcPtr &proc);

private:
};

AosCubeMgr * AosGetCubeMgr() {return AosCubeMgr::getSelf();}
#endif

