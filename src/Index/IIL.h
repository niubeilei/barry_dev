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
// 11/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Index_IIL_h
#define Aos_Index_IIL_h

#include "SEInterfaces/IndexObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/OmnNew.h"

class AosIndex : virtual public OmnRCObject
{
public:
	enum
	{
		eMaxSubiils = 50000,
		eExtraDocids = 5020,
		eMemShreshold = 1000,
		eMinSubiilSize = 300,		// Chen Ding, 2011/01/25

		eInitNumDocids = 10,
		eNormalIILHeaderContentSize = 46,
		eFullIILHeaderContentSize = 62,
		eIILHeaderSize = 200,
		eIILHeaderMemsize = eIILHeaderSize-4,
		eCompIILBitFlag = 0x8000000000000000LL,
		ePersisBitOnVersionFlag = 0x80000000,
		ePersisBitOnIILIDFlag = 0x80000000,
		ePersisBitOnVidFlag = 0x80000000,

		eSuperIILTop 	= 3,
		eSuperIILLow 	= 2,
		eNormalRootIIL  = 1,
		eNormalIIL  	= 0
	};

private:
    u32             mRefcount;	// How many holding the IIL

protected:
	// The following member data are stored in the index record
	AosIILType		mIILType;
	u64				mIILID;		// The IILID
    u64     		mWordId;	// The wordId for the IIL
    u32     		mNumDocs;	// The number of DocIds currently for the IIL
    char      		mFlag;		// Indicate whether the IIL is active
    u32             mHitcount;	// The IIL hit count

	//u32			mTotalDocs;	// The total docs (including multiple IILs)

	u32				mVersion;		// The total docs (including multiple IILs)
	bool			mIsPersis;		// Whether the IIL is persistant or not

	u32     		mIILDiskSize;	// Size in disk
    u64             mOffset;		// The offset in file
    u32             mSeqno;			// The file seqno
	OmnString		mIILName;		// The name of the IIL(the word the IIL using for)
	// The following member data are not stored in index record
	u32				mMemCap;	// Memory allocated for the IIL
    u64 *           mDocids;
	OmnMutexPtr		mLock;
	AosIILPtr		mPrev;
	AosIILPtr		mNext;
	bool			mIsDirty;
	bool			mIsNew;
	bool			mIsGood;

	// Subiil related data
	AosIILPtr		mRootIIL;
	u32				mNumSubiils;
	int				mIILIdx;				// Subiil seqno, or ID.
	int *			mNumEntries;

	static int 		mTotalIILs[eAosIILType_Total];

	//ken 2011/08/19
	vector<u64>		mTrans;
	AosBuffPtr 		mBodyBuff;
	u64 *			mIILIds;
	u64				mLastAddSec;
	bool			mLastAddToHead;
	bool			mCreationOnly;			// Chen Ding, 10/15/2011
	u32				mSiteid;				//ken 2012/02/02

	// SUPERIIL
	int				mLevel;					// Chen Ding, 04/09/2012

protected:

	static u32		mExtraDocids;	
	static u32		mExtraDocidsInit;
	static u32		mMaxIILSize;
	static u32		mMinSubIILSize;
	static bool		mIILMergeFlag;

	static AosMemChecker	mMemChecker;

public:
    AosIIL(const AosIILType type, const bool isPersis);
	AosIIL(
		const u64 &wordid,
		const u64 &iilid,
		const AosIILType type,
		const bool isPersis,
		const OmnString &iilname);
	~AosIIL();

	// Static Functions. All static member functions are thread safe.
	static int			staticNumIILRefs();
	static bool			staticReadHeaderBuff(
							const u64 &iilid, 
							const u32 &siteid, 
							AosBuff &buff, 
							const AosRundataPtr &rdata);
	static bool			staticInit(const AosXmlTagPtr &config);
	static AosIILPtr 	staticCreateIIL(
							const u64 &wordid, 
							const u64 &iilid, 
							const u32 siteid, 
							const AosIILType iiltype,
							const bool is_persis,
							const OmnString &iilname, 
							const bool iilmgrLocked,
							const AosRundataPtr &rdata);

	static AosIILPtr	staticLoadFromFile1(
							const u64 &iilid, 
							const u32 siteid, 
							const bool iscompiil,
							const bool iilmgrLocked, 
							AosBuff &buff, 
							const AosRundataPtr &rdata);
	static AosIILPtr	staticLoadFromFile1(
							const u64 &iilid, 
							const u32 &siteid, 
							const bool iscompiil,
							const bool iilmgrLocked, 
							const AosRundataPtr &rdata);

	static bool			staticStop();
	static bool 		staticIsIILIDUsed(const u64 &iilid, 
										  const u32 siteid, 
										  const AosRundataPtr &rdata);
	static int			getTotalIILs(const AosIILType iiltype){return mTotalIILs[iiltype];}
	static bool 		isIILMerged(){bool rslt = mIILMergeFlag;mIILMergeFlag = false;return rslt;}
	
	static AosDocFileMgrObjPtr getDocFileMgr(
								const int &vid,
								const u32 siteid,
								const AosRundataPtr &rdata);

	static inline void setCompIILIdBit(u64 &iilid)
	{
		iilid |= eCompIILBitFlag;
	}

	static bool saveToFileSafeStatic(
						const u64 &root_iilid,
						const u32 siteid, 
						const AosBuffPtr &headbuff, 
						const AosBuffPtr &bodybuff, 
						const AosRundataPtr &rdata);

	static bool	staticIsIILCreated(
					const u64 &iilid, 
					const u32 siteid,
					const AosIILType type,
					const AosRundataPtr &rdata);

public:
	bool 		isRootIIL()const{return mNumSubiils > 0 && mIILIdx == 0;}
	bool inline isSingleIIL()const{return mNumSubiils == 0 && mIILIdx <=0;}
	u32 		getNumDocs() const {return mNumDocs;}
	bool 		isLeafIIL()const{return mIILIdx > 0;}
	AosIILType	getIILType() const {return mIILType;}
	u64			getIILID() const {return mIILID;}
	void		setIILIdx(const int idx) {mIILIdx = idx;}
	// void		setIILIdx(const u32 idx) {mIILIdx = idx;}
	int			getIILIdx() const {return mIILIdx;}
	u32			getNumSubiils() const {return mNumSubiils;}
	void		setRootIIL(AosIILPtr rootiil){mRootIIL = rootiil;}
	bool		isDirty() const {return mIsDirty;}
	void		setCreationOnly(const bool b) {mCreationOnly = b;}
	void		setNew(const bool isnew){mIsNew = isnew;}
	void		setDirty(const bool dirty){mIsDirty = dirty;}
	AosIILPtr	getNextByIILMgr() const {return mNext;}
	bool 		isLastAddToHead() const {return mLastAddToHead;}
	u64  		getLastAddTimestamp() const {return mLastAddSec;}
	u32			getRefCountByIILMgr() const {return mRefcount;}
	void 		lockIIL() {AOSLOCK(mLock);}
	void 		unlockIIL() {AOSUNLOCK(mLock);}
	bool		finishTransSafe(const AosRundataPtr &rdata);
	u32			getSiteid() const { return mSiteid; };
	u64 		getCompIILID(){return mIILID | eCompIILBitFlag;}
	bool		isCreationOnly() const {return mCreationOnly;}
	u32			addRefCountByIILMgr();
	AosIILPtr	getPrevByIILMgr() const {return mPrev;}
	AosIILPtr	getRootIIL(){return mRootIIL;}
	void 		addTrans(const AosIILTransPtr &trans);
	u32 		getTransNum() { return mTrans.size(); }
	void		setPrevByIILMgr(const AosIILPtr &p) {mPrev = p;}
	void		setNextByIILMgr(const AosIILPtr &p) {mNext = p;}
	void		resetPtrsByIILMgr() {mPrev = 0; mNext = 0;}
	void 		setLastAddTimestamp(const bool add_to_head);

	virtual u64		getIILIDWithCompFlag();
	virtual u64		getIILIDForHash();
	virtual u32		removeRefCountByIILMgr();
	virtual bool	queryNewSafe(
						const AosQueryRsltPtr &query_rslt,
						const AosBitmapPtr &query_bitmap, 
						const AosQueryContextPtr &query_context,
						const AosRundataPtr &rdata) = 0;
	virtual bool	saveToFileSafe(
					const bool iillocked,
					const bool iilmgrLocked,
					const bool force, 
					const AosRundataPtr &rdata);
	virtual bool	isCompIIL(){return false;}
	virtual void	resetSafe(const u64 &wordid, 
							const u64 &iilid,
							const u32 siteid,
							const bool iilmgrLocked, 
							const AosRundataPtr &rdata);
	virtual bool	queryRangeSafe(
						const AosQueryRsltPtr &query_rslt,
						const AosBitmapPtr &query_bitmap,
						const AosQueryContextPtr &query_context,
						const AosRundataPtr &rdata);
	virtual bool	querySafe(
						const AosQueryRsltPtr &query_rslt,
						const AosBitmapPtr &query_bitmap,
						const AosQueryContextPtr &query_context,
						const AosRundataPtr &rdata) = 0;
	virtual bool	preQuerySafe(
						const AosQueryContextPtr &query_context,
						const AosRundataPtr &rdata) = 0;
	virtual bool	deleteIILSafe(
						const bool iilmgrLocked,
						const AosRundataPtr &rdata) = 0;
	virtual bool	loadFromFileLocked(
							const u64 &iilid, 
						  	const u32 &siteid, 
							const bool iilmgrLocked, 
							const AosRundataPtr &rdata);
	virtual bool	returnSubIILsSafe(
						const bool iilmgrLocked, 
						bool &returned, 
						const AosRundataPtr &rdata, 
						const bool returnHeader) = 0;
	virtual bool	returnSubIILsPriv(
						const bool iilmgrLocked, 
						bool &returned, 
						const AosRundataPtr &rdata, 
						const bool returnHeader) = 0;
	virtual u64		nextDocIdSafe2(int &idx,int &iilidx,const bool reverse);
	virtual u64		nextDocIdPriv2(int &idx,int &iilidx,const bool reverse);

	u32 getNumDocsSafe() const 
	{
		mLock->lock();
		u32 numdoc = mNumDocs;
		mLock->unlock();
		return numdoc;
	}

	bool incNumDocsNotSafe()
	{
		mNumDocs++;
		if (mRootIIL.notNull())
		{
			aos_assert_r(mRootIIL, false);
			mRootIIL->incNumEntriesNotSafe(mIILIdx);
		}
		return true;
	}

	bool haveCompIIL() const 
	{
		return (mIILType == eAosIILType_Str || mIILType == eAosIILType_U64);
	}

protected:
	bool 		saveSanityCheck(const AosIILPtr &iil, const AosRundataPtr &rdata); 
	bool 		setDocidContents(AosBuffPtr &buff);
	bool 		expandDocidMem();
	bool		saveFullHeader(const AosRundataPtr &rdata);
	bool		deleteFromLocalFilePriv(const AosRundataPtr &rdata);

	virtual bool	prepareMemoryForReloading();
	virtual u64		getDocIdSafe(const int idx,const AosRundataPtr &rdata) const;
	virtual u64		getDocIdSafe1(int &idx,int &iilidx,const AosRundataPtr &rdata);
	virtual bool 	adjustMemoryProtected();
	virtual u64		getDocIdPriv(const int idx,const AosRundataPtr &rdata) const;
	virtual bool	copyDocidsPriv(const AosQueryRsltPtr &query_rslt,
								   const AosBitmapPtr &query_bitmap,
								   const int start,
								   const int end);

	bool		loadCompFromFilePriv(
					AosBuff &buff, 
					const u32 siteid,
					const bool iilmgrLocked, 
					const AosRundataPtr &rdata);
	bool		saveToTransFileSafe(
					const AosDocTransPtr &docTrans,
					const bool forcesave, 
					const AosRundataPtr &rdata);
	bool		saveToLocalFileSafe(
					const bool forcesave, 
					const AosRundataPtr &rdata);
	char * 		getMemory(const u32 newsize, 
					const char *oldmem, 
					const u32 oldsize);
	bool		loadFromFilePriv(
					const u64 &iilid, 
					const u32 &siteid, 
					const bool iilmgrLocked, 
					AosBuff &buff, 
					const AosRundataPtr &rdata);
	bool		loadFromFilePriv(
					AosBuff &buff, 
					const u32 &siteid, 
					const bool, 
					const AosRundataPtr &rdata);

	bool setNumDocsNotSafe(const u32 nn)
	{
		mNumDocs = nn;
		if (mRootIIL.notNull())
		{
			aos_assert_r(mRootIIL, false);
			mRootIIL->setNumEntriesNotSafe(mIILIdx, nn);
		}
		return true;
	}

	bool decNumDocsNotSafe()
	{
		mNumDocs--;
		if (mRootIIL.notNull())
		{
			aos_assert_r(mRootIIL, false);
			mRootIIL->decNumEntriesNotSafe(mIILIdx);
		}
		return true;
	}

	virtual bool copyDocidsSafe(const AosQueryRsltPtr   &query_rslt,
								   const AosBitmapPtr &query_bitmap,
								   const int start,
								   const int end)
	{
		AOSLOCK(mLock);
		bool rslt = copyDocidsPriv(query_rslt, query_bitmap, start, end);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool decNumDocsNotSafe(const int num)
	{
		mNumDocs -= num;
		if (mRootIIL.notNull())
		{
			aos_assert_r(mRootIIL, false);
			mRootIIL->decNumEntriesNotSafe(mIILIdx);
		}
		return true;
	}

private:
	void			setIILID(const u64 &iilid){mIILID = iilid;};
	virtual u64		getMinDocid() const = 0;
	virtual u64		getMaxDocid() const = 0;
	virtual bool 	isIdxValidSafe(const u32 idx)
					{
						AOSLOCK(mLock);
						bool rslt = (idx >= 0 && (u32)idx < mNumDocs);
						AOSUNLOCK(mLock);
						return rslt;
					}
	bool			loadFromFileSafe1(
							const u64 &iilid, 
						    const u32 siteid, 
							const bool iilmgrLocked, 
							AosBuff &buff, 
							const AosRundataPtr &rdata);
	bool			loadFromFileSafe1(
							const u64 &iilid, 
						  	const u32 &siteid, 
							const bool iilmgrLocked, 
							const AosRundataPtr &rdata);
	u64				getWordIdSafe() const {return mWordId;}

	virtual int	getTotalSafe(const OmnString &value, const AosOpr opr)
	{
		OmnShouldNeverComeHere;
		return -1;
	}

	virtual int	getTotalSafe(const u64 &value, const AosOpr opr)
	{
		OmnShouldNeverComeHere;
		return -1;
	}

	virtual bool checkDocSafe(
		const AosOpr opr,
		const OmnString &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
	{
		OmnShouldNeverComeHere;
		return false;
	}

	virtual bool checkDocSafe(
		const AosOpr opr,
		const u32 &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
	{
		OmnShouldNeverComeHere;
		return false;
	}

	virtual bool	docExistSafe(const u64 &docId, const AosRundataPtr &rdata) const;
	virtual u32		getTotalNumDocsSafe()const
	{
		mLock->lock();
		u32 numdoc = mNumDocs;
		mLock->unlock();
		return numdoc;
	}
	
	bool			resetWordPtrByIILMgr();
	bool			setPtrToWordHashByIILMgr();
	void			setIILIDByIILMgr(const u64 &iilid) {mIILID = iilid;}
	void			incIILIdx() {mIILIdx++;}
	bool			isGood() const {return mIsGood;}
	bool			isNew(){return mIsNew;}
	
	void			setSiteid(const u32 siteid) { mSiteid = siteid; };

	bool	incNumEntriesNotSafe(const int iilidx)
			{
				aos_assert_r(mNumEntries, false);
				aos_assert_r(iilidx >= 0 && iilidx < (int)mNumSubiils, false);
				mNumEntries[iilidx]++;
				return true;
			}
	bool	decNumEntriesNotSafe(const int iilidx)
			{
				aos_assert_r(mNumEntries, false);
				aos_assert_r(iilidx >= 0 && iilidx < (int)mNumSubiils, false);
				mNumEntries[iilidx]--;
				return true;
			}
	bool	setNumEntriesNotSafe(const int iilidx, const int nn)
			{
				aos_assert_r(mNumEntries, false);
				aos_assert_r(iilidx >= 0 && iilidx < (int)mNumSubiils, false);
				mNumEntries[iilidx] = nn;
				return true;
			}

	void			setPersis(const bool isper){mIsPersis = isper;}

	virtual bool	resetSubIILInfo(const AosRundataPtr &rdata);
	virtual bool	resetSpec();
	virtual bool	moveTo(const int startIdx,int &idx,int &iilidx);

	virtual AosBuffPtr	getBodyBuffProtected() const = 0;
	virtual bool 		setContentsProtected(AosBuffPtr &buff, const bool, const AosRundataPtr &rdata) = 0;

	virtual bool 		saveSanityCheckProtected(const AosRundataPtr &rdata) = 0;

	void		resetSelf(const bool iilmgrLocked, const AosRundataPtr &rdata);
	AosBuffPtr 	getBodyBuffMem(const u32 size) const;
	virtual AosIILPtr	createCompIIL(const AosRundataPtr &rdata);
	bool 		setHeaderToBuff(AosBuff &buff);
	
	u64			getDisksize() const {return mIILDiskSize;}
	u32			getSeqno() const {return mSeqno;}
	u64			getOffset() const {return mOffset;}
	u32			getVersion()const{return mVersion;}
	void		setVersion(const u32 &ver){mVersion = ver;}
	bool		isPersis()const{return mIsPersis;}
	bool		returnMemory(const char *mem, const u32 size);
	void		setWordId(const u64 &w) {mWordId = w;}
	void		setSeqno(const u32 s) {mSeqno = s;}
	void		setOffset(const u64 &s) {mOffset = s;}
	bool 		expandMemIIL();

	virtual bool	saveSubIILToTransFileSafe(
						const AosDocTransPtr &docTrans,
						const bool forcesave, 
						const AosRundataPtr &rdata) = 0;

	virtual bool	saveSubIILToLocalFileSafe(
						const bool forcesave, 
						const AosRundataPtr &rdata) = 0;

	virtual bool 	saveCompIILToTransFileSafe(
						const AosIILPtr &compiil,
						const bool forcesave, 
						const AosDocTransPtr &docTrans,
						const AosRundataPtr &rdata);

	virtual bool 	saveCompIILToLocalFileSafe(
						const AosIILPtr &compiil,
						const bool forcesave, 
						const AosRundataPtr &rdata);

	bool			numEntriesIncrease(const int index);
	bool			numEntriesDecrease(const int index);
	virtual void    failInLoadProtection();
	virtual void    failInSetCtntProtection();

	bool		saveToFilePriv(
					const AosDocTransPtr &docTrans,
					const bool iilmgrLocked,
					const bool forcesave,
					const AosRundataPtr &rdata);
	bool		saveToTransFilePriv(
					const AosDocTransPtr &docTrans,
					const bool forcesave, 
					const AosRundataPtr &rdata);
	bool		saveToLocalFilePriv(
					const bool forcesave, 
					const AosRundataPtr &rdata);

	virtual bool	nextDocidSafe(
						int &idx, 
						int &iilidx,
						const bool reverse, 
						const AosOpr opr,
						u64 &value,
						u64 &docid, 
						bool &isunique)
					{ 
						return true;
					}
	bool getDataFromFile(AosBuffPtr &header, AosBuffPtr &body, const AosRundataPtr &rdata);

	void clearTrans() { mTrans.clear(); }
	static bool isCompIIL(const u64 &iilid) { return (iilid & eCompIILBitFlag); }
	bool	setNewHeaderToBuff(AosBuff &buff);
	bool	setCompHeaderToBuff(AosBuff &buff);
	bool	setCompLeafHeaderToBuff(AosBuff &buff);
	bool	setNormalHeaderToBuff(AosBuff &buff);
	bool 	saveRootToFileSafe(const AosRundataPtr &rdata);

	static bool staticCheckIILIDs(
					const u64 &iilid, 
					const u32 siteid,
					int &num,
					vector<bool> &iilid_states, 
					const AosRundataPtr &rdata);
	static bool staticReadHeaders(
					const u64 &iilid, 
					const u32 &siteid, 
					const int num, 
					AosBuff &buff, 
					const AosRundataPtr &rdata);

	// Chen Ding, 01/25/2012
	static bool savingSanityCheck(
					const AosDocFileMgrObjPtr &docFileMgr,
					const u32 seqno, 
					const u64 &offset_old,
					const u64 &offset, 
					const char *data, 
					const u32 docsize, 
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
};

bool inline AOS_IIL_INMEMORY(const u64 &ptr)
{
	return (ptr >> 32) & AOSIIL_INMEMORY_FLAG;
}
#endif
