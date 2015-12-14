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
#ifndef AOS_IILMgr_IILStr_h
#define AOS_IILMgr_IILStr_h

#include "IILMgr/IIL.h"
#include "IILUtil/IILUtil.h"
#include "SEUtil/IILIdx.h"
#include "SearchEngine/SeCommon.h"
#include "Util/Ptrs.h"
#include "Util1/MemMgr.h"

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
	};

private:
	char **			mValues;

	// Member data used to index sub-iils. Only the first
	// IIL will use them. 
	char **			mMinVals;
	char **			mMaxVals;
	AosIILStrPtr *	mSubiils;

	//ken 2011/07/25
	bool			mIsNumAlpha;

public:
	AosIILStr();
	AosIILStr(
			const u64 &wordid, 
			const u64 &iilid,
			const bool isPersis,
			const OmnString &iilname, 
			const AosRundataPtr &rdata);
	AosIILStr(
			const u64 &iilid, 
			const u32 siteid, 
			const AosDfmDocIILPtr &doc,
			const AosRundataPtr &rdata);
	~AosIILStr();

	static bool	staticInit(const AosXmlTagPtr &config);

	// Thread Safe Functions
	bool	nextDocidSubSafe(
				i64 &idx, 
				const bool reverse, 
				const AosOpr opr,
				const OmnString &value,
				u64 &docid, 
				bool &isunique,
				const AosRundataPtr &rdata) 
	{
		AOSLOCK(mLock);
		bool rslt = nextDocidSinglePriv(idx, reverse, opr, value, docid, isunique, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool	nextUniqueValueSafe(
				i64 &idx,
				i64 &iilidx,
				const bool reverse,
				const AosOpr opr,
				const OmnString &value,
				OmnString &unique_value,
				bool &found,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = nextUniqueValuePriv(idx, iilidx, reverse, opr, value, unique_value, found, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool	nextUniqueValueSubSafe(
				i64 &idx, 
				const bool reverse, 
				const AosOpr opr,
				const OmnString &value,
				OmnString &unique_value,
				bool &found) 
	{
		AOSLOCK(mLock);
		bool rslt = nextUniqueValueSinglePriv(idx, reverse, opr, value, unique_value, found);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool	nextDocidPrefixSubSafe(
				i64 &idx, 
				const bool reverse, 
				const OmnString &prefix,
				u64 &docid,
				bool &keepSearch) 
	{
		AOSLOCK(mLock);
		bool rslt = nextDocidPrefixSinglePriv(idx, reverse,prefix, docid,keepSearch);
		AOSUNLOCK(mLock);
		return rslt;
	}
	
	bool	nextDocidSubSafe(
				i64 &idx, 
				const bool reverse, 
				const AosOpr opr1,
				const OmnString &value1,
				const AosOpr opr2,
				const OmnString &value2,
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

	bool	removeDocDirectSafe(
				const OmnString &value,
				const u64 &docid,
				bool &keepSearch,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = removeDocDirectPriv(value, docid, keepSearch, rdata);
		aos_assert_rb(AosIILStrSanityCheck(this), mLock, false);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool	removeDocSeqSafe(
				const OmnString &value, 					
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
				const OmnString &value, 
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
				OmnString &value,
				u64 &docid,
				const AosRundataPtr &rdata) 
	{
		AOSLOCK(mLock);
		bool isunique;
		bool rslt = nextDocidPriv(
						idx, iilidx, reverse, eAosOpr_an, value, docid, isunique, rdata);
		if (rslt && idx >= 0 && iilidx >= 0)
		{
			AosIILStr* iil = 0;
			if (isSingleIIL())
			{
				iil = this;
			}
			else
			{
				AosIILStrPtr the_iil = getSubiilByIndex(iilidx, rdata);
				if (!the_iil)
				{
					AOSUNLOCK(mLock);
					OmnAlarm << "idx: " << idx << ":" << iilidx << enderr;
					idx = -5;
					iilidx = -5;
					docid = AOS_INVDID;
					return false;
				}
				iil = the_iil.getPtr();
			}

			if (idx >= iil->mNumDocs)
			{
				AOSUNLOCK(mLock);
				OmnAlarm << "idx: " << idx << ":" << iilidx << enderr;
				idx = -5;
				iilidx = -5;
				docid = AOS_INVDID;
				return false;
			}

			if (!iil->mValues)
			{
				AOSUNLOCK(mLock);
				OmnAlarm << "idx: " << idx << ":" << iilidx << enderr;
				idx = -5;
				iilidx = -5;
				docid = AOS_INVDID;
				return false;
			}
			value = iil->mValues[idx];
		}
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool	nextDocidSafe(
				AosIILIdx the_idx,
				i64 &idx, 
				i64 &iilidx,
				const bool reverse, 
				const AosOpr opr,
				const OmnString &value,
				u64 &docid, 
				bool &isunique,
				const AosRundataPtr &rdata)
	{
		return nextDocidSafe(
				idx, 
				iilidx,
				reverse, 
				opr,
				value,
				docid,
				isunique,
				rdata); 
	}

	bool	nextDocidSafe(
				i64 &idx, 
				i64 &iilidx,
				const bool reverse, 
				const AosOpr opr,
				const OmnString &value,
				u64 &docid, 
				bool &isunique,
				const AosRundataPtr &rdata) 
	{
		AOSLOCK(mLock);
		bool rslt = nextDocidPriv(
						idx, iilidx, reverse, opr, value, docid, isunique, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool	nextDocidPrefixSafe(
				i64 &idx, 
				i64 &iilidx,
				const bool reverse, 
				const OmnString &prefix,
				u64 &docid,
				const AosRundataPtr &rdata) 
	{
		AOSLOCK(mLock);
		bool rslt = nextDocidPrefixPriv(idx, iilidx, reverse, prefix, docid, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

    bool    removeDocByPrefixSafe(const OmnString &prefix, const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = removeDocByPrefixPriv(prefix, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool	nextDocidSafe(
				i64 &idx, 
				i64 &iilidx,
				const bool reverse, 
				const AosOpr opr1,
				const OmnString &value1,
				const AosOpr opr2,
				const OmnString &value2,
				u64 &docid, 
				bool &isunique,
				const AosRundataPtr &rdata);

	virtual i64	getTotalSafe(const OmnString &value, const AosOpr opr);

	bool 	modifyDocSafe(
				const OmnString &oldvalue, 
				const OmnString &newvalue, 
				const u64 &docid, 
				const bool value_unique,
				const bool docid_unique,
				const AosRundataPtr &rdata)
	{
		return modifyDocSafe(oldvalue, newvalue, docid, value_unique, 
					docid_unique, false, rdata);
	}

	bool 	modifyDocSafe(
				const OmnString &oldvalue, 
				const OmnString &newvalue, 
				const u64 &docid, 
				const bool value_unique,
				const bool docid_unique,
				const bool override_flag, 		// Chen Ding, 07/15/2012
				const AosRundataPtr &rdata);

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

	bool	addDocSeqSafe(
				const OmnString &value, 
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
				const OmnString &value, 
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
				const OmnString &value, 
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

	OmnString	getValueSafeStr(const i64 &idx, u64 &docid)
	{
		aos_assert_r(mNumDocs <= mMemCap, "");
		AOSLOCK(mLock);
		if (idx < 0 || idx >= mNumDocs) 
		{
			docid = AOS_INVDID;
			AOSUNLOCK(mLock);
			return "";
		}
		docid = mDocids[idx];
		OmnString vv = mValues[idx];
		AOSUNLOCK(mLock);
		return vv;
	}

	virtual bool	checkDocSafe(
						const AosOpr opr,
						const OmnString &value,
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool 	prependDocSafe(
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique);

	i64	getDocidsSafe(
				const AosOpr opr,
				const OmnString &value,
				u64 *docids, 
				const i64 &arraysize,
				const AosRundataPtr &rdata);

	u64		incrementDocidSafe(
				const OmnString &key,
				const u64 &dft,
				const AosRundataPtr &rdata);

	bool	incrementDocidSafe(
				const OmnString &key,
				u64 &value,
				const u64 &incvalue,
				const u64 &init_value,
				const bool add_flag,
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

	AosIILStrPtr getSubiilPublic(const i64 &idx, const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		AosIILStrPtr iil = getSubiilByIndex(idx, rdata);
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
						query_context->getStrValue(), 
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

	OmnString		getValueSafe(
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
				return "";
			}
			OmnString value = mValues[idx];
			AOSUNLOCK(mLock);
			return value;
		}
		//mIILIdx = 0;  // Linda, 2010/12/13
		AosIILStrPtr subiil = getSubiilByIndex(iilidx, rdata);
		aos_assert_rb(subiil, mLock, "");

		if (idx < 0 || idx >= subiil->mNumDocs)
		{
			OmnAlarm << "invalid idx: " << idx << ":" << subiil->mNumDocs << enderr;
			AOSUNLOCK(mLock);
			return "";
		}
		OmnString value = subiil->mValues[idx];
		AOSUNLOCK(mLock);
		return value;
	}

	virtual bool	querySingleLikeSafe(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const OmnString &value,
						const bool setBlockSize,
						const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = querySingleLikePriv(query_rslt, query_bitmap, value, setBlockSize, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	virtual bool	saveSubIILToLocalFileSafe(const AosRundataPtr &rdata);

	virtual bool	queryValueSafe(
						vector<OmnString> &values,
						const AosOpr opr,
						const OmnString &value,
						const bool unique_value,
						const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = queryValuePriv(values, opr, value, unique_value, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool	setValueDocUniqueSafe(
				const OmnString &key, 
				const u64 &docid,
				const bool must_same,
				const AosRundataPtr &rdata);

	bool	getSplitValueSafe(
				const AosQueryContextObjPtr &context,
				const i64 &size,
				vector<AosQueryContextObjPtr> &contexts,
				const AosRundataPtr &rdata);

	void	setNumAlpha(){mIsNumAlpha = true;}

	//ken 2011/12/07
	bool	removeFirstValueDocSafe(
				const OmnString &value,
				u64 &docid,
				const bool reverse,
				const AosRundataPtr &rdata);

private:
	bool	nextDocidPriv(
				i64 &idx, 
				i64 &iilidx,
				const bool reverse, 
				const AosOpr opr,
				const OmnString &value,
				u64 &docid, 
				bool &isunique,
				const AosRundataPtr &rdata);

	virtual bool	checkDocPriv(
						const AosOpr opr,
						const OmnString &value,
						const u64 &docid,
						bool &keepSearch,
						const AosRundataPtr &rdata);

	virtual bool		saveSanityCheckProtected(const AosRundataPtr &rdata);
	virtual AosBuffPtr	getBodyBuffProtected() const;
	virtual bool 		setContentsProtected(AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool 		prepareMemoryForReloading();
	virtual u64			getMinDocid() const;
	virtual u64			getMaxDocid() const;


	const char * 	getMinValue() const;
	const char * 	getMaxValue() const;
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

	bool			queryNewLikePriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);
	
	bool			queryNewLikeSinglePriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context,
						const i64 &start_idx,
						const bool reverse,
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
						const OmnString &cur_value,
						const u64 &cur_docid,
						const bool reverse,
						bool &has_data,
						const AosRundataPtr &rdata);

	bool			verifyIdx(
						const i64 &cur_idx,
						const OmnString &cur_value,
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
						const OmnString &cur_value,
						const u64 &cur_docid,
						const bool reverse,
						bool &has_data,
						const AosRundataPtr &rdata);

	virtual AosIILType	getIILType() const {return eAosIILType_Str;}
	virtual bool	firstDoc1(
						i64 &idx, 
						const bool reverse, 
						const AosOpr opr,
						const OmnString &value,
						u64 &docid);
	AosIILStrPtr	getNextSubIIL(const bool forward, const AosRundataPtr &rdata);

	virtual bool	checkDocSafe(
						const AosOpr opr,
						const OmnString &value,
						const u64 &docid,
						bool &keepSearch,
						const AosRundataPtr &rdata);

	bool	nextUniqueValuePriv(
				i64 &idx,
				i64 &iilidx,
				const bool reverse,
				const AosOpr opr,
				const OmnString &value,
				OmnString &unique_value,
				bool &found,
				const AosRundataPtr &rdata);
	
	bool	nextUniqueValueSinglePriv(
				i64 &idx,
				const bool reverse,
				const AosOpr opr,
				const OmnString &value,
				OmnString &unique_value,
				bool &found);

	bool	nextDocidSinglePriv(
				i64 &idx,
				const bool reverse,
				const AosOpr opr1,
				const OmnString &value1,
				const AosOpr opr2,
				const OmnString &value2,
				u64 &docid, 
				bool &isunique,
				const AosRundataPtr &rdata); 

	bool	nextDocidSinglePriv(
				i64 &idx, 
				const bool reverse, 
				const AosOpr opr,
				const OmnString &value,
				u64 &docid, 
				bool &isunique,
				const AosRundataPtr &rdata);

	bool	nextDocidPrefixSinglePriv(
				i64 &idx, 
				const bool reverse, 
				const OmnString &prefix,
				u64 &docid,
				bool &keepSearch); 

	bool	expandMemoryPriv();

	bool	splitListPriv(AosIILObjPtr &subiil, const AosRundataPtr &rdata);

	bool 	addDocPriv(
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const AosRundataPtr &rdata);

	bool	addDocDirectPriv(
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				bool &keepSearch,
				const AosRundataPtr &rdata);

	bool	addDocSeqPriv(
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				i64	&iilidx,
				i64	&idx,
				const AosRundataPtr &rdata);

	bool	addDocSeqDirectPriv(
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				i64	&idx,
				bool &keepSearch,
				const AosRundataPtr &rdata);

	bool	removeDocPriv(
				const OmnString &value, 
				const u64 &docid,
				const AosRundataPtr &rdata);

	bool	removeDocDirectPriv(
				const OmnString &value, 
				const u64 &docid,
				bool &keepSearch,
				const AosRundataPtr &rdata); 

	bool	removeDocSeqPriv(
				const OmnString &value, 
				const u64 &docid,
				i64	&iilidx,
				i64	&idx,
				const AosRundataPtr &rdata); 

	bool	removeDocSeqDirectPriv(
				const OmnString &value, 
				const u64 &docid,
				i64	&iilidx,
				i64	&idx,
				bool &keepSearch,
				const AosRundataPtr &rdata); 

	bool	removeUniqDocPriv(const u64 &docid);
	bool 	appendDocPriv(
				i64 &idx, 
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique,
				bool &keepSearch);

	bool 	insertDocPriv(
				i64 &idx, 
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique,
				const bool docid_unique,
				bool &keepSearch);

	bool	insertBefore(
				const i64 &nn, 
				const u64 &docid, 
				const OmnString &value);

	bool	insertAfter(
				const i64 &nn,
				const u64 &docid,
				const OmnString &value);

	virtual bool	returnSubIILsPriv(const AosRundataPtr &rdata)
	{
		bool returned;
		return returnSubIILsPriv(returned, false, rdata);
	}
	virtual bool	returnSubIILsPriv(
						bool &returned,
						const bool returnHeader,
						const AosRundataPtr &rdata);

	bool	nextDocidPrefixPriv(
				i64 &idx, 
				i64 &iilidx,
				const bool reverse, 
				const OmnString &prefix,
				u64 &docid,
				const AosRundataPtr &rdata);

    bool	removeDocByPrefixPriv(
				const OmnString &prefix,
				const AosRundataPtr &rdata);

	bool 	listSanityCheckPriv();
	bool	nextDocidAN(
				i64 &idx, 
				const int reverse, 
				const OmnString &value,
				u64 &docid);
	bool	nextDocidLT(
				i64 &idx, 
				const int reverse, 
				const OmnString &value,
				u64 &docid);
	bool	nextDocidLE(
				i64 &idx, 
				const int reverse, 
				const OmnString &value,
				u64 &docid);
	bool	nextDocidGT(
				i64 &idx, 
				const int reverse, 
				const OmnString &value,
				u64 &docid);
	bool	nextDocidGE(
				i64 &idx, 
				const int reverse, 
				const OmnString &value,
				u64 &docid);
	bool	nextDocidEQ(
				i64 &idx, 
				const int reverse, 
				const OmnString &value,
				u64 &docid);

	// ken 2011/5/25
	bool	nextDocidLike(
				i64 &idx, 
				const int reverse, 
				const OmnString &value,
				u64 &docid);
	bool	nextDocidNE(
				i64 &idx, 
				const int reverse, 
				const OmnString &value,
				u64 &docid);

	i64 	firstEQ(const i64 &idx, const OmnString &value);
	i64 	firstEQRev(const i64 &idx, const OmnString &value);
	i64 	firstNERev(const i64 &idx, const OmnString &value);
	i64 	firstNE(const i64 &idx, const OmnString &value);
	i64 	firstLERev(const i64 &idx, const OmnString &value);
	i64 	firstLTRev(const i64 &idx, const OmnString &value);
	i64 	firstLE(const i64 &idx, const OmnString &value);
	i64 	firstLT(const i64 &idx, const OmnString &value);
	i64 	firstGERev(const i64 &idx, const OmnString &value);
	i64 	firstGTRev(const i64 &idx, const OmnString &value);
	i64 	firstGT(const i64 &idx, const OmnString &value);
	i64 	firstGE(const i64 &idx, const OmnString &value);
	i64 	lastLT(const OmnString &value);
	i64 	lastLE(const OmnString &value);
	i64 	lastEQ(const i64 &idx, const OmnString &value);
	i64 	firstLE(const OmnString &value);
	i64 	firstLT(const OmnString &value);
	u64 	removeDocByIdxPriv(const i64 &idx, const AosRundataPtr &rdata);

//	bool 	valueLikeMatch(
//				const OmnString &value,
//				const bool reverse);

	bool 	valueLikeMatch(
				const char *v,
				const OmnString &value);

	// Chen Ding, 01/29/2012
	// Taken out to AosIILUtil.h
	// inline int	valueMatch(
	// 				const char *v1,
	// 				const OmnString &value)
	// {
	// 	int vaLen = value.length();
	// 	if (mIsNumAlpha)
	// 	{
	// 		int vLen = strlen(v1);
	// 		if (vLen < vaLen) return -1;
	// 		else if (vLen > vaLen) return 1;
	// 	}
	// 	
	// 	// Ken, 05/30/2011
	// 	if (vaLen <= AosIILUtil::eMaxStrValueLen)
	// 	{
	// 		return strcmp(v1, value.data());
	// 	}
	// 	else
	// 	{
	// 		return strncmp(v1, value.data(), AosIILUtil::eMaxStrValueLen);
	// 	}
	// }

	// Chen Ding, 01/29/2012
	// Taken out to AosIILUtil.h
	// inline bool	valueMatch(
	// 				const char *v1, 
	// 				const AosOpr opr, 
	// 				const OmnString &value)
	// {
	// 	int rslt;
	// 	int vaLen = value.length();
	// 	bool haverslt = false;
	// 	if (mIsNumAlpha)
	// 	{
	// 		int vLen = strlen(v1);
	// 		if (vLen < vaLen)
	// 		{
	// 			rslt = -1;
	// 			haverslt = true;
	// 		}
	// 		else if (vLen > vaLen)
	// 		{
	// 			rslt = 1;
	// 			haverslt = true;
	// 		}
	// 	}
	// 	if (!haverslt)
	// 	{
	// 		// Ken, 05/30/2011
	// 		if (vaLen <= AosIILUtil::eMaxStrValueLen)
	// 		{
	// 			rslt = strcmp(v1, value.data());
	//		}
	//		else
	//		{
	//			rslt = strncmp(v1, value.data(), AosIILUtil::eMaxStrValueLen);
	//		}
	//	}
	//
	//	switch (opr)
	//	{
	//		case eAosOpr_le: return rslt <= 0;
	//		case eAosOpr_lt: return rslt < 0;
	//		case eAosOpr_eq: return rslt == 0;
	//		case eAosOpr_gt: return rslt > 0;
	//		case eAosOpr_ge: return rslt >= 0;
	//		case eAosOpr_ne: return rslt != 0;
	//		default: return false;
	//	}
	//	return false;
	//}

	inline bool findFirstRev(
					i64 &idx, 
					const AosOpr opr, 
					const OmnString &value)
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
					const OmnString &value)
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

	// get sub iil functions for add and remove 
	AosIILStrPtr 	getSubiil(const OmnString &value, const AosRundataPtr &rdata);
	i64			getSubiilIndex(const OmnString &value);
	AosIILStrPtr 	getSubiilByIndex(
						const i64 &idx,
						const AosRundataPtr &rdata);
	AosIILStrPtr 	getIILByIndex(
						const i64 &idx,
						const AosRundataPtr &rdata)
	{
		if (idx == 0)
		{
			AosIILStrPtr thisPtr(this, false);
			return thisPtr;
		}
		return getSubiilByIndex(idx, rdata);
	}
	
	AosIILStrPtr 	getSubiil(
						const bool reverse,
						const AosOpr opr1,
						const OmnString &value1, 
						const AosOpr opr2,
						const OmnString &value2,
						const AosRundataPtr &rdata); 
	bool 		initSubiil(
					u64 *docids, 
					char **values, 
					const i64 &numDocs,
					const i64 &subiilidx, 
					const AosIILStrPtr &rootiil); 
	bool 		addSubiil(
					const AosIILStrPtr &crtsubiil,
					const AosIILStrPtr &nextsubiil);
	i64 	getTotalPriv(const OmnString &value, const AosOpr opr);

	bool	setValue(const i64 &idx, const char *value, const i64 &length)
	{
		// Only 'AosIILUtil::eMaxStrValueLen' is stored
		aos_assert_r(length >= 0, false);
		i64 len = (length > AosIILUtil::eMaxStrValueLen) ? AosIILUtil::eMaxStrValueLen : length;
		if (len <= 0)
		{
			OmnAlarm << "Length invalid: " << len << enderr;
			len = eMinStrLen;
		}

		char *ptr = mValues[idx];
		if (ptr)
		{
			int *size = (int *)&ptr[-4];
			if (!(len <= *size && (len << 1) > *size))
			{
				OmnMemMgrSelf->release(ptr, __FILE__, __LINE__);
				ptr = OmnMemMgrSelf->allocate(len+1, __FILE__, __LINE__);
				aos_assert_r(ptr, false);
				mValues[idx] = ptr;
			}
		}
		else
		{
			ptr = OmnMemMgrSelf->allocate(len+1, __FILE__, __LINE__);
			aos_assert_r(ptr, false);
			mValues[idx] = ptr;
		}

		aos_assert_r(value, false);
		if (len > 0) strncpy(ptr, value, len);
		ptr[len] = 0;
		return true;
	}

	bool	checkMemory() const;
	bool 	updateIndexData(
				const i64 &idx,
				const bool changeMax,
				const bool changeMin);
	bool 	createSubiilIndex();
	i64	getDocidsPriv(
				const AosOpr opr,
				const OmnString &value,
				u64 *docids,
				const i64 &arraysize,
				i64 &curPos,
				const AosRundataPtr &rdata);

	// Chen Ding, Bug1217
	AosIILStrPtr getSubIIL(
					const i64 &startidx,
					const OmnString &value,
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
					const OmnString &value,
					const AosOpr opr,
					const bool fromHead);

	OmnString	getFirstValue()
	{
		if (mNumDocs > 0) return mValues[0];
		else return "";
	}

	OmnString	getLastValue()
	{
		if (mNumDocs > 0) return mValues[mNumDocs-1];
		else return "";
	}

	u64	getDocIdPriv1(i64 &idx, i64 &iilidx, const AosRundataPtr &rdata);

	virtual bool 	sanityTestForSubiils();
	bool			splitSanityCheck();

	// Ketty 2013/01/15
	//virtual AosIILObjPtr	createCompIIL(const AosRundataPtr &rdata);

	OmnString getValue(const i64 &idx) const 
	{
		aos_assert_r(mValues, "");
		aos_assert_r(idx < mNumDocs, "");
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

	OmnString getRandomValue(u64 &docid, const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		OmnString value = getRandomValuePriv(docid, rdata);
		AOSUNLOCK(mLock);
		return value;
	}

	virtual bool	queryRangePriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);

	virtual bool	queryValuePriv(
						vector<OmnString> &values,
		 				const AosOpr opr,
		 				const OmnString &value,
						const bool unique_value,
						const AosRundataPtr &rdata);

	virtual bool	queryValueSinglePriv(
						vector<OmnString> &values,
		 				const AosOpr opr,
		 				const OmnString &value,
						const bool unique_value,
						const bool getall,
						const AosRundataPtr &rdata);

	virtual bool	queryValueNEPriv(
						vector<OmnString> &values,
		 				const OmnString &value,
						const bool unique_value,
						const AosRundataPtr &rdata);

	virtual bool	queryValueNESinglePriv(
						vector<OmnString> &values,
		 				const OmnString &value,
						const bool unique_value,
						const bool getall,
						const AosRundataPtr &rdata);

	virtual bool	queryValueLikePriv(
						vector<OmnString> &values,
		 				const OmnString &value,
						const bool unique_value,
						const AosRundataPtr &rdata);

	virtual bool	queryValueLikeSinglePriv(
						vector<OmnString> &values,
		 				const OmnString &value,
						const bool unique_value);

	virtual bool	queryPriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosOpr opr,
						const OmnString &value,
						const AosRundataPtr &rdata);

	virtual bool	querySinglePriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosOpr opr,
						const OmnString &value,
						const AosRundataPtr &rdata);

	virtual bool	querySingleNEPriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const OmnString &value,
						const AosRundataPtr &rdata);

	virtual bool	queryNEPriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const OmnString &value,
						const AosRundataPtr &rdata);

	virtual bool	querySingleLikePriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const OmnString &value,
						const bool setBlockSize,
						const AosRundataPtr &rdata);
	
	virtual bool	queryLikePriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const OmnString &value,
						const AosRundataPtr &rdata);

	bool 			verifyDuplicatedEntriesPriv(const AosRundataPtr &rdata);
	OmnString		getRandomValuePriv(u64 &docid, const AosRundataPtr &rdata);

	bool    copyData(
				char** values, 
				u64 *docids,
				const i64 &buff_len, 
				i64 &num_docs,
				const AosRundataPtr &rdata);

	bool    copyDataSingle(
				char** values, 
				u64 *docids, 
				const i64 &buff_len,
				i64 &num_docs);

	bool docidSortedSanityCheck(const i64 &crtidx);

	// Chen Ding, 12/07/2010
	bool	incrementCounter(const OmnString &counter_id, const u64 &value, const AosRundataPtr &rdata);

	// James, 01/04/2011
	bool	appendDocidSafe(
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique, 
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = addDocPriv(value, docid, value_unique, false, rdata);
		aos_assert_rb(AosIILStrSanityCheck(this), mLock, false);
		AOSUNLOCK(mLock);
		return rslt;
	}

	i64	getSubIILIndexByPrefix(
				const i64 &startidx, 
				const OmnString &value,
				const bool fromHead);
	
	bool	nextDocidPrefix(
				i64 &idx, 
				const int reverse, 
				const OmnString &prefix,
				u64 &docid);

	i64	firstPrefixRev(const i64 &idx, const OmnString &prefix);
	
	bool	mergeSubiilPriv(const i64 &iilidx, const AosRundataPtr &rdata);
	
	bool 	appendDocToSubiil(
				char **values, 
				u64 *docids, 
				const i64 &numDocs,
				const bool addToHead);

	bool 	removeDocFromSubiil(
				const i64 &numDocs,
				const bool delFromHead);
	
	bool 	removeDocFromSubiil2Safe(
				const i64 &start_idx,
				const i64 &end_idx,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = removeDocFromSubiil2Priv(start_idx, end_idx, rdata);
		aos_assert_rb(AosIILStrSanityCheck(this), mLock, false);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool 	removeDocFromSubiil2Priv(
				const i64 &start_idx,
				const i64 &end_idx,
				const AosRundataPtr &rdata);
							   			 
	bool	removeDocsOnOneSub(
				const i64 &iilidx,
				const i64 &startidx,
				const i64 &endidx,
				const AosRundataPtr &rdata);

	bool 	mergeSubiilForwardPriv(
				const i64 &iilidx,
				const i64 &numDocToMove,
				const AosRundataPtr &rdata);

	bool	mergeSubiilBackwardPriv(
				const i64 &iilidx,
				const i64 &numDocToMove,
				const AosRundataPtr &rdata);

	bool	getValueDocidPtr(
				char ** &valuePtr,
				u64*    &docidPtr,
				const i64 &offset);

	bool 	removeSubiil(
				const i64 &iilidx,
				const AosRundataPtr &rdata);

	i64		getSubiilIndexSeq(
					const OmnString &value,
					const i64 &iilidx,
					const AosRundataPtr &rdata);

	AosIILStrPtr	getSubiilSeq(
						const OmnString &value,
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

	virtual bool	copyDocidsPriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const i64 &start,
						const i64 &end);

	// Chen Ding, 06/26/2011
	bool	isNumAlpha() const {return mIsNumAlpha;}
    void    exportIILSafe(
				vector<OmnString> 	&values,
			   	vector<u64>			&docids,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		exportIILPriv(values,docids,rdata);
		AOSUNLOCK(mLock);
	}

    void    exportIILSingleSafe(
				vector<OmnString> 	&values,
			   	vector<u64>			&docids,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		exportIILSinglePriv(values,docids,rdata);
		AOSUNLOCK(mLock);
	}

	void    exportIILPriv(
				vector<OmnString> 	&values,
				vector<u64>			&docids,
				const AosRundataPtr &rdata);
    void    exportIILSinglePriv(
				vector<OmnString> 	&values,
				vector<u64>			&docids,
				const AosRundataPtr &rdata);


	// Ketty 2013/01/15
	//AosIILCompStrPtr 	retrieveCompIIL(const bool, const AosRundataPtr &rdata);
	//bool	returnCompIIL(const AosIILObjPtr &iil, const AosRundataPtr &rdata);

	// Chen Ding, 05/05/2012
	bool queryColumn(
				const OmnString &value1, 
				const AosOpr opr1, 
				const OmnString &value2, 
				const AosOpr opr2, 
				const AosStrStrArrayPtr data,
				const AosRundataPtr &rdata);
	


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
	virtual bool	batchDelSafe(
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

