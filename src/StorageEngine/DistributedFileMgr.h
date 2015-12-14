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
#ifndef Aos_StorageEngine_DistributedFileMgr_h
#define Aos_StorageEngine_DistributedFileMgr_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/DistributedFileMgrObj.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "StorageEngine/Ptrs.h"
#include "Util/HashUtil.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"


OmnDefineSingletonClass(AosDistributedFileMgrSingleton,
		                AosDistributedFileMgr,
						AosDistributedFileMgrSelf,
						OmnSingletonObjId::eDistributedFileMgr,
						"DistributedFileMgr");


class AosDistributedFileMgr : public AosDistributedFileMgrObj
{
	OmnDefineRCObject;
	
public:
	AosDistributedFileMgr();
	~AosDistributedFileMgr();
	
	// Singleton class interface
	static AosDistributedFileMgr*	getSelf();
	virtual bool            start();
	virtual bool            stop();
	virtual bool            config(const AosXmlTagPtr &def);
	
	bool readFile(const OmnString &objid, 
				const int64_t &segment_id,
				AosBuffPtr &buff, 
				const AosRundataPtr &rdata);

	AosXmlTagPtr createFile(const OmnString &filename,
				const bool is_fixed_length,
				const int min_segment_size,
				const int max_segment_size, 
				const AosRundataPtr &rdata);

	bool readLocalFile(
				const OmnString &objid,
				const u32 dist_id,
				const int64_t &segment_id,
				AosBuffPtr &buff,
				const AosRundataPtr &rdata);

	bool readRemoteFile(
				const OmnString &objid,
				const u32 dist_id,
				const int64_t &segment_id,
				AosBuffPtr &buff,
				const AosRundataPtr &rdata);

	OmnString constructFname(const OmnString &objid, const int64_t &segment_id);
private:
};
#endif

