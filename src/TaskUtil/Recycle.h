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
// 06/18/2013	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TaskUtil_Recycle_h
#define AOS_TaskUtil_Recycle_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/File.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/ThreadedObj.h"
#include <queue>

OmnDefineSingletonClass(AosRecycleSingleton,
						AosRecycle,
						AosRecycleSelf,
						OmnSingletonObjId::eRecycle,
						"Recycle");

class AosRecycle: virtual public OmnThreadedObj 
{
	OmnDefineRCObject;

public:
	struct RInfo
	{
		int physicalid;	
		u64 file_id;

		bool operator <(const RInfo &rhs) const 
		{
			if (physicalid == rhs.physicalid)
			{
				return file_id < rhs.file_id;
			}
			return physicalid < rhs.physicalid;
		}
	};
private:
	struct GReq
	{
		vector<RInfo> grinfo;
		AosRundataPtr rdata;
	};

private:
	OmnMutexPtr         mLock;
	OmnCondVarPtr       mCondVar;
	OmnThreadPtr        mThread;
	queue<GReq> 		mQueue; 

public:
	AosRecycle();
	~AosRecycle();

	// Singleton class interface
    static AosRecycle * getSelf();
    virtual bool   	start();
    virtual bool    stop();
    virtual bool	config(const AosXmlTagPtr &def);

	//ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

private:
	bool 	deleteFile(vector<RInfo> &grinfo, const AosRundataPtr &rdata);

public:
	bool	addRequest(
				vector<RInfo> &grinfo,
				const AosRundataPtr &rdata);

	bool	diskRecycle(
				const int physicalid, 
				const u64 &file_id,
				const AosRundataPtr &rdata);
};

#endif
