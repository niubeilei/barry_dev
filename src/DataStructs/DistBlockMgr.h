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
#ifndef AOS_DataStructs_DistBlockMgr_h
#define AOS_DataStructs_DistBlockMgr_h

#include "DataStructs/DistBlock.h"
#include "DataStructs/Ptrs.h"
#include "Util/TransId.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"

class AosDistBlockMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	AosDistBlockMgr();
	~AosDistBlockMgr();

	virtual bool getStatDoc(
			const AosRundataPtr &rdata, 
			AosDistBlock &dist_block,
			const u64 &docid,
			const bool create,
			char *&stat_doc,
			int64_t &stat_doc_len);

	virtual bool saveDistBlock(
			const AosRundataPtr &rdata, 
			AosDistBlock &dist_block);

	virtual u64  getEntryByStatId(const u64 &stat_id);

	virtual void setSnapShots(
			map<int, u64> &snap_ids, 
			map<int, u64> &binary_snap_ids);

	virtual AosXmlTagPtr createBinaryDoc(
			const AosRundataPtr &rdata,
			const u64 &docid,
			const AosBuffPtr &buff);

	virtual AosXmlTagPtr retrieveBinaryDoc(
			const AosRundataPtr &rdata,
			const u64 &docid, 
			AosBuffPtr &buff);

	virtual bool modifyBinaryDoc(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &doc,
			const AosBuffPtr &buff);

	virtual bool resetStatIdMgr(const AosBuffPtr &buff);
	
	virtual u64 getDocsPerDistBlock();

private:
	bool createOneEmptyDocid();

	bool getDistBlock(
			const AosRundataPtr &rdata, 
			AosDistBlock &dist_block,
			const u64 &docid,
			const bool create);

	int getDistBlockVid(
			const u64 &did,
			const u64 &ext_docid,
			const int docs_per_distblock);

	bool getDistBlockLocal(
			const AosRundataPtr &rdata, 
			AosDistBlock &dist_block,
			const u64 &stat_docid);
	virtual u64 getDocPerDistBlock();

public:
	static AosBuffPtr retrieveBinaryDoc(
			const u64 did,
			const bool is_local,
			const AosRundataPtr &rdata);

private:
	bool sanityCheck(
			const u64 &did,
			const u64 &ext_docid,
			const int docs_per);
public:
	//virtual void resetContents(const AosBuffPtr &buff);
};
#endif
