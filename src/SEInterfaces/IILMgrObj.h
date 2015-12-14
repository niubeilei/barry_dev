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
// 07/24/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_IILMgrObj_h
#define Aos_SEInterfaces_IILMgrObj_h

#include "IILUtil/IILUtil.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/IDTransMapObj.h"
#include "SEInterfaces/IILObj.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"

class AosIILIdx;

class AosIILMgrObj : virtual public OmnRCObject
{
protected:
	static AosIILMgrObjPtr		smIILMgr;

public:
	AosIILMgrObj();
	virtual ~AosIILMgrObj();
	
	virtual bool 	StrBatchAddMergeSafe(
						const OmnString &iilname,
						const int64_t &size,
						const AosIILExecutorObjPtr &executor,
						const bool true_delete,
						const AosRundataPtr &rdata) = 0;

	virtual bool	StrBatchIncMergeSafe(
						const OmnString &iilname,
						const int64_t &size,
						const u64 &dftvalue, 
						const AosIILUtil::AosIILIncType incType,
						const bool true_delete,
						const AosRundataPtr &rdata) = 0;

	// Chen Ding, 12/10/2012
	virtual u64		getIILID(
						const OmnString &word,
						const bool create,
						const AosRundataPtr &rdata) = 0;

	virtual bool	querySafeByIIL(
						const AosIILObjPtr &iil,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context, 
						const AosRundataPtr &rdata) = 0;

	virtual bool	rebuildBitmap(
						const AosIILObjPtr &iil,
						const AosRundataPtr &rdata) = 0;

	virtual bool	checkIsIILIDUsed(
						const u64 &iilid, 
						const u32 siteid,
						const AosRundataPtr &rdata) = 0;
	
	virtual bool	addTrans(
						const u64 &iilid,
						const u32 siteid,
						const AosIILTransPtr &trans,
						const AosRundataPtr &rdata) = 0;
	
	virtual bool	procTrans(
						const u64 &id,
						const u32 siteid,
						const AosIDTransVectorPtr &p,
						const AosRundataPtr &rdata) = 0;

	virtual bool 	svrIsUp(
						const AosRundataPtr &rdata, 
						const u32 svr_id) = 0;

	virtual AosIILObjPtr firstLeaf(
						const AosRundataPtr &rdata, 
						const OmnString &iilname, 
						AosIILIdx &idx,
						AosBitmapTreeObjPtr &tree) = 0;

	virtual AosIILObjPtr nextLeaf(
						const AosRundataPtr &rdata, 
						const OmnString &iilname, 
						AosIILIdx &idx,
						AosBitmapTreeObjPtr &tree) = 0;

	virtual AosIILObjPtr firstLeaf(
						const AosRundataPtr &rdata, 
						const OmnString &iilname, 
						AosIILIdx &idx) = 0;

	virtual AosIILObjPtr nextLeaf(
						const AosRundataPtr &rdata, 
						const OmnString &iilname, 
						AosIILIdx &idx) = 0;

	virtual bool mergeSnapshot(
						const u32 virtual_id,
						const u64 &target_snap_id,
						const u64 &merge_snap_id,
						const AosTransId &trans_id,
						const AosRundataPtr &rdata) = 0;

	virtual u64 createSnapshot(
						const u32 virtual_id,
						const u64 snap_id,
						const AosTransId &trans_id,
						const AosRundataPtr &rdata) = 0;

	virtual bool commitSnapshot(
						const u32 virtual_id,
						const u64 &snap_id,
						const AosTransId &trans_id,
						const AosRundataPtr &rdata) = 0;

	virtual bool rollbackSnapshot(
						const u32 virtual_id,
						const u64 &snap_id,
						const AosTransId &trans_id,
						const AosRundataPtr &rdata) = 0;

	virtual u64	getDocidByObjid(
						const OmnString &objid,
						const AosRundataPtr &rdata) = 0;

	virtual bool unbindObjid(
						const OmnString &objid,
						const u64 &docid,
						const AosRundataPtr &rdata) = 0;

	// Add by Young, 2015/09/21
	virtual AosIILObjPtr getIILPublic(
						const u64 &iilid, 
						const u32 siteid,
						const u64 snap_id, 
						const AosIILType iiltype,
						const AosRundataPtr &rdata) = 0;

	//Add By Yazong Ma
	virtual AosIILObjPtr loadIILPublic(
						const u64 &iilid,
						const u32 siteid,
						const u64 &snap_id,
						AosIILType &iiltype,
						const AosRundataPtr &rdata) = 0; 

	virtual AosIILObjPtr createIILPublic(
						const u64 &iilid, 
						const u32 siteid, 
						const u64 &snap_id,
						const AosIILType type, 
						const bool isPersis, 
						const AosRundataPtr &rdata) = 0;

	virtual bool returnIILPublic(
						const AosIILObjPtr &iil, 
						const AosRundataPtr &rdata) = 0;

	virtual bool parseIILID(
						const u64 &iilid,
						u64 &localid,
						int &vid,
						const AosRundataPtr &rdata) = 0;

	static void setIILMgr(const AosIILMgrObjPtr &iilmgr) {smIILMgr = iilmgr;}
	static AosIILMgrObjPtr getIILMgr() {return smIILMgr;}

	virtual OmnString getMemoryStatus() = 0;
};

#endif

