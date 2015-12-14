////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Zykie Networks, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 18 Jun 2015 created by White
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BlobSE_HeaderBuffAndFileMgr_H_
#define Aos_BlobSE_HeaderBuffAndFileMgr_H_
#include "aosUtil/Types.h"
#include "BlobSE/BlobHeader.h"
#include "BlobSE/Ptrs.h"
#include "RawSE/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Thread/Ptrs.h"
#include "Util/HashUtil.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

class AosHeaderBuffAndFileMgr : public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum DefaultValues
	{
		eDirtyHeaderCacheSize		= 10000000,	//10M
		eHeaderFlushThreshold		= 10000,
	};

public:
	struct EpochUnitInfo
	{
		AosRawFilePtr		pHeaderFileForWorking;
		AosRawFilePtr		pHeaderFileForFlushing;
		AosBuffPtr			pHeaderCacheBuffForWorking;
		AosBuffPtr			pHeaderCacheBuffForFlushing;
		u64					ullHeaderFileForWorkingLen;
		u64					ullHeaderFileForFlushingLen;
		u32					ulHeaderFileForFlushingSeqno;
		u32					ulHeaderFileForWorkingSeqno;
		std::vector<u32>	vExistingSeqno;

		EpochUnitInfo& operator = (const EpochUnitInfo& r)
		{
			pHeaderFileForWorking = r.pHeaderFileForWorking;
			pHeaderFileForFlushing = r.pHeaderFileForFlushing;
			pHeaderCacheBuffForWorking = r.pHeaderCacheBuffForWorking;
			pHeaderCacheBuffForFlushing = r.pHeaderCacheBuffForFlushing;
			ullHeaderFileForWorkingLen = r.ullHeaderFileForWorkingLen;
			ullHeaderFileForFlushingLen = r.ullHeaderFileForFlushingLen;
			ulHeaderFileForFlushingSeqno = r.ulHeaderFileForFlushingSeqno;
			ulHeaderFileForWorkingSeqno = r.ulHeaderFileForWorkingSeqno;
			vExistingSeqno = r.vExistingSeqno;
			return *this;
		}
	};
	typedef std::hash_map<const u32, EpochUnitInfo, u32_hash, u32_cmp> map_t;

	struct EpochUnitInfoForRecover
	{
		AosRawFilePtr		pHeaderFile;
		AosBuffPtr			pBuff;
		u32					ulHeaderFileLen;
		u32					ulHeaderFileSeqno;
	};
	typedef std::hash_map<const u32, EpochUnitInfoForRecover, u32_hash, u32_cmp> map_recover_t;

private:
	OmnMutex*			mLockRaw;
	OmnMutexPtr			mLock;
	map_t				mEpocUnitMap;
	AosHeaderFileMgrPtr	mHeaderFileMgr;
	u32					mHeaderFileSizeThreshold;
	u32					mHeaderSaved;
	u32					mHeaderFlushThreshold;
	u32					mHeaderCacheSize;
	map_recover_t		mRecoverMap;
	u32					mCompactionTimeWindowInUnits;

public:
    AosHeaderBuffAndFileMgr(
    		AosRundata*					rdata,
    		const AosHeaderFileMgrPtr	&pHeaderFileMgr,
			const AosXmlTagPtr			&conf);

    virtual ~AosHeaderBuffAndFileMgr();

    int getEpochUnitInfoByTimestampNoLock(
    		AosRundata		*rdata,
    		const u64		timestamp,
			EpochUnitInfo	&sEpochUnitInfo);

    int saveHeaderToCacheBuff(
    		AosRundata						*rdata,
    		const AosBlobHeaderPtr			&pHeader,
    		AosBlobHeader::HeaderPointer	&sHeaderPointer);

    int switchHeaderBuffAndFile(AosRundata *rdata);

    int saveOldHeaderFileSeqnosToMetafile(
    		AosRundata				*rdata,
			const u32				ulSeqnoForFlushing,
			const u32				ulSeqnoForWorking,
			const std::vector<u32>	&vExistingSeqno);

    int appendHeaderToBuffForFlushingNoLock(
    		AosRundata						*rdata,
			const AosBlobHeaderPtr			&pHeader,
    		AosBlobHeader::HeaderPointer	&sNextHeader);

    int getTheOldestHeaderByNextHeaderPointerNoLock(
    		AosRundata						*rdata,
    		const u64						docid,
    		AosBlobHeader::HeaderPointer	&sNextHeader,
			AosBlobHeaderPtr				&pHeader);

    bool isHeaderCachedInWorkingBuff(const AosBlobHeader::HeaderPointer &sNextHeader);

    bool isHeaderInBuffToFlush(const AosBlobHeader::HeaderPointer &sNextHeader);

    char* getHeaderCacheForWorkingPointerByHeaderPointerNoLock(const AosBlobHeader::HeaderPointer &sNextHeader);

    char* getHeaderCacheForFlushingPointerByHeaderPointerNoLock(const AosBlobHeader::HeaderPointer &sNextHeader);

    bool needToFlush();

    int saveDirtyHeaderCacheIntoOldHeaderFile(AosRundata *rdata);

    int switchHeaderFileForFlushingNoLock(AosRundata *rdata);

    void dumpEpocDayMap(const char* pCaller);

	u32 getMinAvailableSeqnoOfEpochUnit(
			AosRundata	*rdata,
			const u32	ulEpochUnit);

	int delSeqnoInEpochUnitInfo(
    		AosRundata	*rdata,
			const u32	ulSeqno);

	//for disk recovery
	int allocateSpaceForRecreatedHeaders(
    		AosRundata			*rdata,
			AosBlobHeaderPtr	&pHeader);

	int saveRecreatedHeaderToBuff(
    		AosRundata			*rdata,
			AosBlobHeaderPtr	&pHeader);

	int flushRecreatedHeaders(AosRundata *rdata);

private:
    int getTheOldestHeaderInCacheBuffForWorkingNoLock(
    		const u64						docid,
    		AosBlobHeader::HeaderPointer	&sNextHeader,
			AosBlobHeaderPtr				&pHeader);

    u32 getExpirationUnitByTimestamp(const u64 timestamp);
};

#endif /* Aos_BlobSE_HeaderBuffAndFileMgr_H_ */
