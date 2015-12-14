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
// 11/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Index_IndexStr_h
#define AOS_Index_IndexStr_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "IILMgr/IIL.h"
#include "IILUtil/IILUtil.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/IILObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "Thread/Ptrs.h"
#include "Thread/Mutex.h"

using namespace std;

#define AosIILStrSanityCheck(x) true

class AosIILStr : public AosIIL
{
	OmnDefineRCObject;

private:
	OmnMutexPtr		mLock;
	bool			mIsNumAlpha;

public:
	AosIILStr(
			const bool isPersis, 
			const AosRundataPtr &rdata);
	~AosIILStr();

	virtual u64		getDocIdSafe1(int &idx,int &iilidx,const AosRundataPtr &rdata);
	u32				getNumDocs() const {return mNumDocs;}
	void			setNumAlpha(){mIsNumAlpha = true;}

	bool nextDocidSubSafe(
				int &idx, 
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

	bool nextUniqueValueSafe(
				int &idx,
				int &iilidx,
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

	bool nextUniqueValueSubSafe(
				int &idx, 
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

	bool nextDocidPrefixSubSafe(
				int &idx, 
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
	
	bool nextDocidSubSafe(
				int &idx, 
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

	bool removeDocSafe(
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

	bool removeDocDirectSafe(
				const OmnString &value,
				const u64 &docid,
				bool &keepSearch,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = removeDocDirectPriv(value, docid,keepSearch, rdata);
		aos_assert_rb(AosIILStrSanityCheck(this), mLock, false);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool removeDocSeqSafe(
				const OmnString &value, 					
				const u64 &docid,
				int &iilidx,
				int &idx,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = removeDocSeqPriv(value, docid,iilidx,idx, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}
	
	bool removeDocSeqDirectSafe(
				const OmnString &value, 
				const u64 &docid,
				int	&iilidx,
				int &idx,
				bool &keepSearch,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = removeDocSeqDirectPriv(value, docid,iilidx,idx,keepSearch, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	virtual u64	nextDocIdSafe2(
					int &idx,
					int &iilidx,
					const bool reverse,
					const AosRundataPtr &rdata);

	bool nextDocidSafe(
				int &idx, 
				int &iilidx,
				const bool reverse, 
				OmnString &value,
				u64 &docid,
				const AosRundataPtr &rdata);

	bool nextDocidSafe(
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
		return nextDocidSafe( idx, iilidx, reverse, opr, value, docid, isunique, rdata); 
	}

	bool nextDocidSafe(
				int &idx, 
				int &iilidx,
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

	bool nextDocidPrefixSafe(
				int &idx, 
				int &iilidx,
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

    bool removeDocByPrefixSafe(const OmnString &prefix, const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = removeDocByPrefixPriv(prefix, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}
	
	bool nextDocidSafe(
				int &idx, 
				int &iilidx,
				const bool reverse, 
				const AosOpr opr1,
				const OmnString &value1,
				const AosOpr opr2,
				const OmnString &value2,
				u64 &docid, 
				bool &isunique,
				const AosRundataPtr &rdata);

	virtual int	getTotalSafe(const OmnString &value, const AosOpr opr);

	bool 	modifyDocSafe(
				const OmnString &oldvalue, 
				const OmnString &newvalue, 
				const u64 &docid, 
				const bool value_unique,
				const bool docid_unique,
				const bool iilmgrLocked,
				const bool override_flag, 		// Chen Ding, 07/15/2012
				const AosRundataPtr &rdata);

	bool modifyDocSafe(
				const OmnString &oldvalue, 
				const OmnString &newvalue, 
				const u64 &docid, 
				const bool value_unique,
				const bool docid_unique,
				const bool iilmgrLocked,
				const AosRundataPtr &rdata)
	{
		return modifyDocSafe(oldvalue, newvalue, docid, value_unique, 
					docid_unique, iilmgrLocked, false, rdata);
	}

	bool addDocSafe(
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const bool iilmgrLocked,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = addDocPriv(
						value, docid, value_unique, docid_unique, iilmgrLocked, rdata);
		aos_assert_rb(AosIILStrSanityCheck(this), mLock, false);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool addDocSeqSafe(
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique,
				int	&iilidx,
				int	&idx,
				const bool iilmgrLocked,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = addDocSeqPriv(
						value, docid, value_unique, docid_unique, iilidx,idx,iilmgrLocked, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool addDocDirectSafe(
				const OmnString &value, 
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

	bool addDocSeqDirectSafe(
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				int &idx,
				const bool iilmgrLocked,
				bool &keepSearch,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = addDocSeqDirectPriv(
						value, docid, value_unique, docid_unique, idx, iilmgrLocked, keepSearch, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	OmnString getValueSafeStr(const int idx, u64 &docid);

	virtual bool preQuerySafe(
				const AosQueryContextPtr &query_context,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = preQueryPriv(query_context,rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	virtual bool queryNewSafe(
				const AosQueryRsltPtr &query_rslt,
				const AosBitmapPtr &query_bitmap, 
				const AosQueryContextPtr &query_context,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = queryNewPriv(query_rslt,query_bitmap,query_context,rdata);
		AOSUNLOCK(mLock);
		return rslt; 
	}
	
	virtual bool deleteIILSafe(
				const bool iilmgrLocked,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = deleteIILPriv(iilmgrLocked, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}
	
	virtual u32	getTotalNumDocsSafe() const
	{
		mLock->lock();
		u32 numdoc = mNumDocs;
		if(isRootIIL())
		{
			numdoc = 0;
			for(u32 i =0;i < (u32)mNumSubiils;i++)
			{
				numdoc += mNumEntries[i];
			}
		}
		mLock->unlock();
		return numdoc;
	}

	AosIILStrPtr getSubiilPublic(const int idx, const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		AosIILStrPtr iil = getSubiilByIndex(idx, false, rdata);
		AOSUNLOCK(mLock);
		return iil;
	}

	virtual bool querySafe(
				const AosQueryRsltPtr &query_rslt,
				const AosBitmapPtr &query_bitmap,
				const AosQueryContextObjPtr &query_context,
				const AosRundataPtr &rdata)
	{
		aos_assert_r(query_context,false);
		AOSLOCK(mLock);
		bool rslt = queryPriv(query_rslt, query_bitmap, query_context->getOpr(), 
							  query_context->getStrValue(), rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	virtual bool queryRangeSafe(
				const AosQueryRsltPtr &query_rslt,
				const AosBitmapPtr &query_bitmap,
				const AosQueryContextPtr &query_context,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = queryRangePriv(query_rslt, query_bitmap, query_context, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	OmnString getValueSafe(const int idx, const int iilidx, const AosRundataPtr &rdata);

	virtual bool querySingleLikeSafe(
				const AosQueryRsltPtr &query_rslt,
				const AosBitmapPtr &query_bitmap,
				const OmnString &value,
				const bool setBlockSize,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = querySingleLikePriv(query_rslt, query_bitmap, value, setBlockSize, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	virtual bool queryValueSafe(
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

	virtual bool checkDocSafe(
				const AosOpr opr,
				const OmnString &value,
				const u64 &docid,
				const AosRundataPtr &rdata);

	bool 	prependDocSafe(
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const bool iilmgrLocked);

	int		getDocidsSafe(
				const AosOpr opr,
				const OmnString &value,
				u64 *docids, 
				const int arraysize,
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


	bool	addBlockSafe(
				char * entries,
				const int size,
				const int num,
				const bool iilmgrLocked,
				const AosRundataPtr &rdata);

	bool	increaseBlockSafe(
				char * &entries,
				const int size,
				const int num,
				const u64 &dftValue,
				const AosIILUtil::AosIILIncType incType,
				const bool iilmgrLocked,
				const AosRundataPtr &rdata);


	virtual bool saveSubIILToTransFileSafe(
				const AosDocTransPtr &docTrans,
				const bool forcesave, 
				const AosRundataPtr &rdata);

	virtual bool saveSubIILToLocalFileSafe(
				const bool forcesave, 
				const AosRundataPtr &rdata);

	bool	setValueDocUniqueSafe(
				const OmnString &key, 
				const u64 &docid, 
				const bool must_same, 
				const AosRundataPtr &rdata);

	bool	getSplitValueSafe(
				const AosQueryContextPtr &context,
				const int size,
				vector<AosQueryContextPtr> &contexts,
				const bool iilmgrlock,
				const AosRundataPtr &rdata);

private:
	virtual bool	queryValuePriv(
						vector<OmnString> &values,
		 				const AosOpr opr,
		 				const OmnString &value,
						const bool unique_value,
						const AosRundataPtr &rdata);

	virtual bool	querySingleLikePriv(
						const AosQueryRsltPtr &query_rslt,
						const AosBitmapPtr &query_bitmap,
						const OmnString &value,
						const bool setBlockSize,
						const AosRundataPtr &rdata);
	
	virtual bool	queryRangePriv(
						const AosQueryRsltPtr &query_rslt,
						const AosBitmapPtr &query_bitmap,
						const AosQueryContextPtr &query_context,
						const AosRundataPtr &rdata);

	bool	preQueryPriv(
				const AosQueryContextPtr &query_context,
				const AosRundataPtr &rdata);

	bool	addDocSeqDirectPriv(
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				int	&idx,
				const bool iilmgrLocked,
				bool &keepSearch,
				const AosRundataPtr &rdata);

	bool 	addDocPriv(
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const bool iilmgrLocked,
				const AosRundataPtr &rdata);

	bool			queryNewPriv(
						const AosQueryRsltPtr &query_rslt,
						const AosBitmapPtr &query_bitmap, 
						const AosQueryContextPtr &query_context,
						const AosRundataPtr &rdata);

	bool	addDocSeqPriv(
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				int	&iilidx,
				int	&idx,
				const bool iilmgrLocked,
				const AosRundataPtr &rdata);

	bool 	addDocDirectPriv(
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const bool iilmgrLocked,
				bool &keepSearch,
				const AosRundataPtr &rdata);

	virtual bool	queryPriv(
						const AosQueryRsltPtr &query_rslt,
						const AosBitmapPtr &query_bitmap,
						const AosOpr opr,
						const OmnString &value,
						const AosRundataPtr &rdata);
	bool	nextDocidPriv(
				int &idx, 
				int &iilidx,
				const bool reverse, 
				const AosOpr opr,
				const OmnString &value,
				u64 &docid, 
				bool &isunique,
				const AosRundataPtr &rdata);

	bool	removeDocDirectPriv(
				const OmnString &value, 
				const u64 &docid,
				bool &keepSearch,
				const AosRundataPtr &rdata); 

	bool	removeDocSeqDirectPriv(
				const OmnString &value, 
				const u64 &docid,
				int	&iilidx,
				int	&idx,
				bool &keepSearch,
				const AosRundataPtr &rdata); 

	AosIILStrPtr 	getSubiilByIndex(
						const int idx,
						const bool iilmgrlocked,
						const AosRundataPtr &rdata);

	bool	nextDocidPrefixPriv(
				int &idx, 
				int &iilidx,
				const bool reverse, 
				const OmnString &prefix,
				u64 &docid,
				const AosRundataPtr &rdata);

	bool	removeDocSeqPriv(
				const OmnString &value, 
				const u64 &docid,
				int	&iilidx,
				int	&idx,
				const AosRundataPtr &rdata); 

	bool	removeDocPriv(
				const OmnString &value, 
				const u64 &docid,
				const AosRundataPtr &rdata);

	bool 	nextDocidSinglePriv(
				int &idx,
				const bool reverse,
				const AosOpr opr1,
				const OmnString &value1,
				const AosOpr opr2,
				const OmnString &value2,
				u64 &docid, 
				bool &isunique,
				const AosRundataPtr &rdata); 

	bool	nextDocidSinglePriv(
				int &idx, 
				const bool reverse, 
				const AosOpr opr,
				const OmnString &value,
				u64 &docid, 
				bool &isunique,
				const AosRundataPtr &rdata);

	bool	nextUniqueValuePriv(
				int &idx,
				int &iilidx,
				const bool reverse,
				const AosOpr opr,
				const OmnString &value,
				OmnString &unique_value,
				bool &found,
				const AosRundataPtr &rdata);
	
	bool	nextUniqueValueSinglePriv(
				int &idx,
				const bool reverse,
				const AosOpr opr,
				const OmnString &value,
				OmnString &unique_value,
				bool &found);

	bool	nextDocidPrefixSinglePriv(
				int &idx, 
				const bool reverse, 
				const OmnString &prefix,
				u64 &docid,
				bool &keepSearch); 

    bool	removeDocByPrefixPriv(
				const OmnString &prefix,
				const AosRundataPtr &rdata);

	virtual bool	deleteIILPriv(
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);
	/*
	bool	removeUniqDocPriv(const u64 &docid);
	virtual bool	checkDocPriv(
						const AosOpr opr,
						const OmnString &value,
						const u64 &docid,
						bool &keepSearch,
						const AosRundataPtr &rdata);

	virtual bool		saveSanityCheckProtected(const AosRundataPtr &rdata);
	virtual AosBuffPtr	getBodyBuffProtected() const;
	virtual bool 		setContentsProtected(AosBuffPtr &buff, const bool, const AosRundataPtr &rdata);
	virtual bool 		prepareMemoryForReloading(const bool iilmgrLocked);
	virtual u64			getMinDocid() const;
	virtual u64			getMaxDocid() const;


	const char * 	getMinValue() const;
	const char * 	getMaxValue() const;
	virtual bool	resetSubIILInfo(const AosRundataPtr &rdata);
	virtual bool	resetSpec();

	virtual bool	deleteIILSinglePriv(
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);


	bool			queryNewLikePriv(
						const AosQueryRsltPtr &query_rslt,
						const AosBitmapPtr &query_bitmap, 
						const AosQueryContextPtr &query_context,
						const AosRundataPtr &rdata);
	
	bool			queryNewLikeSinglePriv(
						const AosQueryRsltPtr &query_rslt,
						const AosBitmapPtr &query_bitmap, 
						const AosQueryContextPtr &query_context,
						const int &start_idx,
						const bool reverse,
						const AosRundataPtr &rdata);

	bool			queryNewNEPriv(
						const AosQueryRsltPtr &query_rslt,
						const AosBitmapPtr &query_bitmap, 
						const AosQueryContextPtr &query_context,
						const AosRundataPtr &rdata);
	
	bool			queryPagingProc(
						const AosQueryRsltPtr &query_rslt,
						const AosBitmapPtr &query_bitmap,
						const AosQueryContextPtr &query_context,
						int &start_idx,
						int &start_iilidx,
						int &end_idx,
						int &end_iilidx,
						bool &has_data,
						const AosRundataPtr &rdata);
	
	void			nextIndexFixErrorIdx(
						int &cur_idx,
						int &cur_iilidx,
						const OmnString &cur_value,
						const u64 &cur_docid,
						const bool &reverse,
						bool &has_data,
						const AosRundataPtr &rdata);

	bool			verifyIdx(
						const int &cur_idx,
						const OmnString &cur_value,
						const u64 &cur_docid);
	
	int				compPos(
						const int &idx1,
						const int &iilidx1,
						const int &idx2,
						const int &iilidx2);

	bool			copyDataWithCheck(
						const AosQueryRsltPtr &query_rslt,
						const AosBitmapPtr &query_bitmap,
						const AosQueryContextPtr &query_context,
						const int &start_idx,
						const int &start_iilidx,
						const int &end_idx,
						const int &end_iilidx,
						const AosRundataPtr &rdata);
	
	bool			copyDataSingleWithCheck(
						const AosQueryRsltPtr &query_rslt,
						const AosBitmapPtr &query_bitmap,
						const AosQueryContextPtr &query_context,
						const int &start_idx,
						const int &end_idx,
						const bool reverse, 
						bool  keep_search,
						const AosRundataPtr &rdata);

	bool			copyData(
						const AosQueryRsltPtr &query_rslt,
						const AosBitmapPtr &query_bitmap,
						const AosQueryContextPtr &query_context,
						const int &start_idx,
						const int &start_iilidx,
						const int &end_idx,
						const int &end_iilidx,
						const AosRundataPtr &rdata);
	
	bool			copyDataSingle(
						const AosQueryRsltPtr &query_rslt,
						const AosBitmapPtr &query_bitmap,
						const AosQueryContextPtr &query_context,
						const int &start_idx,
						const int &end_idx,
						const AosRundataPtr &rdata);
	bool			nextQueryPos(int &idx,int &iilidx);
	bool			prevQueryPos(int &idx,int &iilidx);
	bool			locateIdx(
						int &cur_idx,
						int &cur_iilidx,
						const OmnString &cur_value,
						const u64 &cur_docid,
						const bool reverse,
						bool &has_data,
						const AosRundataPtr &rdata);
	
	virtual AosIILType	getIILType() const {return eAosIILType_Str;}
	virtual bool	firstDoc1(
						int &idx, 
						const bool reverse, 
						const AosOpr opr,
						const OmnString &value,
						u64 &docid);
	AosIILStrPtr	getNextSubIIL(const bool &forward, const AosRundataPtr &rdata);
	virtual bool	checkDocSafe(
						const AosOpr opr,
						const OmnString &value,
						const u64 &docid,
						bool &keepSearch,
						const AosRundataPtr &rdata);
	
	

	bool	expandMemoryPriv();
	
	bool 	splitListPriv(const bool iilmgrLocked,AosIILPtr &subiil, const AosRundataPtr &rdata);



	bool 	appendDocPriv(
				u32 &idx, 
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique,
				const bool iilmgrLocked,
				bool &keepSearch);

	bool 	insertDocPriv(
				u32 &idx, 
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique,
				const bool docid_unique,
				const bool iilmgrLocked,
				bool &keepSearch);

	bool	insertBefore(
				const int nn, 
				const u64 &docid, 
				const OmnString &value);

	bool 	insertAfter(
				const int nn, 
				const u64 &docid, 
				const OmnString &value);

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



	virtual bool	returnSubIILsPriv(
						const bool iilmgrLocked,
						bool &returned,
						const AosRundataPtr &rdata,
						const bool returnHeader);

	bool 	listSanityCheckPriv();
	bool	nextDocidAN(
				int &idx, 
				const int reverse, 
				const OmnString &value,
				u64 &docid);
	bool	nextDocidLT(
				int &idx, 
				const int reverse, 
				const OmnString &value,
				u64 &docid);
	bool	nextDocidLE(
				int &idx, 
				const int reverse, 
				const OmnString &value,
				u64 &docid);
	bool	nextDocidGT(
				int &idx, 
				const int reverse, 
				const OmnString &value,
				u64 &docid);
	bool	nextDocidGE(
				int &idx, 
				const int reverse, 
				const OmnString &value,
				u64 &docid);
	bool	nextDocidEQ(
				int &idx, 
				const int reverse, 
				const OmnString &value,
				u64 &docid);

	// ken 2011/5/25
	bool	nextDocidLike(
				int &idx, 
				const int reverse, 
				const OmnString &value,
				u64 &docid);
	
	int 	firstEQ(const int idx, const OmnString &value);
	int 	firstEQRev(const int idx, const OmnString &value);
	int		nextDocidNE(
				int &idx, 
				const int reverse, 
				const OmnString &value,
				u64 &docid);
	int 	firstNERev(const int idx, const OmnString &value);
	int 	firstNE(const int idx, const OmnString &value);
	int 	firstLERev(const int idx, const OmnString &value);
	int 	firstLTRev(const int idx, const OmnString &value);
	int 	firstLE(const int idx, const OmnString &value);
	int 	firstLT(const int idx, const OmnString &value);
	int 	firstGERev(const int idx, const OmnString &value);
	int 	firstGTRev(const int idx, const OmnString &value);
	int 	firstGT(const int idx, const OmnString &value);
	int 	firstGE(const int idx, const OmnString &value);
	int 	lastLT(const OmnString &value);
	int 	lastLE(const OmnString &value);
	int 	lastEQ(const int idx, const OmnString &value);
	int 	firstLE(const OmnString &value);
	int 	firstLT(const OmnString &value);
	u64 	removeDocByIdxPriv(const int idx, const AosRundataPtr &rdata);

	bool 	valueLikeMatch(
				const char *v,
				const OmnString &value);

	inline bool findFirstRev(
					int &idx, 
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
					int &idx, 
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

	AosIILStrPtr 	getSubiil(const OmnString &value, const AosRundataPtr &rdata);
	int				getSubiilIndex(const OmnString &value);
	AosIILStrPtr 	getIILByIndex(
						const int idx,
						const bool iilmgrlocked,
						const AosRundataPtr &rdata)
	{
		if(idx == 0)
		{
			AosIILStrPtr thisPtr(this, false);
			return thisPtr;
		}
		return getSubiilByIndex(idx,iilmgrlocked,rdata);
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
					const int numDocs,
					const int subiilid, 
					const AosIILStrPtr &rootiil, 
					const bool iilmgrLocked); 
	bool 		addSubiil(
					const AosIILStrPtr &crtsubiil,
					const AosIILStrPtr &nextsubiil);
	int 		getTotalPriv(const OmnString &value, const AosOpr opr);

	bool	setValue(const int idx, const char *value, const int length)
	{
		// Only 'AosIILUtil::eMaxStrValueLen' is stored
		aos_assert_r(length >= 0, false);
		int len = ((u32)length>AosIILUtil::eMaxStrValueLen)?AosIILUtil::eMaxStrValueLen:length;
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
				const int idx,
				const bool changeMax,
				const bool changeMin);
	bool 	createSubiilIndex();
	int		getDocidsPriv(
				const AosOpr opr,
				const OmnString &value,
				u64 *docids,
				const int arraysize,
				int &curPos,
				const AosRundataPtr &rdata);
	
	AosIILStrPtr getSubIIL(
					const int startidx,
					const OmnString &value,
					const AosOpr opr,
					const bool fromHead,
					const AosRundataPtr &rdata)
	{
		int iilidx = getSubIILIndex(startidx, value, opr, fromHead);
		if (iilidx < 0) return 0;
		return getSubiilByIndex(iilidx, false, rdata);
	}

	int		getSubIILIndex(
				const int startidx, 
				const OmnString &value,
				const AosOpr opr,
				const bool fromHead);

	OmnString	getFirstValue()
	{
		if(mNumDocs > 0)return mValues[0];
		else return "";
	}

	OmnString	getLastValue()
	{
		if(mNumDocs > 0)return mValues[mNumDocs-1];
		else return "";
	}

	u64	getDocIdPriv1(int &idx, int &iilidx, const AosRundataPtr &rdata);

	virtual bool 	sanityTestForSubiils();
	bool			splitSanityCheck();

	virtual AosIILPtr	createCompIIL(const AosRundataPtr &rdata);

	OmnString getValue(const u32 idx) const 
	{
		aos_assert_r(mValues, "");
		aos_assert_r(idx < mNumDocs, "");
		return mValues[idx];
	}

	u64 getDocid(const u32 idx) const 
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
		OmnString objid = getRandomValuePriv(docid, rdata);
		AOSUNLOCK(mLock);
		return objid;
	}


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


	virtual bool	querySinglePriv(
						const AosQueryRsltPtr &query_rslt,
						const AosBitmapPtr &query_bitmap,
						const AosOpr opr,
						const OmnString &value,
						const AosRundataPtr &rdata);

	virtual bool	querySingleNEPriv(
						const AosQueryRsltPtr &query_rslt,
						const AosBitmapPtr &query_bitmap,
						const OmnString &value,
						const AosRundataPtr &rdata);
	
	virtual bool	queryNEPriv(
						const AosQueryRsltPtr &query_rslt,
						const AosBitmapPtr &query_bitmap,
						const OmnString &value,
						const AosRundataPtr &rdata);

	virtual bool	queryLikePriv(
						const AosQueryRsltPtr &query_rslt,
						const AosBitmapPtr &query_bitmap,
						const OmnString &value,
						const AosRundataPtr &rdata);

	bool 			verifyDuplicatedEntriesPriv(const AosRundataPtr &rdata);
	OmnString		getRandomValuePriv(u64 &docid, const AosRundataPtr &rdata);

	bool    copyData(
				char** values, 
				u64 *docids,
				const int buff_len, 
				int &num_docs,
				const AosRundataPtr &rdata);

	bool    copyDataSingle(
				char** values, 
				u64 *docids, 
				const int buff_len,
				int &num_docs);
						   
	bool docidSortedSanityCheck(const int crtidx);

	bool	incrementCounter(const OmnString &counter_id, const u64 &value, const AosRundataPtr &rdata);

	bool	appendDocidSafe(
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool iilmgrLocked,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = addDocPriv(
						value, docid, value_unique, false, iilmgrLocked, rdata);
		aos_assert_rb(AosIILStrSanityCheck(this), mLock, false);
		AOSUNLOCK(mLock);
		return rslt;
	}

	int		getSubIILIndexByPrefix(
				const int startidx, 
				const OmnString &value,
				const bool fromHead);
	
	bool	nextDocidPrefix(
				int &idx, 
				const int reverse, 
				const OmnString &prefix,
				u64 &docid);

	int		firstPrefixRev(const int idx, const OmnString &prefix);
	
	bool	mergeSubiilPriv(const bool iilmgrLocked, const int iilidx, const AosRundataPtr &rdata);
	
	bool 	appendDocToSubiil(
				char **values, 
				u64 *docids, 
				const int numDocs,
				const bool addToHead);

	bool 	removeDocFromSubiil(
				const int numDocs,
				const bool delFromHead);
	
	bool 	removeDocFromSubiil2Safe(
				const int start_idx,
				const int end_idx,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = removeDocFromSubiil2Priv(start_idx, end_idx, rdata);
		aos_assert_rb(AosIILStrSanityCheck(this), mLock, false);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool 	removeDocFromSubiil2Priv(
				const int start_idx,
				const int end_idx,
				const AosRundataPtr &rdata);
							   			 
	bool	removeDocsOnOneSub(
				const int iilidx,
				const int startidx,
				const int endidx,
				const AosRundataPtr &rdata);

	bool 	mergeSubiilForwardPriv(
				const bool iilmgrLocked,
				const int iilidx,
				const int numDocToMove,
				const AosRundataPtr &rdata);
	
	bool 	mergeSubiilBackwardPriv(
				const bool iilmgrLocked,
				const int iilidx,
				const int numDocToMove,
				const AosRundataPtr &rdata);

	bool	getValueDocidPtr(
				char ** &valuePtr,
				u64*    &docidPtr,
				const int offset);

	bool 	removeSubiil(
				const int iilidx,
				const bool iilmgrLocked,
				const AosRundataPtr &rdata);

	int		getSubiilIndexSeq(const OmnString &value, const int iilidx);

	AosIILStrPtr	getSubiilSeq(const OmnString &value, int &iilidx, const AosRundataPtr &rdata);

	bool			checkDocidUnique(
						const bool rslt, 
						const int idx, 
						u64 &docid,
						bool &isunique,
						const AosRundataPtr &rdata);

	virtual bool	copyDocidsPriv(
						const AosQueryRsltPtr &query_rslt,
						const AosBitmapPtr &query_bitmap,
						const int start,
						const int end);

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
	
	bool	removeFirstValueDocSafe(
				const OmnString &value,
				u64 &docid,
				const bool reverse,
				const AosRundataPtr &rdata);

	void    exportIILPriv(
				vector<OmnString> 	&values,
			   	vector<u64>			&docids,
				const AosRundataPtr &rdata);
    void    exportIILSinglePriv(
				vector<OmnString> 	&values,
			   	vector<u64>			&docids,
				const AosRundataPtr &rdata);

	AosIILCompStrPtr 	retrieveCompIIL(const bool, const AosRundataPtr &rdata);
	
	bool	returnCompIIL(const AosIILPtr &iil, const AosRundataPtr &rdata);

	bool queryColumn(
				const OmnString &value1, 
				const AosOpr opr1, 
				const OmnString &value2, 
				const AosOpr opr2, 
				const AosStrStrArrayPtr data,
				const AosRundataPtr &rdata);
	

	bool	preQueryNEPriv(
				const AosQueryContextPtr &query_context,
				const AosRundataPtr &rdata);
	*/
};
#endif

