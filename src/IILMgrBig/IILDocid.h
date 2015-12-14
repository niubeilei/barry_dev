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
#ifndef Omn_IILMgr_IILDocid_h
#define Omn_IILMgr_IILDocid_h

#if 0
#include "IILMgrBig/IIL.h"

class AosIILDocid : public AosIIL
{

private:

	// Used to manage subiils
	AosIILDocidPtr 		*mSubiils;
	u64 *				mMinVals;
	u64 *				mMaxVals;

public:
	AosIILDocid(
			const bool isPersis,
			const bool iilmgrLocked, 
			const AosRundataPtr &rdata);
	AosIILDocid(
			const u64 &wordid, 
			const u64 &iilid,
			const bool isPersis,
			const OmnString &iilname, 
			const bool iilmgrLocked, 
			const AosRundataPtr &rdata);
	AosIILDocid(
			const u64 &iilid, 
			AosBuff &buff, 
			const bool iilmgrLocked, 
			const AosRundataPtr &rdata);
	~AosIILDocid();

public:
	virtual bool		removeUniqDocSafe(const u64 &docid);
//	virtual int 		resetSubiilsPriv(AosIILPtr subiils[eMaxSubiils]);
	bool				docExistSafe(const u64 &docId, const AosRundataPtr &rdata);
	virtual u64			getMinDocid() const;
	virtual u64			getMaxDocid() const;

	bool 		addDocSafe(const u64 &docid, const AosRundataPtr &rdata)
	{
		// OmnScreen << "IILDocid add: " << this << endl;
		mLock->lock();
		bool rslt = addDocPriv(docid, true, rdata);
		mLock->unlock();
		return rslt;
	}

	bool 		addDocSeqSafe(const u64 &docid,
							  int &iilidx,
							  int &idx,
							  const bool iilmgrLocked,
							  const AosRundataPtr &rdata)
	{
		// OmnScreen << "IILDocid add: " << this << endl;
		mLock->lock();
		bool rslt = addDocSeqPriv(docid,iilidx,idx,iilmgrLocked, rdata);
		mLock->unlock();
		return rslt;
	}


	bool		removeUniqDocSafe(
					const u64 &docid,
					int &numFound,
					const AosRundataPtr &rdata);
	bool		removeUniqDocDirectPriv(
					const u64 &docid,
					int &numFound,
					bool &hasmore,
					const AosRundataPtr &rdata);

	bool		removeDocSafe(const u64 &docid, const AosRundataPtr &rdata)
	{
		mLock->lock();
		bool rslt = removeDocPriv(docid, rdata);
		mLock->unlock();
		return rslt;
	}
	
	bool		removeDocPriv(const u64 &docid, const AosRundataPtr &rdata);
	bool		removeDocSeqSafe(
					const u64 &docid,
					int &iilidx,
					int &idx,
					const bool iilmgrLocked,
					const AosRundataPtr &rdata)
	{
		// OmnScreen << "IILDocid add: " << this << endl;
		mLock->lock();
		bool rslt = removeDocSeqPriv(docid, iilidx, idx, iilmgrLocked, rdata);
		mLock->unlock();
		return rslt;
	}
								 
	bool		removeDocDirectPriv(const u64 &docid, const AosRundataPtr &rdata);
	bool		removeDocSeqDirectPriv(const u64 &docid,int &iilidx,int &idx, const AosRundataPtr &rdata);

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
	virtual int			getTotalSafe(const OmnString &value, 
							const AosOpr opr);
	virtual int			getTotalSafe(const u64 &value, 
							const AosOpr opr);
	virtual void		resetSafe(const u64 &wordid, const u64 &iilid,const bool iilmgrLocked, const AosRundataPtr &rdata);

	bool		resetData();

	virtual bool 		setContentsProtected(AosBuffPtr &buff, const bool, const AosRundataPtr &rdata);
	virtual AosBuffPtr	getBodyBuffProtected() const;
	virtual bool		saveSanityCheckProtected(const AosRundataPtr &rdata);
	virtual bool 		adjustMemoryProtected();


	virtual AosIILType	getIILType() const {return eAosIILType_Docid;}
	virtual bool		returnSubIILsSafe(const bool iilmgrLocked,bool &returned, const AosRundataPtr &rdata)
	{
        AOSLOCK(mLock);
        bool rslt = returnSubIILsPriv(iilmgrLocked,returned, rdata);
        AOSUNLOCK(mLock);
		return rslt;		
	}
	virtual bool		returnSubIILsPriv(const bool iilmgrLocked,bool &returned, const AosRundataPtr &rdata);
//	int					retrieveNewIILIDFromMgr();

	
	virtual u32 getTotalNumDocsSafe() const
	{
		mLock->lock();
		u32 numdoc = mNumDocs;
		if(isRootIIL())
		{
			numdoc = 0;
			for(int i =0;i < mNumSubiils;i++)
			{
				numdoc += mNumEntries[i];
			}
		}
		mLock->unlock();
		return numdoc;
	}

	virtual bool		resetSubIILInfo(const AosRundataPtr &rdata);

	virtual u64			getDocIdSafe1(int &idx,int &iilidx, const AosRundataPtr &rdata);

	virtual bool		querySafe(
							const AosQueryRsltObjPtr& query_rslt,
							const AosBitmapObjPtr& query_bitmap,
							const AosRundataPtr &rdata)
	{
        AOSLOCK(mLock);
        bool rslt = queryPriv(query_rslt, query_bitmap, rdata);
        AOSUNLOCK(mLock);
		return rslt;
	}

	virtual bool		queryDirectSafe(const AosQueryRsltObjPtr& query_rslt,const AosBitmapObjPtr& query_bitmap)
	{
        AOSLOCK(mLock);
        bool rslt = queryDirectPriv(query_rslt,query_bitmap);
        AOSUNLOCK(mLock);
		return rslt;
	}

	// Chen Ding, 12/07/2010
	bool			getDocidBody(const AosBuffPtr &buff) const;

private:

	virtual bool		queryPriv(
							const AosQueryRsltObjPtr& query_rslt,
							const AosBitmapObjPtr& query_bitmap,
							const AosRundataPtr &rdata);
	virtual bool		queryDirectPriv(const AosQueryRsltObjPtr& query_rslt,const AosBitmapObjPtr& query_bitmap);
	virtual bool 		readCompIIL();
	virtual bool		firstDoc1(
							int &idx, 
							const bool reverse, 
							const AosOpr opr,
							const OmnString &value,
							u64 &docid);
	virtual bool		firstDoc1(
							int &idx, 
							const bool reverse, 
							const AosOpr opr,
							const u32 &value,
							u64 &docid);


	bool		setMaxVals(int index,const u64 value);
	bool		setMinVals(int index,const u64 value);
	bool 		updateIndexData(
						const int idx,
						const bool changeMax,
						const bool changeMin,
						const AosRundataPtr &rdata);

	bool 		addDocSeqPriv(
						const u64 &docid,
						int &iilidx,
						int &idx,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);
	bool 		removeDocSeqPriv(
					const u64 &docid,
					int &iilidx,
					int &idx,
					const bool iilmgrLocked,
					const AosRundataPtr &rdata);

private:

	bool 		insertDocid(
					const u64 &docid,
					const bool iilmgrLocked,
					const AosRundataPtr &rdata);
	bool 		expandMem();
	bool 		getFirstDocIdx(const u64 &docid, int &theidx);


	// Subiil related member functions
	bool 		splitListPriv(
					const bool iilmgrLocked,
					AosIILDocidPtr &subiil,
					const AosRundataPtr &rdata);
	bool 		initSubiil(
					u64 *docids, 
					const int numDocs,
					const int subiilid, 
					const AosIILDocidPtr &rootiil, 
					const bool iilmgrLocked); 
	bool 		addSubiil(
					const AosIILDocidPtr &crtsubiil,
					const AosIILDocidPtr &nextsubiil,
					const AosRundataPtr &rdata);
	bool 		createSubiilIndex();

	const u64 	getMinValue() const;

	const u64  	getMaxValue() const;

	bool prepareMemoryForReloading2(const bool iilmgrLocked);

	bool 		addDocPriv(
					const u64 &docid,
					const bool iilmgrLocked,
					const AosRundataPtr &rdata);

	AosIILDocidPtr	getSubiil(const u64 value, const AosRundataPtr &rdata);

//	AosIILDocidPtr	getSubiilSeq(const u64 value,int &iilidx);

	AosIILDocidPtr	getSubiilByIndex(const int idx, const AosRundataPtr &rdata);
	bool			docExistDirectPriv(const u64 &docid);
	int				getSubiilIndex(const u64 value);
	u64				getDocIdPriv1(int &idx,int &iilidx, const AosRundataPtr &rdata);

protected:
	virtual bool	saveSubIILToFileSafe(const bool forcesave, const AosRundataPtr &rdata);
	virtual void    failInSetCtntProtection();
private:
	// add by shawn	
	bool mergeSubiilPriv(const bool iilmgrLocked,const int iilidx, const AosRundataPtr &rdata);
	bool mergeSubiilForwardPriv(
			const bool iilmgrLocked,
			const int iilidx,
			const int numDocToMove,
			const AosRundataPtr &rdata);
	bool mergeSubiilBackwardPriv(
			const bool iilmgrLocked,
			const int iilidx,
			const int numDocToMove,
			const AosRundataPtr &rdata);

	bool getDocidPtr(u64*    &docidPtr,
				     const int offset);

	bool appendDocToSubiil(u64 *docids, 
						   const int numDocs,
						   const bool addToHead);
						   
	bool removeDocFromSubiil(const int numRemove, const bool delFromHead);
	bool removeSubiil(
			const int iilidx,
			const bool iilmgrLocked,
			const AosRundataPtr &rdata);

	bool removeSubiilIndex();
	int	 getSubiilIndexSeq(const u64 value,const int iilidx);

	bool			insertDocidSeq(
						const u64 &docid,
						int &iilidx,
						int &idx,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

	// Chen Ding, 05/27/2011
	virtual bool saveIILsToFileSafe(const OmnFilePtr &file, u32 &crt_pos);
public:
    void    exportIILSafe(
				   	vector<u64>			&docids,
					const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		exportIILPriv(docids,rdata);
		AOSUNLOCK(mLock);
	}
    void    exportIILSingleSafe(
				   	vector<u64>			&docids,
					const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		exportIILSinglePriv(docids,rdata);
		AOSUNLOCK(mLock);
	}
private:
	void    exportIILPriv(
				   	vector<u64>			&docids,
					const AosRundataPtr &rdata);
    void    exportIILSinglePriv(
				   	vector<u64>			&docids,
					const AosRundataPtr &rdata);

};

#endif
#endif
