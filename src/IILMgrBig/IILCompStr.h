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
#ifndef AOS_IILMgrBig_IILCompStr_h
#define AOS_IILMgrBig_IILCompStr_h

#include "IILMgrBig/IIL.h"
#include "IILUtil/IILUtil.h"
#include "SearchEngine/SeCommon.h"
#include "Util1/MemMgr.h"

#include <string.h>
#include <vector>

using namespace std;


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
	AosIILCompStr(
		const bool isPersis,
		const bool iilmgrLocked, 
		const AosRundataPtr &rdata);
	AosIILCompStr(
		const u64 &iilid, 
		const u32 siteid, 
		AosBuff &buff, 
		const bool iilmgrLocked, 
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
		AOSLOCK(mLock);
		bool rslt = addDocPriv(value, docid, value_unique, docid_unique, rdata);
//		aos_assert_rb(AosIILCompStrSanityCheck(this), mLock, false);
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
//		aos_assert_rb(AosIILCompStrSanityCheck(this), mLock, false);
		AOSUNLOCK(mLock);
		return rslt;
	}

private:
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

	virtual u32 getTotalNumDocsSafe()const
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

	bool    copyData(
				char** values, 
				u64 *docids,
				const int buff_len, 
				int &num_docs,
				const AosRundataPtr &rdata);
	
	virtual bool	isCompIIL(){return true;}
	void	setNumAlpha(const bool isnumalpha){mIsNumAlpha = isnumalpha;}
	bool	isNumAlpha() const {return mIsNumAlpha;}

protected:

	virtual bool	saveSubIILToTransFileSafe(
						const AosDocTransPtr &docTrans,
						const bool forcesave, 
						const AosRundataPtr &rdata);
	virtual bool	saveSubIILToLocalFileSafe(
						const bool forcesave, 
						const AosRundataPtr &rdata);
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
public:
	virtual bool	preQuerySafe(
						const AosQueryContextObjPtr &query_context,
						const bool iilmgrLocked, 
						const AosRundataPtr &rdata)
	{
		OmnShouldNeverComeHere;
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
				const int buff_len,
				int &num_docs);

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
	int					getSubiilIndex(const u64 &docid);
	AosIILCompStrPtr 	getSubiilByIndex(const int idx, const AosRundataPtr &rdata);

	virtual bool	firstDoc(
						int &idx, 
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
	// 	if(mIsNumAlpha)
	// 	{
	// 		int vLen = strlen(v1);
	// 		if(vLen < vaLen)
	// 		{
	// 			rslt = -1;
	// 			haverslt = true;
	// 		}
	// 		else if(vLen > vaLen)
	// 		{
	// 			rslt = 1;
	// 			haverslt = true;
	// 		}
	// 	}
	// 	if(!haverslt)
	// 	{
	// 		// Ken, 05/30/2011
	// 		if ((u32)vaLen <= AosIILUtil::eMaxStrValueLen)
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
				int &idx, 
				const int reverse, 
				const u64 &docid);

	int 	firstEQ(const int idx, const u64 &docid);
	int 	firstEQRev(const int idx, const u64 &docid);

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

	// Chen Ding, 01/29/2012
	// Taken out to AosIILUtil.h
	// inline int	valueMatch(
	// 				const char *v1,
	// 				const OmnString &value)
	// {
	// 	int vaLen = value.length();
	// 	if(mIsNumAlpha)
	// 	{
	// 		int vLen = strlen(v1);
	// 		if(vLen < vaLen) return -1;
	// 		else if(vLen > vaLen) return 1;
	// 	}
	// 	
	// 	// Ken, 05/30/2011
	// 	if ((u32)vaLen <= AosIILUtil::eMaxStrValueLen)
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
				const int idx,
				const bool changeMax,
				const bool changeMin);

	bool	expandMemoryPriv();
	bool	checkMemory() const;

	bool 	insertBefore(
				const int nn, 
				const u64 &docid, 
				const OmnString &value);

	bool 	insertAfter(
				const int nn, 
				const u64 &docid, 
				const OmnString &value);

	bool	insertDocPriv(
				u32 &idx, 
				const OmnString &value, 
				const u64 &docid, 
				const bool value_unique,
				const bool docid_unique,
				bool &keepSearch);

	bool 	splitListPriv(const bool iilmgrLocked,AosIILPtr &subiil, const AosRundataPtr &rdata);

	bool 	initSubiil(
				u64 *docids, 
				char **values, 
				const int numDocs,
				const int subiilid, 
				const AosIILCompStrPtr &rootiil, 
				const bool iilmgrLocked);

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

	AosIILCompStrPtr	getNextSubIIL(const bool &forward, const AosRundataPtr &rdata);

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
				const bool iilmgrLocked,
				const int iilidx, 
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

	bool 	appendDocToSubiil(
				char **values, 
				u64 *docids, 
				const int numDocs,
				const bool addToHead);

	bool 	removeDocFromSubiil(
				const int numDocs,
				const bool delFromHead);

	bool 	removeSubiil(
				const int iilidx,
				const bool iilmgrLocked,
				const AosRundataPtr &rdata);

	virtual bool	resetSubIILInfo(const AosRundataPtr &rdata);
	virtual bool	returnSubIILsSafe(
						const bool iilmgrLocked,
						bool &returned,
						const AosRundataPtr &rdata,
						bool const returnHeader)
	{
        AOSLOCK(mLock);
        bool rslt = returnSubIILsPriv(iilmgrLocked, returned, rdata, returnHeader);
        AOSUNLOCK(mLock);
		return rslt;		
	}
	virtual bool		returnSubIILsPriv(
							const bool iilmgrLocked,
							bool &returned, 
							const AosRundataPtr &rdata,
							const bool returnHeader);
	virtual AosIILType	getIILType() const {return eAosIILType_CompStr;}
	//virtual bool 		saveIILsToFileSafe(const OmnFilePtr &file, u32 &crt_pos, const AosRundataPtr &rdata);
private:
	virtual bool		saveSanityCheckProtected(const AosRundataPtr &rdata)
	{
		static AosIILPtr lsTestIIL = OmnNew AosIILCompStr(false,true, rdata);
		static OmnMutex lsLock;
	
		lsLock.lock();
		bool rslt = AosIIL::saveSanityCheck(lsTestIIL, rdata);
		lsLock.unlock();
		return rslt;
	}

	virtual AosBuffPtr	getBodyBuffProtected() const;

	virtual bool 		setContentsProtected(AosBuffPtr &buff, const bool, const AosRundataPtr &rdata);

	virtual bool 		prepareMemoryForReloading(const bool iilmgrLocked);

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

