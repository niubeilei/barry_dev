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
#ifndef AOS_DataStructs_LocalDistBlockMgr_h
#define AOS_DataStructs_LocalDistBlockMgr_h

#include "DataStructs/DistBlockMgr.h"
#include "DataStructs/DistBlock.h"
#include "DataStructs/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"

class AosLocalDistBlockMgr : virtual public AosDistBlockMgr
{

	AosStatIdMgrPtr		mStatIdMgr;
public:
	AosLocalDistBlockMgr(
			const AosXmlTagPtr &control_doc, 
			const AosBuffPtr &buff);

	~AosLocalDistBlockMgr();

	static bool getStatDocByDocid(
			const AosRundataPtr &rdata, 
			AosDistBlock &dist_block,
			const u64 &dist_block_docid,
			const u64 &stat_docid,
			char *&stat_doc,
			int64_t &stat_doc_len);

	u64  getEntryByStatId(const u64 &stat_id);
	
	static void KettyInitPrintTime();
	static void KettyPrintTime();

	static AosBuffPtr retrieveBinaryDoc(
			const u64 &did,
			const AosRundataPtr &rdata);

private:
	static bool getDistBlockByDocid(
			const AosRundataPtr &rdata, 
			AosDistBlock &dist_block,
			const u64 &dist_block_docid,
			const u64 &stat_docid);
};
#endif
