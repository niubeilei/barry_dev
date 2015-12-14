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
#ifndef Omn_IILMgrBig_IILHit_h
#define Omn_IILMgrBig_IILHit_h

#include "IILMgrBig/IIL.h"

#include <vector>
using namespace std;

class AosIILHit : public AosIIL
{

private:
	AosIILHitPtr *	mSubiils;
	u64 *			mMinVals;
	u64 *			mMaxVals;
	
public:
	AosIILHit(
			const bool isPersis,
			const bool iilmgrLocked, 
			const AosRundataPtr &rdata);
	AosIILHit(
			const u64 &wordid, 
			const u64 &iilid,
			const bool isPersis,
			const OmnString &iilname, 
			const bool iilmgrlocked, 
			const AosRundataPtr &rdata);
	AosIILHit(
			const u64 &iilid, 
			const u32 siteid,
			const AosDfmDocIILPtr &doc,         //Ketty 2012/11/15
			const bool iilmgrLocked, 
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

	bool	addDocSafe(const u64 &docid, const AosRundataPtr &rdata)
	{
		// OmnScreen << "IILDocid add: " << this << endl;
		mLock->lock();
		bool rslt = addDocPriv(docid, false, rdata);
		mLock->unlock();
		return rslt;
	}

	bool	removeDocSafe(const u64 &docid, const AosRundataPtr &rdata)
	{
		mLock->lock();
		bool rslt = removeDocPriv(docid, rdata);
		mLock->unlock();
		return rslt;
	}

	virtual bool	preQuerySafe(
						const AosQueryContextObjPtr &query_context,
						const bool iilmgrLocked, 
						const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = preQueryPriv(query_context,iilmgrLocked,rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}
private:
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
				int &iilidx,
				int &idx,
				const bool iilmgrLocked,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = addDocSeqPriv(docid,iilidx,idx, iilmgrLocked,rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool	removeDocSeqSafe(
				const u64 &docid,
				int &iilidx,
				int &idx,
				const bool iilmgrLocked,
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = removeDocSeqPriv(docid,iilidx,idx,iilmgrLocked,rdata);
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

	virtual AosIILType getIILType() const {return eAosIILType_Hit;}

	virtual u32 getTotalNumDocsSafe() const
	{
		mLock->lock();
		u32 numdoc = mNumDocs;
		if(isRootIIL())
		{
			numdoc = 0;
			for(u32 i =0;i < mNumSubiils;i++)
			{
				numdoc += mNumEntries[i];
			}
		}
		mLock->unlock();
		return numdoc;
	}

	// Chen Ding, 04/18/2012
	virtual u64	getTotalSafe(
			const OmnString &value, 
			const AosOpr opr, 
			const AosRundataPtr &rdata);

	// Chen Ding, 04/18/2012
	virtual u64	getTotalSafe(
			const u64 &value, 
			const AosOpr opr, 
			const AosRundataPtr &rdata);

	virtual bool 		adjustMemoryProtected();

	virtual bool		checkDocSafe(
							const AosOpr opr,
							const OmnString &value,
							const u64 &docid,
							const AosRundataPtr &rdata);
	virtual bool		checkDocSafe(
							const AosOpr opr,
							const u32 &value,
							const u64 &docid,
							const AosRundataPtr &rdata);

	virtual bool 		setContentsProtected(
							AosBuffPtr &buff,
							const bool,
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

	u64		getNextEntrySafe(int &idx, int &iilidx, const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		u64 docid = getNextEntryPriv(idx, iilidx, rdata);
		AOSUNLOCK(mLock);
		return docid;
	}

/// not in use
	virtual u64			getDocIdSafe1(
							int &idx,
							int &iilidx, 
							const AosRundataPtr &rdata)
	{
		int did = 0;
		AOSLOCK(mLock);
		did = getDocIdPriv1(idx, iilidx, rdata);
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
	//virtual bool	saveSubIILToTransFileSafe(
	//					const AosDocTransPtr &docTrans,
	//					const bool forcesave, 
	//					const AosRundataPtr &rdata);
	virtual bool	saveSubIILToLocalFileSafe(
						const bool forcesave, 
						const AosRundataPtr &rdata);

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
	
	AosIILHitPtr	getSubiilByIndex(const int idx, const AosRundataPtr &rdata);
	
	bool 			addDocPriv(
						const u64 &docid,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

	AosIILHitPtr	getSubiil(
						const u64 value, 
						const AosRundataPtr &rdata);

	bool 			insertDocid(
						const u64 &docid,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

	int				getSubiilIndex(const u64 value);

	bool 			splitListPriv(
						const bool iilmgrLocked,
						AosIILHitPtr &subiil,
						const AosRundataPtr &rdata);

	bool 			expandMem();

	bool 			updateIndexData(
						const int idx,
						const bool changeMax,
						const bool changeMin,
						const AosRundataPtr &rdata);
	bool 			initSubiil(
						u64 *docids, 
						const int numDocs,
						const int subiilid, 
						const AosIILHitPtr &rootiil, 
						const bool iilmgrLocked); 

	bool 			addSubiil(
						const AosIILHitPtr &crtsubiil,
						const AosIILHitPtr &nextsubiil,
						const AosRundataPtr &rdata);

	const u64 		getMinValue() const;
	const u64  		getMaxValue() const;

	bool 			prepareMemoryForReloading2(const bool iilmgrLocked);
	bool 			createSubiilIndex();

	bool			removeDocPriv(const u64 &docid, const AosRundataPtr &rdata);
	bool			removeDocDirectPriv(const u64 &docid, const AosRundataPtr &rdata);
	bool 			getFirstDocIdx(const u64 &docid, int &theidx);

	bool 			mergeSubiilPriv(
						const bool iilmgrLocked,
						const int iilidx,
						const AosRundataPtr &rdata);
	bool			mergeSubiilForwardPriv(
						const bool iilmgrLocked,
						const int iilidx,
						const int numDocToMove,
						const AosRundataPtr &rdata);
	bool			mergeSubiilBackwardPriv(
						const bool iilmgrLocked,
						const int iilidx,
						const int numDocToMove,
						const AosRundataPtr &rdata);

	bool			getDocidPtr(
						u64* &docidPtr,
				    	const int offset);

	bool			appendDocToSubiil(
						u64 *docids, 
						const int numDocs,
						const bool addToHead);
						   
	bool			removeDocFromSubiil(
						const int numRemove,
						const bool delFromHead);

	bool			removeSubiil(
						const int iilidx,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

	virtual bool	queryPriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosRundataPtr &rdata);

	bool	queryPagingProc(
				const AosQueryRsltObjPtr &query_rslt,
				const AosBitmapObjPtr &query_bitmap,
 				const AosQueryContextObjPtr &query_context,
				int &start_idx,
				int &start_iilidx,
				int &end_idx,
				int &end_iilidx,
				bool &has_data,
				const AosRundataPtr &rdata);

	bool	copyData(
				const AosQueryRsltObjPtr &query_rslt,
				const AosBitmapObjPtr &query_bitmap,
 				const AosQueryContextObjPtr &query_context,
				const int &start_idx,
				const int &start_iilidx,
				const int &end_idx,
				const int &end_iilidx,
				const AosRundataPtr &rdata);

	bool	copyDataWithCheck(
				const AosQueryRsltObjPtr &query_rslt,
				const AosBitmapObjPtr &query_bitmap,
 				const AosQueryContextObjPtr &query_context,
				const int &start_idx,
				const int &start_iilidx,
				const int &end_idx,
				const int &end_iilidx,
				const AosRundataPtr &rdata);

	bool	copyDataSingle(
				const AosQueryRsltObjPtr &query_rslt,
				const AosBitmapObjPtr &query_bitmap,
 				const AosQueryContextObjPtr &query_context,
				const int &start_idx,
				const int &end_idx,
				const AosRundataPtr &rdata);

	bool	copyDataSingleWithCheck(
				const AosQueryRsltObjPtr &query_rslt,
				const AosBitmapObjPtr &query_bitmap,
 				const AosQueryContextObjPtr &query_context,
				const int &start_idx,
				const int &end_idx,
				const bool reverse, 
				bool  keep_search,
				const AosRundataPtr &rdata);


	void	nextIndexFixErrorIdx(
				int &cur_idx,
				int &cur_iilidx,
				const u64 &cur_docid,
				const bool reverse,
				bool &has_data,
				const AosRundataPtr &rdata);
	bool	verifyIdx(
				int &cur_idx,
				const u64 &cur_docid);
	bool	locateIdx(
				int &cur_idx,
				int &cur_iilidx,
				const u64 &cur_docid,
				const bool reverse,
				bool &has_data,
				const AosRundataPtr &rdata);
	int 	firstLTRev(const u64 &docid);
	int 	firstGT(const u64 &docid);
				
				
	bool	nextQueryPos(int &idx,int &iilidx);
	bool	prevQueryPos(int &idx,int &iilidx);

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
						int &iilidx,
						int &idx,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

	bool 			removeDocSeqPriv(
						const u64 &docid,
						int &iilidx,
						int &idx,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

	int	 			getSubiilIndexSeq(const u64 value,const int iilidx);
	
	bool			insertDocidSeq(
						const u64 &docid,
						int &iilidx,
						int &idx,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

	bool			removeDocSeqDirectPriv(
						const u64 &docid,
						int &iilidx,
						int &idx, 
						const AosRundataPtr &rdata);

	virtual bool	returnSubIILsPriv(
						const bool iilmgrLocked,
						bool &returned, 
						const AosRundataPtr &rdata,
						const bool returnHeader);

	virtual bool	firstDoc1(
						int &idx, 
						const bool reverse, 
						const AosOpr opr,
						const OmnString &value,
						u64 &docid);

	virtual bool	firstDoc1(
						int &idx, 
						const bool reverse, 
						const AosOpr opr,
						const u32 &value,
						u64 &docid);

	bool 			removeSubiilIndex();

	virtual u64		getMinDocid() const;
	virtual u64		getMaxDocid() const;

	virtual bool	saveSanityCheckProtected(const AosRundataPtr &rdata);
	bool			docExistPriv(const u64 &docId, const AosRundataPtr &rdata);

	bool			docExistDirectSafe(const u64 &docId, const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = docExistDirectPriv(docId,rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	bool			docExistDirectPriv(const u64 &docid, const AosRundataPtr &rdata);

	u64 			getNextEntryPriv(int &idx, int &iilidx, const AosRundataPtr &rdata);
	int				firstSubLTRev(const u64 &cur_docid);
	int				firstSubGT(const u64 &cur_docid);

	bool	preQueryPriv(
				const AosQueryContextObjPtr &query_context,
				const bool iilmgrLocked, 
				const AosRundataPtr &rdata);
/////////////////////////
// no in use
	bool			sanityCheck();
	u64				getDocIdPriv1(
						const int &idx,
						const int &iilidx, 
						const AosRundataPtr &rdata);
//	bool 		expandMemIIL();
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

