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
#ifndef AOS_IILMgr_IILU64_h
#define AOS_IILMgr_IILU64_h

#include "IILMgr/IIL.h"
#include "IILUtil/IILUtil.h"
#include "SEUtil/IILIdx.h"
#include "SearchEngine/SeCommon.h"
#include "Util/Ptrs.h"
#include "Util1/MemMgr.h"

#include <vector>

using namespace std;


#define AosIILU64SanityCheck(x) true
// #define AosIILU64SanityCheck(x) (x)->listSanityCheck()

class AosIILU64 : public AosIIL
{

private:
	u64*			mValues;

	// Member data used to index sub-iils. Only the first
	// IIL will use them. 
	u64 *			mMinVals;
	u64 *			mMaxVals;
	AosIILU64Ptr *	mSubiils;

public:
	AosIILU64();
	AosIILU64(
			const u64 &wordid, 
			const u64 &iilid,
			const bool isPersis,
			const OmnString &iilname, 
			const AosRundataPtr &rdata);
	AosIILU64(
			const u64 &iilid, 
			const u32 siteid, 
			const AosDfmDocIILPtr &doc,
			const AosRundataPtr &rdata);
	~AosIILU64();

	static bool	staticInit(const AosXmlTagPtr &config);

	// Thread Safe Functions
	bool	nextDocidSubSafe(
				i64 &idx, 
				const bool reverse, 
				const AosOpr opr,
				u64 &value,
				u64 &docid, 
				bool &isunique,
				const AosRundataPtr &rdata) 
	{
		AOSLOCK(mLock);
		bool rslt = nextDocidSinglePriv(idx, reverse, opr, value, docid, isunique, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool	nextDocidSubSafe(
				i64 &idx, 
				const bool reverse, 
				const AosOpr opr1,
				const u64 &value1,
				const AosOpr opr2,
				const u64 &value2,
				u64 &docid, 
				bool &isunique,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = nextDocidSinglePriv(
						idx, reverse, opr1, value1, opr2, value2, docid, isunique, rdata);
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

	bool	removeDocDirectSafe(
				const u64 &value,
				const u64 &docid,
				bool &keepSearch,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = removeDocDirectPriv(value, docid, keepSearch, rdata);
		aos_assert_rb(AosIILU64SanityCheck(this), mLock, false);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool	removeDocSeqSafe(
				const u64 &value, 					
				const u64 &docid,
				i64 &iilidx,
				i64 &idx,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = removeDocSeqPriv(value, docid,iilidx,idx, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}
	
	bool	removeDocSeqDirectSafe(
				const u64 &value, 
				const u64 &docid,
				i64 &iilidx,
				i64 &idx,
				bool &keepSearch,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = removeDocSeqDirectPriv(value, docid,iilidx,idx,keepSearch, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	virtual u64	nextDocIdSafe2(
					i64 &idx,
					i64 &iilidx,
					const bool reverse,
					const AosRundataPtr &rdata);

	bool	nextDocidSafe(
				i64 &idx, 
				i64 &iilidx,
				const bool reverse, 
				u64 &value,
				u64 &docid,
				const AosRundataPtr &rdata) 
	{
		AOSLOCK(mLock);
		bool isunique;
		bool rslt = nextDocidPriv(
						idx, iilidx, reverse, eAosOpr_an, value, docid, isunique, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool	nextDocidSafe(
				AosIILIdx the_idx,
				i64 &idx, 
				i64 &iilidx,
				const bool reverse, 
				const AosOpr opr,
				u64 &value,
				u64 &docid, 
				bool &isunique,
				const AosRundataPtr &rdata)
	{
		return nextDocidSafe(idx, iilidx, reverse,  opr, value, docid, isunique, rdata); 
	}

	bool	nextDocidSafe(
				i64 &idx, 
				i64 &iilidx,
				const bool reverse, 
				const AosOpr opr,
				u64 &value,
				u64 &docid, 
				bool &isunique,
				const AosRundataPtr &rdata) 
	{
		AOSLOCK(mLock);
		bool rslt = nextDocidPriv(idx, iilidx, reverse, opr, value, docid, isunique, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool	nextDocidSafe(
				i64 &idx, 
				i64 &iilidx,
				const bool reverse, 
				const AosOpr opr1,
				const u64 &value1,
				const AosOpr opr2,
				const u64 &value2,
				u64 &docid, 
				bool &isunique,
				const AosRundataPtr &rdata);

	virtual i64		getTotalSafe(const u64 &value, const AosOpr opr);

	bool 	modifyDocSafe(
				const u64 &oldvalue, 
				const u64 &newvalue, 
				const u64 &docid, 
				const bool value_unique,
				const bool docid_unique,
				const AosRundataPtr &rdata);

	bool 	addDocSafe(
				const u64 &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = addDocPriv(value, docid, value_unique, docid_unique, rdata);
		aos_assert_rb(AosIILU64SanityCheck(this), mLock, false);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool	addDocSeqSafe(
				const u64 &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique,
				i64 &iilidx,
				i64 &idx,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = addDocSeqPriv(value, docid, value_unique, docid_unique, iilidx, idx, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool	addDocDirectSafe(
				const u64 &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				bool &keepSearch,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = addDocDirectPriv(value, docid, value_unique, docid_unique, keepSearch, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool	addDocSeqDirectSafe(
				const u64 &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				i64 &idx,
				bool &keepSearch,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = addDocSeqDirectPriv(value, docid, value_unique, docid_unique, idx, keepSearch, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}
	virtual bool	checkDocSafe(
						const AosOpr opr,
						const u64 &value,
						const u64 &docid,
						bool &keepSearch,
						const AosRundataPtr &rdata);

	bool 	prependDocSafe(
				const u64 &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique);

	i64	getDocidsSafe(
				const AosOpr opr,
				u64 &value,
				u64 *docids, 
				const i64 &arraysize,
				const AosRundataPtr &rdata);
							
							
	bool 	incrementDocidSafe(
				const u64 &entry_id, 
				u64 &value,
				const u64 &incValue, 
				const u64 &initValue, 
				const bool add_flag,
				const u64 &dft_value,
				const AosRundataPtr &rdata);

	virtual u64		getDocIdSafe1(
						i64 &idx,
						i64 &iilidx,
						const AosRundataPtr &rdata);

	virtual bool	preQuerySafe(
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = preQueryPriv(query_context,rdata);
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
		bool rslt = queryNewPriv(query_rslt,query_bitmap,query_context,rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	virtual bool	bitmapQueryNewSafe(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata)
	{
		bool rslt = queryNewSafe(0,query_bitmap,query_context,rdata);
		return rslt;
	}
						

	virtual bool	bitmapRsltQueryNewSafe(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata)
	{
		bool rslt = queryNewSafe(query_rslt,query_bitmap,query_context,rdata);
		return rslt;
	}

	virtual i64 getTotalNumDocsSafe() const
	{
		AOSLOCK(mLock);
		i64 numdoc = mNumDocs;
		if (isParentIIL())
		{
			numdoc = 0;
			for(i64 i = 0; i<mNumSubiils; i++)
			{
				numdoc += mNumEntries[i];
			}
		}
		AOSUNLOCK(mLock);
		return numdoc;
	}

	AosIILU64Ptr getSubiilPublic(const i64 &idx, const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		AosIILU64Ptr iil = getSubiilByIndex(idx, rdata);
		AOSUNLOCK(mLock);
		return iil;
	}

	virtual bool	querySafe(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata)
	{
		aos_assert_r(query_context,false);
		AOSLOCK(mLock);
		bool rslt = queryPriv(
						query_rslt, 
						query_bitmap, 
						query_context->getOpr(), 
						query_context->getStrValue().toU64(0), 
						rdata);
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

	u64				getValueSafe(
						const i64 &idx,
						const i64 &iilidx,
						const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		if (isSingleIIL())
		{
			if (!mValues || idx < 0 || idx >= mNumDocs)
			{
				OmnAlarm << "invalid idx: " << idx << ":" << mNumDocs << enderr;
				AOSUNLOCK(mLock);
				return 0;
			}
			u64 value = mValues[idx];
			AOSUNLOCK(mLock);
			return value;
		}

		AosIILU64Ptr subiil = getSubiilByIndex(iilidx, rdata);
		aos_assert_rb(subiil, mLock, 0);

		if (idx < 0 || idx >= subiil->mNumDocs)
		{
			OmnAlarm << "invalid idx: " << idx << ":" << subiil->mNumDocs << enderr;
			AOSUNLOCK(mLock);
			return 0;
		}
		u64 value = subiil->mValues[idx];
		AOSUNLOCK(mLock);
		return value;
	}

	virtual bool	saveSubIILToLocalFileSafe(const AosRundataPtr &rdata);

	virtual bool	queryValueSafe(
						vector<u64> &values,
						const AosOpr opr,
						const u64 &value,
						const bool unique_value,
						const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = queryValuePriv(values, opr, value, unique_value, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool	setValueDocUniqueSafe(
				const u64 &key,
				const u64 &docid,
				const bool must_same,
				const AosRundataPtr &rdata);

private:

	u64		nextDocidPriv(
				i64 &idx,
				i64 &iilidx,
				const bool reverse,
				const AosRundataPtr &rdata);

	bool	nextDocidPriv(
				i64 &idx, 
				const bool reverse, 
				const AosOpr opr1,
				const u64 &value1,
				const AosOpr opr2,
				const u64 &value2,
				u64 &docid, 
				bool &isunique); 

	bool	nextDocidPriv(
				i64 &idx, 
				i64 &iilidx,
				const bool reverse, 
				const AosOpr opr,
				u64 &value,
				u64 &docid, 
				bool &isunique,
				const AosRundataPtr &rdata);

	virtual bool	checkDocPriv(
						const AosOpr opr,
						const u64 &value,
						const u64 &docid,
						bool &keepSearch,
						const AosRundataPtr &rdata);

	virtual bool		saveSanityCheckProtected(const AosRundataPtr &rdata);
	virtual AosBuffPtr	getBodyBuffProtected() const;
	virtual bool 		setContentsProtected(AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool 		prepareMemoryForReloading();
	virtual u64			getMinDocid() const;
	virtual u64			getMaxDocid() const;


	u64 			getMinValue() const;
	u64 			getMaxValue() const;
	virtual bool	resetSubIILInfo(const AosRundataPtr &rdata);
	virtual bool	resetSpec();

	virtual bool	deleteIILPriv(
						const bool true_delete,
						const AosRundataPtr &rdata);
	virtual bool	deleteIILSinglePriv(const AosRundataPtr &rdata);

	bool			queryNewPriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);

						
	bool			queryNewNEPriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);
	
	bool			queryPagingProc(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						i64 &start_idx,
						i64 &start_iilidx,
						i64 &end_idx,
						i64 &end_iilidx,
						bool &has_data,
						const AosRundataPtr &rdata);
	
	void			nextIndexFixErrorIdx(
						i64 &cur_idx,
						i64 &cur_iilidx,
						const u64 &cur_value,
						const u64 &cur_docid,
						const bool reverse,
						bool &has_data,
						const AosRundataPtr &rdata);

	bool			verifyIdx(
						const i64 &cur_idx,
						const u64 &cur_value,
						const u64 &cur_docid);
	
	i64			compPos(
						const i64 &idx1,
						const i64 &iilidx1,
						const i64 &idx2,
						const i64 &iilidx2);

	bool			copyDataWithCheck(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const i64 &start_idx,
						const i64 &start_iilidx,
						const i64 &end_idx,
						const i64 &end_iilidx,
						const AosRundataPtr &rdata);
	
	bool			copyDataSingleWithCheck(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const i64 &start_idx,
						const i64 &end_idx,
						const bool reverse, 
						bool  keep_search,
						const AosRundataPtr &rdata);

	bool			copyData(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const i64 &start_idx,
						const i64 &start_iilidx,
						const i64 &end_idx,
						const i64 &end_iilidx,
						const AosRundataPtr &rdata);
	
	bool			copyDataSingle(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const i64 &start_idx,
						const i64 &end_idx,
						const AosRundataPtr &rdata);
	bool			nextQueryPos(i64 &idx,i64 &iilidx);
	bool			prevQueryPos(i64 &idx,i64 &iilidx);
	bool			locateIdx(
						i64 &cur_idx,
						i64 &cur_iilidx,
						const u64 &cur_value,
						const u64 &cur_docid,
						const bool reverse,
						bool &has_data,
						const AosRundataPtr &rdata);

	virtual AosIILType	getIILType() const {return eAosIILType_U64;}
	virtual bool	firstDoc1(
						i64 &idx, 
						const bool reverse, 
						const AosOpr opr,
						const u64 &value,
						u64 &docid);
	AosIILU64Ptr	getNextSubIIL(const bool forward, const AosRundataPtr &rdata);

	virtual bool	checkDocSafe(
						const AosOpr opr,
						const u64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool	nextDocidSinglePriv(
				i64 &idx,
				const bool reverse,
				const AosOpr opr1,
				const u64 &value1,
				const AosOpr opr2,
				const u64 &value2,
				u64 &docid, 
				bool &isunique,
				const AosRundataPtr &rdata){return false;}; 

	bool	nextDocidSinglePriv(
				i64 &idx, 
				const bool reverse, 
				const AosOpr opr,
				u64 &value,
				u64 &docid, 
				bool &isunique,
				const AosRundataPtr &rdata);

	bool	expandMemoryPriv();

	bool	splitListPriv(AosIILObjPtr &subiil, const AosRundataPtr &rdata);

	bool 	addDocPriv(
				const u64 &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const AosRundataPtr &rdata);

	bool	addDocDirectPriv(
				const u64 &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				bool &keepSearch,
				const AosRundataPtr &rdata);

	bool	addDocSeqPriv(
				const u64 &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				i64	&iilidx,
				i64	&idx,
				const AosRundataPtr &rdata);

	bool	addDocSeqDirectPriv(
				const u64 &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				i64	&idx,
				bool &keepSearch,
				const AosRundataPtr &rdata);

	bool	removeDocPriv(
				const u64 &value, 
				const u64 &docid,
				const AosRundataPtr &rdata);

	bool	removeDocDirectPriv(
				const u64 &value, 
				const u64 &docid,
				bool &keepSearch,
				const AosRundataPtr &rdata); 

	bool	removeDocSeqPriv(
				const u64 &value, 
				const u64 &docid,
				i64	&iilidx,
				i64	&idx,
				const AosRundataPtr &rdata); 

	bool	removeDocSeqDirectPriv(
				const u64 &value, 
				const u64 &docid,
				i64	&iilidx,
				i64	&idx,
				bool &keepSearch,
				const AosRundataPtr &rdata); 

	bool	removeUniqDocPriv(const u64 &docid);
	bool 	insertDocPriv(
				i64 &idx, 
				const u64 &value, 
				const u64 &docid, 
				const bool value_unique,
				const bool docid_unique,
				bool &keepSearch);

	bool	insertBefore(
				const i64 &nn, 
				const u64 &docid, 
				const u64 &value);

	bool	insertAfter(
				const i64 &nn, 
				const u64 &docid, 
				const u64 &value);

	virtual bool	returnSubIILsPriv(const AosRundataPtr &rdata)
	{
		bool returned;
		return returnSubIILsPriv(returned, false, rdata);
	}
	virtual bool	returnSubIILsPriv(
						bool &returned,
						const bool returnHeader,
						const AosRundataPtr &rdata);

	bool 	listSanityCheckPriv();
	bool	nextDocidAN(
				i64 &idx, 
				const int reverse, 
				const u64 &value,
				u64 &docid);
	bool	nextDocidAN2(
				i64 &idx, 
				const int reverse, 
				u64 &value,
				u64 &docid);
	bool	nextDocidLT(
				i64 &idx, 
				const int reverse, 
				const u64 &value,
				u64 &docid);
	bool	nextDocidLE(
				i64 &idx, 
				const int reverse, 
				const u64 &value,
				u64 &docid);
	bool	nextDocidGT(
				i64 &idx, 
				const int reverse, 
				const u64 &value,
				u64 &docid);
	bool	nextDocidGE(
				i64 &idx, 
				const int reverse, 
				const u64 &value,
				u64 &docid);
	bool	nextDocidEQ(
				i64 &idx, 
				const int reverse, 
				const u64 &value,
				u64 &docid);
	bool	nextDocidNE(
				i64 &idx, 
				const int reverse, 
				const u64 &value,
				u64 &docid);

	i64 	firstEQ(const i64 &idx, const u64 &value);
	i64 	firstEQRev(const i64 &idx, const u64 &value);
	i64 	firstNERev(const i64 &idx, const u64 &value);
	i64 	firstNE(const i64 &idx, const u64 &value);
	i64 	firstLERev(const i64 &idx, const u64 &value);
	i64 	firstLTRev(const i64 &idx, const u64 &value);
	i64 	firstLE(const i64 &idx, const u64 &value);
	i64 	firstLT(const i64 &idx, const u64 &value);
	i64 	firstGERev(const i64 &idx, const u64 &value);
	i64 	firstGTRev(const i64 &idx, const u64 &value);
	i64 	firstGT(const i64 &idx, const u64 &value);
	i64 	firstGE(const i64 &idx, const u64 &value);
	i64 	lastLT(const u64 &value);
	i64 	lastLE(const u64 &value);
	i64 	lastEQ(const i64 &idx, const u64 &value);
	i64 	firstLE(const u64 &value);
	i64 	firstLT(const u64 &value);
	u64 	removeDocByIdxPriv(const i64 &idx, const AosRundataPtr &rdata);

	inline bool		valueMatch(
						const u64 &v1, 
							const AosOpr opr, 
							const u64 &v2)
	{
		switch (opr)
		{
		case eAosOpr_le: return v1 <= v2 ;
		case eAosOpr_lt: return v1 <  v2;
		case eAosOpr_eq: return v1 == v2;
		case eAosOpr_gt: return v1 >  v2;
		case eAosOpr_ge: return v1 >= v2;
		case eAosOpr_ne: return v1 != v2;
		default: return false;
		}
		return false;
	}

	inline bool findFirstRev(
					i64 &idx, 
					const AosOpr opr, 
					const u64 &value)
	{
		switch (opr)
		{
		case eAosOpr_le: return firstLERev(idx, value);
		case eAosOpr_lt: return firstLTRev(idx, value);
		case eAosOpr_eq: return firstEQRev(idx, value);
		case eAosOpr_ne: return firstNERev(idx, value);
		default: idx = -5; return false;
		}
		idx = -5;
		return false;
	}

	inline bool findFirst(
					i64 &idx, 
					const AosOpr opr, 
					const u64 &value)
	{
		switch (opr)
		{
		case eAosOpr_ge: return firstGE(idx, value);
		case eAosOpr_gt: return firstGT(idx, value);
		case eAosOpr_eq: return firstEQ(idx, value);
		case eAosOpr_ne: return firstNE(idx, value);
		default: idx = -5; return false;
		}
		idx = -5;
		return false;
	}

	// Chen Ding, Bug1217
	// AosIILU64Ptr getSubiil(
	// 				const bool reverse,
	// 				const AosOpr opr,
	// 				const u64 &value);

	AosIILU64Ptr 	getSubiil(const u64 &value, const AosRundataPtr &rdata);
	i64		 	getSubiilIndex(const u64 &value);
	AosIILU64Ptr 	getSubiilByIndex(
						const i64 &idx,
						const AosRundataPtr &rdata);
	AosIILU64Ptr 	getIILByIndex(
						const i64 &idx,
						const AosRundataPtr &rdata)
	{
		if (idx == 0)
		{
			AosIILU64Ptr thisPtr(this, false);
			return thisPtr;
		}
		return getSubiilByIndex(idx, rdata);
	}
	
	AosIILU64Ptr 	getSubiil(
						const bool reverse,
						const AosOpr opr1,
						const u64 &value1, 
						const AosOpr opr2,
						const u64 &value2,
						const AosRundataPtr &rdata); 
	bool 		initSubiil(
					u64 *docids, 
					u64 *values, 
					const i64 &numDocs,
					const i64 &subiilidx, 
					const AosIILU64Ptr &rootiil); 
	bool 		addSubiil(
					const AosIILU64Ptr &crtsubiil,
					const AosIILU64Ptr &nextsubiil);
	i64 	getTotalPriv(const u64 &value, const AosOpr opr);

	bool	checkMemory() const;
	bool 	updateIndexData(
				const i64 &idx,
				const bool changeMax,
				const bool changeMin);
	bool 	createSubiilIndex();
	i64	getDocidsPriv(
				const AosOpr opr,
				u64 &value,
				u64 *docids,
				const i64 &arraysize,
				i64 &curPos,
				const AosRundataPtr &rdata);

	AosIILU64Ptr getSubIIL(
					const i64 &startidx,
					const u64 &value,
					const AosOpr opr,
					const bool fromHead,
					const AosRundataPtr &rdata)
	{
		i64 iilidx = getSubIILIndex(startidx, value, opr, fromHead);
		if (iilidx < 0) return 0;
		return getSubiilByIndex(iilidx, rdata);
	}

	i64		getSubIILIndex(
					const i64 &startidx,
					const u64 &value,
					const AosOpr opr,
					const bool fromHead);

	u64			getFirstValue()
	{
		if (mNumDocs > 0) return mValues[0];
		else return 0;
	}

	u64			getLastValue()
	{
		if (mNumDocs > 0) return mValues[mNumDocs-1];
		else return 0;
	}

	u64	getDocIdPriv1(i64 &idx, i64 &iilidx, const AosRundataPtr &rdata);

	virtual bool 	sanityTestForSubiils();
	bool			splitSanityCheck();

	// Ketty 2013/01/15
	//virtual AosIILObjPtr	createCompIIL(const AosRundataPtr &rdata);

	u64 getValue(const i64 &idx) const 
	{
		aos_assert_r(mValues, 0);
		aos_assert_r(idx < mNumDocs, 0);
		return mValues[idx];
	}

	u64 getDocid(const i64 &idx) const 
	{
		aos_assert_r(mDocids, 0);
		aos_assert_r(idx < mNumDocs, 0);
		return mDocids[idx]; 
	}

	bool verifyDuplicatedEntries(const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = verifyDuplicatedEntriesPriv(rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	u64 getRandomValue(u64 &docid, const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		u64 value = getRandomValuePriv(docid, rdata);
		AOSUNLOCK(mLock);
		return value;
	}

	virtual bool	queryRangePriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);

	virtual bool	queryValuePriv(
						vector<u64> &values,
		 				const AosOpr opr,
		 				const u64 &value,
						const bool unique_value,
						const AosRundataPtr &rdata);

	virtual bool	queryValueSinglePriv(
						vector<u64> &values,
		 				const AosOpr opr,
		 				const u64 &value,
						const bool unique_value,
						const bool getall,
						const AosRundataPtr &rdata);

	virtual bool	queryValueNEPriv(
						vector<u64> &values,
		 				const u64 &value,
						const bool unique_value,
						const AosRundataPtr &rdata);

	virtual bool	queryValueNESinglePriv(
						vector<u64> &values,
		 				const u64 &value,
						const bool unique_value,
						const bool getall,
						const AosRundataPtr &rdata);

	virtual bool	queryPriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosOpr opr,
						const u64 &value,
						const AosRundataPtr &rdata);

	virtual bool	querySinglePriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosOpr opr,
						const u64 &value,
						const AosRundataPtr &rdata);

	virtual bool	querySingleNEPriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const u64 &value,
						const AosRundataPtr &rdata);

	virtual bool	queryNEPriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const u64 &value,
						const AosRundataPtr &rdata);

	virtual bool	copyDocidsPriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const i64 &start,
						const i64 &end);

	bool 			verifyDuplicatedEntriesPriv(const AosRundataPtr &rdata);
	u64 			getRandomValuePriv(u64 &docid, const AosRundataPtr &rdata);

	bool    copyData(
				u64 values[AOSSENG_TORTURER_MAX_DOCS], 
				u64 *docids,
				const i64 &buff_len, 
				i64 &num_docs,
				const AosRundataPtr &rdata);

	bool    copyDataSingle(
				u64 values[AOSSENG_TORTURER_MAX_DOCS], 
				u64 *docids, 
				const i64 &buff_len,
				i64 &num_docs);

	bool docidSortedSanityCheck(const i64 &crtidx);

	u64		getValue(const i64 &idx, const i64 &iilidx, const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		if (isSingleIIL())
		{
			u64 value = mValues[idx];
			AOSUNLOCK(mLock);
			return value;
		}
		AosIILU64Ptr subiil = getSubiilByIndex(iilidx, rdata);
		aos_assert_rb(subiil, mLock, 0);
		
		if (idx < 0 || idx >= subiil->mNumDocs)
		{
			OmnAlarm << "invalid idx: " << idx << ":" << subiil->mNumDocs << enderr;
			AOSUNLOCK(mLock);
			return false;
		}
		u64 value = subiil->mValues[idx];
		AOSUNLOCK(mLock);
		return value;
	}

	bool updateKeyedValue(
			const u64 &key,
			const bool inc_flag,
			const u64 &delta, 
			u64 &new_value,
			const AosRundataPtr &rdata);

	//james 11/08
	bool updateKeyedValue(const u64 &key, const u64 &value, const AosRundataPtr &rdata);

	//Tank 12/22/2010

	bool appendManualOrder(const u64 &docid, u64 &value, const AosRundataPtr &rdata); 
	bool moveManualOrder(u64 &v1, const u64 &d1, u64 &v2, const u64 &d2, const OmnString flag, const AosRundataPtr &rdata); 
	bool swapManualOrder(const u64 &value1, const u64 &docid1, const u64 &value2, const u64 &docid2, const AosRundataPtr &rdata); 
	bool removeManualOrder(const u64 &value, const u64 &docid, const AosRundataPtr &rdata);
private:
	//Tank 12/24/2010
	bool reassignValuesPriv(const AosRundataPtr &rdata);
	bool reassignValuesDirectSafe();

	// add by shawn	
	bool mergeSubiilPriv(const i64 &iilidx,const AosRundataPtr &rdata);
	bool 	mergeSubiilForwardPriv(
				const i64 &iilidx,
				const i64 &numDocToMove,
				const AosRundataPtr &rdata);

	bool	mergeSubiilBackwardPriv(
				const i64 &iilidx,
				const i64 &numDocToMove,
				const AosRundataPtr &rdata);

	bool	getValueDocidPtr(
				u64* 	&valuePtr,
				u64*    &docidPtr,
				const i64 &offset);

	bool	appendDocToSubiil(
				u64 *values, 
				u64 *docids, 
				const i64 &numDocs,
				const bool addToHead);
						   
	bool	removeDocFromSubiil(
				const i64 &numRemove, 
				const bool delFromHead);

	bool 	removeSubiil(
				const i64 &iilidx,
				const AosRundataPtr &rdata);

	i64		getSubiilIndexSeq(
					const u64 &value,
					const i64 &iilidx,
					const AosRundataPtr &rdata);

	AosIILU64Ptr	getSubiilSeq(
						const u64 &value,
						i64 &iilidx,
						const AosRundataPtr &rdata);

	// Chen Ding, 05/27/2011
	//virtual bool saveIILsToFileSafe(const OmnFilePtr &file, u32 &crt_pos, const AosRundataPtr &rdata);

	// Chen Ding, 06/26/2011
	bool			checkDocidUnique(
						const bool rslt, 
						const i64 &idx, 
						u64 &docid,
						bool &isunique,
						const AosRundataPtr &rdata);

    bool    removeDocByValueSafe(
                    const u64 &value,
                    i64 &entriesRemoved,
                    const AosRundataPtr &rdata)
			{
				AOSLOCK(mLock);
				bool rslt = removeDocByValuePriv(value, entriesRemoved, rdata);
				aos_assert_rb(AosIILU64SanityCheck(this), mLock, false);
				AOSUNLOCK(mLock);
				return rslt;
			}
	bool	removeDocByValueDirectSafe(const u64 &value, 
                    				   i64 &entriesRemoved,
									   bool &keepSearch,
									   bool &shrinked, 
									   const AosRundataPtr &rdata)
			{
				AOSLOCK(mLock);
				bool rslt = removeDocByValueDirectPriv(value, 
													   entriesRemoved,
													   keepSearch, 
													   shrinked,
													   rdata);
				aos_assert_rb(AosIILU64SanityCheck(this), mLock, false);
				AOSUNLOCK(mLock);
				return rslt;
			}

private:

    bool    removeDocByValuePriv(
                    const u64 &value,
                    i64   &entriesRemoved,
                    const AosRundataPtr &rdata);
	bool	removeDocByValueDirectPriv(const u64			&value, 
                    				   i64      		&entriesRemoved,
									   bool 				&keepSearch,
									   bool 				&shrinked, 
									   const AosRundataPtr  &rdata);


    void    exportIILSafe(
				vector<u64> 		&values,
			   	vector<u64>			&docids,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		exportIILPriv(values,docids,rdata);
		AOSUNLOCK(mLock);
	}

    void    exportIILSingleSafe(
				vector<u64> 		&values,
			   	vector<u64>			&docids,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		exportIILSinglePriv(values,docids,rdata);
		AOSUNLOCK(mLock);
	}

	void    exportIILPriv(
				vector<u64>		 	&values,
				vector<u64>			&docids,
				const AosRundataPtr &rdata);
    void    exportIILSinglePriv(
				vector<u64>		 	&values,
				vector<u64>			&docids,
				const AosRundataPtr &rdata);


	// Ketty 2013/01/15
	//AosIILCompU64Ptr 	retrieveCompIIL(const bool, const AosRundataPtr &rdata);
	//bool	returnCompIIL(const AosIILObjPtr &iil, const AosRundataPtr &rdata);


	bool	preQueryPriv(
				const AosQueryContextObjPtr &query_context,
				const AosRundataPtr &rdata);

	bool	preQueryNEPriv(
				const AosQueryContextObjPtr &query_context,
				const AosRundataPtr &rdata);

public:
	virtual bool	batchAddSafe(
						char * &entries,
						const i64 &size,
						const i64 &num,
						const AosIILExecutorObjPtr &executor,
						const AosRundataPtr &rdata);

public:
	virtual void setSnapShotId(const u64 &snap_id);
	virtual void resetSnapShotId();

	virtual void setSnapShotIdPriv(const u64 &snap_id);
	virtual void resetSnapShotIdPriv();

	virtual bool resetIIL(const AosRundataPtr &rdata);
};

#endif

