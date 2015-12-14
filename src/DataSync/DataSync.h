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
// 2015/03/13 Created by Ketty Guo 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataSync_DataSync_h
#define AOS_DataSync_DataSync_h

#include "SEInterfaces/DataSyncObj.h"
#include "Util/String.h"

#include <vector>
using namespace std;


class AosDataSync: virtual public AosDataSyncObj
{
	OmnDefineRCObject;

	enum
	{
		eNormalExitOff = 0,		// 4 byte.

	};

private:
	bool	mInited;
	u32		mCubeId;
	int		mSelfSvrId;
	AosClusterObj *mSelfCluster;
	OmnString	mSysInitFname;
	
	bool	mNeedSyncData;

public:
	AosDataSync();
	~AosDataSync();

	virtual bool init(const AosRundataPtr &rdata);
	virtual bool start(const AosRundataPtr &rdata);
	virtual bool stop(const AosRundataPtr &rdata);

private:
	bool 	checkNeedSyncData(const AosRundataPtr &rdata);

	bool 	normalStart(const AosRundataPtr &rdata);

	bool 	syncDataBase(
				const AosRundataPtr &rdata,
				vector<int> &bkp_cube_sids);

	bool 	syncWithOneBkp(
				const AosRundataPtr &rdata,
				vector<int> &bkp_cube_sids,
				bool &sync_succ);

	bool 	syncOneDFMWithOneBkp(
				const AosRundataPtr &rdata,
				const AosDocFileMgrObjPtr &dfm,
				vector<int> &bkp_cube_sids,
				bool &sync_succ);

	bool 	syncWithEachBkp(
				const AosRundataPtr &rdata,
				vector<int> &bkp_cube_sids,
				bool &sync_succ);

	bool 	syncOneDFMWithEachBkp(
				const AosRundataPtr &rdata,
				const AosDocFileMgrObjPtr &dfm,
				vector<int> &bkp_cube_sids,
				bool &sync_succ);

	bool 	syncDataPriv(
				const AosRundataPtr &rdata, 
				const AosDocFileMgrObjPtr dfm,
				const int bkp_cube_svrid,
				bool &sync_succ,
				AosCubeStatus::E &bkp_cube_sts);

	bool 	getBkpCubeSids(
				const AosRundataPtr &rdata,
				vector<int> &bkp_cube_sids);

	bool 	hasBkpCube(const AosRundataPtr &rdata);


};

#endif
