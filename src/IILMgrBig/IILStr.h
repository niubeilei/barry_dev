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
#ifndef AOS_IILMgrBig_IILStr_h
#define AOS_IILMgrBig_IILStr_h

#include "BatchQuery/Ptrs.h"
#include "IILMgrBig/IIL.h"
#include "SEUtil/IILIdx.h"
#include "IILUtil/IILUtil.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SearchEngine/SeCommon.h"
#include "Util1/MemMgr.h"
#include "Util/StrU64Array.h"

#include <string.h>
#include <vector>

using namespace std;


#define AosIILStrSanityCheck(x) true
// #define AosIILStrSanityCheck(x) (x)->listSanityCheck()

class AosIILStr : public AosIIL
{

public:
	enum
	{
		eMinStrLen = 5,
		eMaxSubChangedToSave = 10,
		
		// Ken Lee, 2013/01/03
		//eAddBlockMaxSize = 20000,
		//eAddBlockMinSize = 4000,
		//eAddBlockFillSize = 16000,
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
	char **			mValues;
	
	char **			mMinVals;
	u64 *			mMinDocids;
	char **			mMaxVals;
	u64 *			mMaxDocids;
	AosIILStrPtr *	mSubiils;
	bool			mIsNumAlpha;
	
	int				mLevel;
	AosIILStrPtr	mParentIIL;

	
	DistrType		mDistrType;
	DistrStatus		mDistrStatus;
	//static AosMemCheckerPtr	mMemChecker;

public:
	AosIILStr(
			const bool isPersis, 
			const bool iilmgrLocked, 
			const AosRundataPtr &rdata);
	AosIILStr(
			const u64 &wordid, 
			const u64 &iilid,
			const bool isPersis,
			const OmnString &iilname, 
			const bool iilmgrlocked, 
			const AosRundataPtr &rdata);
	AosIILStr(
			const u64 &iilid, 
			const u32 siteid, 
			const AosDfmDocIILPtr &doc,         //Ketty 2012/11/15
			const bool iilmgrLocked, 
			const AosRundataPtr &rdata);
	~AosIILStr();

	static bool	staticInit(const AosXmlTagPtr &config);
				
	virtual bool 	hasParentIIL()const{return mParentIIL;}
	virtual bool 	isRootIIL()const{return !mParentIIL;}
	virtual bool 	isChildIIL()const{return mParentIIL;}
	virtual bool	isLeafIIL()const{return mLevel == 0;}
	virtual bool	isBranchIIL()const{return (mLevel > 0 && mParentIIL);}
	virtual bool	isParentIIL()const{return mLevel > 0;}
	virtual bool	isSingleIIL()const{return (mLevel == 0 && !mParentIIL);}
	virtual int 	getLevel() const {return mLevel;}
	virtual AosIILPtr	getRootIIL()	
	{		
		AosIILStrPtr strRoot = mParentIIL;		
		if(!mParentIIL)		
		{			
			AosIILStrPtr thisPtr(this, false);			
			strRoot = thisPtr;		
		}		
		else		
		{			
			while(!strRoot->isRootIIL())			
			{				
				strRoot = strRoot->getParentIIL();			
			}		
		}		
		return strRoot;	
	}

	AosIILStrPtr getParentIIL()const{return mParentIIL;}
	void	setNumAlpha(){mIsNumAlpha = true;}
	bool	isNumAlpha() const {return mIsNumAlpha;}
	virtual bool		saveSanityCheckProtected(const AosRundataPtr &rdata);
	virtual AosBuffPtr	getBodyBuffProtected() const;
	virtual bool 		setContentsProtected(AosBuffPtr &buff, const bool, const AosRundataPtr &rdata);
	virtual bool 		prepareMemoryForReloading(const bool iilmgrLocked);
	virtual u64			getMinDocid() const;
	virtual u64			getMaxDocid() const;
	const char * 		getMinValue() const;
	const char * 		getMaxValue() const;
	
	// Chen Ding, 06/30/2012
	inline bool	addDocSafe(
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const bool iilmgrlocked,
				const AosRundataPtr &rdata)
	{
		return addDocSafe(value, docid, value_unique, docid_unique, rdata);
	}

	bool 	addDocSafe(
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = addDocPriv(value, docid, value_unique, docid_unique, rdata);
		aos_assert_rb(AosIILStrSanityCheck(this), mLock, false);
		AOSUNLOCK(mLock);
		return rslt;
	}
	
	bool	deleteIILSafe(
				const bool iilmgrLocked,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = deleteIILPriv(iilmgrLocked, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool 	addDocRecSafe(
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = addDocRecPriv(value, docid, value_unique, docid_unique, rdata);
		aos_assert_rb(AosIILStrSanityCheck(this), mLock, false);
		AOSUNLOCK(mLock);
		return rslt;
	}
	
	bool	removeDocSafe(
				const OmnString &value,
				const u64 &docid,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = removeDocPriv(value, docid, rdata);
		aos_assert_rb(AosIILStrSanityCheck(this), mLock, false);
		AOSUNLOCK(mLock);
		return rslt;
	}
	
	bool	removeDocRecSafe(
				const OmnString &value,
				const u64 &docid,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = removeDocRecPriv(value, docid, rdata);
		aos_assert_rb(AosIILStrSanityCheck(this), mLock, false);
		AOSUNLOCK(mLock);
		return rslt;
	}
	
	bool 	modifyDocSafe(
				const OmnString &oldvalue, 
				const OmnString &newvalue, 
				const u64 &docid, 
				const bool value_unique,
				const bool docid_unique,
				const AosRundataPtr &rdata);
// shell function	
	bool 	modifyDocSafe(
				const OmnString &oldvalue, 
				const OmnString &newvalue, 
				const u64 &docid, 
				const bool value_unique,
				const bool docid_unique,
				const bool iilmgrlocked,
				const bool override_flag,
				const AosRundataPtr &rdata)
	{
		return modifyDocSafe(oldvalue, newvalue, docid, value_unique, docid_unique, rdata);
	}

	bool 	modifyDocSafe(
				const OmnString &oldvalue, 
				const OmnString &newvalue, 
				const u64 &docid, 
				const bool value_unique,
				const bool docid_unique,
				const bool iilmgrlocked,
				const AosRundataPtr &rdata)
	{
		return modifyDocSafe(oldvalue, newvalue, docid, value_unique, docid_unique, rdata);
	}

	// shell function
	bool	nextDocidSafe(
				AosIILIdx the_idx,
				int &idx, 
				int &iilidx,
				const bool reverse, 
				const AosOpr opr,
				const OmnString &value,
				u64 &docid, 
				bool &isunique,
				const AosRundataPtr &rdata)
	{
		return nextDocidSafeFind(
				the_idx, 
				value,
				opr,
				docid,
				isunique,
				reverse, 
				rdata); 
	}

	bool	nextDocidSafeFind(
				AosIILIdx &idx, 
				const OmnString &value,
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
				const OmnString &key,
				const u64 &dft,
				const AosRundataPtr &rdata)
	{
		u64 docid;
		bool rslt = incrementDocidSafe(key, docid, 1, dft, true, rdata);
		if (!rslt) return dft;
		return docid;
	}

	bool incrementDocidSafe(
				const OmnString &key,
				u64 &value,
				const u64 &inc_value,
				const u64 &init_value,
				const bool add_flag,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = incrementDocidPriv(key, value, inc_value, init_value, add_flag, rdata);
		aos_assert_rb(AosIILStrSanityCheck(this), mLock, false);
		AOSUNLOCK(mLock);
		return rslt;
	}
	
	virtual u64 getTotalSafe(
				const OmnString &value, 
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
						const bool iilmgrlock, 
						const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = preQueryPriv(query_context,iilmgrlock, rdata);
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
	//AosIILCompStrPtr 	retrieveCompIIL(
	//						const bool iilmgr_locked,
	//						const AosRundataPtr &rdata);
	
	//bool				returnCompIIL(
	//						const AosIILPtr &iil,
	//						const AosRundataPtr &rdata);
	
	//virtual AosIILPtr	createCompIIL(const AosRundataPtr &rdata);

	bool	removeFirstValueDocSafe(
				const OmnString &value,
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
				const OmnString &key, 
				const u64 &docid, 
				const bool must_same, 
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = setValueDocUniquePriv(key, docid, must_same, rdata);
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
	
	bool	setValue(
				const int idx, 
				const char *value,
				const int length);

	bool 	insertDocSinglePriv(
				int &idx, 
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique,
				const bool docid_unique,
				const AosRundataPtr &rdata);

	bool	insertBefore(
				const int idx, 
				const u64 &docid, 
				const OmnString &value);

	bool 	insertAfter(
				const int idx, 
				const u64 &docid, 
				const OmnString &value);
	
	bool 	addDocPriv(
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const AosRundataPtr &rdata);
	
	bool 	addDocRecPriv(
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const AosRundataPtr &rdata);
	
	bool 	addDocSinglePriv(
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const AosRundataPtr &rdata);
	
	bool	removeDocPriv(
				const OmnString &value,
				const u64 &docid,
				const AosRundataPtr &rdata);
	
	bool	removeDocRecPriv(
				const OmnString &value,
				const u64 &docid,
				const AosRundataPtr &rdata);
	
	bool 	removeDocSinglePriv(
				const OmnString &value, 
				const u64 &docid,
				const AosRundataPtr &rdata); 
	
	bool	nextDocidAN(
				int &idx, 
				const bool reverse, 
				const OmnString &value,
				u64 &docid);
	bool	nextDocidLT(
				int &idx, 
				const bool reverse, 
				const OmnString &value,
				u64 &docid);
	bool	nextDocidLE(
				int &idx, 
				const bool reverse, 
				const OmnString &value,
				u64 &docid);
	bool	nextDocidGT(
				int &idx, 
				const bool reverse, 
				const OmnString &value,
				u64 &docid);
	bool	nextDocidGE(
				int &idx, 
				const bool reverse, 
				const OmnString &value,
				u64 &docid);
	bool	nextDocidEQ(
				int &idx, 
				const bool reverse, 
				const OmnString &value,
				u64 &docid);
	int		nextDocidNE(
				int &idx, 
				const bool reverse, 
				const OmnString &value,
				u64 &docid);
	bool	nextDocidLike(
				int &idx, 
				const bool reverse, 
				const OmnString &value,
				u64 &docid);
	bool	nextDocidPrefix(
				int &idx, 
				const bool reverse, 
				const OmnString &prefix,
				u64 &docid);
	
	int		firstPrefixRev(const int idx, const OmnString &prefix);
	int 	firstEQ(const int idx, const OmnString &value);
	int 	firstEQRev(const int idx, const OmnString &value);
	int 	firstNE(const int idx, const OmnString &value);
	int 	firstNERev(const int idx, const OmnString &value);
	int 	firstLE(const int idx, const OmnString &value);
	int 	firstLERev(const int idx, const OmnString &value);
	int 	firstLT(const int idx, const OmnString &value);
	int 	firstLTRev(const int idx, const OmnString &value);
	int 	firstGE(const int idx, const OmnString &value);
	int 	firstGERev(const int idx, const OmnString &value);
	int 	firstGT(const int idx, const OmnString &value);
	int 	firstGTRev(const int idx, const OmnString &value);
	int 	lastLT(const OmnString &value);
	int 	lastLE(const OmnString &value);
	int 	lastEQ(const int idx, const OmnString &value);
	int 	firstLE(const OmnString &value);
	int 	firstLT(const OmnString &value);
	
	bool	nextDocidPrivFind(
				AosIILIdx &idx, 
				const OmnString &value,
				const AosOpr opr,
				u64 &docid, 
				bool &isunique,
				const bool reverse, 
				const AosRundataPtr &rdata);
	
	bool	nextDocidSinglePriv(
				int &idx, 
				const bool reverse, 
				const AosOpr opr,
				const OmnString &value,
				u64 &docid, 
				bool &isunique,
				const AosRundataPtr &rdata);
	
	u64		getTotalPriv(
				const OmnString &value,
				const AosOpr opr,
				const AosRundataPtr &rdata);

	u64 	getTotalSinglePriv(
				const OmnString &value,
				const AosOpr opr,
				const AosRundataPtr &rdata);
	
	virtual bool	returnSubIILsPriv(
						const bool iilmgrLocked,
						bool &returned,
						const AosRundataPtr &rdata,
						const bool returnHeader);
	
	bool	incrementDocidPriv(
				const OmnString &key,
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
				const bool iilmgrLocked,
				AosIILPtr &subiil,
				const AosRundataPtr &rdata);

	bool	mergeSubiilPriv(
				const bool iilmgrLocked,
				const int iilidx,
				const AosRundataPtr &rdata);
	
	bool 	initSubiil(
				u64 *docids, 
				char **values, 
				const int numDocs,
				const int subiilid, 
				const AosIILStrPtr &rootiil, 
				const bool iilmgrLocked); 

	bool 	addSubiil(
				const AosIILStrPtr &crtsubiil,
				const AosIILStrPtr &nextsubiil,
				const AosRundataPtr &rdata);
	
	bool	checkDocidUnique(
				const bool rslt, 
				const int idx, 
				u64 &docid,
				bool &isunique,
				const AosRundataPtr &rdata);
	
	bool	setValueDocUniquePriv(
				const OmnString &key, 
				const u64 &docid, 
				const bool must_same, 
				const AosRundataPtr &rdata);

	bool	setValueDocUniqueSinglePriv(
				const OmnString &key, 
				const u64 &docid, 
				const bool must_same, 
				const AosRundataPtr &rdata);

// ken not implimented
public:
	bool	getValueByIdxSafe(
				AosIILIdx &idx, 
				OmnString &value,
				u64 &docid,
				const AosRundataPtr &rdata);
	bool	getValueByIdxRecSafe(
				AosIILIdx &idx, 
				OmnString &value,
				u64 &docid,
				const AosRundataPtr &rdata);

private:
	// find a value and docid by a real idx;
	bool	getValueByIdxRecPriv(
				AosIILIdx &idx, 
				OmnString &value,
				u64 &docid,
				const AosRundataPtr &rdata);
	bool	getValueByIdxSinglePriv(
				AosIILIdx &idx, 
				OmnString &value,
				u64 &docid,
				const AosRundataPtr &rdata);
// ken not implimented finish

	u64 getTotalNEPriv(const OmnString &value, const AosRundataPtr &rdata);
	u64 getTotalLikePriv(const OmnString &value, const AosRundataPtr &rdata);
	u64 getTotalPrefixPriv(const OmnString &value, const AosRundataPtr &rdata);

	bool	incrementDocidSingleSafe(
				const OmnString &key,
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
				const OmnString &key,
				u64 &value,
				const u64 &inc_value,
				const u64 &init_value,
				const bool add_flag,
				const AosRundataPtr &rdata);
/////////////////////////////////////////////////////////////
// shawn functions not orignized
public:
	bool			addBlockSafe(
						char * &entries,
						const int size,
						const int num,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);
	
	bool			addBlockRecSafe(
						char * &entries,
						const int size,
						int &num,
						int &subChanged,
						const bool &isTail,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

private:
	bool 			splitCheck();
	AosIILStrPtr	splitLeafContent(
						const bool iilmgrLocked, 
						const AosRundataPtr &rdata);

	bool			splitListSinglePriv(
						const vector<AosIILStrPtr> &subiil_list,
						const bool iilmgrLocked, 
						const AosRundataPtr &rdata);
	bool			subiilSplited(
						const int iilIdx,
						const vector<AosIILStrPtr> &subiil_list,
						const bool iilmgrLocked, 
						const AosRundataPtr &rdata);

	AosIILStrPtr	splitContentUtil(
						const bool iilmgrLocked, 
						const AosRundataPtr &rdata);



	bool 			initSubiilLeaf(
						u64 *docids,
						char **values, 
						const int numDocs,
						const bool iilmgrLocked);

	bool 			initSubiilParent(
						u64 *minDocids,
						char **minValues, 
						u64 *maxDocids,
						char **maxValues, 
						u64 *iilids,
						int *numEntries,
						AosIILStrPtr *subiils,
						const int numSubiils,
						const bool iilmgrLocked);

	void			setParentIIL(const AosIILStrPtr &parent){mParentIIL = parent;}
	void			setIILLevel(const int level){mLevel = level;}
	u32				getNumDocs() const {return mNumDocs;}

	bool			addSubiils(
						const int &iilidx,
						const vector<AosIILStrPtr> &subiil_list,
						const AosRundataPtr &rdata);
	AosIILStrPtr	splitParentContent(
						const bool iilmgrLocked, 
						const AosRundataPtr &rdata);
	bool			splitListRootPriv(
						const vector<AosIILStrPtr> &subiil_list,
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

	bool			addBlockRecPriv(
						char * &entries,
						const int size,
						int &num,
						int &subChanged,
						const bool &isTail,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

	bool			addBlockSinglePriv(
						char * &entries,
						const int size,
						int &num,
						int &subChanged,
						const bool isTail,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

	bool			addBlockRebuild(
						char * &entries,
						const int size,
						int &num,
						int &num_handle,
						char** &orig_values,
						u64* &orig_docids,
						int &orig_num,
						int &orig_idx,
						const int iilsize);
							
	int 			binarySearch(
						const char * entries,
						const int size,
						const int num,
						const char * value,
						const u64 &docid);
	
	int 			binarySearchInc(
						const char * entries,
						const int size,
						const int num,
						const char * value,
						const u64 &docid);

	int 			binarySearch2(
						char* value,
						u64 docid);
	
	int				valueMatch(
						const char * entry,
						const int size,
						const char * value,
						const u64 &docid);

	// Chen Ding, 04/23/2012
	// All 'getSubiil(...)' functions are finding the first occurrence functions.
	// All 'next...(...)' functions shall not be used anymore.
	// AosIILStrPtr getSubiilSafe(
	// 			const OmnString &value, 
	// 			const u64 &docid,
	// 			const bool reverse, 
	// 			const bool iilmgrLocked,
	// 			const AosRundataPtr &rdata)
	// {
	// 	AOSLOCK(mLock);
	// 	AosIILIdx idx;
	// 	AosIILStrPtr iil = getSubiilPriv(idx, value, docid, false, false, 
	// 							reverse, iilmgrLocked, rdata);
	// 	AOSUNLOCK(mLock);
	// 	return iil;
	// }

	// AosIILStrPtr getSubiilPriv(
	// 			const OmnString &value, 
	// 			const u64 &docid,
	// 			const bool reverse, 
	// 			const bool iilmgrLocked,
	// 			const AosRundataPtr &rdata)
	// {
	// 	AosIILIdx idx;
	// 	return getSubiilPriv(idx, value, docid, false, false, reverse, iilmgrLocked, rdata);
	// }

	AosIILStrPtr 	getSubiilByCondPriv(
						const OmnString &value, 
						const u64 &docid,
						const AosOpr opr,
						const bool reverse, 
						const AosRundataPtr &rdata)
	{
		AosIILIdx idx;
		return getSubiilByCondPriv(idx, value, docid, opr, reverse, rdata);
	}
	
	AosIILStrPtr 	getSubiilByCondPriv(
						const OmnString &value, 
						const AosOpr opr,
						const bool reverse, 
						const AosRundataPtr &rdata)
	{
		AosIILIdx idx;
		return getSubiilByCondPriv(idx, value, opr, reverse, rdata);
	}

	AosIILStrPtr	getSubiilByCondPriv(
						AosIILIdx &index, 
						const OmnString &value, 
						const AosOpr opr, 
						const bool reverse, 
						const AosRundataPtr &rdata);
	
	AosIILStrPtr	getSubiilByCondPriv(
						AosIILIdx &index, 
						const OmnString &value, 
						const u64 &docid,
						const AosOpr opr, 
						const bool reverse, 
						const AosRundataPtr &rdata);
	
	AosIILStrPtr	getSubiilPriv(
	 					const OmnString &value, 
	 					const u64 &docid,
	 					const bool value_unique,
	 					const bool docid_unique,
	 					const bool iilmgrLocked, 
	 					const AosRundataPtr &rdata);
	
	AosIILStrPtr	getSubiilByIndexPriv(
			 			const int idx,
						const bool iilmgrlocked,
						const AosRundataPtr &rdata);

	// Chen Ding, 04/22/2012
	bool			hasMoreMembers(const AosIILIdx &idx) const;
	AosIILStrPtr	getFirstSubiilByCondPriv(
						AosIILIdx &idx, 
						const OmnString &value, 
						const AosOpr opr, 
						const bool reverse, 
						const AosRundataPtr &rdata);
	AosIILStrPtr	getFirstSubiilByCondPriv(
						AosIILIdx &idx, 
						const OmnString &value, 
						const u64 &docid,
						const AosOpr opr, 
						const bool reverse, 
						const AosRundataPtr &rdata);
	AosIILStrPtr	nextSubiilByCondPriv(
						AosIILIdx &idx, 
						const OmnString &value, 
						const AosOpr opr, 
						const bool reverse, 
						const AosRundataPtr &rdata);
	AosIILStrPtr	nextSubiilByCondPriv(
						AosIILIdx &idx, 
						const OmnString &value, 
						const u64 &docid,
						const AosOpr opr, 
						const bool reverse, 
						const AosRundataPtr &rdata);
	int				getFirstSubiilIndexGT(
						const OmnString &value,
						const AosRundataPtr &rdata);
	int				getFirstSubiilIndexGT(
						const OmnString &value,
						const u64 &docid,
						const AosRundataPtr &rdata);
	int				getFirstSubiilIndexGE(
						const OmnString &value,
						const AosRundataPtr &rdata);
	int				getFirstSubiilIndexGE(
						const OmnString &value,
						const u64 &docid,
						const AosRundataPtr &rdata);
	int				getLastSubiilIndexLT(
						const OmnString &value,
						const AosRundataPtr &rdata);
	int				getLastSubiilIndexLT(
						const OmnString &value,
						const u64 &docid,
						const AosRundataPtr &rdata);
	int				getLastSubiilIndexLE(
						const OmnString &value,
						const AosRundataPtr &rdata);
	int				getLastSubiilIndexLE(
						const OmnString &value,
						const u64 &docid,
						const AosRundataPtr &rdata);
	int				getLastSubiilIndexPrefix(
						const OmnString &value,
						const AosRundataPtr &rdata);
	int				getLastSubiilIndexPrefix(
						const OmnString &value,
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool			containValuePriv(
						const char *min_value, 
						const char *max_value,
						const OmnString &value, 
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
						const OmnString &value,
						OmnString &unique_value,
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
						const OmnString &value,
						OmnString &unique_value,
						bool &found,
						const AosRundataPtr &rdata);

	// Chen Ding, 04/22/2012
	bool			nextUniqueValueSinglePriv(
						AosIILIdx &idx,
						const bool reverse,
						const AosOpr opr,
						const OmnString &value,
						OmnString &unique_value,
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
	AosIILStrPtr	getSubiil3Priv(
						const OmnString &value,
			            const u64 &docid,
						const bool iilmgrlock,
						const AosRundataPtr &rdata);
	
	int          	getSubiilIndex3Priv(
						const OmnString &value,
			            const u64 &docid,
						const AosRundataPtr &rdata);
	int          	getSubiilIndex3Priv(
						const OmnString &value,
			            const u64 &docid,
						const AosOpr opr,
						const AosRundataPtr &rdata);

public:
	bool			incBlockSafe(
						char * &entries,
						const int size,
						const int num,
						const u64 &dftValue,
						const AosIILUtil::AosIILIncType incType,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

	bool			incBlockRecSafe(
						char * &entries,
						const int size,
						int &num,
						const u64 &dftValue,
						const AosIILUtil::AosIILIncType incType,
						int &subChanged,
						const bool &isTail,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

private:
	bool			incBlockRecPriv(
						char * &entries,
						const int size,
						int &num,
						const u64 &dftValue,
						const AosIILUtil::AosIILIncType incType,
						int &subChanged,
						const bool &isTail,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

	bool			incBlockSinglePriv(
						char * &entries,
						const int size,
						int &num,
						const u64 &dftValue,
						const AosIILUtil::AosIILIncType incType,
						int &subChanged,
						const bool isTail,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

	bool			incBlockRebuild(
						char * &entries,
						const int size,
						int &num,
						const u64 &dftValue,
						const AosIILUtil::AosIILIncType incType,
						int &num_handle,
						char** &orig_values,
						u64* &orig_docids,
						int &orig_num,
						int &orig_idx,
						const int maxiilsize,
						const bool isLastSub);

	int				valueMatch(
						const char * entry,
						const char * value);

	int				valueMatchStr(
						const char * entry1,
						const char * entry2);

	bool			moveDocsForward(
						const AosIILStrPtr &from_iil,
						const AosIILStrPtr &to_iil,
						const int &num_to_move);

	bool			sanityCheckPriv(const AosRundataPtr &rdata);
	bool			sanityCheckRecPriv(int &num_docs, const AosRundataPtr &rdata);
	bool			sanityCheckSinglePriv(int &num_docs, const AosRundataPtr &rdata);
	
	
//shawn functions
private:
	bool			distributeIILPriv(const bool iilmgrlocked,
						 		  const AosRundataPtr &rdata);

	bool 			addDocDistr(
						const OmnString &value, 
						const u64 &docid, 
						const bool value_unique, 
						const bool docid_unique, 
						const AosRundataPtr &rdata);

	bool			removeDocDistr(
						const OmnString &value,
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool 			modifyDocDistr(
						const OmnString &oldvalue, 
						const OmnString &newvalue, 
						const u64 &docid, 
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata);

	bool			addBlockDistrPriv(
						char * &entries,
						const int size,
						const int num,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

	bool			getDistrInfo(int &distr_num,
						vector<u64>		  &iilids, 
						vector<OmnString> &values,
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
	bool			entireCheckRecPriv(OmnString &value,
									   u64		&docid,
							  		   const bool iilmgrLocked,
									   const AosRundataPtr &rdata);
	bool			entireCheckSinglePriv(OmnString &value,
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


	bool			queryNewLikePriv(
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
						const AosQueryRsltObjPtr    &query_rslt,
						const AosBitmapObjPtr  &query_bitmap,
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
						const OmnString &cur_value,
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

	bool			getSplitValueSafe(
						const int &num_blocks,
						const AosQueryRsltObjPtr &query_rslt,
						const bool iilmgrlock, 
						const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = getSplitValuePriv(num_blocks, query_rslt,iilmgrlock,rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

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

	bool			getSplitValuePriv(
						const int &num_blocks,
						const AosQueryRsltObjPtr &query_rslt,
						const bool iilmgrlock, 
						const AosRundataPtr &rdata);


	bool			getSplitValueRecSafe(
						int &num_splitters, 
						int &cur_split_num, 
						int &num_each_split, 
						int &cur_doc_num,
						OmnString &cur_value,
						const AosQueryRsltObjPtr &query_rslt,
						const bool iilmgrlock, 
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
						iilmgrlock, 
						rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}
	bool			getSplitValueRecPriv(
						int &num_splitters, 
						int &cur_split_num, 
						int &num_each_split, 
						int &cur_doc_num,
						OmnString &cur_value,
						const AosQueryRsltObjPtr &query_rslt,
						const bool iilmgrlock, 
						const AosRundataPtr &rdata);

	bool			getSplitValueSinglePriv(
						int &num_splitters, 
						int &cur_split_num, 
						int &num_each_split, 
						int &cur_doc_num,
						OmnString &cur_value,
						const AosQueryRsltObjPtr &query_rslt,
						const bool iilmgrlock, 
						const AosRundataPtr &rdata);
	bool	deleteIILRecSafe(
				const bool iilmgrLocked,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = deleteIILRecPriv(iilmgrLocked, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool	deleteIILPriv(
				const bool iilmgrLocked,
				const AosRundataPtr &rdata);
	bool	deleteIILRecPriv(
				const bool iilmgrLocked,
				const AosRundataPtr &rdata);

	bool	deleteIILSinglePriv(
				const bool iilmgrLocked,
				const AosRundataPtr &rdata);

	int		subInMem()
	{
		if(!mSubiils) return 0;
		int i = 0;
		for(int j = 0;j < (int)mNumSubiils;j++)
		{
			if(mSubiils[j])i++;
		}
		return i;
	}

public:
	//ken 2012/07/31
	bool	getSplitValueSafe(
				const AosQueryContextObjPtr &context,
				const int size,
				vector<AosQueryContextObjPtr> &contexts,
				const bool iilmgrlock,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = getSplitValuePriv(context,size,contexts,iilmgrlock,rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}
private:
	bool	getSplitValuePriv(
				const AosQueryContextObjPtr &context,
				const int size,
				vector<AosQueryContextObjPtr> &contexts,
				const bool iilmgrlock,
				const AosRundataPtr &rdata);

	bool	getSplitValueRec(
				const AosIILUtil::CopyType type,
				const AosIILIdx &start_iilidx,
				const AosIILIdx &end_iilidx,
				const int each,
				int &num_left,
				int &saperator_num_left,
				vector<OmnString> &values,
				const bool iilmgrlock,
				const AosRundataPtr &rdata);

	bool	getSplitValueSingle(
				const AosIILUtil::CopyType type,
				const AosIILIdx &start_iilidx,
				const AosIILIdx &end_iilidx,
				const int each,
				int &num_left,
				int &saperator_num_left,
				vector<OmnString> &values,
				const bool iilmgrlock,
				const AosRundataPtr &rdata);
	
	bool	preQueryNEPriv(
				const AosQueryContextObjPtr &query_context,
				const bool iilmgrlock, 
				const AosRundataPtr &rdata);

	bool	preQueryPriv(
				const AosQueryContextObjPtr &query_context,
				const bool iilmgrlock, 
				const AosRundataPtr &rdata);

	bool	returnSubIILPriv(
				const int idx,
				const bool iilmgrLocked,
				const AosRundataPtr &rdata);

	bool    compWithSVType(
				const OmnString &str1,
				const OmnString &str2,
				AosQueryContextObj::SplitValueType svtype);

	OmnString	getSVStr(
					const OmnString &key,
					AosQueryContextObj::SplitValueType svtype,
					const OmnString &sep);

public:
	bool			delBlockSafe(
						char * &entries,
						const int size,
						const int num,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);
	
	bool			delBlockRecSafe(
						char * &entries,
						const int size,
						int &num,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

private:
	bool			delBlockRecPriv(
						char * &entries,
						const int size,
						int &num,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

	bool			delBlockSinglePriv(
						char * &entries,
						const int size,
						int &num,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

	bool			resetMaxMin(
						const int idx,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

public:
	bool			decBlockSafe(
						char * &entries,
						const int size,
						const int num,
						const bool delete_flag,
						const AosIILUtil::AosIILIncType incType,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

	bool			decBlockRecSafe(
						char * &entries,
						const int size,
						int &num,
						const bool delete_flag,
						const AosIILUtil::AosIILIncType incType,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

private:
	bool			decBlockRecPriv(
						char * &entries,
						const int size,
						int &num,
						const bool delete_flag,
						const AosIILUtil::AosIILIncType incType,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

	bool			decBlockSinglePriv(
						char * &entries,
						const int size,
						int &num,
						const bool delete_flag,
						const AosIILUtil::AosIILIncType incType,
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
	bool getNextDocids(
		char ***values,
		u64 **docids, 
		int &num, 
		AosIILIdx &iilidx,
		const AosRundataPtr &rdata);
	bool incMergeIIL(
		AosIILStrPtr &iil, 
		int size, 
		const u64 dftvalue, 
		const AosIILUtil::AosIILIncType incType,
		const AosRundataPtr &rdata);
	bool addMergeIIL(
		AosIILStrPtr &iil, 
		int size, 
		const AosRundataPtr &rdata);

	// Chen Ding, 2013/03/01
	virtual bool computeQueryResults(
					const AosRundataPtr &rdata, 
					const OmnString &iilname, 
					const AosQueryContextObjPtr &context, 
					const AosBitmapObjPtr &bitmap, 
					const u64 &query_id, 
					const int physical_id);

	bool retrieveQueryBlock(
					const AosRundataPtr &rdata,
					const AosQueryReqObjPtr &query);

private:
	// Chen Ding, 2013/03/03
	bool determineQueryStartAndEnd(
					const AosRundataPtr &rdata, 
					const AosQueryReqObjPtr &query, 
					AosIILIdx &start_pos, 
					AosIILIdx &end_pos);

	// Chen Ding, 2013/03/03
	AosBitmapObjPtr getLeafBitmap(
					const AosRundataPtr &rdata, 
					const AosIILIdx &start_pos, 
					const AosIILIdx &end_pos);

	AosBitmapObjPtr getFirstBitmap(
					const AosRundataPtr &rdata, 
					const AosIILIdx &start_pos, 
					const bool reverse);

	AosBitmapObjPtr getLastBitmap(
					const AosRundataPtr &rdata, 
					const AosIILIdx &end_pos, 
					const bool reverse);

	bool retrieveQueryBlockSingle(
					const AosRundataPtr &rdata, 
					const AosQueryReqObjPtr &query);

	bool retrieveQueryBlockLevelOne(
					const AosRundataPtr &rdata, 
					const AosQueryReqObjPtr &query);

	bool retrieveQueryBlockRec(
					const AosRundataPtr &rdata, 
					const AosQueryReqObjPtr &query);

	virtual u64 getSubIILID(const int idx) const;
	virtual int64_t getNumDocsInSubiil(const int idx) const;
};
#endif

