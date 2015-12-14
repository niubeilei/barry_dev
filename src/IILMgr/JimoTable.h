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
// 	Created By Ken Lee, 2014/07/10
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_IILMgr_JimoTable_h
#define Omn_IILMgr_JimoTable_h

#include "SEInterfaces/IILObj.h"

#include "DfmUtil/Ptrs.h"
#include "IILMgr/Ptrs.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/Mutex.h"
#include "Util/BuffArrayVar.h"
#include "Util/Buff.h"
#include "Util/TransId.h"
/*
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "IDTransMap/Ptrs.h"
#include "IILMgr/Ptrs.h"
#include "IILUtil/Ptrs.h"
#include "IILUtil/IILUtil.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/SeTypes.h"
#include "SEUtil/Siteid.h"
#include "SEUtil/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "Util/Opr.h"
#include "Util1/MemChecker.h"
#include "aosUtil/Types.h"
#include "SearchEngine/Ptrs.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "XmlUtil/XmlTag.h"

#include <stdio.h>
#include <string.h>
*/

class AosJimoTable : public AosIILObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eMinIILSize = 500,
		eMaxIILSize = 5000,
		eMinSubIILs = 500,
		eMaxSubIILs = 5000,

		eExtraSubIILsSpace = 1000,
		eExtraDocids = 5020,
		eMemShreshold = 1000,

		eInitNumDocids = 10,
		eCompressSize = 10000, // 10k Linda, 11/21/2012

		eMaxSubChangedToSave = 10,	// Ken Lee, 2013/06/20
		eMaxSavingThreadsNum = 5,	// Ken Lee, 2013/06/20

		eNumDocsToDistr = 1000000000
	};

	enum DistrType
	{
		eNoDistr = 0,
		eDistrType1 = 1
	};

	enum DistrStatus
	{
		eDistr_Local = 0,
		eDistr1_Root = 1,
		eDistr1_Branch = 2
	};

private:
    u32             mRefcount;	// How many holding the IIL

protected:
	// The following member data are stored in the index record
	AosIILType		mIILType;
	u64				mIILID;
	u32				mSiteid;

	u64				mWordId;
	char			mFlag;
	u32				mHitcount;
	u32				mVersion;
	bool			mIsPersis;
	i64				mCompressedSize;		// iil compressed size

	// The following member data are not stored in index record
	OmnMutexPtr		mLock;
	AosIILObjPtr	mPrev;
	AosIILObjPtr	mNext;
	bool			mIsDirty;
	bool			mIsNew;
	bool			mIsGood;

	i64				mNumDocs;
	i64				mMemSize;

	AosBuffArrayVarPtr	mData;

	int				mLevel;
	AosJimoTablePtr	mParentIIL;
	AosXmlTagPtr	mCmpTag;

	DistrType		mDistrType;
	DistrStatus		mDistrStatus;

	// Subiil related data
	i64				mIILIdx;				// Subiil seqno, or ID.
	i64				mNumSubiils;
	AosJimoTablePtr *	mSubiils;
	u64 *			mIILIds;
	i64 *			mNumEntries;
	i64 *			mSubMemSize;

	AosBuffArrayVarPtr	mMinData;
	AosBuffArrayVarPtr	mMaxData;

	vector<AosTransId>	mTrans;

	AosDfmDocIILPtr	mDfmDoc;				// Ketty 2012/11/15

	u64				mSnapShotId;			// Ken Lee, 2013/09/16

	friend class 	AosIILMgr;

	OmnMutexPtr		mUpdateLock;		// Ken Lee,2013/06/20

public:
	AosJimoTable();
	~AosJimoTable();

	void 			lockIIL() {AOSLOCK(mLock);}
	void 			unlockIIL() {AOSUNLOCK(mLock);}

	AosIILType		getIILType() const {return mIILType;}
	u64				getIILID() const {return mIILID;}
	void			setIILID(const u64 &iilid) {mIILID = iilid;}
	u32				getSiteid() const { return mSiteid;}
	void			setSiteid(const u32 siteid) {mSiteid = siteid;}

	bool			isDirty() const {return mIsDirty;}
	bool			isNew() {return mIsNew;}
	bool			isGood() const {return mIsGood;}
	void			setDirty(const bool dirty) {mIsDirty = dirty;}
	void			setNew(const bool isnew) {mIsNew = isnew;}

	i64 			getNumDocs() const {return mNumDocs;}
	i64 			getMemSize() const {return mMemSize;}
	void			setPersis(const bool isper) {mIsPersis = isper;}
	i64				getNumSubiils() const {return mNumSubiils;}

	void			setRootIIL(AosIILObjPtr rootiil) {}
	AosIILObjPtr	getRootIIL() {return 0;}
	void			setIILIdx(const i64 &idx) {mIILIdx = idx;}
	i64				getIILIdx() const {return mIILIdx;}

	bool 			isRootIIL() const {return !mParentIIL;}
	bool			isParentIIL() const {return mLevel > 0;}
	bool			isSingleIIL() const {return (mLevel == 0 && !mParentIIL);}
	bool			isLeafIIL() const {return mLevel == 0;}
	bool 			isChildIIL() const {return mParentIIL;}
	bool			isBranchIIL() const {return (mLevel > 0 && mParentIIL);}
	int 			getLevel() const {return mLevel;}
	void			setIILLevel(const int level){mLevel = level;}
	bool			isBigIIL() {return true;}
	void			setParentIIL(const AosJimoTablePtr &parent) {mParentIIL = parent;}

	void 			addTrans(const AosTransId &trans);
	void 			finishTrans();
	u32 			getTransNum() { return mTrans.size(); }

	bool			loadFromFileSafe(
						const u64 &iilid, 
						const AosRundataPtr &rdata);
	bool			loadFromFilePriv(
						const u64 &iilid, 
						const AosRundataPtr &rdata);

	bool			saveToFileSafe(const AosRundataPtr &rdata);
	bool			saveToFilePriv(const AosRundataPtr &rdata);
	bool			saveToLocalFileSafe(const AosRundataPtr &rdata);
	bool			saveToLocalFilePriv(const AosRundataPtr &rdata);
	bool			saveSubIILToLocalFileSafe(const AosRundataPtr &rdata);

	void			resetSafe(
						const u64 &wordid, 
						const u64 &iilid,
						const u32 siteid,
						const AosRundataPtr &rdata);
	void			resetPriv(
						const u64 &wordid, 
						const u64 &iilid,
						const u32 siteid,
						const AosRundataPtr &rdata);

	u64				getSnapShotId() {return mSnapShotId;}
	void			setSnapShotId(const u64 &snap_id);
	void			setSnapShotIdPriv(const u64 &snap_id);
	void			resetSnapShotId();
	void			resetSnapShotIdPriv();

	bool			rebuildBitmapSafe(const AosRundataPtr &rdata) {return true;}

	void			setDistrType(const DistrType type){mDistrType = type;}
	void			setDistrStatus(const DistrStatus status){mDistrStatus = status;}

	DistrType		getDistrType()const {return mDistrType;}
	DistrStatus		getDistrStatus()const {return mDistrStatus;}

	void			countDirtyRec(
						int &parent_changed, 
						int &leaf_changed,
						const AosRundataPtr &rdata);

	virtual bool	saveLeafToFileSafe(
						int &numToSave,
						const AosRundataPtr &rdata);
	virtual bool	saveLeafToFilePriv(
						int &numToSave,
						const AosRundataPtr &rdata);
	virtual bool	saveLeafToFileRecSafe(
						int &numToSave,
						const AosRundataPtr &rdata);
	virtual bool	saveLeafToFileRecPriv(
						int &numToSave,
						const AosRundataPtr &rdata);

	AosIILObjPtr	firstLeaf(
						AosIILIdx &idx,
						AosBitmapTreeObjPtr &tree,
						const AosRundataPtr &rdata)
					{
						OmnShouldNeverComeHere;
						return 0;
					}
	AosIILObjPtr	nextLeaf(
						AosIILIdx &idx,
						AosBitmapTreeObjPtr &tree,
						const AosRundataPtr &rdata)
					{
						OmnShouldNeverComeHere;
						return 0;
					}
	AosIILObjPtr	firstLeaf(
						AosIILIdx &idx,
						const AosRundataPtr &rdata)
					{
						OmnShouldNeverComeHere;
						return 0;
					}
	AosIILObjPtr	nextLeaf(
						AosIILIdx &idx,
						const AosRundataPtr &rdata)
					{
						OmnShouldNeverComeHere;
						return 0;
					}

	bool			incNumEntriesNotSafe(const i64 &iilidx)
					{
						aos_assert_r(mNumEntries, false);
						aos_assert_r(iilidx >= 0 && iilidx < mNumSubiils, false);
						mNumEntries[iilidx]++;
						return true;
					}
	bool			decNumEntriesNotSafe(const i64 &iilidx)
					{
						aos_assert_r(mNumEntries, false);
						aos_assert_r(iilidx >= 0 && iilidx < mNumSubiils, false);
						mNumEntries[iilidx]--;
						return true;
					}
	bool			decNumEntriesNotSafe(const i64 &iilidx, const i64 &num)
					{
						aos_assert_r(mNumEntries, false);
						aos_assert_r(iilidx >= 0 && iilidx < mNumSubiils, false);
						mNumEntries[iilidx] -= num;
						return true;
					}
	bool			setNumEntriesNotSafe(const i64 &iilidx, const i64 &num)
					{
						aos_assert_r(mNumEntries, false);
						aos_assert_r(iilidx >= 0 && iilidx < mNumSubiils, false);
						mNumEntries[iilidx] = num;
						return true;
					}
	
	u64				getIILIDForHash();

	u32				getRefCountByIILMgr() const {return mRefcount;}
	u32				addRefCountByIILMgr();
	u32				removeRefCountByIILMgr();

	AosIILObjPtr	getNextByIILMgr() const {return mNext;}
	AosIILObjPtr	getPrevByIILMgr() const {return mPrev;}
	void			setPrevByIILMgr(const AosIILObjPtr &p) {mPrev = p;}
	void			setNextByIILMgr(const AosIILObjPtr &p) {mNext = p;}
	void			resetPtrsByIILMgr() {mPrev = 0; mNext = 0;}

	bool			returnSubIILsSafe(const AosRundataPtr &rdata)
					{
						bool returned;
						return returnSubIILsSafe(returned, false, rdata);
					}
	bool			returnSubIILsSafe(
						bool &returned,
						const bool returnHeader,
						const AosRundataPtr &rdata)
					{
        				AOSLOCK(mLock);
        				bool rslt = returnSubIILsPriv(returned, returnHeader, rdata);
        				AOSUNLOCK(mLock);
						return rslt;		
					}

	i64				getNumDocsSafe() const 
					{
						AOSLOCK(mLock);
						i64 numdoc = mNumDocs;
						AOSUNLOCK(mLock);
						return numdoc;
					}

	bool			batchAddSafe(
						char * &entries,
						const i64 &size,
						const i64 &num,
						const AosIILExecutorObjPtr &executor,
						const AosRundataPtr &rdata)
					{
						OmnNotImplementedYet;
						return false;
					}
	bool			batchDelSafe(
						char * &entries,
						const i64 &size,
						const i64 &num,
						const AosIILExecutorObjPtr &executor,
						const AosRundataPtr &rdata)
					{
						OmnNotImplementedYet;
						return false;
					}
	bool			batchIncSafe(
						char * &entries,
						const i64 &size,
						const i64 &num,
						const u64 &dftValue,
						const AosIILUtil::AosIILIncType incType,
						const AosRundataPtr &rdata)
					{
						OmnNotImplementedYet;
						return false;
					}
	bool			batchDecSafe(
						char * &entries,
						const i64 &size,
						const i64 &num,
						const bool delete_flag,
						const AosIILUtil::AosIILIncType incType,
						const AosRundataPtr &rdata)
					{
						OmnNotImplementedYet;
						return false;
					}

	bool			queryNewSafe(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata) {return false;}
	bool			bitmapQueryNewSafe(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata) {return false;}
	bool			computeQueryResults(
						const AosRundataPtr &rdata, 
						const OmnString &iilname, 
						const AosQueryContextObjPtr &context, 
						const AosBitmapObjPtr &bitmap, 
						const u64 &query_id, 
						const int physical_id) {return false;}
	bool			bitmapRsltQueryNewSafe(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata) {return false;}
	bool			preQuerySafe(
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata) {return false;}
	bool			deleteIILSafe(
						const bool true_delete,
						const AosRundataPtr &rdata)
					{
						AOSLOCK(mLock);
						bool rslt = deleteIILPriv(true_delete, rdata);
						AOSUNLOCK(mLock);
						return rslt;
					}

	bool			setCmpTag(
						const AosXmlTagPtr &cmp_tag,
						const AosRundataPtr &rdata);

	bool 			addDocSafe(
						const OmnString &value,
						const bool value_unique,
						const AosRundataPtr &rdata)
					{
						aos_assert_r(value != "", false);
						AOSLOCK(mLock);
						bool rslt = addDocRecPriv(value, value_unique, rdata);
						AOSUNLOCK(mLock);
						return rslt;
					}
	bool			addDocRecSafe(
						const OmnString &value, 
						const bool value_unique, 
						const AosRundataPtr &rdata)
					{
						AOSLOCK(mLock);
						bool rslt = addDocRecPriv(value, value_unique, rdata);
						AOSUNLOCK(mLock);
						return rslt;
					}
	bool 			updateIndexData(
						const i64 &idx,
						const bool changeMax,
						const bool changeMin,
						const AosRundataPtr &rdata);

	const char * 	getMinValue(int &len) const;
	const char * 	getMaxValue(int &len) const;

private:
	void			failInLoadProtection();
	void			failInSetCtntProtection();
	bool 			readFromDfmDoc(
						const AosDfmDocIILPtr &doc, 
						const AosRundataPtr &rdata);
	bool 			setContentsProtected(
						const AosBuffPtr &buff,
						const AosRundataPtr &rdata);
	bool			setContentsSinglePriv(
						const AosBuffPtr &buff, 
						const AosRundataPtr &rdata);
	bool			createSubiilIndex();
	bool			createMaxMinBuff(const AosRundataPtr &rdata);

	void			resetSelf(const AosRundataPtr &rdata);
	bool			resetSubIILInfo(const AosRundataPtr &rdata);

	bool			returnSubIILsPriv(const AosRundataPtr &rdata)
					{
						bool returned;
						return returnSubIILsPriv(returned, false, rdata);
					}
	bool			returnSubIILsPriv(
						bool &returned,
						const bool returnHeader,
						const AosRundataPtr &rdata);
	bool			deleteIILPriv(
						const bool true_delete,
						const AosRundataPtr &rdata) {return false;}

	AosBuffPtr		getBodyBuffProtected(const AosRundataPtr &rdata);
	AosBuffPtr 		getBodyBuffSinglePriv(const AosRundataPtr &rdata);

	bool			addId(const AosRundataPtr &rdata);
	bool			commitId(const AosRundataPtr &rdata);

	bool			saveBitmapTree(const AosRundataPtr &rdata) {return true;}

	void			clearTrans() {mTrans.clear();}

	bool			getValue(
						const i64 &idx,
						char* &data,
						int &len);
	bool			getMinValue(
						const i64 &idx,
						char* &data,
						int &len);
	bool			getMaxValue(
						const i64 &idx,
						char* &data,
						int &len);

	bool			addDocRecPriv(
						const OmnString &value, 
						const bool value_unique, 
						const AosRundataPtr &rdata);

	bool			addDocSinglePriv(
						const OmnString &value,
						const bool value_unique,
						i64 &insert_idx,
						const AosRundataPtr &rdata);

	AosJimoTablePtr	getSubiilPriv(
						const OmnString &value,
						const AosRundataPtr &rdata);

	i64				getSubiilIndexPriv(
						const OmnString &value,
						const AosRundataPtr &rdata);

	AosJimoTablePtr	getSubiilByIndexPriv(
						const i64 &idx, 
						const AosRundataPtr &rdata);

	AosJimoTablePtr	getSubiilByIndexPriv(
						const u64 &iilid,
						const i64 &iil_idx,
						const AosRundataPtr &rdata);

	bool			setSubiilIndexPriv(
						const AosIILObjPtr &iil,
						const i64 &iil_idx,
						const AosRundataPtr &rdata);

	bool			sanityCheckPriv(const AosRundataPtr &rdata);
	bool			sanityCheckRecPriv(i64 &num_docs, const AosRundataPtr &rdata);
	bool			sanityCheckSinglePriv(i64 &num_docs, const AosRundataPtr &rdata);

	bool 			splitCheck();
	AosJimoTablePtr	splitLeafContent(const AosRundataPtr &rdata);
	AosJimoTablePtr	splitParentContent(const AosRundataPtr &rdata);
	AosJimoTablePtr	splitContentUtil(const AosRundataPtr &rdata);
	bool			splitListPriv(const AosRundataPtr &rdata);
	bool			splitListSinglePriv(
						const vector<AosJimoTablePtr> &subiil_list,
						const AosRundataPtr &rdata);
	bool			splitListRootPriv(
						const vector<AosJimoTablePtr> &subiil_list,
						const AosRundataPtr &rdata);
	bool			subiilSplited(
						const i64 &iilIdx,
						const vector<AosJimoTablePtr> &subiil_list,
						const AosRundataPtr &rdata);
	bool			subiilSplited(const AosRundataPtr &rdata);

	bool			addSubiils(
						const i64 &iilIdx,
						const vector<AosJimoTablePtr> &subiil_list,
						const AosRundataPtr &rdata);
	bool 			initSubiilLeaf(
						const AosBuffArrayVarPtr &array,
						const AosRundataPtr &rdata);
	bool 			initSubiilParent(
						const i64 &num,
						const AosBuffArrayVarPtr &minarray,
						const AosBuffArrayVarPtr &maxarray,
						const u64 *iilids,
						const i64 *numEntries,
						const i64 *subMemSize,
						const AosJimoTablePtr *subiils);

public:
	bool			batchAddSafe(
						const AosBuffArrayVarPtr &array,
						const AosRundataPtr &rdata);

	bool			batchAddRecSafe(
						const AosBuffArrayVarPtr &array,
						i64 &crt_idx,
						i64 &subChanged,
						const bool isTail,
						const AosRundataPtr &rdata);

	bool			batchAddParentThrdSafe(
						const u64 &iilid,
						const i64 &iil_idx,
						const AosBuffArrayVarPtr &array,
						i64 &crt_idx,
						i64 &num_handle,
						const AosRundataPtr &rdata);

	bool			batchAddSingleThrdSafe(
						const AosBuffArrayVarPtr &array,
						i64 &crt_idx,
						i64 &num_handle,
						const AosRundataPtr &rdata);

	bool			batchAddRebuild(
						const AosBuffArrayVarPtr &array,
						i64 &crt_idx,
						i64 &num_handle,
						const AosBuffArrayVarPtr &orig_array,
						i64 &orig_idx,
						i64 &orig_num,
						const i64 &iilsize,
						const AosRundataPtr &rdata);

	void			lockUpdate(){mUpdateLock->lock();}
	void			unlockUpdate(){mUpdateLock->unlock();}

private:
	bool			batchAddPriv(
						const AosBuffArrayVarPtr &array,
						const AosRundataPtr &rdata);

	bool			batchAddRecPriv(
						const AosBuffArrayVarPtr &array,
						i64 &crt_idx,
						i64 &subChanged,
						const bool isTail,
						const AosRundataPtr &rdata);

	bool			batchAddSinglePriv(
						const AosBuffArrayVarPtr &array,
						i64 &crt_idx,
						i64 &subChanged,
						const bool isTail,
						const AosRundataPtr &rdata);

	bool			batchAddSingleThrdPriv(
						const AosBuffArrayVarPtr &array,
						i64 &crt_idx,
						i64 &num_handle,
						const AosRundataPtr &rdata);

	bool			batchAddRebuildCount(
						const AosBuffArrayVarPtr &array,
						const i64 &crt_idx,
        				const i64 &num_handle,
						i64 &new_handle,
        				const AosBuffArrayVarPtr &orig_array,
						const i64 &orig_idx,
    				    i64 &orig_handle,
    				    const i64 &iilsize,
    				    const AosRundataPtr &rdata);

	i64			binarySearch(
						const AosBuffArrayVarPtr &array,
						const i64 &crt_idx,
						const char * data,
						const int len);

public:
	// For HIT IIL
	virtual bool    addDocSafe(const u64 &docid, const AosRundataPtr &rdata) { return false; }
	virtual bool	removeDocSafe(const u64 &docid, const AosRundataPtr &rdata) { return false; }
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
						const AosRundataPtr &rdata) { return false; }
	virtual bool 	addDocSafe(
						const OmnString &value, 
						const u64 &docid, 
						const bool value_unique, 
						const bool docid_unique, 
						const AosRundataPtr &rdata) { return false; }
	virtual bool	removeDocSafe(
						const OmnString &value,
						const u64 &docid,
						const AosRundataPtr &rdata) { return false; }
	virtual bool	incrementDocidSafe(
						const OmnString &key,
						u64 &value,
						const u64 &incvalue,
						const u64 &init_value,
						const bool add_flag,
						const AosRundataPtr &rdata) { return false; }
	virtual bool	removeFirstValueDocSafe(
						const OmnString &value,
						u64 &docid,
						const bool reverse,
						const AosRundataPtr &rdata) { return false; }
	virtual bool 	modifyDocSafe(
						const OmnString &oldvalue, 
						const OmnString &newvalue, 
						const u64 &docid, 
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata) { return false; }
	virtual bool	setValueDocUniqueSafe(
						const OmnString &key, 
						const u64 &docid,
						const bool must_same,
						const AosRundataPtr &rdata) { return false; }
	//For U64 IIL
	virtual bool	nextDocidSafe(
						i64 &idx, 
						i64 &iilidx,
						const bool reverse, 
						const AosOpr opr,
						u64 &value,
						u64 &docid, 
						bool &isunique,
						const AosRundataPtr &rdata) { return false; }

	virtual bool 	addDocSafe(
						const u64 &value, 
						const u64 &docid, 
						const bool value_unique, 
						const bool docid_unique, 
						const AosRundataPtr &rdata) { return false; }
	virtual bool	nextDocidSafe(
						AosIILIdx the_idx,
						i64 &idx, 
						i64 &iilidx,
						const bool reverse, 
						const AosOpr opr,
						u64 &value,
						u64 &docid, 
						bool &isunique,
						const AosRundataPtr &rdata) { return false; }
	virtual bool 	incrementDocidSafe(
						const u64 &entry_id, 
						u64 &value,
						const u64 &incValue, 
						const u64 &initValue, 
						const bool add_flag,
						const u64 &dft_value,
						const AosRundataPtr &rdata) {return false; }
	virtual bool 	modifyDocSafe(
						const u64 &oldvalue, 
						const u64 &newvalue, 
						const u64 &docid, 
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata) { return false; }
	virtual bool	removeDocSafe(
						const u64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata) { return false; }
	virtual bool	setValueDocUniqueSafe(
						const u64 &key,
						const u64 &docid,
						const bool must_same,
						const AosRundataPtr &rdata) { return false; }
	virtual bool	getSplitValueSafe(
						const AosQueryContextObjPtr &context,
						const i64 &size,
						vector<AosQueryContextObjPtr> &contexts,
						const AosRundataPtr &rdata) { return false; }

	virtual bool	nextDocidPrivFind(
						AosIILIdx &idx, 
						const OmnString &value,
						const AosOpr opr,
						u64 &docid, 
						bool &isunique,
						const bool reverse, 
						const AosRundataPtr &rdata) { return false;}

	virtual bool 	addDocPriv(
						const OmnString &value, 
						const u64 &docid, 
						const bool value_unique, 
						const bool docid_unique, 
						const AosRundataPtr &rdata) { return false; }
	/*
	AosJimoTable(
		const u64 &wordid,
		const u64 &iilid,
		const AosIILType type,
		const bool isPersis,
		const OmnString &iilname);

	// Static Functions. All static member functions are thread safe.
	static i64		staticNumIILRefs();
	static bool			staticInit(const AosXmlTagPtr &config);
	static AosIILObjPtr	staticCreateIIL(
							const AosIILType iiltype,
							const AosRundataPtr &rdata);
	static AosIILObjPtr	staticCreateIIL(
							const u64 &wordid, 
							const u64 &iilid, 
							const u32 siteid, 
							const AosIILType iiltype,
							const bool is_persis,
							const OmnString &iilname, 
							const AosRundataPtr &rdata);

	static AosIILObjPtr	staticLoadFromFile(
							const u64 &iilid, 
							const u64 &snap_id,
							const AosRundataPtr &rdata);

	static bool			staticStop();
	static i64		getTotalIILs(const AosIILType iiltype){return mTotalIILs[iiltype];}
	
	static bool			staticIsIILCreated(
							const u64 &iilid, 
							const u32 siteid,
							const u64 &snap_id,
							const AosIILType type,
							const AosRundataPtr &rdata);

public:

	void 				finishTrans(); // Ken Lee, 2013/06/06
	void 				setLastAddTimestamp(const bool add_to_head);

	bool				isBigIIL() {return (mIILType == eAosIILType_BigStr || mIILType == eAosIILType_BigU64);}

	virtual u64			getIILIDWithCompFlag();

	virtual bool		queryRangeSafe(
							const AosQueryRsltObjPtr &query_rslt,
							const AosBitmapObjPtr &query_bitmap,
							const AosQueryContextObjPtr &query_context,
							const AosRundataPtr &rdata);
	virtual bool		querySafe(
							const AosQueryRsltObjPtr &query_rslt,
							const AosBitmapObjPtr &query_bitmap,
							const AosQueryContextObjPtr &query_context,
							const AosRundataPtr &rdata) = 0;
	virtual u64			nextDocIdSafe2(
							i64 &idx,
							i64 &iilidx,
							const bool reverse);
	virtual u64			nextDocIdPriv2(
							i64 &idx,
							i64 &iilidx,
							const bool reverse);

	bool				incNumDocsNotSafe()
						{
							mNumDocs++;
							if (mRootIIL.notNull())
							{
								aos_assert_r(mRootIIL, false);
								mRootIIL->incNumEntriesNotSafe(mIILIdx);
							}
							return true;
						}

protected:
	bool 				saveSanityCheck(const AosIILObjPtr &iil, const AosRundataPtr &rdata); 
	bool 				setDocidContents(AosBuffPtr &buff);
	bool 				expandDocidMem();
	bool				deleteFromLocalFilePriv(const AosRundataPtr &rdata);

	virtual bool		prepareMemoryForReloading();
	virtual u64			getDocIdSafe(const i64 &idx, const AosRundataPtr &rdata) const;
	virtual u64			getDocIdSafe1(i64 &idx, i64 &iilidx, const AosRundataPtr &rdata);
	virtual bool 		adjustMemoryProtected();
	virtual u64			getDocIdPriv(const i64 &idx, const AosRundataPtr &rdata) const;
	virtual bool		copyDocidsPriv(
							const AosQueryRsltObjPtr &query_rslt,
							const AosBitmapObjPtr &query_bitmap,
							const i64 &start,
							const i64 &end);

protected:
	char * 				getMemory(
							const i64 &newsize, 
							const char *oldmem, 
							const i64 &oldsize);

	bool				setNumDocsNotSafe(const i64 &num)
						{
							mNumDocs = num;
							if (mRootIIL.notNull())
							{
								aos_assert_r(mRootIIL, false);
								mRootIIL->setNumEntriesNotSafe(mIILIdx, num);
							}
							return true;
						}

	bool				decNumDocsNotSafe()
						{
							aos_assert_r(mNumDocs > 0, false);
							mNumDocs--;
							if (mRootIIL.notNull())
							{
								aos_assert_r(mRootIIL, false);
								mRootIIL->decNumEntriesNotSafe(mIILIdx);
							}
							return true;
						}

	virtual bool		copyDocidsSafe(
							const AosQueryRsltObjPtr   &query_rslt,
							const AosBitmapObjPtr &query_bitmap,
							const i64 &start,
							const i64 &end)
						{
							AOSLOCK(mLock);
							bool rslt = copyDocidsPriv(query_rslt, query_bitmap, start, end);
							AOSUNLOCK(mLock);
							return rslt;
						}

	bool				decNumDocsNotSafe(const i64 &num)
						{
							aos_assert_r(mNumDocs > num, false);
							mNumDocs -= num;
							if (mRootIIL.notNull())
							{
								aos_assert_r(mRootIIL, false);
								mRootIIL->decNumEntriesNotSafe(mIILIdx, num);
							}
							return true;
						}

private:
	virtual u64			getMinDocid() const = 0;
	virtual u64			getMaxDocid() const = 0;
	virtual bool 		isIdxValidSafe(const i64 &idx)
						{
							AOSLOCK(mLock);
							bool rslt = (idx >= 0 && idx < mNumDocs);
							AOSUNLOCK(mLock);
							return rslt;
						}
	u64					getWordIdSafe() const {return mWordId;}

	virtual i64		getTotalSafe(
							const OmnString &value, 
							const AosOpr opr, 
							const AosRundataPtr &rdata)
						{
							OmnShouldNeverComeHere;
							return -1;
						}

	virtual i64		getTotalSafe(
							const u64 &value, 
							const AosOpr opr, 
							const AosRundataPtr &rdata)
						{
							OmnShouldNeverComeHere;
							return -1;
						}

	virtual bool		checkDocSafe(
							const AosOpr opr,
							const OmnString &value,
							const u64 &docid,
							const AosRundataPtr &rdata)
						{
							OmnShouldNeverComeHere;
							return false;
						}	

	virtual bool		checkDocSafe(
							const AosOpr opr,
							const u64 &value,
							const u64 &docid,
							const AosRundataPtr &rdata)
						{
							OmnShouldNeverComeHere;
							return false;
						}

	virtual bool		docExistSafe(const u64 &docId, const AosRundataPtr &rdata) const;
	virtual bool 		isNumAlpha()const{return false;}
	
public:


protected:
	bool				resetWordPtrByIILMgr();
	bool				setPtrToWordHashByIILMgr();
	void				setIILIDByIILMgr(const u64 &iilid) {mIILID = iilid;}
	void				incIILIdx() {mIILIdx++;}
	

	virtual bool		moveTo(const i64 &startIdx, i64 &idx, i64 &iilidx);

private:

	virtual bool 		saveSanityCheckProtected(const AosRundataPtr &rdata) = 0;

	AosBuffPtr 			getBodyBuffMem(const i64 &size) const;
	
private:
	u32					getVersion()const{return mVersion;}
	void				setVersion(const u32 &ver){mVersion = ver;}
	bool				isPersis()const{return mIsPersis;}
	bool				returnMemory(const char *mem, const i64 &size);
	void				setWordId(const u64 &w) {mWordId = w;}

	bool 				expandMemIIL();


protected:

	bool				numEntriesIncrease(const i64 &index);
	bool				numEntriesDecrease(const i64 &index);

	virtual bool		nextDocidSafe(
							i64 &idx, 
							i64 &iilidx,
							const bool reverse, 
							const AosOpr opr,
							u64 &value,
							u64 &docid, 
							bool &isunique)
						{ 
							return true;
						}


	static bool			staticCheckIILIDs(
							const u64 &iilid, 
							const u32 siteid,
							i64 &num,
							vector<bool> &iilid_states, 
							const AosRundataPtr &rdata);
	static bool			staticReadHeaders(
							const u64 &iilid, 
							const u32 siteid, 
							const i64 &num, 
							AosBuff &buff, 
							const AosRundataPtr &rdata);

	static bool			savingSanityCheck(
							const AosDocFileMgrObjPtr &docFileMgr,
							const u32 seqno, 
							const u64 &offset_old,
							const u64 &offset, 
							const char *data, 
							const i64 &docsize, 
							const i64 &compressed_size,
							const AosRundataPtr &rdata);

public:
	virtual bool		deleteFromLocalFileByID(
							const u64 iilid,
							const u64 &snap_id,
							const AosRundataPtr &rdata);

private:
	virtual bool		returnSubIILsPriv(const AosRundataPtr &rdata) = 0;
	virtual bool		returnSubIILsPriv(
							bool &returned,
							const bool returnHeader,
							const AosRundataPtr &rdata) = 0;

public:
	virtual bool		getSplitValueSafe(
							const AosQueryContextObjPtr &context,
							const i64 &size,
							vector<AosQueryContextObjPtr> &contexts,
							const AosRundataPtr &rdata)
						{
							OmnNotImplementedYet;
							return false;
						}

	virtual bool resetIIL(const AosRundataPtr &rdata) = 0; 

	static int getNumIILsCreated();

	*/
};

#endif
