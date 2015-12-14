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
// 	Created: 10/15/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_IILMgr_IIL_h
#define Omn_IILMgr_IIL_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "DfmUtil/Ptrs.h"
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
#include "Thread/Mutex.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "Util/Opr.h"
#include "Util1/MemChecker.h"
#include "aosUtil/Types.h"
#include "SearchEngine/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/IILObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "XmlUtil/XmlTag.h"

#include <stdio.h>
#include <string.h>

#define AOS_SUPPORT_BITMAP_QUERY 
#define AOS_BITMAP_QUERY_START  if(!executor->isDisable()){
#define AOS_BITMAP_QUERY_END	}   	

class AosBuff;
class AosIILMgr;
const u32 AOSIIL_INMEMORY_FLAG = 0x80000000;


class AosIIL : public AosIILObj
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
		eCommonIILHeaderContentSize = 30,	// Ketty 2012/10/30
		//eNormalIILHeaderContentSize = 46,
		//eFullIILHeaderContentSize = 62,
		eNormalIILHeaderContentSize = 50,	// Linda, 11/15/2012
		eFullIILHeaderContentSize = 70,		// Linda, 11/15/2012
		eIILHeaderSize = 200,
		eIILHeaderMemsize = eIILHeaderSize-4,
		eCompIILBitFlag = 0x8000000000000000LL,
		ePersisBitOnIILIDFlag = 0x80000000,
		ePersisBitOnVidFlag = 0x80000000,

		eSuperIILTop 	= 3,
		eSuperIILLow 	= 2,
		eNormalRootIIL  = 1,
		eNormalIIL  	= 0,
		eGoodIILFlag	= 264467647,	// Chen Ding, 11/06/2012
		eDeletedIILFlag	= 6036746,		// Chen Ding, 11/06/2012
		eCompressSize   = 10000, // 10k Linda, 11/21/2012

		eMaxSubChangedToSave = 10,	// Ken Lee, 2013/06/20
		eMaxSavingThreadsNum = 5	// Ken Lee, 2013/06/20
	};

private:
    u32             mRefcount;	// How many holding the IIL

protected:
	// The following member data are stored in the index record
	AosIILType		mIILType;
	u64				mIILID;		// The IILID
    u64     		mWordId;	// The wordId for the IIL
    i64				mNumDocs;	// The number of DocIds currently for the IIL
    char      		mFlag;		// Indicate whether the IIL is active
    u32             mHitcount;	// The IIL hit count

	//u32			mTotalDocs;	// The total docs (including multiple IILs)

	u32				mVersion;		// The total docs (including multiple IILs)
	bool			mIsPersis;		// Whether the IIL is persistant or not

	//u32     		mIILDiskSize;	// Size in disk				// Ketty 2012/11/15
    //u64           mOffset;		// The offset in file		// Ketty 2012/11/15
    //u32           mSeqno;			// The file seqno		// Ketty 2012/11/15
	i64				mCompressedSize; // iil compressed size
	OmnString		mIILName;		// The name of the IIL(the word the IIL using for)
	// The following member data are not stored in index record
	i64				mMemCap;	// Memory allocated for the IIL
    u64 *           mDocids;
	OmnMutexPtr		mLock;
	AosIILObjPtr	mPrev;
	AosIILObjPtr	mNext;
	bool			mIsDirty;
	bool			mIsNew;
	bool			mIsGood;

	// Subiil related data
	AosIILObjPtr	mRootIIL;
	i64				mNumSubiils;
	i64				mIILIdx;				// Subiil seqno, or ID.
	i64 *			mNumEntries;

	//ken 2011/08/19
	//vector<u64>		mTrans;
	vector<AosTransId>	mTrans;
	u64 *			mIILIds;
	u64				mLastAddSec;
	bool			mLastAddToHead;
//	bool			mCreationOnly;			// Chen Ding, 10/15/2011
	u32				mSiteid;				//ken 2012/02/02

	AosDfmDocIILPtr	mDfmDoc;				// Ketty 2012/11/15
	//static AosDfmDocIILPtr	smDfmDoc;		// Chen Ding, 2013/01/12

	u64				mDebugFlag;				// Debugging only
	
	u64				mSnapShotId;			// Ken Lee, 2013/09/16
	OmnMutexPtr		mBRLock;

public:
	static bool			smNeedCompress;			// Chen Ding DDDDD, 2013/01/26
	static i64			smCompressSize;			// Chen Ding DDDDD, 2013/01/26

	static i64			mExtraDocids;	
	static i64			mExtraDocidsInit;

	static i64			mMaxNormIILSize;
	static i64			mMaxBigIILSize;
	static i64			mMinIILSize;
	static i64			mMaxSubIILs;
	static i64			mMinSubIILs;

	static i64			mAddBlockMaxSize;	// Ken Lee, 2013/01/03
	static i64			mAddBlockMinSize;	// Ken Lee, 2013/01/03
	static i64			mAddBlockFillSize;	// Ken Lee, 2013/01/03

	static bool			mIILMergeFlag;

	static i64			mTotalIILs[eAosIILType_Total];
	static i64			sgNumIILCreated;
	static i64			sgNumIILRefs;
	static OmnMutexPtr	sgLock;

	friend class 		AosIILMgr;

public:
    AosIIL(const AosIILType type);
	AosIIL(
		const u64 &wordid,
		const u64 &iilid,
		const AosIILType type,
		const bool isPersis,
		const OmnString &iilname);
	~AosIIL();

	// Static Functions. All static member functions are thread safe.
	static i64		staticNumIILRefs();
	// static bool			staticReadHeaderBuff(
	// 						const u64 &iilid, 
	// 						const u32 &siteid, 
	// 						//AosBuff &buff, 
	// 						AosIILHeaderEntryPtr &iil_header,	// Ketty 2012/11/15
	// 						const AosRundataPtr &rdata,
	// 						const bool create_flag = false);
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
	static bool 		staticIsIILIDUsed(
							const u64 &iilid, 
							const u64 &snap_id,
							const AosRundataPtr &rdata);
	static i64			getTotalIILs(const AosIILType iiltype){return mTotalIILs[iiltype];}
	static bool 		isIILMerged(){bool rslt = mIILMergeFlag;mIILMergeFlag = false;return rslt;}
	static bool			isCompress(const i64 &size);
	
	static AosDocFileMgrObjPtr getDocFileMgr(
							const u64 &iilid,
							u64 &local_iilid,
							const AosRundataPtr &rdata);

	static inline void	setCompIILIdBit(u64 &iilid)
						{
							iilid |= eCompIILBitFlag;
						}

	//static bool		saveToFileSafeStatic(
	//						const u64 &root_iilid,
	//						const u32 siteid, 
	//						const AosBuffPtr &headbuff, 
	//						const AosBuffPtr &bodybuff, 
	//						const AosRundataPtr &rdata);

	static bool			staticIsIILCreated(
							const u64 &iilid, 
							const u32 siteid,
							const u64 &snap_id,
							const AosIILType type,
							const AosRundataPtr &rdata);

public:
	virtual bool		isRootIIL()const{return mNumSubiils > 0 && mIILIdx == 0;}
	virtual bool		isParentIIL()const{return mNumSubiils > 0 && mIILIdx == 0;}
	virtual bool		isSingleIIL()const{return mNumSubiils == 0 && mIILIdx <=0;}
	virtual bool		isLeafIIL()const{return mIILIdx > 0;}
	virtual bool 		isChildIIL()const{return mIILIdx > 0;}

	i64 				getNumDocs() const {return mNumDocs;}
	AosIILType			getIILType() const {return mIILType;}
	u64					getIILID() const {return mIILID;}
	void				setIILIdx(const i64 &idx) {mIILIdx = idx;}
	i64					getIILIdx() const {return mIILIdx;}
	i64					getNumSubiils() const {return mNumSubiils;}
	void				setRootIIL(AosIILObjPtr rootiil) {mRootIIL = rootiil;}
	bool				isDirty() const {return mIsDirty;}
	//void				setCreationOnly(const bool b) {mCreationOnly = b;}
	void				setNew(const bool isnew){mIsNew = isnew;}
	void				setDirty(const bool dirty){mIsDirty = dirty;}
	void				setIILID(const u64 &iilid){mIILID = iilid;}
	void				setSiteid(const u32 siteid) {mSiteid = siteid;}
	void				setPersis(const bool isper){mIsPersis = isper;}
	AosIILObjPtr		getNextByIILMgr() const {return mNext;}
	bool 				isLastAddToHead() const {return mLastAddToHead;}
	u64  				getLastAddTimestamp() const {return mLastAddSec;}
	u32					getRefCountByIILMgr() const {return mRefcount;}
	void 				lockIIL() {AOSLOCK(mLock);}
	void 				unlockIIL() {AOSUNLOCK(mLock);}
	//bool				finishTransSafe(const AosRundataPtr &rdata);		// Ketty 2013/01/15
	u32					getSiteid() const { return mSiteid; };
	u64 				getCompIILID(){return mIILID | eCompIILBitFlag;}
//	bool				isCreationOnly() const {return mCreationOnly;}
	u32					addRefCountByIILMgr();
	AosIILObjPtr		getPrevByIILMgr() const {return mPrev;}
	virtual AosIILObjPtr getRootIIL(){return mRootIIL;}
	//void 				addTrans(const AosIILTransPtr &trans);
	void 				addTrans(const AosTransId &id);
	void 				finishTrans(); // Ken Lee, 2013/06/06
	u32 				getTransNum() { return mTrans.size(); }
	void				setPrevByIILMgr(const AosIILObjPtr &p) {mPrev = p;}
	void				setNextByIILMgr(const AosIILObjPtr &p) {mNext = p;}
	void				resetPtrsByIILMgr() {mPrev = 0; mNext = 0;}
	void 				setLastAddTimestamp(const bool add_to_head);

	bool				isBigIIL() 
	{
		return (mIILType == eAosIILType_BigStr || 
			mIILType == eAosIILType_BigU64 || 
			mIILType == eAosIILType_BigI64 || 
			mIILType == eAosIILType_BigHit || 
			mIILType == eAosIILType_BigD64);
	}

	virtual u64			getIILIDWithCompFlag();
	virtual u64			getIILIDForHash();
	virtual u32			removeRefCountByIILMgr();
	virtual bool		queryNewSafe(
							const AosQueryRsltObjPtr &query_rslt,
							const AosBitmapObjPtr &query_bitmap, 
							const AosQueryContextObjPtr &query_context,
							const AosRundataPtr &rdata) = 0;
	virtual bool		rebuildBitmapSafe(const AosRundataPtr &rdata);
	virtual bool		bitmapQueryNewSafe(
							const AosQueryRsltObjPtr &query_rslt,
							const AosBitmapObjPtr &query_bitmap, 
							const AosQueryContextObjPtr &query_context,
							const AosRundataPtr &rdata) = 0;

	virtual bool		bitmapRsltQueryNewSafe(
							const AosQueryRsltObjPtr &query_rslt,
							const AosBitmapObjPtr &query_bitmap, 
							const AosQueryContextObjPtr &query_context,
							const AosRundataPtr &rdata) = 0;
	virtual bool		saveToFileSafe(const AosRundataPtr &rdata);
	bool				saveToFilePriv(const AosRundataPtr &rdata);
	bool				saveToLocalFileSafe(const AosRundataPtr &rdata);
	bool				saveToLocalFilePriv(const AosRundataPtr &rdata);
	virtual bool		isCompIIL(){return false;}
	virtual void		resetSafe(
							const u64 &wordid, 
							const u64 &iilid,
							const u32 siteid,
							const AosRundataPtr &rdata);
	virtual void		resetPriv(
							const u64 &wordid, 
							const u64 &iilid,
							const u32 siteid,
							const AosRundataPtr &rdata);
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
	virtual bool		preQuerySafe(
							const AosQueryContextObjPtr &query_context,
							const AosRundataPtr &rdata) = 0;
	virtual bool		deleteIILSafe(
							const bool true_delete,
							const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = deleteIILPriv(true_delete, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}
	virtual bool		returnSubIILsSafe(const AosRundataPtr &rdata)
	{
		bool returned;
		return returnSubIILsSafe(returned, false, rdata);
	}
	virtual bool		returnSubIILsSafe(
							bool &returned,
							const bool returnHeader,
							const AosRundataPtr &rdata)
	{
        AOSLOCK(mLock);
        bool rslt = returnSubIILsPriv(returned, returnHeader, rdata);
        AOSUNLOCK(mLock);
		return rslt;		
	}
	virtual bool		loadFromFilePriv(
							const u64 &iilid, 
							const AosRundataPtr &rdata);
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

	bool				haveCompIIL() const 
						{
							return (mIILType == eAosIILType_Str || mIILType == eAosIILType_U64);
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

	//bool				loadCompFromFilePriv(
	//						AosBuffPtr &buff,		//Ketty 2012/11/15 
	//						const u32 siteid,
	//						const bool iilmgrLocked, 
	//						const AosRundataPtr &rdata);
	
	// Ketty 2013/03/20
	//bool				saveToTransFileSafe(
	//						const AosDocTransPtr &docTrans,
	//						const bool forcesave, 
	//						const AosRundataPtr &rdata);
protected:
	char * 				getMemory(
							const i64 &newsize, 
							const char *oldmem, 
							const i64 &oldsize);
	void				resetSelf(const AosRundataPtr &rdata);
	// Ketty 2013/01/15
	/*
	bool				loadFromFilePriv(
							const AosDfmDocIILPtr &doc,		// Ketty 2013/01/15 
							const u32 &siteid, 
							const bool iilmgrLocked, 
							const AosRundataPtr &rdata);
	bool				loadFromFilePriv(
							const u32 &siteid, 
							const bool, 
							const AosRundataPtr &rdata);
	*/

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
	// Ketty 2012/11/15
	bool				loadFromFileSafe(
							const u64 &iilid, 
							const AosRundataPtr &rdata);
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
	virtual i64			getNumDocsSafe() const 
						{
							AOSLOCK(mLock);
							i64 numdoc = mNumDocs;
							AOSUNLOCK(mLock);
							return numdoc;
						}

	// Chen Ding, 11/06/2012
	bool				isGood() const {return mDebugFlag == eGoodIILFlag && mIsGood;}
	u64					getDebugFlag() const {return mDebugFlag;}

protected:
	bool				resetWordPtrByIILMgr();
	bool				setPtrToWordHashByIILMgr();
	void				setIILIDByIILMgr(const u64 &iilid) {mIILID = iilid;}
	void				incIILIdx() {mIILIdx++;}
	bool				isNew(){return mIsNew;}
	
	bool				incNumEntriesNotSafe(const i64 &iilidx)
						{
							aos_assert_r(mNumEntries, false);
							aos_assert_r(iilidx >= 0 && iilidx < mNumSubiils, false);
							mNumEntries[iilidx]++;
							return true;
						}
	bool				decNumEntriesNotSafe(const i64 &iilidx)
						{
							aos_assert_r(mNumEntries, false);
							aos_assert_r(iilidx >= 0 && iilidx < mNumSubiils, false);
							mNumEntries[iilidx]--;
							return true;
						}
	bool				decNumEntriesNotSafe(const i64 &iilidx, const i64 &num)
						{
							aos_assert_r(mNumEntries, false);
							aos_assert_r(iilidx >= 0 && iilidx < mNumSubiils, false);
							mNumEntries[iilidx] -= num;
							return true;
						}
	bool				setNumEntriesNotSafe(const i64 &iilidx, const i64 &num)
						{
							aos_assert_r(mNumEntries, false);
							aos_assert_r(iilidx >= 0 && iilidx < mNumSubiils, false);
							mNumEntries[iilidx] = num;
							return true;
						}

	virtual bool		resetSubIILInfo(const AosRundataPtr &rdata);
	virtual bool		resetSpec();
	virtual bool		moveTo(const i64 &startIdx, i64 &idx, i64 &iilidx);

private:
	virtual AosBuffPtr	getBodyBuffProtected() const = 0;
	virtual bool 		setContentsProtected(AosBuffPtr &buff, const AosRundataPtr &rdata) = 0;

	virtual bool 		saveSanityCheckProtected(const AosRundataPtr &rdata) = 0;

	AosBuffPtr 			getBodyBuffMem(const i64 &size) const;
	
	//Ketty 2013/01/15
	//virtual AosIILObjPtr	createCompIIL(const AosRundataPtr &rdata);
	
private:
	//u64				getDisksize() const {return mIILDiskSize;}		Ketty 2012/11/15
	//u32				getSeqno() const {return mSeqno;}		Ketty 2012/11/15
	//u64				getOffset() const {return mOffset;}		Ketty 2012/11/15
	// u32				getTotalDocs() const {return mTotalDocs;}

	u32					getVersion()const{return mVersion;}
	void				setVersion(const u32 &ver){mVersion = ver;}
	bool				isPersis()const{return mIsPersis;}
	bool				returnMemory(const char *mem, const i64 &size);
	void				setWordId(const u64 &w) {mWordId = w;}
	//void				setSeqno(const u32 s) {mSeqno = s;}		Ketty 2012/11/15
	//void				setOffset(const u64 &s) {mOffset = s;}	Ketty 2012/11/15

	bool 				expandMemIIL();

protected:
	// Ketty 2012/11/19
	//virtual bool		saveSubIILToTransFileSafe(
	//						const AosDocTransPtr &docTrans,
	//						const bool forcesave, 
	//						const AosRundataPtr &rdata) = 0;
	virtual bool		saveSubIILToLocalFileSafe(const AosRundataPtr &rdata) = 0;

	/*
	virtual bool 		saveCompIILToTransFileSafe(
							const AosIILObjPtr &compiil,
							const bool forcesave, 
							const AosDocTransPtr &docTrans,
							const AosRundataPtr &rdata);
	*/

	// Ketty 2013/01/15
	//virtual bool 		saveCompIILToLocalFileSafe(
	//						const AosIILObjPtr &compiil,
	//						const bool forcesave, 
	//						const AosRundataPtr &rdata);
	bool				numEntriesIncrease(const i64 &index);
	bool				numEntriesDecrease(const i64 &index);
	virtual void    	failInLoadProtection();
	virtual void    	failInSetCtntProtection();

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
	//bool				getDataFromFile(
	//						AosBuffPtr &header,
	//						AosBuffPtr &body,
	//						const AosRundataPtr &rdata);

	void				clearTrans() { mTrans.clear(); }
	static bool			isCompIIL(const u64 &iilid) { return (iilid & eCompIILBitFlag); }

	// Chen Ding, 2015/05/25
	// static bool			staticCheckIILIDs(
	// 						const u64 &iilid, 
	// 						const u32 siteid,
	// 						i64 &num,
	// 						vector<bool> &iilid_states, 
	// 						const AosRundataPtr &rdata);
	
	static bool			staticReadHeaders(
							const u64 &iilid, 
							const u32 siteid, 
							const i64 &num, 
							AosBuff &buff, 
							const AosRundataPtr &rdata);

	// Chen Ding, 01/25/2012
	static bool			savingSanityCheck(
							const AosDocFileMgrObjPtr &docFileMgr,
							const u32 seqno, 
							const u64 &offset_old,
							const u64 &offset, 
							const char *data, 
							const i64 &docsize, 
							const i64 &compressed_size,
							const AosRundataPtr &rdata);

	// Chen Ding, 02/18/2012
	// static inline bool isPersisIIL(const u64 &iilid)
	// {
	// 	// In the current implementations, iilids are only four bytes and
	// 	// the highest bit is used to indicate whether it is a persistent
	// 	// IIL. In the future, we may want to expand IILIDs to be more than
	// 	// four bytes. 
	// 	return (iilid & ePersisBitOnIILIDFlag);
	// }
	
public:

	static bool			staticGetIILType(               
			    			AosIILType &iiltype,
							const u64 &iilid,
							const u64 &snap_id,
							const AosRundataPtr &rdata);

	virtual bool		deleteFromLocalFileByID(
							const u64 iilid,
							const u64 &snap_id,
							const AosRundataPtr &rdata);

private:
	//bool				setHeaderFromBuff(const AosBuffPtr &buff);	// Ketty 2013/01/15
	//bool				setHeaderToBuff(const AosBuffPtr &buff);		// Ketty 2013/01/15
	// Ketty 2013/01/15
	bool 				readFromDfmDoc(
							const AosDfmDocIILPtr &doc, 
							const AosRundataPtr &rdata);
	//void 				setNewDfmDoc(const AosDfmDocIILPtr doc);
	//bool				resetDfmDoc(const AosDfmDocIILPtr &doc);
	
	virtual bool		deleteIILPriv(
							const bool true_delete,
							const AosRundataPtr &rdata) = 0;
	virtual bool		returnSubIILsPriv(const AosRundataPtr &rdata) = 0;
	virtual bool		returnSubIILsPriv(
							bool &returned,
							const bool returnHeader,
							const AosRundataPtr &rdata) = 0;

public:
	// Chen Ding, 2013/03/01
	virtual bool		computeQueryResults(
							const AosRundataPtr &rdata, 
							const OmnString &iilname, 
							const AosQueryContextObjPtr &context, 
							const AosBitmapObjPtr &bitmap, 
							const u64 &query_id, 
							const int physical_id);


	virtual bool		batchAddSafe(
							char * &entries,
							const i64 &size,
							const i64 &num,
							const AosIILExecutorObjPtr &executor,
							const AosRundataPtr &rdata)
						{
							OmnNotImplementedYet;
							return false;
						}
	virtual bool		batchDelSafe(
							char * &entries,
							const i64 &size,
							const i64 &num,
							const AosIILExecutorObjPtr &executor,
							const AosRundataPtr &rdata)
						{
							OmnNotImplementedYet;
							return false;
						}
	virtual bool		batchIncSafe(
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
	virtual bool		batchDecSafe(
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
	virtual bool		getSplitValueSafe(
							const AosQueryContextObjPtr &context,
							const i64 &size,
							vector<AosQueryContextObjPtr> &contexts,
							const AosRundataPtr &rdata)
						{
							OmnNotImplementedYet;
							return false;
						}

	virtual AosIILObjPtr firstLeaf(AosIILIdx &idx,
								   AosBitmapTreeObjPtr &tree,
								   const AosRundataPtr &rdata)
						{
							OmnShouldNeverComeHere;
							return 0;
						}
	virtual AosIILObjPtr nextLeaf(AosIILIdx &idx,
								  AosBitmapTreeObjPtr &tree,
								  const AosRundataPtr &rdata)
						{
							OmnShouldNeverComeHere;
							return 0;
						}
	virtual AosIILObjPtr firstLeaf(AosIILIdx &idx,
								  const AosRundataPtr &rdata)
						{
							OmnShouldNeverComeHere;
							return 0;
						}
	virtual AosIILObjPtr nextLeaf(AosIILIdx &idx,
								  const AosRundataPtr &rdata)
						{
							OmnShouldNeverComeHere;
							return 0;
						}
	virtual void setSnapShotId(const u64 &snap_id) = 0;
	virtual void resetSnapShotId() = 0;
	virtual void setSnapShotIdPriv(const u64 &snap_id) = 0;
	virtual void resetSnapShotIdPriv() = 0;
	virtual u64 getSnapShotId(){return mSnapShotId;}
	virtual bool resetIIL(const AosRundataPtr &rdata) = 0; 

	virtual bool saveBitmapTree(const AosRundataPtr &rdata){return true;}

	// Chen Ding, 2013/10/26
	static int getNumIILsCreated();

private:
	bool addId(const AosRundataPtr &rdata);
	bool commitId(const AosRundataPtr &rdata);

public:
	static i64 getRsltNumDocs(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap)
	{
		aos_assert_r(query_rslt || query_bitmap, -1);
		if (query_rslt) return query_rslt->getNumDocs();
		return query_bitmap->getNumBits();
	}
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
	virtual bool	setCmpTag(
						const AosXmlTagPtr &cmp_tag,
						const AosRundataPtr &rdata) { return false; }
	virtual bool	batchAddSafe(
						const AosBuffArrayVarPtr &array,
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
};

bool inline AOS_IIL_INMEMORY(const u64 &ptr)
{
	return (ptr >> 32) & AOSIIL_INMEMORY_FLAG;
}

#endif
