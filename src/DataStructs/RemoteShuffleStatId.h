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
// 09/26/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataStructs_RemoteShuffleStatId_h
#define AOS_DataStructs_RemoteShuffleStatId_h

#include "DataStructs/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"

class AosRemoteShuffleStatId : virtual public OmnRCObject 
{
	OmnDefineRCObject;

private:
	int					mDocsPerDistBlock;
	u64 				mCtrlDocid;
	int64_t				mCrtIdx;
	u64					mCrtDocid;
	map<int, u64>       mSnapIds;	

public:
	AosRemoteShuffleStatId(
			const AosXmlTagPtr &control_doc);

	~AosRemoteShuffleStatId();

	int getPhysicalIdByStatId(
			const AosRundataPtr &rdata,
			const u64 &stat_id);

	u32 getVirtualIdByStatId(
			const AosRundataPtr &rdata,
			const u64 &stat_id);

	void setSnapShots(map<int, u64> &bsnap_ids);

private:
	u64  getDocid(
			const AosRundataPtr &rdata,
			const u64 &stat_id,
			int &vid);

	u64  getStatBinaryDocidPriv(
			const AosRundataPtr &rdata, 
			const u64 &stat_id);

	bool addStatBinaryDocidPriv(
			const AosRundataPtr &rdata, 
			const u64 &stat_id, 
			u64 &binary_docid);

	u64  getDocidPriv(
			const AosRundataPtr &rdata,
			const int vid,
			const u64 &stat_id);
};
#endif
