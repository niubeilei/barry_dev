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
// 	Created: 08/10/2011 by Shawn Li
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILMgr_IILCompU64_h
#define AOS_IILMgr_IILCompU64_h

#include "IILMgr/IIL.h"
#include "IILMgr/IILUtil.h"
#include "SearchEngine/SeCommon.h"
#include "Util1/MemMgr.h"

#include <string.h>

/*
#define AosIILCompU64SanityCheck(x) true
// #define AosIILStrSanityCheck(x) (x)->listSanityCheck()

class AosIILCompU64 : public AosIIL
{

public:
	enum
	{
		eMinStrLen = 5,
	};

private:
	u64*						mValues;

	// Member data used to index sub-iils. Only the first
	// IIL will use them. 
	u64*						mMinDocids;
	u64*						mMaxDocids;
	AosIILCompU64Ptr *			mSubiils;

public:
	AosIILCompU64(
		const bool isPersis,
		const bool iilmgrLocked, 
		const AosRundataPtr &rdata);
	AosIILCompU64(
		const u64 &iilid, 
		const u32 siteid, 
		AosBuffPtr &header_buff, 	//Ketty 2012/11/15
		const bool iilmgrLocked, 
		const AosRundataPtr &rdata);
	AosIILCompU64(
		const u64 &wordid, 
		const u64 &iilid,
		const bool isPersis,
		const OmnString &iilname, 
		const bool iilmgrlocked, 
		const AosRundataPtr &rdata);

	~AosIILCompU64();

	bool 			addDocSafe(
						const u64 &value, 
						const u64 &docid, 
						const bool value_unique, 
						const bool docid_unique, 
						const bool iilmgrLocked,
						const AosRundataPtr &rdata)
	{
		// Ketty 2013/01/15
		OmnNotImplementedYet;
		return false;
		//AOSLOCK(mLock);
		//bool rslt = addDocPriv(value, docid, 
		//	value_unique, docid_unique, iilmgrLocked, rdata);
//		//aos_assert_rb(AosIILCompU64SanityCheck(this), mLock, false);
		//AOSUNLOCK(mLock);
		//return rslt;
	}

	bool	removeDocSafe(
				const u64 &value, 
				const u64 &docid, 
				const AosRundataPtr &rdata)
	{
		// Ketty 2013/01/15
		OmnNotImplementedYet;
		return false;
		
		//AOSLOCK(mLock);
		//bool rslt = removeDocPriv(value, docid, rdata);
//		//aos_assert_rb(AosIILCompU64SanityCheck(this), mLock, false);
		//AOSUNLOCK(mLock);
		//return rslt;
	}

	virtual bool	deleteIILSafe(
						const bool iilmgrLocked,
						const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = deleteIILPriv(iilmgrLocked, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}


private:
	virtual bool	deleteIILPriv(
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);
	virtual bool	deleteIILSinglePriv(
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);
	virtual bool	checkDocSafe(
						const u64 &docid,
						const u64 &value,
						const AosOpr opr,
						const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = checkDocPriv(docid, value, opr, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	virtual i64 getTotalNumDocsSafe()const
	{
		mLock->lock();
		i64 numdoc = mNumDocs;
		if (isParentIIL())
		{
			numdoc = 0;
			for(i64 i=0; i<mNumSubiils; i++)
			{
				numdoc += mNumEntries[i];
			}
		}
		mLock->unlock();
		return numdoc;
	}

	virtual bool	querySafe(
						const AosQueryRsltObjPtr& query_rslt,
						const AosBitmapObjPtr& query_bitmap,
		 				const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata)
	{
		OmnShouldNeverComeHere;
		return false;
	}

	virtual bool	bitmapQueryNewSafe(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata)
	{
		OmnNotImplementedYet;
		return false;
	}
						

	virtual bool	bitmapRsltQueryNewSafe(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata)
	{
		OmnNotImplementedYet;
		return false;
	}

	bool    copyData(
				u64 values[AOSSENG_TORTURER_MAX_DOCS], 
				u64 *docids, 
				const i64 &buff_len,
				i64 &num_docs,
				const AosRundataPtr &rdata);
	
	virtual bool	isCompIIL(){return true;}

	virtual bool	preQuerySafe(
						const AosQueryContextObjPtr &query_context,
						const bool iilmgrlock, 
						const AosRundataPtr &rdata)
	{
		OmnShouldNeverComeHere;
		return false;
	}

	virtual bool	queryNewSafe(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = queryNewPriv(query_rslt,query_bitmap,query_context,rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}
protected:
	bool			queryNewPriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);
	//virtual bool	saveSubIILToTransFileSafe(
	//					const AosDocTransPtr &docTrans,
	//					const bool forcesave, 
	//					const AosRundataPtr &rdata);
	virtual bool	saveSubIILToLocalFileSafe(
						const bool forcesave, 
						const AosRundataPtr &rdata);

private:
	bool    copyDataSingle(
				u64 values[AOSSENG_TORTURER_MAX_DOCS], 
				u64 *docids, 
				const i64 &buff_len,
				i64 &num_docs);
	bool	checkDocSingleSafe(
				const u64 &docid,
				const u64 &value,
				const AosOpr opr,
				bool &keepSearch,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = checkDocSinglePriv(docid, value, opr, keepSearch, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool	checkDocPriv(
				const u64 &docid,
				const u64 &value,
				const AosOpr opr,
				const AosRundataPtr &rdata);

	bool	checkDocSinglePriv(
				const u64 &docid,
				const u64 &value,
				const AosOpr opr,
				bool &keepSearch,
				const AosRundataPtr &rdata);

	AosIILCompU64Ptr 	getSubiil(const u64 &docid, const AosRundataPtr &rdata);
	i64				getSubiilIndex(const u64 &docid);
	AosIILCompU64Ptr 	getSubiilByIndex(const i64 &idx, const AosRundataPtr &rdata);

	virtual bool	firstDoc(
						i64 &idx, 
						const bool reverse, 
						const u64 &docid);

	bool	nextDocidEQ(
				i64 &idx, 
				const i64 &reverse, 
				const u64 &docid);

	i64	firstEQ(const i64 &idx, const u64 &docid);
	i64	firstEQRev(const i64 &idx, const u64 &docid);

	bool 	addDocPriv(
				const u64 &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const bool iilmgrLocked,
				const AosRundataPtr &rdata);

	bool 	addDocDirectSafe(
				const u64 &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const bool iilmgrLocked,
				bool &keepSearch,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = addDocDirectPriv(
						value, docid, value_unique, docid_unique, iilmgrLocked, keepSearch, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool	addDocDirectPriv(
				const u64 &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const bool iilmgrLocked,
				bool &keepSearch,
				const AosRundataPtr &rdata);

	bool	updateIndexData(
				const i64 &idx,
				const bool changeMax,
				const bool changeMin);

	bool	expandMemoryPriv();
	bool	checkMemory() const;

	bool 	insertBefore(
				const i64 &nn, 
				const u64 &docid, 
				const u64 &value);

	bool 	insertAfter(
				const i64 &nn, 
				const u64 &docid, 
				const u64 &value);

	bool 	insertDocPriv(
				i64 &idx, 
				const u64 &value, 
				const u64 &docid, 
				const bool value_unique,
				const bool docid_unique,
				const bool iilmgrLocked,
				bool &keepSearch);
	bool 	splitListPriv(
				const bool iilmgrLocked,
				AosIILObjPtr &subiil,
				const AosRundataPtr &rdata);

	bool 	initSubiil(
				u64 *docids, 
				u64 *values, 
				const i64 &numDocs,
				const i64 &subiilid, 
				const AosIILCompU64Ptr &rootiil, 
				const bool iilmgrLocked);

	bool 	addSubiil(
				const AosIILCompU64Ptr &crtsubiil,
				const AosIILCompU64Ptr &nextsubiil);

	bool	createSubiilIndex();

	bool	removeDocDirectSafe(
				const u64 &value, 
				const u64 &docid,
				bool &keepSearch, 
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = removeDocDirectPriv(value, docid,keepSearch, rdata);
		aos_assert_rb(AosIILCompU64SanityCheck(this), mLock, false);
		AOSUNLOCK(mLock);
		return rslt;
	}

	AosIILCompU64Ptr	getNextSubIIL(const bool forward, const AosRundataPtr &rdata);

	bool	removeDocPriv(
				const u64 &value, 
				const u64 &docid,
				const AosRundataPtr &rdata); 
	
	bool	removeDocDirectPriv(
				const u64 &value, 
				const u64 &docid,
				bool &keepSearch,
				const AosRundataPtr &rdata); 

	bool 	mergeSubiilPriv(
				const bool iilmgrLocked,
				const i64 &iilidx, 
				const AosRundataPtr &rdata);

	bool	mergeSubiilForwardPriv(
				const bool iilmgrLocked,
				const i64 &iilidx,
				const i64 &numDocToMove, 
				const AosRundataPtr &rdata);
	
	bool 	mergeSubiilBackwardPriv(
				const bool iilmgrLocked,
				const i64 &iilidx,
				const i64 &numDocToMove, 
				const AosRundataPtr &rdata);

	bool	getValueDocidPtr(
				u64* 	 &valuePtr,
				u64*    &docidPtr,
				const i64 &offset);

	bool 	appendDocToSubiil(
				u64 *values, 
				u64 *docids, 
				const i64 &numDocs,
				const bool addToHead);

	bool 	removeDocFromSubiil(
				const i64 &numDocs,
				const bool delFromHead);

	bool	removeSubiil(
				const i64 &iilidx,
				const bool iilmgrLocked,
				const AosRundataPtr &rdata);

	virtual bool	resetSubIILInfo(const AosRundataPtr &rdata);
	virtual bool	returnSubIILsSafe(
						const bool iilmgrLocked,
						bool &returned,
						const AosRundataPtr &rdata,
						const bool returnHeader)
	{
        AOSLOCK(mLock);
        bool rslt = returnSubIILsPriv(iilmgrLocked,returned, rdata, returnHeader);
        AOSUNLOCK(mLock);
		return rslt;		
	}

	virtual AosIILType	getIILType() const {return eAosIILType_CompU64;}
	//virtual bool 		saveIILsToFileSafe(const OmnFilePtr &file, u32 &crt_pos, const AosRundataPtr &rdata);
	
private:
	virtual bool	saveSanityCheckProtected(const AosRundataPtr &rdata)
	{
		static AosIILObjPtr lsTestIIL = OmnNew AosIILCompU64(false,true, rdata);
		static OmnMutex lsLock;
	
		lsLock.lock();
		bool rslt = AosIIL::saveSanityCheck(lsTestIIL, rdata);
		lsLock.unlock();
		return rslt;
	}

	virtual AosBuffPtr	getBodyBuffProtected() const;

	virtual bool 	setContentsProtected(AosBuffPtr &buff, const bool, const AosRundataPtr &rdata);

	virtual bool 	prepareMemoryForReloading(const bool iilmgrLocked);

	virtual u64		getMinDocid() const;

	virtual u64		getMaxDocid() const;

	virtual bool	resetSpec();

	virtual bool	returnSubIILsPriv(
						const bool iilmgrLocked,
						bool &returned, 
						const AosRundataPtr &rdata,
						const bool returnHeader);

	virtual bool 	sanityTestForSubiils()
	{
		return true;	
	}

	bool			splitSanityCheck()
	{
		return true;	
	}

	inline bool		valueMatch(
						const u64 v1, 
						const AosOpr opr, 
						const u64 v2)
	{
		switch (opr)
		{
			case eAosOpr_le: return v1 <= v2 ;
			case eAosOpr_lt: return v1 <  v2;
			case eAosOpr_eq: return v1 == v2;
			case eAosOpr_gt: return v1 >  v2;
			case eAosOpr_ge: return v1 >= v2;
			case eAosOpr_ne: return v1 != v2;
			case eAosOpr_an: return true;
			default: return false;
		}
		return false;
	}

public:

};
*/
#endif

