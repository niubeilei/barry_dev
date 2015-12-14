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
// In Release 3.1, DocFileMgrNew is implemented through DataletSE. 
// DocFileMgrNew (this class) becomes a wrapper to it. Each 
// DocFileMgrNew is identified by DFMID. DFMID identifies the 
// instance of DataletSE. DatalsetSEs are managed by Raft. 
//
// There can be multiple DocFileMgrNew instances. All share the 
// same Raft. 
//
// This instance runs on DocEngine. It packages requests and sends them
// to Raft on DocStore. When Raft receives the request, it retrieves the
// DFMID and forwards the request to its DataletSE. After that, if it 
// needs to replicate, it will replicate as needed. When the majority 
// has responded, Raft sends a response back to the caller.
//
// Modification History:
// 2015/05/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DocFileMgrNew/DocFileMgrNew.h"


AosDocFileMgrNew::AosDocFileMgrNew(
		AosRundata *rdata,
		const u32 virtual_id,
		const u32 dfm_id,
		const AosDfmConfig &config)
:
mVirtualId(virtual_id),
mDFMID(dfm_id),
mConfig(config)
{
}


AosDocFileMgrNew::~AosDocFileMgrNew()
{
}

	
bool 
AosDocFileMgrNew::start()
{
	// This function starts a DocFileMgrNew
	OmnNotImplementedYet;
	return false;
}


bool 
AosDocFileMgrNew::stop()
{
	// This function stops a DocFileMgrNew
	OmnNotImplementedYet;
	return false;
}


bool 
AosDocFileMgrNew::createDoc(
		AosRundata* rdata, 
		const u64 docid,
		const u64 &snap_id,
		const AosBuff &doc_data)
{
	// This function creates a doc. This function should be called 
	// by DocSvr on DocEngine. 

	// Construct the data block
	AosBuff buff(doc_data->dataLen() + 100);
	buff.appendU8(AosDocFileMgrNewObj::eCreate);
	buff.appendU64(docid);
	buff.appendU64(snap_id);
	buff.appendBuff(doc_data);
	aos_assert_rg(mRaft, rdata, false, AosErrMsg("raft_null"));
	bool 
	if (!(rdata, docid, buff))
	{
		AosLogError(rdata, true, "failed_creating_doc") << enderr;
		return false;
	}

	return true;
}


bool 
AosDocFileMgrNew::modifyDoc(AosRundata* rdata, const AosDfmDocPtr &doc)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosDocFileMgrNew::deleteDoc(AosRundata *rdata, const AosDfmDocPtr &doc)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosDocFileMgrNew::readDoc(AosRundata *rdata, AosBuffPtr &body_data)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosDocFileMgrNew::removeAllFiles(AosRundata *rdata)
{
	OmnNotImplementedYet;
	return false;
}


u64 
AosDocFileMgrNew::createSnapshot(AosRundata *rdata, const u64 snap_id)
{
}


bool 
AosDocFileMgrNew::commitSnapshot(AosRundata *rdata, const u64 snap_id)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosDocFileMgrNew::rollbackSnapshot(AosRundata *rdata, const u64 snap_id)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosDocFileMgrNew::mergeSnapshot(
		AosRundata *rdata, 
		const u64 target_snap_id, 
		const u64 merger_snap_id)
{
	OmnNotImplementedYet;
	return false;
}

#endif
