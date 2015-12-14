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
#ifndef Omn_IILMgrBig_IIL_h
#define Omn_IILMgrBig_IIL_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "DocTrans/Ptrs.h"
#include "IDTransMap/Ptrs.h"
#include "IILClient/Ptrs.h"
#include "IILMgrBig/DfmDocIIL.h"
#include "IILMgrBig/Ptrs.h"
#include "IILMgrBig/IILDistrPolicy.h"
#include "IILUtil/Ptrs.h"
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
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "XmlUtil/XmlTag.h"

#include <stdio.h>
#include <string.h>

class AosBuff;
class AosIILMgr;
const u32 AOSIIL_INMEMORY_FLAG = 0x80000000;


class AosIIL : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eMinIILSize = 500,
		eMaxIILSize = 20000,
		eMinSubIILs = 500,
		eMaxSubIILs = 20000,

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
		eCompressSize   = 10000 // 10k Linda, 11/21/2012
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

	//u32     		mIILDiskSize;	// Size in disk				// Ketty 2012/11/15
    //u64             mOffset;		// The offset in file		// Ketty 2012/11/15
    //u32             mSeqno;			// The file seqno		// Ketty 2012/11/15
	u32				mCompressedSize; // iil compressed size
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
	u64 *			mIILIds;
	u64				mLastAddSec;
	bool			mLastAddToHead;
	bool			mCreationOnly;			// Chen Ding, 10/15/2011
	u32				mSiteid;				//ken 2012/02/02

	AosDfmDocIILPtr	mDfmDoc;				// Ketty 2012/11/15
	static AosDfmDocIILPtr	smDfmDoc;		// Chen Ding, 2013/01/12
	static bool		smNeedCompress;			// Chen Ding DDDDD, 2013/01/26
	static u32		smCompressSize;			// Chen Ding DDDDD, 2013/01/26

	u64				mDebugFlag;				// Debugging only

protected:

	static u32		mExtraDocids;	
	static u32		mExtraDocidsInit;

	static u32		mMaxIILSize;
	static u32		mMinIILSize;
	static u32		mMaxSubIILs;
	static u32		mMinSubIILs;

	static u32		mAddBlockMaxSize;	// Ken Lee, 2013/01/03
	static u32 		mAddBlockMinSize;	// Ken Lee, 2013/01/03
	static u32		mAddBlockFillSize;	// Ken Lee, 2013/01/03

	static bool		mIILMergeFlag;

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
	//static bool			staticReadHeaderBuff(
	//						const u64 &iilid, 
	//						const u32 &siteid, 
	//						AosBuff &buff, 
	//						const AosRundataPtr &rdata);
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
							const bool iilmgrLocked, 
							const AosRundataPtr &rdata);

	static bool			staticStop();
	static bool 		staticIsIILIDUsed(const u64 &iilid, 
										  const AosRundataPtr &rdata);
	static int			getTotalIILs(const AosIILType iiltype){return mTotalIILs[iiltype];}
	static bool 		isIILMerged(){bool rslt = mIILMergeFlag;mIILMergeFlag = false;return rslt;}
	
	static AosDocFileMgrObjPtr getDocFileMgr(
							const u64 &iilid,
							u64 &local_iilid,
							const AosRundataPtr &rdata);

public:
	virtual bool 	isLeafIIL()const{return mIILIdx > 0;}
	virtual bool 	isRootIIL()const{return mNumSubiils > 0 && mIILIdx == 0;}
	virtual bool inline 	isSingleIIL()const{return mNumSubiils == 0 && mIILIdx <=0;}
	virtual bool 	isParentIIL()const{return isRootIIL();}
	virtual AosIILPtr	getRootIIL(){return mRootIIL;}

	// Public get's functions
	u32			getNumSubiils() const {return mNumSubiils;}
	u64  		getLastAddTimestamp() const {return mLastAddSec;}
	u32			getRefCountByIILMgr() const {return mRefcount;}
	u64			getIILID() const {return mIILID;}
	u64 		getCompIILID(){return mIILID | eCompIILBitFlag;}
	AosIILPtr	getPrevByIILMgr() const {return mPrev;}
	AosIILType	getIILType() const {return mIILType;}
	u32 		getTransNum() { return mTrans.size(); }
	AosIILPtr	getNextByIILMgr() const {return mNext;}
	u32			getSiteid() const { return mSiteid; };

	// Public set's functions
	void	setRootIIL(AosIILPtr rootiil){mRootIIL = rootiil;}
	void	setIILIdx(const int idx) {mIILIdx = idx;}
	void	setDirty(const bool dirty){mIsDirty = dirty;}
	void	setNew(const bool isnew){mIsNew = isnew;}
	void	setCreationOnly(const bool b) {mCreationOnly = b;}
	void	setIILID(const u64 &iilid){mIILID = iilid;};
	void	setPrevByIILMgr(const AosIILPtr &p) {mPrev = p;}
	void	setNextByIILMgr(const AosIILPtr &p) {mNext = p;}
	void 	setLastAddTimestamp(const bool add_to_head);
	
	int		getIILIdx() const {return mIILIdx;}

	// Public functions
	bool 	isLastAddToHead() const {return mLastAddToHead;}
	bool	isDirty(){return mIsDirty;}
	bool	isCreationOnly() const {return mCreationOnly;}
	void 	lockIIL() {AOSLOCK(mLock);}
	void 	unlockIIL() {AOSUNLOCK(mLock);}
	bool	finishTransSafe(const AosRundataPtr &rdata);
	u32		addRefCountByIILMgr();
	void 	addTrans(const AosIILTransPtr &trans);
	void	resetPtrsByIILMgr() {mPrev = 0; mNext = 0;}

	bool	haveCompIIL() const 
	{
		return (mIILType == eAosIILType_Str || mIILType == eAosIILType_U64);
	}

	virtual bool	isCompIIL(){return false;}
	virtual u32		removeRefCountByIILMgr();
	virtual u64		getIILIDForHash();
	virtual bool	returnSubIILsPriv(
						const bool iilmgrLocked, 
						bool &returned, 
						const AosRundataPtr &rdata, 
						const bool returnHeader) = 0;
	virtual bool	returnSubIILsSafe(
						const bool iilmgrLocked, 
						bool &returned, 
						const AosRundataPtr &rdata, 
						const bool returnHeader) = 0;

	virtual bool	preQuerySafe(
						const AosQueryContextObjPtr &query_context,
						const bool iilmgrlock, 
						const AosRundataPtr &rdata) = 0;

	static inline void setCompIILIdBit(u64 &iilid)
	{
		iilid |= eCompIILBitFlag;
	}

	//static bool saveToFileSafeStatic(
	//					const u64 &root_iilid,
	//					const u32 siteid, 
	//					const AosBuffPtr &headbuff, 
	//					const AosBuffPtr &bodybuff, 
	//					const AosRundataPtr &rdata);

	static bool	staticIsIILCreated(
					const u64 &iilid, 
					const u32 siteid,
					const AosIILType type,
					const AosRundataPtr &rdata);

	virtual bool	queryNewSafe(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata) = 0;

	virtual u64		getIILIDWithCompFlag();
	//virtual bool	loadFromFileLocked(
	//						const u64 &iilid, 
	//					  	const u32 &siteid, 
	//						const bool iilmgrLocked, 
	//						const AosRundataPtr &rdata);

	virtual bool	loadFromFilePriv(
							const u64 &iilid, 
							const bool iilmgrLocked, 
							const AosRundataPtr &rdata);

	virtual void	resetSafe(const u64 &wordid, 
							const u64 &iilid,
							const u32 siteid,
							const bool iilmgrLocked, 
							const AosRundataPtr &rdata);

	virtual bool	saveToFileSafe(
					const bool iillocked,
					const bool iilmgrLocked,
					const bool force, 
					const AosRundataPtr &rdata);

	virtual bool	querySafe(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata) = 0;

	virtual bool	queryRangeSafe(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);
protected:
	// Thread-Safe Functions
	virtual u64		getMinDocid() const = 0;
	virtual u64		getMaxDocid() const = 0;
	virtual u64		getDocIdSafe(const int idx,const AosRundataPtr &rdata) const;

	virtual u64		nextDocIdSafe2(int &idx,int &iilidx,const bool reverse);
	virtual u64		nextDocIdPriv2(int &idx,int &iilidx,const bool reverse);

	virtual u64		getDocIdPriv(const int idx,const AosRundataPtr &rdata) const;
	virtual u64		getDocIdSafe1(int &idx,int &iilidx,const AosRundataPtr &rdata);
	virtual bool 	isIdxValidSafe(const u32 idx)
					{
						AOSLOCK(mLock);
						bool rslt = (idx >= 0 && (u32)idx < mNumDocs);
						AOSUNLOCK(mLock);
						return rslt;
					}

	bool			loadFromFileSafe1(
							const u64 &iilid, 
							const bool iilmgrLocked, 
							const AosRundataPtr &rdata);
	u64				getWordIdSafe() const {return mWordId;}

	// Chen Ding, 04/18/2012
	// virtual int	getTotalSafe(const OmnString &value, const AosOpr opr)
	virtual u64	getTotalSafe(
			const OmnString &value, 
			const AosOpr opr, 
			const AosRundataPtr &rdata)
	{
		OmnShouldNeverComeHere;
		return -1;
	}

	// Chen Ding, 04/18/2012
	// virtual int	getTotalSafe(const u64 &value, const AosOpr opr)
	virtual u64 getTotalSafe(
			const u64 &value, 
			const AosOpr opr, 
			const AosRundataPtr &rdata)
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

	// Following functions are not supposed to be called by other 
	virtual bool	docExistSafe(const u64 &docId, const AosRundataPtr &rdata) const;
	virtual bool	prepareMemoryForReloading();
	u32 			getNumDocs() const {return mNumDocs;}
	virtual u32		getTotalNumDocsSafe()const
	{
		mLock->lock();
		u32 numdoc = mNumDocs;
		mLock->unlock();
		return numdoc;
	}
	
public:
	u32				getNumDocsSafe() const 
	{
		mLock->lock();
		u32 numdoc = mNumDocs;
		mLock->unlock();
		return numdoc;
	}

	// Chen Ding, 11/06/2012
	bool			isGood() const {return mDebugFlag == eGoodIILFlag && mIsGood;}
	u64				getDebugFlag() const {return mDebugFlag;}

protected:
	// These functions shall be called by IILMgr only and 
	// the lock should be locked.

	bool			resetWordPtrByIILMgr();
	bool			setPtrToWordHashByIILMgr();
	void			setIILIDByIILMgr(const u64 &iilid) {mIILID = iilid;}
	void			incIILIdx() {mIILIdx++;}
	void			setIILIdx(const u32 idx) {mIILIdx = idx;}
	bool			isNew(){return mIsNew;}
	
	void			setSiteid(const u32 siteid) { mSiteid = siteid; };

	bool	incNumDocsNotSafe()
			{
				mNumDocs++;
				if (mRootIIL.notNull())
				{
					aos_assert_r(mRootIIL, false);
					mRootIIL->incNumEntriesNotSafe(mIILIdx);
				}
				return true;
			}
	bool	decNumDocsNotSafe()
			{
				mNumDocs--;
				if (mRootIIL.notNull())
				{
					aos_assert_r(mRootIIL, false);
					mRootIIL->decNumEntriesNotSafe(mIILIdx);
				}
				return true;
			}
	bool	decNumDocsNotSafe(const int num)
			{
				mNumDocs -= num;
				if (mRootIIL.notNull())
				{
					aos_assert_r(mRootIIL, false);
					mRootIIL->decNumEntriesNotSafe(mIILIdx);
				}
				return true;
			}
	bool	setNumDocsNotSafe(const u32 nn)
			{
				mNumDocs = nn;
				if (mRootIIL.notNull())
				{
					aos_assert_r(mRootIIL, false);
					mRootIIL->setNumEntriesNotSafe(mIILIdx, nn);
				}
				return true;
			}
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
	virtual bool	copyDocidsSafe(const AosQueryRsltObjPtr   &query_rslt,
								   const AosBitmapObjPtr &query_bitmap,
								   const int start,
								   const int end)
	{
		AOSLOCK(mLock);
		bool rslt = copyDocidsPriv(query_rslt, query_bitmap, start, end);
		AOSUNLOCK(mLock);
		return rslt;
	}


protected:
	virtual bool	copyDocidsPriv(const AosQueryRsltObjPtr &query_rslt,
								   const AosBitmapObjPtr &query_bitmap,
								   const int start,
								   const int end);

	virtual AosBuffPtr	getBodyBuffProtected() const = 0;
	virtual bool 		setContentsProtected(AosBuffPtr &buff, const bool, const AosRundataPtr &rdata) = 0;

	// Virtual Functions
	virtual bool 		adjustMemoryProtected();
	virtual bool 		saveSanityCheckProtected(const AosRundataPtr &rdata) = 0;

	char * 		getMemory(const u32 newsize, 
					const char *oldmem, 
					const u32 oldsize);
	void		resetSelf(const bool iilmgrLocked, const AosRundataPtr &rdata);
	bool 		expandDocidMem();
	bool 		setDocidContents(AosBuffPtr &buff);
	AosBuffPtr 	getBodyBuffMem(const u32 size) const;
	bool 		saveSanityCheck(const AosIILPtr &iil, const AosRundataPtr &rdata); 
	//bool		loadFromFilePriv(
	//				AosBuff &buff, 
	//				const u32 &siteid, 
	//				const bool, 
	//				const AosRundataPtr &rdata);
	//bool		loadCompFromFilePriv(
	//				AosBuff &buff, 
	//				const u32 siteid,
	//				const bool iilmgrLocked, 
	//				const AosRundataPtr &rdata);
	//virtual AosIILPtr	createCompIIL(const AosRundataPtr &rdata);
	//bool 		setHeaderToBuff(AosBuff &buff);
	//bool		saveFullHeader(const AosRundataPtr &rdata);
	
private:
	//u64			getDisksize() const {return mIILDiskSize;}
	//u32			getSeqno() const {return mSeqno;}
	//u64			getOffset() const {return mOffset;}

	u32			getVersion()const{return mVersion;}
	void		setVersion(const u32 &ver){mVersion = ver;}
	bool		isPersis()const{return mIsPersis;}
	
	/*
	bool		isIILInMemory(const u64 &ptr)
				{ return (ptr >> 32) & AOSIIL_INMEMORY_FLAG;}
	*/

	bool		returnMemory(const char *mem, const u32 size);

	void		setWordId(const u64 &w) {mWordId = w;}
	//void		setSeqno(const u32 s) {mSeqno = s;}
	//void		setOffset(const u64 &s) {mOffset = s;}

	bool 		expandMemIIL();

	static bool		isCompress(const u32 &size);

protected:
	//virtual bool	saveSubIILToTransFileSafe(
	//					const AosDocTransPtr &docTrans,
	//					const bool forcesave, 
	//					const AosRundataPtr &rdata) = 0;

	virtual bool	saveSubIILToLocalFileSafe(
						const bool forcesave, 
						const AosRundataPtr &rdata) = 0;

	//virtual bool 	saveCompIILToTransFileSafe(
	//					const AosIILPtr &compiil,
	//					const bool forcesave, 
	//					const AosDocTransPtr &docTrans,
	//					const AosRundataPtr &rdata);

	//virtual bool 	saveCompIILToLocalFileSafe(
	//					const AosIILPtr &compiil,
	//					const bool forcesave, 
	//					const AosRundataPtr &rdata);

	bool			numEntriesIncrease(const int index);
	bool			numEntriesDecrease(const int index);
	
	virtual void    failInLoadProtection();
	virtual void    failInSetCtntProtection();

	//bool		saveToFilePriv(
	//				const AosDocTransPtr &docTrans,
	//				const bool iilmgrLocked,
	//				const bool forcesave,
	//				const AosRundataPtr &rdata);
	//bool		saveToTransFileSafe(
	//				const AosDocTransPtr &docTrans,
	//				const bool forcesave, 
	//				const AosRundataPtr &rdata);
	//bool		saveToTransFilePriv(
	//				const AosDocTransPtr &docTrans,
	//				const bool forcesave, 
	//				const AosRundataPtr &rdata);
	bool 		saveToFilePriv(
					const bool iilmgrLocked,
					const bool forcesave,
					const AosRundataPtr &rdata); 
	bool		saveToLocalFileSafe(
					const bool forcesave, 
					const AosRundataPtr &rdata);
	bool		saveToLocalFilePriv(
					const bool forcesave, 
					const AosRundataPtr &rdata);

protected:
	// James, 11/12/2010
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
	//bool getDataFromFile(AosBuffPtr &header, AosBuffPtr &body, const AosRundataPtr &rdata);

protected:
	

	void clearTrans() { mTrans.clear(); }
	static bool isCompIIL(const u64 &iilid) { return (iilid & eCompIILBitFlag); }

//	bool	saveToFilePriv(
//			const AosDocTransPtr &docTrans,
//			const AosRundataPtr &rdata);

	//bool	setNewHeaderToBuff(AosBuff &buff);
	//bool	setCompHeaderToBuff(AosBuff &buff);
	//bool	setCompLeafHeaderToBuff(AosBuff &buff);
	//bool	setNormalHeaderToBuff(AosBuff &buff);
	//bool 	saveRootToFileSafe(const AosRundataPtr &rdata);

	// Chen Ding, 11/07/2011
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
					const u32 compressed_size,
					const AosRundataPtr &rdata);

	virtual bool deleteFromLocalFilePriv(const AosRundataPtr &rdata);

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
	// Chen Ding, 01/03/2013
	virtual bool retrieveQueryProcBlock(
				AosBuffPtr &buff, 
				const AosQueryContextObjPtr &context, 
				const AosRundataPtr &rdata) = 0;

	// Chen Ding, 01/03/2013
	virtual bool retrieveNodeList(
				AosBuffPtr &buff, 
				const AosQueryContextObjPtr &context, 
				const AosRundataPtr &rdata) = 0;

	// Chen Ding, 2013/01/14
	virtual bool retrieveIILBitmap(
				const OmnString &iilname,
				AosBitmapObjPtr &bitmap, 
				const AosBitmapObjPtr &partial_bitmap, 
				const AosBitmapTreeObjPtr &bitmap_tree, 
				AosRundataPtr &rdata) = 0;

	static bool staticGetIILType(               
			        AosIILType &iiltype,
					const u64 &iilid,
					const AosRundataPtr &rdata);

private:
	// Ketty 2013/01/15
	bool        readFromDfmDoc(const AosDfmDocIILPtr &doc,
			        const bool iilmgrLocked,
					const AosRundataPtr &rdata);
	void        setNewDfmDoc(const AosDfmDocIILPtr doc);
	bool        resetDfmDoc(const AosDfmDocIILPtr &doc);

public:
	// Chen Ding, 2013/03/01
	virtual bool computeQueryResults(
					const AosRundataPtr &rdata, 
					const OmnString &iilname, 
					const AosQueryContextObjPtr &context, 
					const AosBitmapObjPtr &bitmap, 
					const u64 &query_id, 
					const int physical_id);

	// Chen Ding, 2013/03/03
	virtual bool retrieveQueryBlock(
						const AosRundataPtr &rdata, 
						const AosQueryReqObjPtr &query);

	virtual bool retrieveNodeList(
						const AosRundataPtr &rdata, 
						const AosQueryReqObjPtr &query);
	virtual u64 getSubIILID(const int idx) const = 0;
};

bool inline AOS_IIL_INMEMORY(const u64 &ptr)
{
	return (ptr >> 32) & AOSIIL_INMEMORY_FLAG;
}
#endif
