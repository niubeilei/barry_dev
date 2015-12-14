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
// 	Created: 12/11/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_IILMgr_IILHit_h
#define Omn_IILMgr_IILHit_h

#include "IILMgr/IIL.h"


class AosIILHit : public AosIIL
{
	AosIILHitPtr *	mSubiils;
	u64 *			mMinVals;
	u64 *			mMaxVals;
	
public:
	AosIILHit();
	AosIILHit(
			const u64 &wordid, 
			const u64 &iilid,
			const bool isPersis,
			const OmnString &iilname, 
			const AosRundataPtr &rdata);
	AosIILHit(
			const u64 &iilid, 
			const u32 siteid,
			const AosDfmDocIILPtr &doc, 		//Ketty 2012/11/15
			const AosRundataPtr &rdata); 
	~AosIILHit();

	virtual bool	querySafe(
						const AosQueryRsltObjPtr& query_rslt,
						const AosBitmapObjPtr& query_bitmap,
		 				const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata)
	{
        AOSLOCK(mLock);
        bool rslt = queryPriv(query_rslt, query_bitmap, rdata);
        AOSUNLOCK(mLock);
		return rslt;
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


	bool	addDocSafe(const u64 &docid, const AosRundataPtr &rdata)
	{
		// OmnScreen << "IILDocid add: " << this << endl;
		AOSLOCK(mLock);
		bool rslt = addDocPriv(docid, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool	removeDocSafe(const u64 &docid, const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = removeDocPriv(docid, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}
	
	// Ketty 2013/09/06 moved from private to public. for HitRlbTesterCheckTrans	
	bool			docExistDirectSafe(const u64 &docId, const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = docExistDirectPriv(docId,rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}


private:
	virtual bool	deleteIILPriv(
						const bool true_delete,
						const AosRundataPtr &rdata);
	virtual bool	deleteIILSinglePriv(const AosRundataPtr &rdata);

	virtual bool	preQuerySafe(
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = preQueryPriv(query_context,rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	virtual bool		queryNewSafe(
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

	bool	addDocSeqSafe(
				const u64 &docid,
				i64 &iilidx,
				i64 &idx,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = addDocSeqPriv(docid,iilidx,idx, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool	removeDocSeqSafe(
				const u64 &docid,
				i64 &iilidx,
				i64 &idx,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = removeDocSeqPriv(docid,iilidx,idx,rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}
	
    void	exportIILSafe(
				vector<u64> &docids,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		exportIILPriv(docids,rdata);
		AOSUNLOCK(mLock);
	}
		
	virtual bool	returnSubIILsPriv(const AosRundataPtr &rdata)
	{
		bool returned;
		return returnSubIILsPriv(returned, false, rdata);
	}
	virtual bool	returnSubIILsPriv(
						bool &returned, 
						const bool returnHeader,
						const AosRundataPtr &rdata); 

	virtual AosIILType getIILType() const {return eAosIILType_Hit;}

	virtual i64 getTotalNumDocsSafe() const
	{
		AOSLOCK(mLock);
		i64 numdoc = mNumDocs;
		if (isParentIIL())
		{
			numdoc = 0;
			for(i64 i=0; i<mNumSubiils; i++)
			{
				numdoc += mNumEntries[i];
			}
		}
		AOSUNLOCK(mLock);
		return numdoc;
	}

	virtual i64		getTotalSafe(
							const OmnString &value, 
							const AosOpr opr);
	virtual i64		getTotalSafe(
							const u64 &value, 
							const AosOpr opr);

	virtual bool 		adjustMemoryProtected();

	virtual bool		checkDocSafe(
							const AosOpr opr,
							const OmnString &value,
							const u64 &docid,
							const AosRundataPtr &rdata);
	virtual bool		checkDocSafe(
							const AosOpr opr,
							const u64 &value,
							const u64 &docid,
							const AosRundataPtr &rdata);

	virtual bool 		setContentsProtected(
							AosBuffPtr &buff,
							const AosRundataPtr &rdata);
	virtual AosBuffPtr	getBodyBuffProtected() const;


	virtual bool		resetSubIILInfo(const AosRundataPtr &rdata);
	
//	virtual void		resetSafe(
//							const u64 &wordid,
//							const u64 &iilid,
//							const bool iilmgrLocked,
//							const AosRundataPtr &rdata);

	bool	docExistSafe(const u64 &docId, const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = docExistPriv(docId,rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	u64		getNextEntrySafe(i64 &idx, i64 &iilidx, const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		u64 docid = getNextEntryPriv(idx, iilidx, rdata);
		AOSUNLOCK(mLock);
		return docid;
	}

/// not in use
	virtual u64			getDocIdSafe1(
							i64 &idx,
							i64 &iilidx, 
							const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		u64 did = getDocIdPriv1(idx, iilidx, rdata);
		AOSUNLOCK(mLock);
		return did;
	}

	virtual bool	getDocidBody(const AosBuffPtr &buff) const;
//	virtual bool 	modifyDocSafe(const u64 &docid, const u64 &value);
//	virtual bool 	modifyDocSafe(const u64 &docid, const OmnString &value);

protected:
	bool			queryNewPriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);
	virtual void    failInSetCtntProtection();
	virtual bool	saveSubIILToLocalFileSafe(const AosRundataPtr &rdata);

private:	

    void	exportIILSingleSafe(
				vector<u64>	&docids,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		exportIILSinglePriv(docids,rdata);
		AOSUNLOCK(mLock);
	}

	void    		exportIILPriv(
						vector<u64>	&docids,
						const AosRundataPtr &rdata);
    void    		exportIILSinglePriv(
						vector<u64>	&docids,
						const AosRundataPtr &rdata);
	
	AosIILHitPtr	getSubiilByIndex(const i64 &idx, const AosRundataPtr &rdata);
	
	bool 			addDocPriv(
						const u64 &docid,
						const AosRundataPtr &rdata);

	AosIILHitPtr	getSubiil(
						const u64 value, 
						const AosRundataPtr &rdata);

	bool 			insertDocid(
						const u64 &docid,
						const AosRundataPtr &rdata);

	i64			getSubiilIndex(const u64 &value);

	bool 			splitListPriv(
						AosIILHitPtr &subiil,
						const AosRundataPtr &rdata);

	bool 			expandMem();

	bool 			updateIndexData(
						const i64 &idx,
						const bool changeMax,
						const bool changeMin,
						const AosRundataPtr &rdata);
	bool 			initSubiil(
						u64 *docids, 
						const i64 &numDocs,
						const i64 &subiilidx, 
						const AosIILHitPtr &rootiil); 

	bool 			addSubiil(
						const AosIILHitPtr &crtsubiil,
						const AosIILHitPtr &nextsubiil,
						const AosRundataPtr &rdata);

	const u64 		getMinValue() const;
	const u64  		getMaxValue() const;

	bool 			prepareMemoryForReloading2();
	bool 			createSubiilIndex();

	bool			removeDocPriv(const u64 &docid, const AosRundataPtr &rdata);
	bool			removeDocDirectPriv(const u64 &docid, const AosRundataPtr &rdata);
	bool 			getFirstDocIdx(const u64 &docid, i64 &theidx);

	bool 			mergeSubiilPriv(
						const i64 &iilidx,
						const AosRundataPtr &rdata);
	bool			mergeSubiilForwardPriv(
						const i64 &iilidx,
						const i64 &numDocToMove,
						const AosRundataPtr &rdata);
	bool			mergeSubiilBackwardPriv(
						const i64 &iilidx,
						const i64 &numDocToMove,
						const AosRundataPtr &rdata);

	bool			getDocidPtr(
						u64* &docidPtr,
				    	const i64 &offset);

	bool			appendDocToSubiil(
						u64 *docids, 
						const i64 &numDocs,
						const bool addToHead);
						   
	bool			removeDocFromSubiil(
						const i64 &numRemove,
						const bool delFromHead);

	bool			removeSubiil(
						const i64 &iilidx,
						const AosRundataPtr &rdata);

	virtual bool	queryPriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosRundataPtr &rdata);

	bool	queryPagingProc(
				const AosQueryRsltObjPtr &query_rslt,
				const AosBitmapObjPtr &query_bitmap,
 				const AosQueryContextObjPtr &query_context,
				i64 &start_idx,
				i64 &start_iilidx,
				i64 &end_idx,
				i64 &end_iilidx,
				bool &has_data,
				const AosRundataPtr &rdata);

	bool	copyData(
				const AosQueryRsltObjPtr &query_rslt,
				const AosBitmapObjPtr &query_bitmap,
 				const AosQueryContextObjPtr &query_context,
				const i64 &start_idx,
				const i64 &start_iilidx,
				const i64 &end_idx,
				const i64 &end_iilidx,
				const AosRundataPtr &rdata);

	bool	copyDataWithCheck(
				const AosQueryRsltObjPtr &query_rslt,
				const AosBitmapObjPtr &query_bitmap,
 				const AosQueryContextObjPtr &query_context,
				const i64 &start_idx,
				const i64 &start_iilidx,
				const i64 &end_idx,
				const i64 &end_iilidx,
				const AosRundataPtr &rdata);

	bool	copyDataSingle(
				const AosQueryRsltObjPtr &query_rslt,
				const AosBitmapObjPtr &query_bitmap,
 				const AosQueryContextObjPtr &query_context,
				const i64 &start_idx,
				const i64 &end_idx,
				const AosRundataPtr &rdata);

	bool	copyDataSingleWithCheck(
				const AosQueryRsltObjPtr &query_rslt,
				const AosBitmapObjPtr &query_bitmap,
 				const AosQueryContextObjPtr &query_context,
				const i64 &start_idx,
				const i64 &end_idx,
				const bool reverse, 
				bool  keep_search,
				const AosRundataPtr &rdata);


	void	nextIndexFixErrorIdx(
				i64 &cur_idx,
				i64 &cur_iilidx,
				const u64 &cur_docid,
				const bool reverse,
				bool &has_data,
				const AosRundataPtr &rdata);
	bool	verifyIdx(
				i64 &cur_idx,
				const u64 &cur_docid);
	bool	locateIdx(
				i64 &cur_idx,
				i64 &cur_iilidx,
				const u64 &cur_docid,
				const bool reverse,
				bool &has_data,
				const AosRundataPtr &rdata);
	i64	firstLTRev(const u64 &docid);
	i64	firstGT(const u64 &docid);
				
				
	bool	nextQueryPos(i64 &idx, i64 &iilidx);
	bool	prevQueryPos(i64 &idx, i64 &iilidx);

	bool	checkDocs(
		const AosQueryRsltObjPtr &query_rslt,
		const AosRundataPtr &rdata);
	virtual bool	queryDirectSafe(
						const AosQueryRsltObjPtr& query_rslt,
						const AosBitmapObjPtr& query_bitmap)
	{
        AOSLOCK(mLock);
        bool rslt = queryDirectPriv(query_rslt,query_bitmap);
        AOSUNLOCK(mLock);
		return rslt;
	}

	virtual bool	queryDirectPriv(
						const AosQueryRsltObjPtr& query_rslt,
						const AosBitmapObjPtr& query_bitmap);

	bool 			addDocSeqPriv(
						const u64 &docid,
						i64 &iilidx,
						i64 &idx,
						const AosRundataPtr &rdata);

	bool 			removeDocSeqPriv(
						const u64 &docid,
						i64 &iilidx,
						i64 &idx,
						const AosRundataPtr &rdata);

	i64 		getSubiilIndexSeq(const u64 &value, const i64 &iilidx);
	
	bool			insertDocidSeq(
						const u64 &docid,
						i64 &iilidx,
						i64 &idx,
						const AosRundataPtr &rdata);

	bool			removeDocSeqDirectPriv(
						const u64 &docid,
						i64 &iilidx,
						i64 &idx, 
						const AosRundataPtr &rdata);

	virtual bool	firstDoc1(
						i64 &idx, 
						const bool reverse, 
						const AosOpr opr,
						const OmnString &value,
						u64 &docid);

	virtual bool	firstDoc1(
						i64 &idx, 
						const bool reverse, 
						const AosOpr opr,
						const u64 &value,
						u64 &docid);

	bool 			removeSubiilIndex();

	virtual u64		getMinDocid() const;
	virtual u64		getMaxDocid() const;

	virtual bool	saveSanityCheckProtected(const AosRundataPtr &rdata);
	bool			docExistPriv(const u64 &docId, const AosRundataPtr &rdata);

	//bool			docExistDirectSafe(const u64 &docId, const AosRundataPtr &rdata)
	//{
	//	AOSLOCK(mLock);
	//	bool rslt = docExistDirectPriv(docId,rdata);
	//	AOSUNLOCK(mLock);
	//	return rslt;
	//}

	bool			docExistDirectPriv(const u64 &docid, const AosRundataPtr &rdata);

	u64 			getNextEntryPriv(i64 &idx, i64 &iilidx, const AosRundataPtr &rdata);
	i64			firstSubLTRev(const u64 &cur_docid);
	i64			firstSubGT(const u64 &cur_docid);


	bool	preQueryPriv(
				const AosQueryContextObjPtr &query_context,
				const AosRundataPtr &rdata);

/////////////////////////
// no in use
	bool			sanityCheck();
	u64				getDocIdPriv1(
						const i64 &idx,
						const i64 &iilidx, 
						const AosRundataPtr &rdata);
//	bool 		expandMemIIL();

public:
	virtual void setSnapShotId(const u64 &snap_id);
	virtual void resetSnapShotId();

	virtual void setSnapShotIdPriv(const u64 &snap_id);
	virtual void resetSnapShotIdPriv();

	virtual bool resetIIL(const AosRundataPtr &rdata);
};

#endif

