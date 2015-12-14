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
#ifndef Aos_SEInterfaces_IILObj_h
#define Aos_SEInterfaces_IILObj_h

#include "IILUtil/IILUtil.h"
#include "SEUtil/IILIdx.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEUtil/SeTypes.h"
#include "Util/String.h"
#include "Util/TransId.h"


class AosIILObj : virtual public OmnRCObject
{
public:
	virtual u64			getIILID() const = 0;
	virtual AosIILType	getIILType() const = 0;
	virtual i64			getNumDocs() const = 0;
//	virtual u64 *getDocids() const = 0;
//	virtual char ** getValues() const = 0;

	virtual void		finishTrans() = 0;
	virtual void		addTrans(const AosTransId &id) = 0;
	virtual u32 		getTransNum() = 0;

	virtual bool	computeQueryResults(
						const AosRundataPtr &rdata, 
						const OmnString &iilname, 
						const AosQueryContextObjPtr &context, 
						const AosBitmapObjPtr &bitmap, 
						const u64 &query_id, 
						const int physical_id) = 0;
	virtual bool 	bitmapQueryNewSafe(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata) = 0;
	virtual bool	bitmapRsltQueryNewSafe(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata) = 0;
	virtual bool	deleteIILSafe(
						const bool true_delete,
						const AosRundataPtr &rdata) = 0;
	virtual bool	preQuerySafe(
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata) = 0;
	virtual bool	queryNewSafe(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata) = 0;
	virtual bool	batchAddSafe(
						char * &entries,
						const i64 &size,
						const i64 &num,
						const AosIILExecutorObjPtr &executor,
						const AosRundataPtr &rdata) = 0;
	virtual bool	batchDelSafe(
						char * &entries,
						const i64 &size,
						const i64 &num,
						const AosIILExecutorObjPtr &executor,
						const AosRundataPtr &rdata) = 0;
	virtual bool	batchIncSafe(
						char * &entries,
						const i64 &size,
						const i64 &num,
						const u64 &dftValue,
						const AosIILUtil::AosIILIncType incType,
						const AosRundataPtr &rdata) = 0;

	virtual void	resetSnapShotId() = 0;
	virtual u64		getSnapShotId() = 0;
	virtual void	setSnapShotId(const u64 &snap_id) = 0;
	virtual void	setSnapShotIdPriv(const u64 &snap_id) = 0;

	virtual void 	lockIIL() = 0;
	virtual void	unlockIIL() = 0;
	virtual bool	isDirty() const = 0;
	virtual bool	isGood() const = 0;
	virtual void	setNew(const bool isnew) = 0;
	virtual void	setDirty(const bool dirty) = 0;

	virtual bool	isRootIIL() const = 0;
	virtual bool	isSingleIIL() const = 0;
	virtual bool 	isChildIIL() const = 0;
	virtual bool	isParentIIL() const = 0;
	virtual AosIILObjPtr	getRootIIL() = 0;
	virtual void	setRootIIL(AosIILObjPtr rootiil) = 0;

	virtual u64		getIILIDForHash() = 0;
	virtual u32		getSiteid() const = 0;
	virtual void	setSiteid(const u32 siteid) = 0;
	virtual void	setIILID(const u64 &iilid) = 0;
	virtual void	setPersis(const bool isper) = 0;

	virtual void	setIILIdx(const i64 &idx) = 0;
	virtual i64		getNumSubiils() const = 0;

	virtual u32		addRefCountByIILMgr() = 0;
	virtual u32		removeRefCountByIILMgr() = 0;
	virtual u32		getRefCountByIILMgr() const = 0;
	virtual void	resetPtrsByIILMgr() = 0;
	virtual AosIILObjPtr	getPrevByIILMgr() const  = 0;
	virtual AosIILObjPtr	getNextByIILMgr() const  = 0;
	virtual void	setPrevByIILMgr(const AosIILObjPtr &p) = 0;
	virtual void	setNextByIILMgr(const AosIILObjPtr &p) = 0;

	virtual bool	incNumEntriesNotSafe(const i64 &iilidx) = 0;
	virtual bool	setNumEntriesNotSafe(const i64 &iilidx, const i64 &num) = 0;
	virtual bool	decNumEntriesNotSafe(const i64 &iilidx) = 0;
	virtual bool	decNumEntriesNotSafe(const i64 &iilidx, const i64 &num) = 0;

	virtual bool	loadFromFileSafe(
						const u64 &iilid, 
						const AosRundataPtr &rdata) = 0;
	virtual bool	loadFromFilePriv(
						const u64 &iilid, 
						const AosRundataPtr &rdata) = 0;
	virtual bool	saveToFileSafe(const AosRundataPtr &rdata) = 0;
	virtual bool	saveToFilePriv(const AosRundataPtr &rdata) = 0;
	virtual bool	returnSubIILsSafe(
						bool &returned,
						const bool returnHeader,
						const AosRundataPtr &rdata) = 0;
	virtual void	resetSafe(
						const u64 &wordid, 
						const u64 &iilid,
						const u32 siteid,
						const AosRundataPtr &rdata) = 0;
	virtual void	resetPriv(
						const u64 &wordid, 
						const u64 &iilid,
						const u32 siteid,
						const AosRundataPtr &rdata) = 0;

	virtual AosIILObjPtr	firstLeaf(
						AosIILIdx &idx,
						const AosRundataPtr &rdata) = 0;
	virtual AosIILObjPtr	firstLeaf(
						AosIILIdx &idx,
						AosBitmapTreeObjPtr &tree,
						const AosRundataPtr &rdata) = 0;
	virtual AosIILObjPtr	nextLeaf(
						AosIILIdx &idx,
						const AosRundataPtr &rdata) = 0;
	virtual AosIILObjPtr	nextLeaf(
						AosIILIdx &idx,
						AosBitmapTreeObjPtr &tree,
						const AosRundataPtr &rdata) = 0;

	virtual bool	rebuildBitmapSafe(const AosRundataPtr &rdata) = 0;

	// For Hit IIL
	virtual bool	addDocSafe(const u64 &docid, const AosRundataPtr &rdata) = 0;
	virtual bool	removeDocSafe(const u64 &docid, const AosRundataPtr &rdata) = 0;
	
	// For Str IIL
	virtual bool	nextDocidSafe(
						AosIILIdx the_idx,
						i64 &idx, 
						i64 &iilidx,
						const bool reverse, 
						const AosOpr opr,
						const OmnString &value,
						u64 &docid, 
						bool &isunique,
						const AosRundataPtr &rdata) = 0;
	virtual bool 	addDocSafe(
						const OmnString &value, 
						const u64 &docid, 
						const bool value_unique, 
						const bool docid_unique, 
						const AosRundataPtr &rdata) = 0;
	virtual bool	removeDocSafe(
						const OmnString &value,
						const u64 &docid,
						const AosRundataPtr &rdata) = 0;
	virtual bool	incrementDocidSafe(
						const OmnString &key,
						u64 &value,
						const u64 &incvalue,
						const u64 &init_value,
						const bool add_flag,
						const AosRundataPtr &rdata) = 0; 
	virtual bool	removeFirstValueDocSafe(
						const OmnString &value,
						u64 &docid,
						const bool reverse,
						const AosRundataPtr &rdata) = 0;
	virtual bool 	modifyDocSafe(
						const OmnString &oldvalue, 
						const OmnString &newvalue, 
						const u64 &docid, 
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata) = 0;
	virtual bool	setValueDocUniqueSafe(
						const OmnString &key, 
						const u64 &docid,
						const bool must_same,
						const AosRundataPtr &rdata) = 0;
	// For U64 IIL 
	virtual bool	nextDocidSafe(
						i64 &idx, 
						i64 &iilidx,
						const bool reverse, 
						const AosOpr opr,
						u64 &value,
						u64 &docid, 
						bool &isunique,
						const AosRundataPtr &rdata) = 0;
	virtual bool 	addDocSafe(
						const u64 &value, 
						const u64 &docid, 
						const bool value_unique, 
						const bool docid_unique, 
						const AosRundataPtr &rdata) = 0;
	virtual bool	nextDocidSafe(
						AosIILIdx the_idx,
						i64 &idx, 
						i64 &iilidx,
						const bool reverse, 
						const AosOpr opr,
						u64 &value,
						u64 &docid, 
						bool &isunique,
						const AosRundataPtr &rdata) = 0;
	virtual bool 	incrementDocidSafe(
						const u64 &entry_id, 
						u64 &value,
						const u64 &incValue, 
						const u64 &initValue, 
						const bool add_flag,
						const u64 &dft_value,
						const AosRundataPtr &rdata) = 0;
	virtual bool 	modifyDocSafe(
						const u64 &oldvalue, 
						const u64 &newvalue, 
						const u64 &docid, 
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata) = 0;
	virtual bool	removeDocSafe(
						const u64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata) = 0;
	virtual bool	setValueDocUniqueSafe(
						const u64 &key,
						const u64 &docid,
						const bool must_same,
						const AosRundataPtr &rdata) = 0;

	virtual bool	setCmpTag(
						const AosXmlTagPtr &cmp_tag,
						const AosRundataPtr &rdata) = 0;
	virtual bool	batchAddSafe(
						const AosBuffArrayVarPtr &array,
						const AosRundataPtr &rdata) = 0;

	//For IIL
	virtual bool		getSplitValueSafe(
							const AosQueryContextObjPtr &context,
							const i64 &size,
							vector<AosQueryContextObjPtr> &contexts,
							const AosRundataPtr &rdata) = 0;

	virtual bool		nextDocidPrivFind(
							AosIILIdx &idx, 
							const OmnString &value,
							const AosOpr opr,
							u64 &docid, 
							bool &isunique,
							const bool reverse, 
							const AosRundataPtr &rdata) = 0;

	virtual bool 		addDocPriv(
							const OmnString &value, 
							const u64 &docid, 
							const bool value_unique, 
							const bool docid_unique, 
							const AosRundataPtr &rdata) = 0;
};

#endif

