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
// 	Created: 2013/03/14 by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILMgr_IILBigD64_h
#define AOS_IILMgr_IILBigD64_h

#include "API/AosApi.h"
#include "BatchQuery/Ptrs.h"
#include "IILMgr/IIL.h"
#include "IILMgr/IILThrdShellProc.h"
#include "IILUtil/IILUtil.h"
#include "SearchEngine/SeCommon.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/BitmapTreeObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEUtil/IILIdx.h"
#include "Util1/MemMgr.h"
#include "Util/D64U64Array.h"

#include <string.h>
#include <vector>

using namespace std;


#define AosIILBigD64SanityCheck(x) true
// #define AosIILBigD64SanityCheck(x) (x)->listSanityCheck()

class AosIILBigD64 : public AosIIL
{

public:
	enum
	{
		eNumDocsToDistr = 1000000000,
		eMaxLevel = 4,
		eMaxBitmapRebuildNum = 5000,
		eMaxNumOprIn = 10000
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

	struct InCond
	{
		AosOpr			mOpr;
		d64				mValue1;
		d64				mValue2;
		InCond(const OmnString &cond_str)
		{
			vector<OmnString> key_strs;
			AosSplitStr(cond_str,",",key_strs,3);
			aos_assert(key_strs.size() >= 2);
			mOpr = AosOpr_toEnum(key_strs[0]);
			mValue1 = key_strs[1].toD64();
			if(key_strs.size() >= 3)
			{
				mValue2 = key_strs[2].toD64();
			}
		}
	};

private:
	d64 *				mValues;
	
	d64 *				mMinVals;
	u64 *				mMinDocids;
	d64*				mMaxVals;
	u64 *				mMaxDocids;
	AosIILBigD64Ptr *	mSubiils;
	
	int					mLevel;
	AosIILBigD64Ptr		mParentIIL;

	DistrType			mDistrType;
	DistrStatus			mDistrStatus;
	bool				mHasBitmap;
	AosBitmapTreeObjPtr	mBitmapTree;

	OmnMutexPtr			mUpdateLock;	// Ken Lee,2013/06/20

public:
	AosIILBigD64();
	AosIILBigD64(
			const u64 &wordid, 
			const u64 &iilid,
			const bool isPersis,
			const OmnString &iilname, 
			const AosRundataPtr &rdata);
	AosIILBigD64(
			const u64 &iilid, 
			const u32 siteid, 
			const AosDfmDocIILPtr &doc,
			const AosRundataPtr &rdata);
	~AosIILBigD64();

	static bool		staticInit(const AosXmlTagPtr &config);
				
	virtual bool 	isRootIIL()const{return !mParentIIL;}
	virtual bool	isParentIIL()const{return mLevel > 0;}
	virtual bool	isSingleIIL()const{return (mLevel == 0 && !mParentIIL);}
	virtual bool	isLeafIIL()const{return mLevel == 0;}
	virtual bool 	isChildIIL()const{return mParentIIL;}
	virtual bool	isBranchIIL()const{return (mLevel > 0 && mParentIIL);}
	virtual int 	getLevel() const {return mLevel;}

	virtual AosIILObjPtr getRootIIL()	
	{		
		AosIILBigD64Ptr root = mParentIIL;		
		if (!mParentIIL)		
		{
			AosIILBigD64Ptr thisptr(this, false);			
			root = thisptr;		
		}
		else		
		{			
			while(!root->isRootIIL())			
			{				
				root = root->getParentIIL();			
			}		
		}		
		return root;	
	}

	AosIILBigD64Ptr getParentIIL() const {return mParentIIL;}

	virtual bool	saveSanityCheckProtected(const AosRundataPtr &rdata);

	virtual AosBuffPtr	getBodyBuffProtected() const;
	virtual bool 		setContentsProtected(AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool 		prepareMemoryForReloading();

	virtual u64		getMinDocid() const;
	virtual u64		getMaxDocid() const;
	const d64 		getMinValue() const;
	const d64		getMaxValue() const;
	
	bool 	addDocSafe(
				const d64 &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = addDocPriv(value, docid, value_unique, docid_unique, rdata);
		aos_assert_rb(AosIILBigD64SanityCheck(this), mLock, false);
		AOSUNLOCK(mLock);
		return rslt;
	}
	
	bool 	addDocRecSafe(
				const d64 &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = addDocRecPriv(value, docid, value_unique, docid_unique, rdata);
		aos_assert_rb(AosIILBigD64SanityCheck(this), mLock, false);
		AOSUNLOCK(mLock);
		return rslt;
	}
	
	bool	removeDocSafe(
				const d64 &value,
				const u64 &docid,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = removeDocPriv(value, docid, rdata);
		aos_assert_rb(AosIILBigD64SanityCheck(this), mLock, false);
		AOSUNLOCK(mLock);
		return rslt;
	}
	
	bool	removeDocRecSafe(
				const d64 &value,
				const u64 &docid,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = removeDocRecPriv(value, docid, rdata);
		aos_assert_rb(AosIILBigD64SanityCheck(this), mLock, false);
		AOSUNLOCK(mLock);
		return rslt;
	}
	
	bool 	modifyDocSafe(
				const d64 &oldvalue, 
				const d64 &newvalue, 
				const u64 &docid, 
				const bool value_unique,
				const bool docid_unique,
				const AosRundataPtr &rdata);

	// shell function
	bool	nextDocidSafe(
				AosIILIdx the_idx,
				i64 &idx,
				i64 &iilidx,
				const bool reverse, 
				const AosOpr opr,
				const d64 &value,
				u64 &docid, 
				bool &isunique,
				const AosRundataPtr &rdata)
	{
		return nextDocidSafeFind(the_idx, value, opr, docid, isunique, reverse, rdata); 
	}

	bool	nextDocidSafeFind(
				AosIILIdx &idx, 
				const d64 &value,
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
				const d64 &key,
				const d64 &dft,
				const AosRundataPtr &rdata)
	{
		u64 docid;
		bool rslt = incrementDocidSafe(key, docid, 1, dft, true, rdata);
		if (!rslt) return dft;
		return docid;
	}

	bool incrementDocidSafe(
				const d64 &key,
				u64 &value,
				const u64 &inc_value,
				const u64 &init_value,
				const bool add_flag,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = incrementDocidPriv(key, value, inc_value, init_value, add_flag, rdata);
		aos_assert_rb(AosIILBigD64SanityCheck(this), mLock, false);
		AOSUNLOCK(mLock);
		return rslt;
	}
	
	virtual i64 getTotalSafe(
				const d64 &value, 
				const AosOpr opr, 
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		i64 total = getTotalPriv(value, opr, rdata);
		AOSUNLOCK(mLock);
		return total;
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
	
	virtual bool	preQuerySafe(
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = preQueryPriv(query_context, rdata);
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
		bool rslt = queryNewPriv(query_rslt, query_bitmap, query_context, rdata);
		AOSUNLOCK(mLock);
		return rslt; 
	}

	virtual bool	bitmapQueryNewSafe(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata)
	{
		// Test if there is bitmap tree there. If not exist, rebuild it
		bool rslt = true;
		if(mLevel > 0)
		{
			AosBitmapTreeObjPtr tree = getBitmapTree(rdata);
			if(!tree)
			{
				// tree not exist, rebuild all the bitmaps with trees
//				rslt = rebuildBitmapSafe(rdata);
				OmnAlarm << "The bitmaps are not exist, rebuild it now. IILID:" << mIILID << enderr;
				aos_assert_r(rslt, false);
				return false;
			}			
		}

		AOSLOCK(mLock);
		rslt = bitmapQueryNewPriv(query_rslt, query_bitmap, query_context, rdata);
		AOSUNLOCK(mLock);
		return rslt; 
	}

	virtual bool	bitmapRsltQueryNewSafe(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = bitmapRsltQueryNewPriv(query_rslt, query_bitmap, query_context, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	virtual bool	queryNewSafe(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = queryNewPriv(query_rslt, query_bitmap, rdata);
		AOSUNLOCK(mLock);
		return rslt; 
	}

	bool	removeFirstValueDocSafe(
				const d64 &value,
				u64 &docid,
				const bool reverse,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		AosIILIdx idx;
		bool isunique = true;
		bool rslt = nextDocidPrivFind(idx, value, eAosOpr_eq, docid, isunique, reverse, rdata);
		if (rslt)
		{
			rslt = removeDocPriv(value, docid, rdata);
			aos_assert_rl(rslt, mLock, false);
		}
		AOSUNLOCK(mLock);
		return rslt;
	}
	
	bool 	updateIndexData(
				const i64 &idx,
				const bool changeMax,
				const bool changeMin,
				const AosRundataPtr &rdata);
	
	bool	setValueDocUniqueSafe(
				const d64 &key, 
				const u64 &docid, 
				const bool must_same, 
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = setValueDocUniquePriv(key, docid, must_same, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	virtual bool	saveSubIILToLocalFileSafe(const AosRundataPtr &rdata);

private:
	bool	sanityTestForSubiils();
	bool	splitSanityCheck();
	
	bool 	insertDocSinglePriv(
				i64 &idx, 
				const d64 &value, 
				const u64 &docid, 
				const bool value_unique,
				const bool docid_unique,
				const AosRundataPtr &rdata);

	bool	insertBefore(
				const i64 &idx, 
				const u64 &docid, 
				const d64 &value);

	bool 	insertAfter(
				const i64 &idx, 
				const u64 &docid, 
				const d64 &value);
	
	bool 	addDocPriv(
				const d64 &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const AosRundataPtr &rdata);
	
	bool 	addDocRecPriv(
				const d64 &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const AosRundataPtr &rdata);
	
	bool 	addDocSinglePriv(
				const d64 &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const AosRundataPtr &rdata);
	
	bool	removeDocPriv(
				const d64 &value,
				const u64 &docid,
				const AosRundataPtr &rdata);
	
	bool	removeDocRecPriv(
				const d64 &value,
				const u64 &docid,
				const AosRundataPtr &rdata);
	
	bool 	removeDocSinglePriv(
				const d64 &value, 
				const u64 &docid,
				const AosRundataPtr &rdata); 
	
	bool	nextDocidAN(
				i64 &idx, 
				const bool reverse, 
				const d64 &value,
				u64 &docid);
	bool	nextDocidLT(
				i64 &idx, 
				const bool reverse, 
				const d64 &value,
				u64 &docid,
				const AosRundataPtr &rdata);
	bool	nextDocidLE(
				i64 &idx, 
				const bool reverse, 
				const d64 &value,
				u64 &docid,
				const AosRundataPtr &rdata);
	bool	nextDocidGT(
				i64 &idx, 
				const bool reverse, 
				const d64 &value,
				u64 &docid,
				const AosRundataPtr &rdata);
	bool	nextDocidGE(
				i64 &idx, 
				const bool reverse, 
				const d64 &value,
				u64 &docid,
				const AosRundataPtr &rdata);
	bool	nextDocidEQ(
				i64 &idx, 
				const bool reverse, 
				const d64 &value,
				u64 &docid,
				const AosRundataPtr &rdata);
	bool	nextDocidNE(
				i64 &idx, 
				const bool reverse, 
				const d64 &value,
				u64 &docid,
				const AosRundataPtr &rdata);
	
	i64 firstEQ(const i64 &idx, const d64 &value, const AosRundataPtr &rdata);
	i64 firstEQRev(const i64 &idx, const d64 &value, const AosRundataPtr &rdata);
	i64 firstNE(const i64 &idx, const d64 &value, const AosRundataPtr &rdata);
	i64 firstNERev(const i64 &idx, const d64 &value, const AosRundataPtr &rdata);
	i64 firstLE(const i64 &idx, const d64 &value, const AosRundataPtr &rdata);
	i64 firstLERev(const i64 &idx, const d64 &value, const AosRundataPtr &rdata);
	i64 firstLT(const i64 &idx, const d64 &value, const AosRundataPtr &rdata);
	i64 firstLTRev(const i64 &idx, const d64 &value, const AosRundataPtr &rdata);
	i64 firstGE(const i64 &idx, const d64 &value, const AosRundataPtr &rdata);
	i64 firstGERev(const i64 &idx, const d64 &value, const AosRundataPtr &rdata);
	i64 firstGT(const i64 &idx, const d64 &value, const AosRundataPtr &rdata);
	i64 firstGTRev(const i64 &idx, const d64 &value, const AosRundataPtr &rdata);
	i64 lastLT(const d64 &value, const AosRundataPtr &rdata);
	i64 lastLE(const d64 &value, const AosRundataPtr &rdata);
	i64 lastEQ(const i64 &idx, const d64 &value, const AosRundataPtr &rdata);
	i64 firstLE(const d64 &value, const AosRundataPtr &rdata);
	i64 firstLT(const d64 &value, const AosRundataPtr &rdata);
	
	i64 firstLE(const i64 &idx, const d64 &value, const u64 &docid, const AosRundataPtr &rdata);
	i64 firstLERev(const i64 &idx, const d64 &value, const u64 &docid, const AosRundataPtr &rdata);
	i64 firstLT(const i64 &idx, const d64 &value, const u64 &docid, const AosRundataPtr &rdata);
	i64 firstLTRev(const i64 &idx, const d64 &value, const u64 &docid, const AosRundataPtr &rdata);
	i64 firstGE(const i64 &idx, const d64 &value, const u64 &docid, const AosRundataPtr &rdata);
	i64 firstGERev(const i64 &idx, const d64 &value, const u64 &docid, const AosRundataPtr &rdata);
	i64 firstGT(const i64 &idx, const d64 &value, const u64 &docid, const AosRundataPtr &rdata);
	i64 firstGTRev(const i64 &idx, const d64 &value, const u64 &docid, const AosRundataPtr &rdata);

	bool	nextDocidPrivFind(
				AosIILIdx &idx, 
				const d64 &value,
				const AosOpr opr,
				u64 &docid, 
				bool &isunique,
				const bool reverse, 
				const AosRundataPtr &rdata);
	
	bool	nextDocidSinglePriv(
				i64 &idx, 
				const bool reverse, 
				const AosOpr opr,
				const d64 &value,
				u64 &docid, 
				bool &isunique,
				const AosRundataPtr &rdata);
	
	i64	getTotalPriv(
				const d64 &value,
				const AosOpr opr,
				const AosRundataPtr &rdata);

	i64	getTotalSinglePriv(
				const d64 &value,
				const AosOpr opr,
				const AosRundataPtr &rdata);
	
	virtual bool	returnSubIILsPriv(const AosRundataPtr &rdata)
	{
		bool returned;
		return returnSubIILsPriv(returned, false, rdata);
	}
	virtual bool	returnSubIILsPriv(
						bool &returned,
						const bool returnHeader,
						const AosRundataPtr &rdata);
	
	bool	incrementDocidPriv(
				const d64 &key,
				u64 &value,
				const u64 &incvalue,
				const u64 &init_value,
				const bool add_flag,
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

	//ken 2012/09/04
	bool	clearIILPriv(const AosRundataPtr &rdata);

	bool 	createSubiilIndex();
	
	bool 	splitListPriv(
				AosIILObjPtr &subiil,
				const AosRundataPtr &rdata);

	bool	mergeSubiilPriv(
				const i64 &iilidx,
				const AosRundataPtr &rdata);
	
	bool 	initSubiil(
				u64 *docids, 
				d64 *values, 
				const i64 &numDocs,
				const i64 &subiilidx, 
				const AosIILBigD64Ptr &rootiil);

	bool 	addSubiil(
				const AosIILBigD64Ptr &crtsubiil,
				const AosIILBigD64Ptr &nextsubiil,
				const AosRundataPtr &rdata);
	
	bool	checkDocidUnique(
				const bool rslt, 
				const i64 &idx, 
				u64 &docid,
				bool &isunique,
				const AosRundataPtr &rdata);
	
	bool	setValueDocUniquePriv(
				const d64 &key, 
				const u64 &docid, 
				const bool must_same, 
				const AosRundataPtr &rdata);

	bool	setValueDocUniqueSinglePriv(
				const d64 &key, 
				const u64 &docid, 
				const bool must_same, 
				const AosRundataPtr &rdata);

// ken not implimented
public:
	bool	getValueByIdxSafe(
				AosIILIdx &idx, 
				d64 &value,
				u64 &docid,
				const AosRundataPtr &rdata);
	bool	getValueByIdxRecSafe(
				AosIILIdx &idx, 
				d64 &value,
				u64 &docid,
				const AosRundataPtr &rdata);

private:
	// find a value and docid by a real idx;
	bool	getValueByIdxRecPriv(
				AosIILIdx &idx, 
				d64 &value,
				u64 &docid,
				const AosRundataPtr &rdata);
	bool	getValueByIdxSinglePriv(
				AosIILIdx &idx, 
				d64 &value,
				u64 &docid,
				const AosRundataPtr &rdata);
// ken not implimented finish

	i64 getTotalNEPriv(const d64 &value, const AosRundataPtr &rdata);

	bool	incrementDocidSingleSafe(
				const d64 &key,
				u64 &value,
				const u64 &inc_value,
				const u64 &init_value,
				const bool add_flag,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = incrementDocidSinglePriv(key, value, 
				inc_value, init_value, add_flag, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool	incrementDocidSinglePriv(
				const d64 &key,
				u64 &value,
				const u64 &inc_value,
				const u64 &init_value,
				const bool add_flag,
				const AosRundataPtr &rdata);
/////////////////////////////////////////////////////////////
// shawn functions not orignized
public:
	bool			batchAddSafe(
						char * &entries,
						const i64 &size,
						const i64 &num,
						const AosIILExecutorObjPtr &executor,
						const AosRundataPtr &rdata);
	
	bool			batchAddRecSafe(
						char * &entries,
						const i64 &size,
						i64 &num,
						i64 &subChanged,
						const bool isTail,
						const AosRundataPtr &rdata);

	bool			batchAddParentThrdSafe(
						const u64 &iilid,
						const i64 &iil_idx,
						char * &entries,
						const i64 &size,
						i64 &num,
						const AosRundataPtr &rdata);

	bool			batchAddSingleThrdSafe(
						char * &entries,
						const i64 &size,
						i64 &num,
						const AosRundataPtr &rdata);

	bool			batchAddRebuildCount(
						char * &entries,
						const i64 &size,
						const i64 &num,
						i64 &num_handle,
						d64* orig_values,
						u64* orig_docids,
						const i64 &orig_num,
						i64 &orig_handle,
						const i64 &iilsize,
						const AosRundataPtr &rdata);

	bool			batchAddRebuild(
						char * &entries,
						const i64 &size,
						i64 &num,
						i64 &num_handle,
						d64* orig_values,
						u64* orig_docids,
						i64 &orig_num,
						i64 &orig_idx,
						const i64 &iilsize,
						const AosRundataPtr &rdata);

	void			lockUpdate(){mUpdateLock->lock();}
	void			unlockUpdate(){mUpdateLock->unlock();}

private:
	bool 			splitCheck();
	AosIILBigD64Ptr	splitLeafContent(const AosRundataPtr &rdata);

	bool			splitListSinglePriv(
						const vector<AosIILBigD64Ptr> &subiil_list,
						const AosRundataPtr &rdata);
	bool			subiilSplited(
						const i64 &iilIdx,
						const vector<AosIILBigD64Ptr> &subiil_list,
						const AosIILExecutorObjPtr &executor,
						const AosRundataPtr &rdata);
	bool			subiilSplited(
						const AosIILExecutorObjPtr &executor,
						const AosRundataPtr &rdata);

	AosIILBigD64Ptr	splitContentUtil(const AosRundataPtr &rdata);

	bool 			initSubiilLeaf(
						u64 *docids,
						d64 *values, 
						const i64 &num);

	bool 			initSubiilParent(
						u64 *minDocids,
						d64 *minValues, 
						u64 *maxDocids,
						d64 *maxValues, 
						u64 *iilids,
						i64 *numEntries,
						AosIILBigD64Ptr *subiils,
						const i64 &numSubiils);

	void			setParentIIL(const AosIILBigD64Ptr &parent) {mParentIIL = parent;}
	void			setIILLevel(const int level){mLevel = level;}
public:
	u64 			*getDocids() const {return mDocids;}
	d64				*getValues() const {return mValues;}
private:
	bool			addSubiils(
						const i64 &iilidx,
						const vector<AosIILBigD64Ptr> &subiil_list,
						const AosRundataPtr &rdata);
	AosIILBigD64Ptr	splitParentContent(const AosRundataPtr &rdata);
	bool			splitListRootPriv(
						const vector<AosIILBigD64Ptr> &subiil_list,
						const AosIILExecutorObjPtr &executor,
						const AosRundataPtr &rdata);

	bool 			splitListPriv(
						const AosIILExecutorObjPtr &executor,
						const AosRundataPtr &rdata);
	bool			setContentsSinglePriv(
						AosBuffPtr &buff, 
						const AosRundataPtr &rdata);
	AosBuffPtr 		getBodyBuffSinglePriv() const;

	bool			batchAddPriv(
						char * &entries,
						const i64 &size,
						const i64 &num,
						const AosRundataPtr &rdata);

	bool			batchAddRecPriv(
						char * &entries,
						const i64 &size,
						i64 &num,
						i64 &subChanged,
						const bool isTail,
						const AosRundataPtr &rdata);

	bool			batchAddSinglePriv(
						char * &entries,
						const i64 &size,
						i64 &num,
						i64 &subChanged,
						const bool isTail,
						const AosRundataPtr &rdata);

	bool			batchAddSingleThrdPriv(
						char * &entries,
						const i64 &size,
						i64 &num,
						const AosRundataPtr &rdata);

	i64			binarySearch(
						const char * entries,
						const i64 &size,
						const i64 &num,
						const d64 &key,
						const u64 &value);

	i64			binarySearchInc(
						const char * entries,
						const i64 &size,
						const i64 &num,
						const d64 &key);

	int				valueMatch(
						const char * entry,
						const i64 &size,
						const d64 &key,
						const u64 &value);

	int				valueMatch(
						const char * entry,
						const d64 &key);

	int				valueMatchD64(
						const char * entry1,
						const char * entry2);

	AosIILBigD64Ptr	getSubiilByCondPriv(
						const d64 &value, 
						const u64 &docid,
						const AosOpr opr,
						const bool reverse, 
						const AosRundataPtr &rdata)
	{
		AosIILIdx idx;
		return getSubiilByCondPriv(idx, value, docid, opr, reverse, rdata);
	}
	
	AosIILBigD64Ptr	getSubiilByCondPriv(
						const d64 &value, 
						const AosOpr opr,
						const bool reverse, 
						const AosRundataPtr &rdata)
	{
		AosIILIdx idx;
		return getSubiilByCondPriv(idx, value, opr, reverse, rdata);
	}

	AosIILBigD64Ptr	getSubiilByCondPriv(
						AosIILIdx &index, 
						const d64 &value, 
						const AosOpr opr, 
						const bool reverse, 
						const AosRundataPtr &rdata);
	
	AosIILBigD64Ptr	getSubiilByCondPriv(
						AosIILIdx &index, 
						const d64 &value, 
						const u64 &docid,
						const AosOpr opr, 
						const bool reverse, 
						const AosRundataPtr &rdata);
	
	AosIILBigD64Ptr	getSubiilPriv(
	 					const d64 &value, 
	 					const u64 &docid,
	 					const bool value_unique,
	 					const bool docid_unique,
	 					const AosRundataPtr &rdata);
	
	AosIILBigD64Ptr	getSubiilByIndexPriv(
			 			const i64 &idx,
						const AosRundataPtr &rdata);

	// Chen Ding, 04/22/2012
	bool			hasMoreMembers(const AosIILIdx &idx) const;
	AosIILBigD64Ptr	getFirstSubiilByCondPriv(
						AosIILIdx &idx, 
						const d64 &value, 
						const AosOpr opr, 
						const bool reverse, 
						const AosRundataPtr &rdata);
	AosIILBigD64Ptr	getFirstSubiilByCondPriv(
						AosIILIdx &idx, 
						const d64 &value, 
						const u64 &docid,
						const AosOpr opr, 
						const bool reverse, 
						const AosRundataPtr &rdata);
	AosIILBigD64Ptr	nextSubiilByCondPriv(
						AosIILIdx &idx, 
						const d64 &value, 
						const AosOpr opr, 
						const bool reverse, 
						const AosRundataPtr &rdata);
	AosIILBigD64Ptr	nextSubiilByCondPriv(
						AosIILIdx &idx, 
						const d64 &value, 
						const u64 &docid,
						const AosOpr opr, 
						const bool reverse, 
						const AosRundataPtr &rdata);
	i64			getFirstSubiilIndexGT(
						const d64 &value,
						const AosRundataPtr &rdata);
	i64			getFirstSubiilIndexGT(
						const d64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata);
	i64			getFirstSubiilIndexGE(
						const d64 &value,
						const AosRundataPtr &rdata);
	i64			getFirstSubiilIndexGE(
						const d64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata);
	i64			getLastSubiilIndexLT(
						const d64 &value,
						const AosRundataPtr &rdata);
	i64			getLastSubiilIndexLT(
						const d64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata);
	i64			getLastSubiilIndexLE(
						const d64 &value,
						const AosRundataPtr &rdata);
	i64			getLastSubiilIndexLE(
						const d64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool			containValuePriv(
						const d64 &min_value, 
						const d64 &max_value,
						const d64 &value, 
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
						const d64 &value,
						d64 &unique_value,
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
						const d64 &value,
						d64 &unique_value,
						bool &found,
						const AosRundataPtr &rdata);

	// Chen Ding, 04/22/2012
	bool			nextUniqueValueSinglePriv(
						AosIILIdx &idx,
						const bool reverse,
						const AosOpr opr,
						const d64 &value,
						d64 &unique_value,
						bool &found, 
						const AosRundataPtr &rdata);

	// Chen Ding, 04/22/2012
	inline bool		isIILIdxValid(const AosIILIdx &idx)
	{
		aos_assert_r(mLevel >= 0 && mLevel < AosIILIdx::eMaxNumLevels, false);
		i64 iilidx = idx.getIdx(mLevel);
		return iilidx >= 0 && iilidx < mNumSubiils;
	}

	// Chen Ding, 04/22/2012
	AosIILBigD64Ptr	getSubiil3Priv(
						const d64 &value,
			            const u64 &docid,
						const AosRundataPtr &rdata);

	i64			getSubiilIndex3Priv(
						const d64 &value,
			            const u64 &docid,
						const AosRundataPtr &rdata);

	i64			getSubiilIndex3Priv(
						const d64 &value,
			            const u64 &docid,
						const AosOpr opr,
						const AosRundataPtr &rdata);

	// Ken Lee, 2013/06/24
	bool			setSubiilIndexPriv(
						const AosIILObjPtr &iil,
						const i64 &iil_idx,
						const AosRundataPtr &rdata);

	// Ken Lee, 201307/05
	AosIILBigD64Ptr	getSubiilByIndexPriv(
						const u64 &iilid,
						const i64 &iil_idx,
						const AosRundataPtr &rdata);

public:
	bool			batchIncSafe(
						char * &entries,
						const i64 &size,
						const i64 &num,
						const u64 &dftValue,
						const AosIILUtil::AosIILIncType incType,
						const AosRundataPtr &rdata);

	bool			batchIncRecSafe(
						char * &entries,
						const i64 &size,
						i64 &num,
						const u64 &dftValue,
						const AosIILUtil::AosIILIncType incType,
						i64 &subChanged,
						const bool isTail,
						const AosRundataPtr &rdata);

	bool			batchIncParentThrdSafe(
						const u64 &iilid,
						const i64 &iil_idx,
						char * &entries,
						const i64 &size,
						i64 &num,
						const u64 &dftValue,
						const AosIILUtil::AosIILIncType incType,
						const AosRundataPtr &rdata);

	bool			batchIncSingleThrdSafe(
						char * &entries,
						const i64 &size,
						i64 &num,
						const u64 &dftValue,
						const AosIILUtil::AosIILIncType incType,
						const AosRundataPtr &rdata);

private:
	bool			batchIncPriv(
						char * &entries,
						const i64 &size,
						const i64 &num,
						const u64 &dftValue,
						const AosIILUtil::AosIILIncType incType,
						const AosRundataPtr &rdata);

	bool			batchIncRecPriv(
						char * &entries,
						const i64 &size,
						i64 &num,
						const u64 &dftValue,
						const AosIILUtil::AosIILIncType incType,
						i64 &subChanged,
						const bool isTail,
						const AosRundataPtr &rdata);

	bool			batchIncSinglePriv(
						char * &entries,
						const i64 &size,
						i64 &num,
						const u64 &dftValue,
						const AosIILUtil::AosIILIncType incType,
						i64 &subChanged,
						const bool isTail,
						const AosRundataPtr &rdata);

	bool			batchIncSingleThrdPriv(
						char * &entries,
						const i64 &size,
						i64 &num,
						const u64 &dftValue,
						const AosIILUtil::AosIILIncType incType,
						const AosRundataPtr &rdata);

	bool			batchIncRebuild(
						char * &entries,
						const i64 &size,
						i64 &num,
						const u64 &dftValue,
						const AosIILUtil::AosIILIncType incType,
						i64 &num_handle,
						d64* orig_values,
						u64* orig_docids,
						i64 &orig_num,
						i64 &orig_idx,
						const i64 &iilsize,
						const bool isLastSub,
						const AosRundataPtr &rdata);

	bool			moveDocsForward(
						const AosIILBigD64Ptr &from_iil,
						const AosIILBigD64Ptr &to_iil,
						const i64 &num_to_move);

	bool			sanityCheckPriv(const AosRundataPtr &rdata);
	bool			sanityCheckRecPriv(i64 &num_docs, const AosRundataPtr &rdata);
	bool			sanityCheckSinglePriv(i64 &num_docs, const AosRundataPtr &rdata);
	
//shawn functions
private:
	bool			distributeIILPriv(const AosRundataPtr &rdata);

	bool 			addDocDistr(
						const d64 &value, 
						const u64 &docid, 
						const bool value_unique, 
						const bool docid_unique, 
						const AosRundataPtr &rdata);

	bool			removeDocDistr(
						const d64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool 			modifyDocDistr(
						const d64 &oldvalue, 
						const d64 &newvalue, 
						const u64 &docid, 
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata);

	bool			getDistrInfo(
						i64 &distr_num,
						vector<u64> &iilids, 
						vector<d64> &values,
						vector<u64> &docids);

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
	bool			entireCheckSafe(const AosRundataPtr &rdata);
private:
	bool			entireCheckRecPriv(
						d64 &value,
						u64 &docid,
						const AosRundataPtr &rdata);
	bool			entireCheckSinglePriv(
						d64 &value,
						u64 &docid,
						const AosRundataPtr &rdata);

public:
	bool			queryNewPriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);

	bool			bitmapQueryNewPriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);

	bool			bitmapRsltQueryNewPriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);

	bool			queryNewNEPriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);

	bool			queryNewInPriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);

	bool			bitmapQueryNewNEPriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);

	bool			bitmapQueryNewInPriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);

	bool			bitmapRsltQueryNewNEPriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);

	bool			bitmapRsltQueryNewInPriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);

	bool			queryPagingProc(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						AosIILIdx &start_iilidx,
						AosIILIdx &end_iilidx,
						bool &has_data,
						const bool need_paging,
						const AosRundataPtr &rdata);

	bool			queryPagingProcDistinct(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						AosIILIdx &start_iilidx,
						AosIILIdx &end_iilidx,
						bool &has_data,
						const bool need_paging,
						const AosRundataPtr &rdata);

	bool			copyData(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosIILIdx &start_iilidx,
						const AosIILIdx &end_iilidx,
						const AosRundataPtr &rdata);

	bool			copyBitmap(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosIILIdx &start_iilidx,
						const AosIILIdx &end_iilidx,
						const AosRundataPtr &rdata);

	bool			copyDataWithBitmap(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosIILIdx &start_iilidx,
						const AosIILIdx &end_iilidx,
						const AosRundataPtr &rdata);

	bool			copyDataWithCheck(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosIILIdx &start_iilidx,
						const AosIILIdx &end_iilidx,
						const AosRundataPtr &rdata);

	bool			copyDataRec(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosIILIdx &start_iilidx,
						const AosIILIdx &end_iilidx,
						AosIILIdx &cur_iilidx,
						const AosIILUtil::CopyType copytype, 
						const AosRundataPtr &rdata);

	bool			copyBitmapRec(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosIILIdx &start_iilidx,
						const AosIILIdx &end_iilidx,
						AosIILIdx &cur_iilidx,
						const AosIILUtil::CopyType copytype, 
						const AosRundataPtr &rdata);

	bool			copyDataWithBitmapRec(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosIILIdx &start_iilidx,
						const AosIILIdx &end_iilidx,
						AosIILIdx &cur_iilidx,
						const AosIILUtil::CopyType copytype, 
						const AosRundataPtr &rdata);

	bool			copyBitmapSingle(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosIILIdx &start_iilidx,
						const AosIILIdx &end_iilidx,
						AosIILIdx &cur_iilidx,
						const AosIILUtil::CopyType copytype, 
						const AosRundataPtr &rdata);

	bool			copyDataSingle(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosIILIdx &start_iilidx,
						const AosIILIdx &end_iilidx,
						AosIILIdx &cur_iilidx,
						const AosIILUtil::CopyType copytype, 
						const AosRundataPtr &rdata);

	void			nextIndexFixErrorIdx(
						AosIILIdx &start_iilidx,
						AosIILIdx &end_iilidx,
						AosIILIdx &cur_iilidx,
						const d64 &cur_value,
						const u64 &cur_docid,
						const bool reverse,
						bool &has_data,
						const AosRundataPtr &rdata);
						
	i64			countNumRec(
						const AosIILIdx start_iilidx,
						const AosIILIdx end_iilidx,
						const AosRundataPtr &rdata);

	i64			countNumToEndRec(
						const AosIILIdx start_iilidx,
					   	const AosRundataPtr &rdata);

	i64			countNumFromStartRec(
						const AosIILIdx end_iilidx,
					   	const AosRundataPtr &rdata);

	i64			countNumSingle(
						const AosIILIdx start_iilidx,
						const AosIILIdx end_iilidx);

	i64			countNumToEndSingle(const AosIILIdx start_iilidx);

	i64			countNumFromStartSingle(const AosIILIdx end_iilidx);

	bool			moveToRec(
						AosIILIdx &start_iilidx,
						AosIILIdx &end_iilidx,
						u64 &page_start,
						const bool &reverse,
						bool &has_data,
						const AosIILUtil::CopyType copytype, 
						const AosRundataPtr &rdata);

	bool			moveToSingle(
						AosIILIdx &start_iilidx,
						AosIILIdx &end_iilidx,
						u64 &page_start,
						const bool &reverse,
						bool &has_data,
						const AosIILUtil::CopyType copytype, 
						const AosRundataPtr &rdata);

	bool			queryPagingProcRec(
						const AosQueryContextObjPtr &query_context,
						AosIILIdx &start_iilidx,
						AosIILIdx &end_iilidx,
						i64 &bsize,
						const bool reverse,
						const bool from_start,
						const AosRundataPtr &rdata);

	bool			queryPagingProcSingle(
						const AosQueryContextObjPtr &query_context,
						AosIILIdx &start_iilidx,
						AosIILIdx &end_iilidx,
						i64 &bsize,
						const bool reverse,
						const bool from_start,
						const AosRundataPtr &rdata);

private:
	bool			prevQueryPosSingle(
						AosIILIdx &iilidx,
						const AosRundataPtr &rdata);
	
	bool			prevQueryPosL1(
						AosIILIdx &iilidx,
						const AosRundataPtr &rdata);
	
	bool			prevQueryPosRec(
						AosIILIdx &iilidx,
						const AosRundataPtr &rdata);
	
	bool			lastQueryPosSingle(
						AosIILIdx &iilidx,
						const AosRundataPtr &rdata);
	
	bool			lastQueryPosL1(
						AosIILIdx &iilidx,
						const AosRundataPtr &rdata);
	
	bool			lastQueryPosRec(
						AosIILIdx &iilidx,
						const AosRundataPtr &rdata);
	
	bool			nextQueryPosSingle(
						AosIILIdx &iilidx,
						const AosRundataPtr &rdata);
	
	bool			nextQueryPosL1(
						AosIILIdx &iilidx,
						const AosRundataPtr &rdata);
	
	bool			nextQueryPosRec(
						AosIILIdx &iilidx,
						const AosRundataPtr &rdata);


	bool			deleteIILRecSafe(
						vector<u64> &iilids,
						const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = deleteIILRecPriv(iilids, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool			deleteIILPriv(
						const bool true_delete,
						const AosRundataPtr &rdata);
	bool			deleteIILRecPriv(
						vector<u64> &iilids,
						const AosRundataPtr &rdata);

	i64		subInMem()
	{
		if (!mSubiils) return 0;
		i64 i = 0;
		for(i64 j = 0;j < mNumSubiils;j++)
		{
			if (mSubiils[j]) i++;
		}
		return i;
	}
public:
	//ken 2012/07/31
	bool	getSplitValueSafe(
				const AosQueryContextObjPtr &context,
				const i64 &size,
				vector<AosQueryContextObjPtr> &contexts,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		context->setIILType(getIILType());
		bool rslt = getSplitValuePriv(context, size, contexts, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

private:
	bool	getSplitValuePriv(
				const AosQueryContextObjPtr &context,
				const i64 &size,
				vector<AosQueryContextObjPtr> &contexts,
				const AosRundataPtr &rdata);

	bool	getSplitValueRecPriv(
				i64 &num_splitters, 
				i64 &cur_split_num, 
				i64 &num_each_split, 
				i64 &cur_doc_num,
				d64 &cur_value,
				const AosQueryRsltObjPtr &query_rslt,
				const AosRundataPtr &rdata);

	bool			getSplitValueRecSafe(
						i64 &num_splitters, 
						i64 &cur_split_num, 
						i64 &num_each_split, 
						i64 &cur_doc_num,
						d64 &cur_value,
						const AosQueryRsltObjPtr &query_rslt,
						const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = getSplitValueRecSafe(
						num_splitters, 
						cur_split_num, 
						num_each_split, 
						cur_doc_num,
						cur_value,
						query_rslt,
						rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool			getSplitValueSinglePriv(
						i64 &num_splitters, 
						i64 &cur_split_num, 
						i64 &num_each_split, 
						i64 &cur_doc_num,
						d64 &cur_value,
						const AosQueryRsltObjPtr &query_rslt,
						const AosRundataPtr &rdata);

	bool			getSplitValuePriv(
						const i64 &num_blocks,
						const AosQueryRsltObjPtr &query_rslt,
						const AosRundataPtr &rdata);

	bool			getSplitValueRec(
						const AosIILUtil::CopyType type,
						const AosIILIdx &start_iilidx,
						const AosIILIdx &end_iilidx,
						const i64 &each,
						i64 &num_left,
						i64 &saperator_num_left,
						vector<d64> &values,
						const AosRundataPtr &rdata);

	bool			getSplitValueSingle(
						const AosIILUtil::CopyType type,
						const AosIILIdx &start_iilidx,
						const AosIILIdx &end_iilidx,
						const i64 &each,
						i64 &num_left,
						i64 &saperator_num_left,
						vector<d64> &values,
						const AosRundataPtr &rdata);
		
	bool	preQueryNEPriv(
				const AosQueryContextObjPtr &query_context,
				const AosRundataPtr &rdata);

	bool	preQueryPriv(
				const AosQueryContextObjPtr &query_context,
				const AosRundataPtr &rdata);

	bool	returnSubIILPriv(
				const i64 &idx,
				const AosRundataPtr &rdata);

public:
	bool			batchDelSafe(
						char * &entries,
						const i64 &size,
						const i64 &num,
						const AosIILExecutorObjPtr &executor,
						const AosRundataPtr &rdata);
	
	bool			batchDelRecSafe(
						char * &entries,
						const i64 &size,
						i64 &num,
						const AosRundataPtr &rdata);

private:
	bool			batchDelPriv(
						char * &entries,
						const i64 &size,
						const i64 &num,
						const AosRundataPtr &rdata);

	bool			batchDelRecPriv(
						char * &entries,
						const i64 &size,
						i64 &num,
						const AosRundataPtr &rdata);

	bool			batchDelSinglePriv(
						char * &entries,
						const i64 &size,
						i64 &num,
						const AosRundataPtr &rdata);

	bool			resetMaxMin(
						const i64 &idx,
						const AosRundataPtr &rdata);

public:
	bool			batchDecSafe(
						char * &entries,
						const i64 &size,
						const i64 &num,
						const bool delete_flag,
						const AosIILUtil::AosIILIncType incType,
						const AosRundataPtr &rdata);

	bool			batchDecRecSafe(
						char * &entries,
						const i64 &size,
						i64 &num,
						const bool delete_flag,
						const AosIILUtil::AosIILIncType incType,
						const AosRundataPtr &rdata);

private:
	bool			batchDecPriv(
						char * &entries,
						const i64 &size,
						const i64 &num,
						const bool delete_flag,
						const AosIILUtil::AosIILIncType incType,
						const AosRundataPtr &rdata);

	bool			batchDecRecPriv(
						char * &entries,
						const i64 &size,
						i64 &num,
						const bool delete_flag,
						const AosIILUtil::AosIILIncType incType,
						const AosRundataPtr &rdata);

	bool			batchDecSinglePriv(
						char * &entries,
						const i64 &size,
						i64 &num,
						const bool delete_flag,
						const AosIILUtil::AosIILIncType incType,
						const AosRundataPtr &rdata);

public:
	AosBitmapTreeObjPtr getBitmapTree(const AosRundataPtr &rdata);

	AosBitmapObjPtr createBitmap();
	bool			setBitmapTree(
						const AosBitmapTreeObjPtr &tree,
						const AosRundataPtr &rdata);

	bool			buildBitmap(
						const AosBitmapObjPtr &bitmap,
						const AosRundataPtr &rdata);

	bool			checkWithBitmap(
						const int &pos,
						const AosBitmapObjPtr &query_bitmap,
						bool &parent_ignored,
						u64 &crt_parent_id,
						vector<u64> &child_list,
						const AosRundataPtr &rdata);

	virtual bool	saveBitmapTree(const AosRundataPtr &rdata);

	bool			rebuildLevelOne(
						const u64 &index, 
						const u64 num, 
						const u64 node_id,
						const AosRundataPtr &rdata);

private:
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

	virtual bool	rebuildBitmapSafe(const AosRundataPtr &rdata);
	bool			clearBitmapPriv(
						AosIILExecutorObjPtr &executor,
						const AosRundataPtr &rdata);
	bool			clearBitmapRecPriv(
						AosIILExecutorObjPtr &executor,
						const AosRundataPtr &rdata);
	bool			rebuildBitmapPriv(
						AosIILExecutorObjPtr &executor,
						const AosRundataPtr &rdata);
	bool			rebuildBitmapRecPriv(
						AosIILExecutorObjPtr &executor,
						const AosRundataPtr &rdata);
	bool			checkValueDocidRec(
						const AosIILIdx &cur_iilidx,
						const d64 &cur_value,
						const u64 &cur_docid,
						const AosRundataPtr &rdata);
	bool			locateIdx(
						AosIILIdx &end_iilidx,
						AosIILIdx &start_iilidx,
						AosIILIdx &cur_iilidx,
						const d64 &cur_value,
						const u64 &cur_docid,
						const bool reverse,
						bool &has_data,
						const AosRundataPtr &rdata);

	bool			setLimitation(
						AosIILIdx &end_iilidx,
						AosIILIdx &start_iilidx,
						const d64 &cur_value,
						const u64 &cur_docid,
						const bool reverse,
						bool &has_data,
						const AosRundataPtr &rdata);
	bool			findPosPriv(
						const AosOpr opr,
						const d64 &value,
						const u64 &docid, 
						AosIILIdx &idx,
						const bool reverse, 
						const AosRundataPtr &rdata);
						
	bool			findPosSinglePriv(
						const AosOpr opr,
						const d64 &value,
						const u64 &docid, 
						AosIILIdx &idx,
						const bool reverse, 
						const AosRundataPtr &rdata);

	bool			getDistrMap(
						const AosQueryContextObjPtr &query_context,
						const AosIILIdx &start_iilidx,
						const AosIILIdx &end_iilidx,
						const bool reverse,
						const i64 &total,
						const AosRundataPtr &rdata);
	
	bool			getDistrMapRec(
						const AosQueryContextObjPtr &query_context,
						const AosIILIdx &start_iilidx,
						const AosIILIdx &end_iilidx,
						AosIILIdx &cur_iilidx,
						vector<d64> &values, 
						vector<u64> &docids, 
						vector<u64> &num_entries,
						const bool reverse,
						const u64 &step,
						const AosRundataPtr &rdata);

public:
	virtual void	setSnapShotId(const u64 &snap_id);
	virtual void	resetSnapShotId();
	virtual void	setSnapShotIdPriv(const u64 &snap_id);
	virtual void	resetSnapShotIdPriv();
	virtual bool	resetIIL(const AosRundataPtr &rdata);

private:
	bool			copyDataSingleWithThrd(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const i64 &start,
						const i64 &end,
						AosIILIdx &cur_iilidx,
						const AosRundataPtr &rdata);

	bool	rebuildBitmapPriv1(const AosRundataPtr &rdata);
public:
	bool			copyDataSingleThrdSafe(
						AosIILBigD64CopyDataSingleThrd *thrd,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const i64 &start,
						const i64 &end,
						const AosRundataPtr &rdata);

};

#endif

