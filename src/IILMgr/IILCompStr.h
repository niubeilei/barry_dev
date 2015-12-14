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
#ifndef AOS_IILMgr_IILCompStr_h
#define AOS_IILMgr_IILCompStr_h

#include "IILMgr/IIL.h"
#include "IILUtil/IILUtil.h"
#include "SearchEngine/SeCommon.h"
#include "Util1/MemMgr.h"

#include <string.h>
#include <vector>

using namespace std;


/*
#define AosIILCompStrSanityCheck(x) true
// #define AosIILStrSanityCheck(x) (x)->listSanityCheck()

class AosIILCompStr : public AosIIL
{

public:
	enum
	{
		eMinStrLen = 5,
	};

private:
	char **				mValues;

	// Member data used to index sub-iils. Only the first
	// IIL will use them. 
	u64*				mMinDocids;
	u64*				mMaxDocids;
	AosIILCompStrPtr *	mSubiils;
	bool				mIsNumAlpha;

public:
	AosIILCompStr();
	AosIILCompStr(
		const u64 &iilid, 
		const u32 siteid, 
		AosBuffPtr &header_buff,		//Ketty 2012/11/15 
		const AosRundataPtr &rdata);
	AosIILCompStr(
		const u64 &wordid, 
		const u64 &iilid,
		const bool isPersis,
		const OmnString &iilname, 
		const bool iilmgrlocked, 
		const AosRundataPtr &rdata);

	~AosIILCompStr();

	void	setNumAlpha(){mIsNumAlpha = true;}

	virtual bool	querySafe(
						const AosQueryRsltObjPtr& query_rslt,
						const AosBitmapObjPtr& query_bitmap,
		 				const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata)
	{
		OmnShouldNeverComeHere;
		return false;
	}

	bool	addDocSafe(
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const AosRundataPtr &rdata)
	{
		// Ketty 2013/01/15
		OmnNotImplementedYet;
		return false;
		//AOSLOCK(mLock);
		//bool rslt = addDocPriv(value, docid, value_unique, docid_unique, rdata);
//		aos_assert_rb(AosIILCompStrSanityCheck(this), mLock, false);
		//AOSUNLOCK(mLock);
		//return rslt;
	}

	bool	removeDocSafe(
				const OmnString &value, 
				const u64 &docid, 
				const AosRundataPtr &rdata)
	{
		// Ketty 2013/01/15
		OmnNotImplementedYet;
		return false;
		//AOSLOCK(mLock);
		//bool rslt = removeDocPriv(value, docid, rdata);
//		aos_assert_rb(AosIILCompStrSanityCheck(this), mLock, false);
		//AOSUNLOCK(mLock);
		//return rslt;
	}

	virtual bool	deleteIILSafe(const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = deleteIILPriv(rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

private:
	virtual bool	deleteIILPriv(
						const AosRundataPtr &rdata);
	virtual bool	deleteIILSinglePriv(
						const AosRundataPtr &rdata);
	virtual bool	checkDocSafe(
						const u64 &docid,
						const OmnString &value,
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

	bool    copyData(
				char** values, 
				u64 *docids,
				const i64 &buff_len, 
				i64 &num_docs,
				const AosRundataPtr &rdata);
	
	virtual bool	isCompIIL(){return true;}
	void	setNumAlpha(const bool isnumalpha){mIsNumAlpha = isnumalpha;}
	bool	isNumAlpha() const {return mIsNumAlpha;}

protected:
	// Ketty 2012/11/19
	//virtual bool	saveSubIILToTransFileSafe(
	//					const AosDocTransPtr &docTrans,
	//					const bool forcesave, 
	//					const AosRundataPtr &rdata);
	virtual bool	saveSubIILToLocalFileSafe(
						const bool forcesave, 
						const AosRundataPtr &rdata);

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
private:
	bool			queryNewPriv(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);

	bool    copyDataSingle(
				char** values, 
				u64 *docids, 
				const i64 &buff_len,
				i64 &num_docs);

	bool	checkDocSingleSafe(
				const u64 &docid,
				const OmnString &value,
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
				const OmnString &value,
				const AosOpr opr,
				const AosRundataPtr &rdata);

	bool	checkDocSinglePriv(
				const u64 &docid,
				const OmnString &value,
				const AosOpr opr,
				bool &keepSearch,
				const AosRundataPtr &rdata);

	AosIILCompStrPtr 	getSubiil(const u64 &docid, const AosRundataPtr &rdata);
	i64				getSubiilIndex(const u64 &docid);
	AosIILCompStrPtr 	getSubiilByIndex(const i64 &idx, const AosRundataPtr &rdata);

	virtual bool	firstDoc(
						i64 &idx, 
						const bool reverse, 
						const u64 &docid);
	
	// Chen Ding, 01/29/2012
	// Taken out to AosIILUtil.h
	// inline bool 	valueMatch(
	// 					const char *v1, 
	// 					const AosOpr opr, 
	// 					const OmnString &value)
	// {
	// 	int rslt;
	// 	int vaLen = value.length();
	// 	
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
	// 		}
	// 		else
	// 		{
	// 			rslt = strncmp(v1, value.data(), AosIILUtil::eMaxStrValueLen);
	// 		}
	// 	}
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

	bool	nextDocidEQ(
				i64 &idx, 
				const int reverse, 
				const u64 &docid);

	i64	firstEQ(const i64 &idx, const u64 &docid);
	i64	firstEQRev(const i64 &idx, const u64 &docid);

	bool 	addDocPriv(
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				const AosRundataPtr &rdata);

	bool 	addDocDirectSafe(
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

	bool	addDocDirectPriv(
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique, 
				const bool docid_unique, 
				bool &keepSearch,
				const AosRundataPtr &rdata);

	bool	setValue(const i64 &idx, const char *value, const int length)
	{
		// Only 'AosIILUtil::eMaxStrValueLen' is stored
		aos_assert_r(length >= 0, false);
		int len = (length > AosIILUtil::eMaxStrValueLen) ? AosIILUtil::eMaxStrValueLen : length;
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
	// 	
	// 	OmnShouldNeverComeHere;
	// 	return 0;
	// }

	bool 	updateIndexData(
				const i64 &idx,
				const bool changeMax,
				const bool changeMin);

	bool	expandMemoryPriv();
	bool	checkMemory() const;

	bool 	insertBefore(
				const i64 &nn, 
				const u64 &docid, 
				const OmnString &value);

	bool 	insertAfter(
				const i64 &nn, 
				const u64 &docid, 
				const OmnString &value);

	bool	insertDocPriv(
				i64 &idx, 
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique,
				const bool docid_unique,
				bool &keepSearch);

	bool 	splitListPriv(AosIILObjPtr &subiil, const AosRundataPtr &rdata);

	bool 	initSubiil(
				u64 *docids, 
				char **values, 
				const i64 &numDocs,
				const i64 &subiilid, 
				const AosIILCompStrPtr &rootiil); 

	bool 	addSubiil(
				const AosIILCompStrPtr &crtsubiil,
				const AosIILCompStrPtr &nextsubiil);

	bool 	createSubiilIndex();

	bool	removeDocDirectSafe(
				const OmnString &value, 
				const u64 &docid,
				bool &keepSearch, 
				const AosRundataPtr &rdata)
	{
		AOSLOCK(mLock);
		bool rslt = removeDocDirectPriv(value, docid,keepSearch, rdata);
		aos_assert_rb(AosIILCompStrSanityCheck(this), mLock, false);
		AOSUNLOCK(mLock);
		return rslt;
	}

	AosIILCompStrPtr	getNextSubIIL(const bool forward, const AosRundataPtr &rdata);

	bool	removeDocPriv(
				const OmnString &value, 
				const u64 &docid,
				const AosRundataPtr &rdata); 

	bool	removeDocDirectPriv(
				const OmnString &value, 
				const u64 &docid,
				bool &keepSearch,
				const AosRundataPtr &rdata); 

	bool 	mergeSubiilPriv(
				const i64 &iilidx, 
				const AosRundataPtr &rdata);

	bool 	mergeSubiilForwardPriv(
				const i64 &iilidx,
				const i64 &numDocToMove, 
				const AosRundataPtr &rdata);
	
	bool 	mergeSubiilBackwardPriv(
				const i64 &iilidx,
				const i64 &numDocToMove, 
				const AosRundataPtr &rdata);

	bool	getValueDocidPtr(
				char ** &valuePtr,
				u64*    &docidPtr,
				const i64 &offset);

	bool 	appendDocToSubiil(
				char **values, 
				u64 *docids, 
				const i64 &numDocs,
				const bool addToHead);

	bool 	removeDocFromSubiil(
				const i64 &numDocs,
				const bool delFromHead);

	bool 	removeSubiil(
				const i64 &iilidx,
				const AosRundataPtr &rdata);

	virtual bool	resetSubIILInfo(const AosRundataPtr &rdata);
	virtual bool	returnSubIILsSafe(
						bool &returned,
						const AosRundataPtr &rdata,
						bool const returnHeader)
	{
        AOSLOCK(mLock);
        bool rslt = returnSubIILsPriv(returned, rdata, returnHeader);
        AOSUNLOCK(mLock);
		return rslt;		
	}
	virtual bool		returnSubIILsPriv(
							bool &returned, 
							const AosRundataPtr &rdata,
							const bool returnHeader);
	virtual AosIILType	getIILType() const {return eAosIILType_CompStr;}
	//virtual bool 		saveIILsToFileSafe(const OmnFilePtr &file, u32 &crt_pos, const AosRundataPtr &rdata);
private:
	virtual bool		saveSanityCheckProtected(const AosRundataPtr &rdata)
	{
		static AosIILObjPtr lsTestIIL = OmnNew AosIILCompStr(false,true, rdata);
		static OmnMutex lsLock;
	
		lsLock.lock();
		bool rslt = AosIIL::saveSanityCheck(lsTestIIL, rdata);
		lsLock.unlock();
		return rslt;
	}

	virtual AosBuffPtr	getBodyBuffProtected() const;

	virtual bool 		setContentsProtected(AosBuffPtr &buff, const AosRundataPtr &rdata);

	virtual bool 		prepareMemoryForReloading();

	virtual u64			getMinDocid() const;

	virtual u64			getMaxDocid() const;

	virtual bool		resetSpec();


	virtual bool 		sanityTestForSubiils()
	{
		return true;	
	}

	bool				splitSanityCheck()
	{
		return true;	
	}

public:
};
*/
#endif

