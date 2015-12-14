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
// 2014/08/11 Created by Ketty
////////////////////////////////////////////////////////////////////////////

#include "StatServer/StatSvr.h"

#include "Thread/Mutex.h"
#include "StatServer/StatCube.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "ReliableFile/ReliableFile.h"
#include "Debug/Debug.h"


AosStatSvr::AosStatSvr()
:
mLock(OmnNew OmnMutex()),
mCrtCachedNum(0)
{
}


AosStatSvr::~AosStatSvr()
{
}


AosStatCubePtr
AosStatSvr::getStatCube(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &stat_cube_conf)
{
	aos_assert_r(stat_cube_conf, 0);

	//yang,2015/09/23
	//mLock->lock();

	OmnString stat_cube_key;
	stat_cube_key << stat_cube_conf->getAttrU64("stat_id", 0); 
	aos_assert_r(stat_cube_key != "0", 0);	
	stat_cube_key << "_" << stat_cube_conf->getAttrU32("cube_id", 0);

	
	AosStatCubePtr stat_cube = findFromCache(stat_cube_key);
	if(stat_cube)
	{
		//mLock->unlock();
		return stat_cube;
	}
	
	stat_cube = createStatCube(rdata, stat_cube_conf);
	aos_assert_rl(stat_cube, mLock, 0);
	
	bool rslt = addToCache(stat_cube_key, stat_cube);
	aos_assert_rl(rslt, mLock, 0);

	//mLock->unlock();
	OmnTagFuncInfo << endl;
	return stat_cube;
}


AosStatCubePtr
AosStatSvr::findFromCache(const OmnString &stat_cube_key)
{
	map<OmnString, ListItr>::iterator itr = mStatMap.find(stat_cube_key);
	if(itr == mStatMap.end())	return 0;
	
	ListItr q_itr = itr->second;
	aos_assert_r(q_itr != mStatCached.end(), 0);
	
	// push to list begin.
	AosStatCubePtr stat_cube = *q_itr;	
	if(q_itr != mStatCached.begin())
	{
		mStatCached.erase(q_itr);
		mStatCached.push_front(stat_cube);
		itr->second = mStatCached.begin();
	}
	OmnTagFuncInfo << endl;
	return stat_cube;
}


AosStatCubePtr
AosStatSvr::createStatCube(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &stat_cube_conf)
{
	AosStatCubePtr stat_cube = OmnNew AosStatCube();
	bool rslt = stat_cube->config(rdata, stat_cube_conf);
	aos_assert_r(rslt, 0);

	return stat_cube;
}


bool
AosStatSvr::addToCache(
		const OmnString &stat_cube_key,
		const AosStatCubePtr &stat_cube)
{
	if(mCrtCachedNum > eMaxCacheNum)
	{
		aos_assert_r(!mStatCached.empty(), false);
		// need delete last one.
		AosStatCubePtr old_stat = mStatCached.back();
		OmnString old_stat_cube_key;
		old_stat_cube_key << old_stat->getStatId();
		old_stat_cube_key << "_" << old_stat->getCubeId();
		
		map<OmnString, ListItr>::iterator itr = mStatMap.find(old_stat_cube_key);
		aos_assert_r(itr != mStatMap.end(), false);
		
		mStatCached.pop_back();
		mStatMap.erase(itr);	
		mCrtCachedNum--;
	}
	
	mStatCached.push_front(stat_cube);
	mStatMap.insert(make_pair(stat_cube_key, mStatCached.begin()));
	mCrtCachedNum++;
	OmnTagFuncInfo << endl;
	return true;
}


bool
AosStatSvr::modify(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &stat_cube_conf,
		const AosBuffPtr &buff,
		AosStatModifyInfo &mdf_info)
{

	//yang,2015/09/23
	mLock->lock();
	AosStatCubePtr stat_cube = getStatCube(rdata, stat_cube_conf);

	aos_assert_r(stat_cube, 0);

	OmnTagFuncInfo << endl;


	bool rslt = stat_cube->modify(rdata, buff, mdf_info);

	mLock->unlock();

	return rslt;
}

u64
AosStatSvr::createStatMetaFile(
		const AosRundataPtr &rdata,
		const u32 cube_id,
		const OmnString &file_prefix)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, 0);

	AosReliableFilePtr file = vfsMgr->createRlbFile(rdata.getPtr(), cube_id,
			file_prefix, 0, AosRlbFileType::eNormal, false);
	aos_assert_r(file, 0);	
	
	OmnTagFuncInfo << endl;
	return file->getFileId();
}

//===========================================
AosBuffPtr
AosStatSvr::retrieve(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &stat_cube_conf,
		vector<u64> &qry_stat_docids,
		const AosXmlTagPtr &stat_qry_conf)
{
	aos_assert_r(stat_cube_conf && stat_qry_conf, 0);
	
	mLock->lock();
	AosStatCubePtr stat_cube = getStatCube(rdata, stat_cube_conf);
	//mLock->unlock();
	
	aos_assert_r(stat_cube, 0);


	AosStatQueryInfo qry_info;
	bool rslt = qry_info.config(stat_qry_conf);
	aos_assert_r(rslt, 0);

	//arvin 2015.08.13
	//JIMODB-335
	for(size_t i = 0 ;i < qry_info.mQryTimeConds.size();i++)
	{
		AosStatTimeArea time_area = qry_info.mQryTimeConds[i];
		// JIMODB-599 Rain 2015-08-25
		if(time_area.start_time > time_area.end_time && time_area.start_time != -1 && time_area.end_time != -1)
		{
			mLock->unlock();
			return 0;
		}
	}

	OmnTagFuncInfo << endl;
	
	//yang,2015/09/23
	AosBuffPtr buff = stat_cube->retrieve(rdata, qry_stat_docids, qry_info);

	mLock->unlock();

	return buff;
}


