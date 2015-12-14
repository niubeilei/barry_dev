///////////////////////////////////////////////////////////////////////////
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
// 2014/01/22 Created by Ketty
// 
////////////////////////////////////////////////////////////////////////////

#ifndef StatServer_StatServer_h
#define StatServer_StatServer_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "SEInterfaces/StatSvrObj.h"
#include "Thread/Ptrs.h"
#include "StatServer/Ptrs.h"

#include <vector>
#include <map>
using namespace std;

class AosStatSvr: public AosStatSvrObj
{
	OmnDefineRCObject;
	
	enum
	{
		eMaxCacheNum = 20,
	};
	
	typedef list<AosStatCubePtr>::iterator ListItr;

private:
	OmnMutexPtr					mLock;
	u32							mCrtCachedNum;
	list<AosStatCubePtr>		mStatCached;
	map<OmnString, ListItr>	mStatMap;

public:
	AosStatSvr();
	~AosStatSvr();

	virtual bool modify(
				const AosRundataPtr &rdata, 
				const AosXmlTagPtr &stat_cube_conf,
				const AosBuffPtr &buff,
				AosStatModifyInfo &mdf_info);
	
	virtual u64 createStatMetaFile(
				const AosRundataPtr &rdata,
				const u32 cube_id,
				const OmnString &file_prefix);

	virtual AosBuffPtr retrieve(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &stat_cube_conf,
				vector<u64> &qry_stat_docids,
				const AosXmlTagPtr &stat_qry_conf);

private:
	AosStatCubePtr getStatCube(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &stat_cube_conf);

	AosStatCubePtr findFromCache(const OmnString &stat_cube_key);

	AosStatCubePtr createStatCube(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &stat_cube_conf);

	bool 	addToCache(
				const OmnString &stat_cube_key,
				const AosStatCubePtr &stat_cube);

};

#endif
