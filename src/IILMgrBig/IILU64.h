////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 	Created: 12/15/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILMgrBig_IILU64_h
#define AOS_IILMgrBig_IILU64_h

#include "BatchQuery/Ptrs.h"
#include "IILMgrBig/IIL.h"
#include "SEUtil/IILIdx.h"
#include "IILUtil/IILUtil.h"
#include "SearchEngine/SeCommon.h"
#include "Util1/MemMgr.h"
#include "Util/U64U64Array.h"

#include <string.h>
#include <vector>

using namespace std;


#define AosIILU64SanityCheck(x) true
// #define AosIILU64SanityCheck(x) (x)->listSanityCheck()

class AosIILU64 : public AosIIL
{

public:
	enum
	{
		eMaxSubChangedToSave = 10,
		//eAddBlockMaxSize = 50000,		// Ken Lee, 2013/01/02
		//eAddBlockMinSize = 20000,		// Ken Lee, 2013/01/02
		//eAddBlockFillSize = 40000,	// Ken Lee, 2013/01/02
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
	u64*			mValues;
	
	u64*			mMinVals;
	u64*			mMinDocids;
	u64*			mMaxVals;
	u64*			mMaxDocids;
	AosIILU64Ptr *	mSubiils;
	
	int				mLevel;
	AosIILU64Ptr	mParentIIL;

	
	DistrType		mDistrType;
	DistrStatus		mDistrStatus;
	//static AosMemCheckerPtr	mMemChecker;

public:
	AosIILU64(
			const bool isPersis, 
			const bool iilmgrLocked, 
			const AosRundataPtr &rdata);
	AosIILU64(
			const u64 &wordid, 
			const u64 &iilid,
			const bool isPersis,
			const OmnString &iilname, 
			const bool iilmgrlocked, 
			const AosRundataPtr &rdata);
	AosIILU64(
			const u64 &iilid, 
			const u32 siteid, 
			const AosDfmDocIILPtr &doc,         //Ketty 2012/11/15
			const bool iilmgrLocked, 
			const AosRundataPtr &rdata);
	~AosIILU64();

	static bool	staticInit(const AosXmlTagPtr &config);
				
	virtual bool 	hasParentIIL()const{return mParentIIL;}
	virtual bool 	isRootIIL()const{return !mParentIIL;}
	virtual bool 	isChildIIL()const{return mParentIIL;}
	virtual bool	isLeafIIL()const{return mLevel == 0;}
	virtual bool	isBranchIIL()const{return (mLevel > 0 && mParentIIL);}
	virtual bool	isParentIIL()const{return mLevel > 0;}
	virtual bool	isSingleIIL()const{return (mLevel == 0 && !mParentIIL);}
	virtual AosIILPtr	getRootIIL()	
	{		
		AosIILU64Ptr u64Root = mParentIIL;		
		if(!mParentIIL)		
		{			
			AosIILU64Ptr thisPtr(this, false);			
			u64Root = thisPtr;		
		}		
		else		
		{			
			while(!u64Root->isRootIIL())			
			{				
				u64Root = u64Root->getParentIIL();			
			}		
		}		
		return u64Root;	
	}

	AosIILU64Ptr getParentIIL()const{return mParentIIL;}
	virtual bool		saveSanityCheckProtected(const AosRundataPtr &rdata);
	virtual AosBuffPtr	getBodyBuffProtected() const;
	virtual bool 		setContentsProtected(AosBuffPtr &buff, const bool, const AosRundataPtr &rdata);
	virtual bool 		prepareMemoryForReloading(const bool iilmgrLocked);
	virtual u64			getMinDocid() const;
	virtual u64			getMaxDocid() const;
	const u64	 		getMinValue() const;
	const u64 			getMaxValue() const;
	
	bool 	addDocSafe(
				const u64 &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const bool iilmgrLocked,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = addDocPriv(value, docid, value_unique, docid_unique, iilmgrLocked,rdata);
		aos_assert_rb(AosIILU64SanityCheck(this), mLock, false);
		AOSUNLOCK(mLock);
		return rslt;
	}
	
	bool 	addDocRecSafe(
				const u64 &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const bool iilmgrLocked,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = addDocRecPriv(value, docid, value_unique, docid_unique, iilmgrLocked, rdata);
		aos_assert_rb(AosIILU64SanityCheck(this), mLock, false);
		AOSUNLOCK(mLock);
		return rslt;
	}
	
	bool	removeDocSafe(
				const u64 &value,
				const u64 &docid,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = removeDocPriv(value, docid, rdata);
		aos_assert_rb(AosIILU64SanityCheck(this), mLock, false);
		AOSUNLOCK(mLock);
		return rslt;
	}
	
	bool	removeDocRecSafe(
				const u64 &value,
				const u64 &docid,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = removeDocRecPriv(value, docid, rdata);
		aos_assert_rb(AosIILU64SanityCheck(this), mLock, false);
		AOSUNLOCK(mLock);
		return rslt;
	}
	
	bool 	modifyDocSafe(
				const u64 &oldvalue, 
				const u64 &newvalue, 
				const u64 &docid, 
				const bool value_unique,
				const bool docid_unique,
				const bool iilmgrLocked,
				const AosRundataPtr &rdata);
	
	bool	nextDocidSafeFind(
				AosIILIdx &idx, 
				const u64 &value,
				const AosOpr opr,
				u64 &docid, 
				bool &isunique,
				const bool reverse, 
				const AosRundataPtr &rdata) 
	{
		AOSLOCK(mLock);
		bool rslt = nextDocidPrivFind(idx, value, opr, docid, isunique, reverse, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	inline u64 incrementDocidSafe(
				const u64 &key,
				const u64 &dft,
				const bool iilmgrLocked,
				const AosRundataPtr &rdata)
	{
		u64 docid;
		bool rslt = incrementDocidSafe(key, docid, 1, dft, true, iilmgrLocked, rdata);
		if (!rslt) return dft;
		return docid;
	}

	// Chen Ding, 2013/02/14
	bool incrementDocidSafe(
				const u64 &key,
				u64 &value,
				const u64 &inc_value,
				const u64 &init_value,
				const bool add_flag,
				const u64 &dft_value,
				const AosRundataPtr &rdata);

	bool incrementDocidSafe(
	 			const u64 &key,
	 			u64 &value,
	 			const u64 &inc_value,
	 			const u64 &init_value,
	 			const bool add_flag,
	 			const bool iilmgrLocked,
	 			const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = incrementDocidPriv(key, value, inc_value, init_value, add_flag, iilmgrLocked, rdata);
		aos_assert_rb(AosIILU64SanityCheck(this), mLock, false);
		AOSUNLOCK(mLock);
		return rslt;
	}
	
	virtual u64 getTotalSafe(
				const u64 &value, 
				const AosOpr opr, 
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		u64 total = getTotalPriv(value, opr, rdata);
		AOSUNLOCK(mLock);
		return total;
	}
	
	virtual bool	returnSubIILsSafe(
						const bool iilmgrLocked,
						bool &returned,
						const AosRundataPtr &rdata,
						const bool returnHeader)
	{
        AOSLOCK(mLock);
        bool rslt = returnSubIILsPriv(iilmgrLocked, returned, rdata, returnHeader);
        AOSUNLOCK(mLock);
		return rslt;		
	}
	
	virtual bool	preQuerySafe(
						const AosQueryContextObjPtr &query_context,
						const bool iilmgrlocked, 
						const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = preQueryPriv(query_context,iilmgrlocked,rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	virtual bool	querySafe(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context, 
						const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = queryPriv(query_rslt, query_bitmap, query_context, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}
	
	virtual bool	queryRangeSafe(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = queryRangePriv(query_rslt, query_bitmap, query_context, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}
	
	virtual bool	queryNewSafe(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context, 
						const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = queryNewPriv(query_rslt,query_bitmap,query_context,true,rdata);
		AOSUNLOCK(mLock);
		return rslt; 
	}
	virtual bool	queryNewSafe(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = queryNewPriv(query_rslt,query_bitmap,rdata);
		AOSUNLOCK(mLock);
		return rslt; 
	}
	// Ketty 2013/01/15
	// comp not support yet.
	//AosIILCompU64Ptr 	retrieveCompIIL(
	//						const bool iilmgr_locked,
	//						const AosRundataPtr &rdata);
	
	//bool				returnCompIIL(
	//						const AosIILPtr &iil,
	//						const AosRundataPtr &rdata);
	
	//virtual AosIILPtr	createCompIIL(const AosRundataPtr &rdata);

	bool	removeFirstValueDocSafe(
				const u64 &value,
				u64 &docid,
				const bool reverse,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		AosIILIdx idx;
		bool isunique;
		bool rslt = nextDocidPrivFind(idx, value, eAosOpr_eq, docid, isunique, reverse, rdata);
		if(rslt)
		{
			rslt = removeDocPriv(value, docid, rdata);
			aos_assert_rl(rslt, mLock, false);
		}
		AOSUNLOCK(mLock);
		return rslt;
	}
	
	bool 	updateIndexData(
				const int idx,
				const bool changeMax,
				const bool changeMin,
				const AosRundataPtr &rdata);
	
	bool	setValueDocUniqueSafe(
				const u64 &key, 
				const u64 &docid, 
				const bool must_same, 
				const bool iilmgrLocked,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = setValueDocUniquePriv(key, docid, must_same, iilmgrLocked, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	//virtual bool	saveSubIILToTransFileSafe(
	//					const AosDocTransPtr &docTrans,
	//					const bool forcesave, 
	//					const AosRundataPtr &rdata);
	virtual bool	saveSubIILToLocalFileSafe(
						const bool forcesave, 
						const AosRundataPtr &rdata);

private:
	bool	sanityTestForSubiils();
	bool	splitSanityCheck();
	
	bool 	insertDocSinglePriv(
				int &idx, 
				const u64 &value, 
				const u64 &docid, 
				const bool value_unique,
				const bool docid_unique,
				const AosRundataPtr &rdata);

	bool	insertBefore(
				const int idx, 
				const u64 &docid, 
				const u64 &value);

	bool 	insertAfter(
				const int idx, 
				const u64 &docid, 
				const u64 &value);
	
	bool 	addDocPriv(
				const u64 &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const bool iilmgrLocked,
				const AosRundataPtr &rdata);
	
	bool 	addDocRecPriv(
				const u64 &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const bool iilmgrLocked,
				const AosRundataPtr &rdata);
	
	bool 	addDocSinglePriv(
				const u64 &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const bool iilmgrLocked,
				const AosRundataPtr &rdata);
	
	bool	removeDocPriv(
				const u64 &value,
				const u64 &docid,
				const AosRundataPtr &rdata);
	
	bool	removeDocRecPriv(
				const u64 &value,
				const u64 &docid,
				const AosRundataPtr &rdata);
	
	bool 	removeDocSinglePriv(
				const u64 &value, 
				const u64 &docid,
				const AosRundataPtr &rdata); 
	
	bool	nextDocidAN(
				int &idx, 
				const bool reverse, 
				const u64 &value,
				u64 &docid);
	bool	nextDocidLT(
				int &idx, 
				const bool reverse, 
				const u64 &value,
				u64 &docid);
	bool	nextDocidLE(
				int &idx, 
				const bool reverse, 
				const u64 &value,
				u64 &docid);
	bool	nextDocidGT(
				int &idx, 
				const bool reverse, 
				const u64 &value,
				u64 &docid);
	bool	nextDocidGE(
				int &idx, 
				const bool reverse, 
				const u64 &value,
				u64 &docid);
	bool	nextDocidEQ(
				int &idx, 
				const bool reverse, 
				const u64 &value,
				u64 &docid);
	int		nextDocidNE(
				int &idx, 
				const bool reverse, 
				const u64 &value,
				u64 &docid);
	
	int 	firstEQ(const int idx, const u64 &value);
	int 	firstEQRev(const int idx, const u64 &value);
	int 	firstNE(const int idx, const u64 &value);
	int 	firstNERev(const int idx, const u64 &value);
	int 	firstLE(const int idx, const u64 &value);
	int 	firstLERev(const int idx, const u64 &value);
	int 	firstLT(const int idx, const u64 &value);
	int 	firstLTRev(const int idx, const u64 &value);
	int 	firstGE(const int idx, const u64 &value);
	int 	firstGERev(const int idx, const u64 &value);
	int 	firstGT(const int idx, const u64 &value);
	int 	firstGTRev(const int idx, const u64 &value);
	int 	lastLT(const u64 &value);
	int 	lastLE(const u64 &value);
	int 	lastEQ(const int idx, const u64 &value);
	int 	firstLE(const u64 &value);
	int 	firstLT(const u64 &value);
	
	bool	nextDocidPrivFind(
				AosIILIdx &idx, 
				const u64 &value,
				const AosOpr opr,
				u64 &docid, 
				bool &isunique,
				const bool reverse, 
				const AosRundataPtr &rdata);
	
	bool	nextDocidSinglePriv(
				int &idx, 
				const bool reverse, 
				const AosOpr opr,
				const u64 &value,
				u64 &docid, 
				bool &isunique,
				const AosRundataPtr &rdata);
	
	u64		getTotalPriv(
				const u64 &value,
				const AosOpr opr,
				const AosRundataPtr &rdata);

	u64 	getTotalSinglePriv(
				const u64 &value,
				const AosOpr opr,
				const AosRundataPtr &rdata);
	
	virtual bool	returnSubIILsPriv(
						const bool iilmgrLocked,
						bool &returned,
						const AosRundataPtr &rdata,
						const bool returnHeader);
	
	bool	incrementDocidPriv(
				const u64 &key,
				u64 &value,
				const u64 &incvalue,
				const u64 &init_value,
				const bool add_flag,
				const bool iilmgrLocked,
				const AosRundataPtr &rdata);

	bool	queryPriv(
				const AosQueryRsltObjPtr &query_rslt,
				const AosBitmapObjPtr &query_bitmap,
				const AosQueryContextObjPtr &query_context,
				const AosRundataPtr &rdata);
	
	bool	queryRangePriv(
				const AosQueryRsltObjPtr &query_rslt,
				const AosBitmapObjPtr &query_bitmap,
				const AosQueryContextObjPtr &query_context,
				const AosRundataPtr &rdata);
	
	bool	queryNewPriv(
				const AosQueryRsltObjPtr &query_rslt,
				const AosBitmapObjPtr &query_bitmap, 
				const AosRundataPtr &rdata);

	bool	expandMemoryPriv();
	bool	checkMemory() const;
	virtual bool	resetSubIILInfo(const AosRundataPtr &rdata);
	virtual bool	resetSpec();
	bool 	createSubiilIndex();
	
	bool 	splitListPriv(
				const bool iilmgrLocked,
				AosIILPtr &subiil,
				const AosRundataPtr &rdata);

	bool	mergeSubiilPriv(
				const bool iilmgrLocked,
				const int iilidx,
				const AosRundataPtr &rdata);
	
	bool 	initSubiil(
				u64 *docids, 
				u64 *values, 
				const int numDocs,
				const int subiilid, 
				const AosIILU64Ptr &rootiil, 
				const bool iilmgrLocked); 

	bool 	addSubiil(
				const AosIILU64Ptr &crtsubiil,
				const AosIILU64Ptr &nextsubiil,
				const AosRundataPtr &rdata);
	
	bool	checkDocidUnique(
				const bool rslt, 
				const int idx, 
				u64 &docid,
				bool &isunique,
				const AosRundataPtr &rdata);
	
	bool	setValueDocUniquePriv(
				const u64 &key, 
				const u64 &docid, 
				const bool must_same, 
				const bool iilmgrLocked,
				const AosRundataPtr &rdata);

	bool	setValueDocUniqueSinglePriv(
				const u64 &key, 
				const u64 &docid, 
				const bool must_same, 
				const bool iilmgrLocked,
				const AosRundataPtr &rdata);

// ken not implimented
public:
	bool	getValueByIdxSafe(
				AosIILIdx &idx, 
				u64 &value,
				u64 &docid,
				const AosRundataPtr &rdata);
	bool	getValueByIdxRecSafe(
				AosIILIdx &idx, 
				u64 &value,
				u64 &docid,
				const AosRundataPtr &rdata);

private:
	// find a value and docid by a real idx;
	bool	getValueByIdxRecPriv(
				AosIILIdx &idx, 
				u64 &value,
				u64 &docid,
				const AosRundataPtr &rdata);
	bool	getValueByIdxSinglePriv(
				AosIILIdx &idx, 
				u64 &value,
				u64 &docid,
				const AosRundataPtr &rdata);
// ken not implimented finish

	u64 getTotalNEPriv(const u64 &value, const AosRundataPtr &rdata);

	bool	incrementDocidSingleSafe(
				const u64 &key,
				u64 &value,
				const u64 &inc_value,
				const u64 &init_value,
				const bool add_flag,
				const bool iilmgrLocked,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = incrementDocidSinglePriv(key, value, 
				inc_value, init_value, add_flag, iilmgrLocked, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool	incrementDocidSinglePriv(
				const u64 &key,
				u64 &value,
				const u64 &inc_value,
				const u64 &init_value,
				const bool add_flag,
				const bool iilmgrLocked,
				const AosRundataPtr &rdata);
/////////////////////////////////////////////////////////////
// shawn functions not orignized
public:
	bool			addBlockSafe(
						u64* entries,
						const int num,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

private:
	bool 			splitCheck();
	AosIILU64Ptr	splitLeafContent(
						const bool iilmgrLocked, 
						const AosRundataPtr &rdata);

	bool			splitListSinglePriv(
						const vector<AosIILU64Ptr> &subiil_list,
						const bool iilmgrLocked, 
						const AosRundataPtr &rdata);
	bool			subiilSplited(
						const int iilIdx,
						const vector<AosIILU64Ptr> &subiil_list,
						const bool iilmgrLocked, 
						const AosRundataPtr &rdata);

	AosIILU64Ptr	splitContentUtil(
						const bool iilmgrLocked, 
						const AosRundataPtr &rdata);



	bool 			initSubiilLeaf(
						u64 *docids,
						u64 *values, 
						const int numDocs,
						const bool iilmgrLocked);

	bool 			initSubiilParent(
						u64 *minDocids,
						u64 *minValues, 
						u64 *maxDocids,
						u64 *maxValues, 
						u64 *iilids,
						int *numEntries,
						AosIILU64Ptr *subiils,
						const int numSubiils,
						const bool iilmgrLocked);

	void			setParentIIL(const AosIILU64Ptr &parent){mParentIIL = parent;}
	void			setIILLevel(const int level){mLevel = level;}
	u32				getNumDocs() const {return mNumDocs;}

	bool			addSubiils(
						const int &iilidx,
						const vector<AosIILU64Ptr> &subiil_list,
						const AosRundataPtr &rdata);
	AosIILU64Ptr	splitParentContent(
						const bool iilmgrLocked, 
						const AosRundataPtr &rdata);
	bool			splitListRootPriv(
						const vector<AosIILU64Ptr> &subiil_list,
						const bool iilmgrLocked, 
						const AosRundataPtr &rdata);

	bool 			splitListPriv(
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);
	bool			setContentsSinglePriv(
							AosBuffPtr &buff, 
							const bool iilmgrLocked,
							const AosRundataPtr &rdata);
	AosBuffPtr 		getBodyBuffSinglePriv() const;

	bool			addBlockRecSafe(
						u64* &entries,
						int &num,
						int &subChanged,
						const bool &isTail,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);
	
	bool			addBlockRecPriv(
						u64* &entries,
						int &num,
						int &subChanged,
						const bool &isTail,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

	bool			addBlockSinglePriv(
						u64* &entries,
						int &num,
						int &subChanged,
						const bool isTail,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

	bool			addBlockRebuild(
						u64* &entries,
						int &num,
						int &num_handle,
						u64* &orig_values,
						u64* &orig_docids,
						int &orig_num,
						int &orig_idx,
						const int iilsize);
							
	int 			binarySearch(
						u64* &entries,
						const int num,
						u64 &value,
						const u64 &docid);

	int 			binarySearch2(
						u64 value,
						u64 docid);
	
	int				valueMatch(
						const u64 &entry,
						const u64 &value,
						const u64 &docid);

	// Chen Ding, 04/23/2012
	// All 'getSubiil(...)' functions are finding the first occurrence functions.
	// All 'next...(...)' functions shall not be used anymore.
	// AosIILU64Ptr getSubiilSafe(
	// 			const u64 &value, 
	// 			const u64 &docid,
	// 			const bool reverse, 
	// 			const bool iilmgrLocked,
	// 			const AosRundataPtr &rdata)
	// {
	// 	AOSLOCK(mLock);
	// 	AosIILIdx idx;
	// 	AosIILU64Ptr iil = getSubiilPriv(idx, value, docid, false, false, 
	// 							reverse, iilmgrLocked, rdata);
	// 	AOSUNLOCK(mLock);
	// 	return iil;
	// }

	// AosIILU64Ptr getSubiilPriv(
	// 			const u64 &value, 
	// 			const u64 &docid,
	// 			const bool reverse, 
	// 			const bool iilmgrLocked,
	// 			const AosRundataPtr &rdata)
	// {
	// 	AosIILIdx idx;
	// 	return getSubiilPriv(idx, value, docid, false, false, reverse, iilmgrLocked, rdata);
	// }

	AosIILU64Ptr 	getSubiilByCondPriv(
						const u64 &value, 
						const u64 &docid,
						const AosOpr opr,
						const bool reverse, 
						const AosRundataPtr &rdata)
	{
		AosIILIdx idx;
		return getSubiilByCondPriv(idx, value, docid, opr, reverse, rdata);
	}
	
	AosIILU64Ptr 	getSubiilByCondPriv(
						const u64 &value, 
						const AosOpr opr,
						const bool reverse, 
						const AosRundataPtr &rdata)
	{
		AosIILIdx idx;
		return getSubiilByCondPriv(idx, value, opr, reverse, rdata);
	}

	AosIILU64Ptr	getSubiilByCondPriv(
						AosIILIdx &index, 
						const u64 &value, 
						const AosOpr opr, 
						const bool reverse, 
						const AosRundataPtr &rdata);
	
	AosIILU64Ptr	getSubiilByCondPriv(
						AosIILIdx &index, 
						const u64 &value, 
						const u64 &docid,
						const AosOpr opr, 
						const bool reverse, 
						const AosRundataPtr &rdata);
	
	AosIILU64Ptr	getSubiilPriv(
	 					const u64 &value, 
	 					const u64 &docid,
	 					const bool value_unique,
	 					const bool docid_unique,
	 					const bool iilmgrLocked, 
	 					const AosRundataPtr &rdata);
	
	AosIILU64Ptr	getSubiilByIndexPriv(
			 			const int idx,
						const bool iilmgrlocked,
						const AosRundataPtr &rdata);

	// Chen Ding, 04/22/2012
	bool			hasMoreMembers(const AosIILIdx &idx) const;
	AosIILU64Ptr	getFirstSubiilByCondPriv(
						AosIILIdx &idx, 
						const u64 &value, 
						const AosOpr opr, 
						const bool reverse, 
						const AosRundataPtr &rdata);
	AosIILU64Ptr	getFirstSubiilByCondPriv(
						AosIILIdx &idx, 
						const u64 &value, 
						const u64 &docid,
						const AosOpr opr, 
						const bool reverse, 
						const AosRundataPtr &rdata);
	AosIILU64Ptr	nextSubiilByCondPriv(
						AosIILIdx &idx, 
						const u64 &value, 
						const AosOpr opr, 
						const bool reverse, 
						const AosRundataPtr &rdata);
	AosIILU64Ptr	nextSubiilByCondPriv(
						AosIILIdx &idx, 
						const u64 &value, 
						const u64 &docid,
						const AosOpr opr, 
						const bool reverse, 
						const AosRundataPtr &rdata);
	int				getFirstSubiilIndexGT(
						const u64 &value,
						const AosRundataPtr &rdata);
	int				getFirstSubiilIndexGT(
						const u64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata);
	int				getFirstSubiilIndexGE(
						const u64 &value,
						const AosRundataPtr &rdata);
	int				getFirstSubiilIndexGE(
						const u64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata);
	int				getLastSubiilIndexLT(
						const u64 &value,
						const AosRundataPtr &rdata);
	int				getLastSubiilIndexLT(
						const u64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata);
	int				getLastSubiilIndexLE(
						const u64 &value,
						const AosRundataPtr &rdata);
	int				getLastSubiilIndexLE(
						const u64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool			containValuePriv(
						const u64 &min_value, 
						const u64 &max_value,
						const u64 &value, 
						const AosOpr opr, 
						const bool reverse, 
						bool &keep_search,
						const AosRundataPtr &rdata);

	// Chen Ding, 04/22/2012
	inline static bool isValidLevel(const int level) { return level >= 0 && level < AosIILIdx::eMaxNumLevels; }

	// Chen Ding, 04/22/2012
	bool			nextUniqueValueSafe(
						AosIILIdx &idx,
						const bool reverse,
						const AosOpr opr,
						const u64 &value,
						u64 &unique_value,
						bool &found,
						const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = nextUniqueValueSafe(idx, reverse, opr, value, unique_value, found, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	// Chen Ding, 04/22/2012
	bool			nextUniqueValuePriv(
						AosIILIdx &idx,
						const bool reverse,
						const AosOpr opr,
						const u64 &value,
						u64 &unique_value,
						bool &found,
						const AosRundataPtr &rdata);

	// Chen Ding, 04/22/2012
	bool			nextUniqueValueSinglePriv(
						AosIILIdx &idx,
						const bool reverse,
						const AosOpr opr,
						const u64 &value,
						u64 &unique_value,
						bool &found, 
						const AosRundataPtr &rdata);

	// Chen Ding, 04/22/2012
	inline bool		isIILIdxValid(const AosIILIdx &idx)
	{
		aos_assert_r(mLevel >= 0 && mLevel < AosIILIdx::eMaxNumLevels, false);
		int iilidx = idx.getIdx(mLevel);
		return iilidx >= 0 && (u32)iilidx < mNumSubiils;
	}

	// Chen Ding, 04/22/2012
	AosIILU64Ptr	getSubiil3Priv(
						const u64 &value,
			            const u64 &docid,
						const bool iilmgrlock,
						const AosRundataPtr &rdata);

	int          	getSubiilIndex3Priv(
						const u64 &value,
			            const u64 &docid,
						const AosRundataPtr &rdata);
	int          	getSubiilIndex3Priv(
						const u64 &value,
			            const u64 &docid,
						const AosOpr opr,
						const AosRundataPtr &rdata);

public:
	bool			increaseBlockSafe(
						u64* entries,
						const int num,
						const u64 &dftValue,
						const AosIILUtil::AosIILIncType incType,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

	bool			increaseBlockRecSafe(
						u64* &entries,
						int &num,
						const u64 &dftValue,
						const AosIILUtil::AosIILIncType incType,
						int &subChanged,
						const bool &isTail,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

private:
	bool			increaseBlockRecPriv(
						u64* &entries,
						int &num,
						const u64 &dftValue,
						const AosIILUtil::AosIILIncType incType,
						int &subChanged,
						const bool &isTail,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

	bool			increaseBlockSinglePriv(
						u64* &entries,
						int &num,
						const u64 &dftValue,
						const AosIILUtil::AosIILIncType incType,
						int &subChanged,
						const bool isTail,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

	bool			increaseBlockRebuild(
							u64* &entries,
							int &num,
							const u64 &dftValue,
							const AosIILUtil::AosIILIncType incType,
							int &num_handle,
							u64* &orig_values,
							u64* &orig_docids,
							int &orig_num,
							int &orig_idx,
							const int maxiilsize,
							const bool isLastSub);

	int				valueMatch(
						const u64 &entry,
						const u64 &value);

	int				valueMatchU64(
						const u64 &entry1,
						const u64 &entry2);

	bool			moveDocsForward(
						const AosIILU64Ptr &from_iil,
						const AosIILU64Ptr &to_iil,
						const int &num_to_move);

	bool			sanityCheckPriv(const AosRundataPtr &rdata);
	bool			sanityCheckRecPriv(int &num_docs, const AosRundataPtr &rdata);
	bool			sanityCheckSinglePriv(int &num_docs, const AosRundataPtr &rdata);
	
	
//shawn functions
private:
	bool			distributeIILPriv(const bool iilmgrlocked,
						 		  const AosRundataPtr &rdata);

	bool 			addDocDistr(
						const u64 &value, 
						const u64 &docid, 
						const bool value_unique, 
						const bool docid_unique, 
						const AosRundataPtr &rdata);

	bool			removeDocDistr(
						const u64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool 			modifyDocDistr(
						const u64 &oldvalue, 
						const u64 &newvalue, 
						const u64 &docid, 
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata);

	bool			addBlockDistrPriv(
						u64* entries,
						const int num,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

	bool			getDistrInfo(int &distr_num,
						vector<u64>		  &iilids, 
						vector<u64> &values,
						vector<u64>		  &docids);

	bool			sendContentToDistr(
						const u64 &iilid,
						const AosRundataPtr &rdata);
	
	void			setDistrType(const DistrType type){mDistrType = type;}
	void			setDistrStatus(const DistrStatus status){mDistrStatus = status;}

	DistrType		getDistrType()const {return mDistrType;}
	DistrStatus		getDistrStatus()const {return mDistrStatus;}

	void			updateDistrInfo();
	bool			needCallDistr1();
public:
	bool			entireCheckSafe(const bool iilmgrLocked,
									const AosRundataPtr &rdata);
private:
	bool			entireCheckRecPriv(u64 &value,
									   u64		&docid,
							  		   const bool iilmgrLocked,
									   const AosRundataPtr &rdata);
	bool			entireCheckSinglePriv(u64 &value,
									   u64		&docid,
							  		   const bool iilmgrLocked,
									   const AosRundataPtr &rdata);

public:
	


	bool			queryNewPriv(
						const AosQueryRsltObjPtr    &query_rslt,
						const AosBitmapObjPtr  &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const bool iilmgrlock, 
						const AosRundataPtr &rdata);


	bool			queryNewNEPriv(
						const AosQueryRsltObjPtr    &query_rslt,
						const AosBitmapObjPtr  &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const bool iilmgrlock, 
						const AosRundataPtr &rdata);

	bool			queryPagingProc(
						const AosQueryContextObjPtr &query_context,
						AosIILIdx &start_iilidx,
						AosIILIdx &end_iilidx,
						bool &has_data,
						const bool iilmgrlock, 
						const AosRundataPtr &rdata);

	bool			copyData(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosIILIdx &start_iilidx,
						const AosIILIdx &end_iilidx,
						const bool iilmgrlock, 
						const AosRundataPtr &rdata);

	bool			copyDataWithCheck(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosIILIdx &start_iilidx,
						const AosIILIdx &end_iilidx,
						const bool iilmgrlock, 
						const AosRundataPtr &rdata);

	bool			copyDataRec(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosIILIdx &start_iilidx,
						const AosIILIdx &end_iilidx,
						AosIILIdx &cur_iilidx,
						const AosIILUtil::CopyType copytype, 
						const bool iilmgrlock, 
						const AosRundataPtr &rdata);

	bool			copyDataSingle(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosIILIdx &start_iilidx,
						const AosIILIdx &end_iilidx,
						AosIILIdx &cur_iilidx,
						const AosIILUtil::CopyType copytype, 
						const bool iilmgrlock, 
						const AosRundataPtr &rdata);

	void			nextIndexFixErrorIdx(
						AosIILIdx &cur_iilidx,
						const u64 &cur_value,
						const u64 &cur_docid,
						const bool &reverse,
						bool &has_data,
						const bool iilmgrlock, 
						const AosRundataPtr &rdata);
						
	int				countNumRec(const AosIILIdx start_iilidx,
					   const AosIILIdx end_iilidx,
					   const bool iilmgrlock, 
					   const AosRundataPtr &rdata);

	int				countNumToEndRec(const AosIILIdx start_iilidx,
									 const bool iilmgrlock, 
					   				 const AosRundataPtr &rdata);

	int				countNumFromStartRec(const AosIILIdx end_iilidx,
									 const bool iilmgrlock, 
					   				 const AosRundataPtr &rdata);

	int				countNumSingle(const AosIILIdx start_iilidx,
						const AosIILIdx end_iilidx);

	int				countNumToEndSingle(const AosIILIdx start_iilidx);

	int				countNumFromStartSingle(const AosIILIdx end_iilidx);

	bool			queryPagingProcRec(
						const AosQueryContextObjPtr &query_context,
						AosIILIdx &start_iilidx,
						AosIILIdx &end_iilidx,
						int &bsize,
						const bool reverse,
						const bool from_start,
						const bool iilmgrlock, 
						const AosRundataPtr &rdata);

	bool			queryPagingProcSingle(
						const AosQueryContextObjPtr &query_context,
						AosIILIdx &start_iilidx,
						AosIILIdx &end_iilidx,
						int &bsize,
						const bool reverse,
						const bool from_start,
						const bool iilmgrlock, 
						const AosRundataPtr &rdata);

private:
	
	bool prevQueryPosSingle(AosIILIdx &iilidx,
							const bool iilmgrlock, 
							const AosRundataPtr &rdata);
	
	bool prevQueryPosL1(AosIILIdx &iilidx,
							const bool iilmgrlock, 
							const AosRundataPtr &rdata);
	
	bool prevQueryPosRec(AosIILIdx &iilidx,
							const bool iilmgrlock, 
							const AosRundataPtr &rdata);
	
	bool lastQueryPosSingle(AosIILIdx &iilidx,
							const bool iilmgrlock, 
							const AosRundataPtr &rdata);
	
	bool lastQueryPosL1(AosIILIdx &iilidx,
							const bool iilmgrlock, 
							const AosRundataPtr &rdata);
	
	bool lastQueryPosRec(AosIILIdx &iilidx,
							const bool iilmgrlock, 
							const AosRundataPtr &rdata);
	
	bool nextQueryPosSingle(AosIILIdx &iilidx,
							const bool iilmgrlock, 
							const AosRundataPtr &rdata);
	
	
	bool nextQueryPosL1(AosIILIdx &iilidx,
							const bool iilmgrlock, 
							const AosRundataPtr &rdata);
	
	bool nextQueryPosRec(AosIILIdx &iilidx,
							const bool iilmgrlock, 
							const AosRundataPtr &rdata);


public:
	bool nextDocidSafe(int &idx,                                                                       
	            int &iilidx,
	            const bool reverse, 
	            const AosOpr opr,
	            u64 &value,
	            u64 &docid, 
	            bool &isunique,
	            const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = nextDocidPriv(
						idx,
			            iilidx,
			            reverse, 
			            opr,
			            value,
			            docid, 
			            isunique,
			            rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool setValueDocUnique(                                                                               
	         const u64 &key,
	         const u64 &docid,
	         const bool must_same,
	         const AosRundataPtr &rdata);
private:
	bool nextDocidPriv(int &idx,
	            int &iilidx,
	            const bool reverse, 
	            const AosOpr opr,
	            u64 &value,
	            u64 &docid, 
	            bool &isunique,
	            const AosRundataPtr &rdata);

	bool	preQueryNEPriv(
				const AosQueryContextObjPtr &query_context,
				const bool iilmgrlocked, 
				const AosRundataPtr &rdata);
	bool	preQueryPriv(
				const AosQueryContextObjPtr &query_context,
				const bool iilmgrlocked, 
				const AosRundataPtr &rdata);
	bool	returnSubIILPriv(
				const int idx,
				const bool iilmgrLocked,
				const AosRundataPtr &rdata);

public:
	// Chen Ding, 01/03/2013
	virtual bool retrieveQueryProcBlock(
				AosBuffPtr &buff, 
				const AosQueryContextObjPtr &context, 
				const AosRundataPtr &rdata);

	// Chen Ding, 01/03/2013
	virtual bool retrieveNodeList(
				AosBuffPtr &buff, 
				const AosQueryContextObjPtr &context, 
				const AosRundataPtr &rdata);

	// Chen Ding, 2013/01/14
	virtual bool retrieveIILBitmap(
				const OmnString &iilname,
				AosBitmapObjPtr &bitmap, 
				const AosBitmapObjPtr &partial_bitmap, 
				const AosBitmapTreeObjPtr &bitmap_tree, 
				AosRundataPtr &rdata);

	// Chen Ding, 2013/03/03
	virtual u64 getSubIILID(const int idx) const;
};
#endif

