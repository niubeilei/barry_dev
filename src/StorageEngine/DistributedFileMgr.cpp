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
// 07/04/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "StorageEngine/DistributedFileMgr.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "XmlUtil/XmlTag.h"


OmnSingletonImpl(AosDistributedFileMgrSingleton,
		         AosDistributedFileMgr,
		         AosDistributedFileMgrSelf,
		        "AosDistributedFileMgr");


AosDistributedFileMgr::AosDistributedFileMgr()
{
}


AosDistributedFileMgr::~AosDistributedFileMgr()
{
}


bool 
AosDistributedFileMgr::config(const AosXmlTagPtr &conf)
{
	return true;
}


bool 
AosDistributedFileMgr::start()
{
	return true;
}


bool 
AosDistributedFileMgr::stop()
{
	return true;
}


bool 
AosDistributedFileMgr::readFile(
		const OmnString &objid, 
		const int64_t &segment_id,
		AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	OmnString fname = constructFname(objid, segment_id);
	u32 dist_id = AosGetHashKey(fname.data(), fname.length());
	if (AosIsVirtualIdLocal(dist_id))
	{
		return readLocalFile(objid, dist_id, segment_id, buff, rdata);
	}

	return readRemoteFile(objid, dist_id, segment_id, buff, rdata);
}


bool
AosDistributedFileMgr::readLocalFile(
		const OmnString &objid, 
		const u32 dist_id,
		const int64_t &segment_id,
		AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	/*
	// Files are saved based on virtuals.
	int virtual_id = AosGetVirtualIdByDistrId(dist_id);

	AosXmlTagPtr createFile(const OmnString &filename,
				const bool is_fixed_length,
				const int min_segment_size,
				const int max_segment_size, 
				const AosRundataPtr &rdata);
	*/
	OmnNotImplementedYet;
	return false;
}

