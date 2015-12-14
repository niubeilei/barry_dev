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
#ifndef AOS_DataStructs_RemoteDistBlockMgr_h
#define AOS_DataStructs_RemoteDistBlockMgr_h

#include "DataStructs/Ptrs.h"
#include "DataStructs/DistBlock.h"
#include "DataStructs/DistBlockMgr.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"

class AosRemoteDistBlockMgr : virtual public AosDistBlockMgr 
{
private:
	u64						mDocsPerDistBlock;
	AosStatIdMgrPtr			mStatIdMgr;

	map<int, u64>			mSnapIds;
	map<int, u64>			mBinarySnapIds;
	bool					mUseSnapShot;

public:
	AosRemoteDistBlockMgr(
			const AosXmlTagPtr &control_doc,
			const AosBuffPtr &buff,
			const bool use_snapshot);

	~AosRemoteDistBlockMgr();

	bool getStatDoc(
			const AosRundataPtr &rdata, 
			AosDistBlock &dist_block,
			const u64 &docid,
			const bool create,
			char *&stat_doc,
			int64_t &stat_doc_len);

	bool saveDistBlock(
			const AosRundataPtr &rdata, 
			AosDistBlock &dist_block);
	
	u64 getDocsPerDistBlock(){return mDocsPerDistBlock;}
	
	AosXmlTagPtr createBinaryDoc(
			const AosRundataPtr &rdata,
			const u64 &docid,
			const AosBuffPtr &buff);

	AosXmlTagPtr  retrieveBinaryDoc(
			const AosRundataPtr &rdata,
			const u64 &docid, 
			AosBuffPtr &buff);

	bool modifyBinaryDoc(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &doc,
			const AosBuffPtr &buff);

	void setSnapShots(
			map<int, u64> &snap_ids, 
			map<int, u64> &binary_snap_ids);
	
	virtual bool resetStatIdMgr(const AosBuffPtr &buff);
	
	virtual u64  getEntryByStatId(const u64 &stat_id);

private:
	bool getDistBlock(
			const AosRundataPtr &rdata, 
			AosDistBlock &dist_block,
			const u64 &docid,
			const bool create);

	AosXmlTagPtr createBinaryDocPriv(
			const AosRundataPtr &rdata,
			const OmnString &docstr,
			const AosBuffPtr &buff,
			const int &vid);

	void getSnapShotId(
			const int vid,
			u64 &snap_id,
			u64 &bsnap_id);

	AosXmlTagPtr getDocByDocid(
			const AosRundataPtr &rdata,
			const u64 &docid);

	bool sanityCheck(
			const u64 &did,
			const u64 &crt_idx,
			const int docs_per);
};
#endif
