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
//	this class is for the headers, implemented as a kind of raw file that
//	has special format
// Modification History:
// 2014-12-19 created by White
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BlobSE_HeaderFile_H_
#define Aos_BlobSE_HeaderFile_H_
#include "BlobSE/BlobHeader.h"
#include "BlobSE/Ptrs.h"
#include "RawSE/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Util/HashUtil.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

using std::u64_hash;
using std::u64_cmp;

class AosHeaderFileMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum DefaultValues
	{
		eHeaderFileSeqnoGuard		= 0,	//header file's seqno must not be 0
		eHeadersPerFile				= 10000000,
		eOldHeaderFileSizeThreshold	= 10000000,
		eCompactionTimeUnitInSec	= 3600 * 24	//one day
	};
	typedef std::vector<AosBlobHeaderPtr>						vec_t;
	typedef std::hash_map<const u64, vec_t, u64_hash, u64_cmp>	map_header_t;

private:
	u32				mSiteID;
	u32				mCubeID;
	u64				mAseID;
	u32				mHeadersPerFile;
	u32				mOldHeaderFileSizeThreshold;
	u32				mHeaderInCacheAndHeaderFileSize;
	u32				mHeaderCustomDataSize;
	u32				mCompactionTimeUnitInSec;
	AosRawSE*		mRawSE;
	AosRawFilePtr	mCurrentOldHeaderFile;

public:
    AosHeaderFileMgr(
    		AosRundata			*rdata,
    		AosRawSE			*rawse,
    		const AosXmlTagPtr	&conf,
    		const u32			header_size,
			const u32			header_custom_data_size);
    ~AosHeaderFileMgr();

    AosBlobHeaderPtr readNormalHeader(
			AosRundata	*rdata,
    		const u64	docid,
			bool		bShowAlarm = true);

    AosBlobHeaderPtr readOldHeader(
    		AosRundata							*rdata,
    		const AosBlobHeader::HeaderPointer&	sNextHeader);

    int overwriteOldHeader(
    		AosRundata							*rdata,
			const AosBlobHeaderPtr				&header,
			const AosBlobHeader::HeaderPointer&	sNextHeader);

    int overwriteNormalHeaderFile(
    		AosRundata		*rdata,
			const u32		ulNormalHeaderFileSeqno,
    		AosBuffPtr&		pBuff);

    bool flushChangeLogToFile(
			AosRundata		*rdata,
			AosBuffArray	*buff,
			i64				start,
			i64				end);

    u32 getNumOfHeaderPerFile() const {return mHeadersPerFile;};

    u32 getNormalHeaderFileSeqnoByDocid(const u64 docid);

    u32 getNormalHeaderFileSize();

    u64 getEndDocidOfNormalHeaderFileByDocid(const u64 docid);

    u64 getNormalHeaderFileRawfidByDocid(const u64 docid);

    u64 getStartDocidOfNormalHeaderFileByDocid(const u64 docid);

    u32 getFirstOldHeaderFileSeqnoByEpochUnit(const u32 epochUnit);

    int serializeHeaderToNormalHeaderFileBuff(
			AosBlobHeaderPtr	&pHeader,
			AosBuffPtr			&pNormalHeaderFileBuff);

    AosBlobHeaderPtr unserializeHeaderFromNormalHeaderFileBuff(
    		const u64	docid,
    		AosBuffPtr	&pNormalHeaderFileBuff);

    AosBuffPtr readNormalHeaderFile(
    		AosRundata	*rdata,
    		const u32 	ullNormalHeaderFileSeqno);

    int saveOldHeaderFileSeqnosToMetafile(
    		AosRundata				*rdata,
    		const u32				ulSeqnoForFlushing,
    		const u32				ulSeqnoForWorking,
    		const std::vector<u32>	&vExistingSeqno);

    int getOldHeaderFileSeqnosByEpochUnitFromMetafile(
    		AosRundata			*rdata,
			const u32			ulEpochUnit,
    		u32					&ulSeqnoForFlushing,
    		u32					&ulSeqnoForWorking,
			std::vector<u32>	&vExistingSeqno);

    AosRawFilePtr getOldHeaderFileBySeqno(
    		AosRundata	*rdata,
    		const u32	ulSeqno);

    bool oldHeaderFileExists(
    		AosRundata	*rdata,
    		const u32	ulSeqno);

	u32 getEpochUnitByTimestamp(const u64 ullTimestamp);

	u32 getEpochUnitBySeqno(const u32 ulSeqno);

	u64 getMetafileRawfidByTimestamp(const u64 timestamp);

	u64 getOldHeaderFileRawfidBySeqno(const u32 seqno);

	u32 getOldHeaderFileSeqnoByRawfid(const u64 rawfid);

	u64 getMetafileRawfidByEpochUnit(const u32 ulEpochUnit);

	u32 getMinAvailableSeqnoOfEpochUnit(
			const u32				ulEpochUnit,
			const u32				ulSeqnoForFlushing,
			const u32				ulSeqnoForWorking,
			const std::vector<u32>	&vExistingSeqno);

	u64 getNormalHeaderOffsetByDocid(const u64 docid);

	u64 getNormalHeaderFileRawfidBySeqno(const u32 seqno);

	bool isOldHeaderFileMetaFileSeqno(const u32 seqno);

	int recoverOldHeader(
			AosRundata			*rdata,
			AosBlobHeaderPtr	&pHeader);

private:
	u32 getMetafileSeqnoByTimestamp(const u64 timestamp);

	u64 getMetafileRawfidBySeqno(const u32 seqno);

	u32 getMetafileSeqnoByEpochUnit(const u32 ulEpochUnit);

	AosBlobHeaderPtr readHeaderFromFile(
			AosRundata	*rdata,
			const u64	rawfid,
			const u64	offset,
			bool		bShowAlarm = true);

	int switchOldHeaderFile(AosRundata *rdata);

	bool openLastOldHeaderFile(AosRundata *rdata);

	int constructHeaderLinkListFromHCL(
			AosRundata		*rdata,
			map_header_t&	map,
			AosBuffArray	*buff,
			i64				start,
			i64				end);

	int constructHeaderLinkListFromNormalHeaderFile(
			AosRundata		*rdata,
			map_header_t&	map,
			AosBuffPtr		pNormalHeaderFileBuff);

	int saveOldHeadersToFile(
			AosRundata	*rdata,
			vec_t&		vec,
			u64&		rawfid,
			u64&		offset);
};
#endif /* Aos_BlobSE_HeaderFile_H_ */
