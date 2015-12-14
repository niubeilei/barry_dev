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
// 2014-12-19 created by White
////////////////////////////////////////////////////////////////////////////
#include "BlobSE/BlobHeader.h"

#include "BlobSE/BlobSE.h"
#include "BlobSE/Entry.h"
#include "BlobSE/HeaderFileMgr.h"
#include "BlobSE/RawfidRange.h"
#include "RawSE/MediaType.h"
#include "RawSE/RawSE.h"
#include "Util/BuffArray.h"

AosHeaderFileMgr::AosHeaderFileMgr(
		AosRundata			*rdata,
		AosRawSE			*rawse,
		const AosXmlTagPtr	&conf,
		const u32			header_size,
		const u32			header_custom_data_size)
:
mHeaderInCacheAndHeaderFileSize(header_size),
mHeaderCustomDataSize(header_custom_data_size)
{
	if (!rawse)
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	mRawSE = rawse;
	AosXmlTagPtr config = conf->getFirstChild("BlobSE", true);
	if (config.isNull())
	{
		rdata->setErrmsg("can not find config tag named BlobSE");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	mSiteID = config->getAttrU32("siteid", AosBlobSE::eSiteID);
	if (AosBlobSE::eSiteID == mSiteID)
	{
		OmnScreen << "site id set to default value:" << mSiteID
				<< ", please make sure this is what you want." << endl;
	}
	mCubeID = config->getAttrU32("cubeid", AosBlobSE::eCubeID);
	if (AosBlobSE::eCubeID == mCubeID)
	{
		OmnScreen << "cube id set to default value:" << mCubeID
				<< ", please make sure this is what you want." << endl;
	}
	mAseID = config->getAttrU32("aseid", AosBlobSE::eAseID);
	if (AosBlobSE::eAseID == mAseID)
	{
		OmnScreen << "ase id set to default value:" << mAseID
				<< ", please make sure this is what you want." << endl;
	}
	config = conf->getFirstChild("HeaderFile", true);
	if (config.isNull())
	{
		rdata->setErrmsg("can not find config tag named HeaderFile");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	mHeadersPerFile = config->getAttrU32("headers_per_file", eHeadersPerFile);
	if (eHeadersPerFile == mHeadersPerFile)
	{
		OmnScreen << "mHeadersPerFile set to default value:" << mHeadersPerFile
				<< ", please make sure this is what you want." << endl;
	}
	mOldHeaderFileSizeThreshold = config->getAttrU32("old_header_file_size", eOldHeaderFileSizeThreshold);
	if (eHeadersPerFile == mOldHeaderFileSizeThreshold)
	{
		OmnScreen << "mOldHeaderFileLenThreshold set to default value:" << mOldHeaderFileSizeThreshold
				<< ", please make sure this is what you want." << endl;
	}
	mCompactionTimeUnitInSec = config->getAttrU32("compaction_time_unit_in_sec", eCompactionTimeUnitInSec);
	if (eCompactionTimeUnitInSec == mCompactionTimeUnitInSec)
	{
		OmnScreen << "mCompactionTimeUnitInSec set to default value:" << mCompactionTimeUnitInSec
				<< ", please make sure this is what you want." << endl;
	}
	bool rslt = openLastOldHeaderFile(rdata);
	if (!rslt)
	{
		rdata->setErrmsg("openLastOldHeaderFile failed.");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosHeaderFileMgr::~AosHeaderFileMgr()
{
    // TODO Auto-generated destructor stub
}


AosBlobHeaderPtr
AosHeaderFileMgr::readNormalHeader(
		AosRundata	*rdata,
		const u64	docid,
		bool		bShowAlarm)
{
	// This function reads the header for 'docid' from the file.
	u64 rawfid = getNormalHeaderFileRawfidByDocid(docid);
	u64 offset = getNormalHeaderOffsetByDocid(docid);
	OmnScreen << "docid:" << docid << " mHeadersPerFile:" << mHeadersPerFile << " header file rawfid:"
			<< rawfid << " offset:" << offset << " mHeaderInCacheAndHeaderFileSize:" << mHeaderInCacheAndHeaderFileSize << endl;
//	if (mRawSE->fileExists(rdata, mSiteID, mCubeID, mAseID, rawfid))
//	{
		AosBlobHeaderPtr pHeader = readHeaderFromFile(rdata, rawfid, offset, bShowAlarm);
		if (pHeader.isNull())
		{
			if (bShowAlarm)
			{
				OmnAlarm << "readHeaderFromFile failed for docid:" << docid
						<< " @rawfid:" << rawfid << " offset:" << offset << enderr;
			}
			else
			{
				OmnScreen << "readHeaderFromFile failed for docid:" << docid
						<< " @rawfid:" << rawfid << " offset:" << offset << endl;
			}
			return NULL;
		}
		pHeader->setDocid(docid);
		return pHeader;
//	}
//	else
//	{
//		return NULL;
//	}
}


AosBlobHeaderPtr
AosHeaderFileMgr::readOldHeader(
		AosRundata							*rdata,
		const AosBlobHeader::HeaderPointer&	sNextHeader)
{
	if (AosBlobHeader::noNextHeader == sNextHeader)
	{
		return NULL;
	}
	u64 rawfid = getOldHeaderFileRawfidBySeqno(sNextHeader.ulSeqno);
	if (0 == rawfid)
	{
		return NULL;
	}
	if (mRawSE->fileExists(rdata, mSiteID, mCubeID, mAseID, rawfid))
	{
		AosBlobHeaderPtr pHeader = readHeaderFromFile(rdata, rawfid, sNextHeader.ulOffset);
		if (pHeader.isNull())
		{
			OmnAlarm << "readHeaderFromFile failed @rawfid:" << rawfid << " offset:" << sNextHeader.ulOffset << enderr;
			return NULL;
		}
		return pHeader;
	}
	else
	{
		return NULL;
	}
}


AosBlobHeaderPtr
AosHeaderFileMgr::readHeaderFromFile(
		AosRundata	*rdata,
		const u64	rawfid,
		const u64	offset,
		bool		bShowAlarm)
{
	AosBlobHeaderPtr pHeader = OmnNew AosBlobHeader(0, mHeaderCustomDataSize);
	if (pHeader.isNull())
	{
		OmnAlarm << "create header obj failed in readHeaderFromFile, rawfid:"
				<< rawfid << " offset:" << offset << enderr;
		return NULL;
	}
	AosBuffPtr buff = OmnNew AosBuff(mHeaderInCacheAndHeaderFileSize AosMemoryCheckerArgs);
	if (buff.isNull())
	{
		OmnAlarm << "create buff obj failed in readHeaderFromFile, rawfid:"
				<< rawfid << " offset:" << offset << enderr;
		return NULL;
	}
	int iRet = mRawSE->readRawFile(rdata, mSiteID, mCubeID, mAseID,
			rawfid, offset, mHeaderInCacheAndHeaderFileSize, buff, AosRawFile::eReadWrite, bShowAlarm);
	if (0 != iRet)
	{
		if (bShowAlarm)
		{
			OmnAlarm << "readRawFile failed on rawfid:" << rawfid << " offset:" << offset << enderr;
		}
		else
		{
			OmnScreen << "readRawFile failed on rawfid:" << rawfid << " offset:" << offset << endl;
		}
		return NULL;
	}
	if (buff.isNull())
	{
		if (bShowAlarm)
		{
			OmnAlarm << "something is wrong with mRawSE->readRawFile, it returns a null buff."
					<< " sited:" << mSiteID << " cubeid:" << mCubeID << " aseid:" << mAseID
					<< " rawfid:" << rawfid << " read len:" << mHeaderInCacheAndHeaderFileSize << enderr;
		}
		else
		{
			OmnScreen << "something is wrong with mRawSE->readRawFile, it returns a null buff."
					<< " sited:" << mSiteID << " cubeid:" << mCubeID << " aseid:" << mAseID
					<< " rawfid:" << rawfid << " read len:" << mHeaderInCacheAndHeaderFileSize << endl;
		}
		return NULL;
	}
	iRet = pHeader->unserializeFromCacheAndHeaderFileBuff(buff->data());
	if (0 != iRet)
	{
		if (bShowAlarm)
		{
			OmnAlarm << "unserializeFromHeaderFileBuff failed on rawfid:" << rawfid
					<< " offset:" << offset << ", iRet=" << iRet << enderr;
		}
		else
		{
			OmnScreen << "unserializeFromHeaderFileBuff failed on rawfid:" << rawfid
					<< " offset:" << offset << ", iRet=" << iRet << endl;
		}
		return NULL;
	}
	return pHeader;
}


bool
AosHeaderFileMgr::flushChangeLogToFile(
		AosRundata		*rdata,
		AosBuffArray	*buff,
		i64				start,
		i64				end)
{
	/*
	// this function save the headers in buff to the file they belong, starts from start to end
	aos_assert_rr(buff, rdata, false);
	map_header_t mapHeader;		//TODO:use std::vector<AosBlobHeaderPtr>[] array
	mapHeader.clear();
	int iRet = constructHeaderLinkListFromHCL(rdata, mapHeader, buff, start, end);
	if (0 != iRet)
	{
		OmnAlarm << "constructHeaderLinkListFromHCL failed" << enderr;
		return false;
	}
	AosBlobHeaderPtr pHeader = OmnNew AosBlobHeader(0, mHeaderCustomDataSize);
	if (!pHeader)
	{
		OmnAlarm << "create AosBlobHeader obj failed for flushing change log to file." << enderr;
		return false;
	}
	HeaderChangeLogEntry* pHeaderChangeLogEntry = (HeaderChangeLogEntry*)buff->getEntry(start);
	u64	start_docid = pHeaderChangeLogEntry->ullDocID;
	u64 rawfid = getNormalHeaderFileRawfidByDocid(start_docid);
	i64 illHeaderFileSize = pHeader->getHeaderInCacheAndHeaderFileLength() * mHeadersPerFile;	//i64 for file_buff->setDataLen
	AosBuffPtr pNormalHeaderFileBuff = readNormalHeaderFile(rdata, rawfid);
	if(pNormalHeaderFileBuff.isNull())
	{
		OmnAlarm << "readNormalHeaderFile failed for rawfid:" << rawfid << enderr;
		return false;
	}
	iRet = constructHeaderLinkListFromNormalHeaderFile(rdata, mapHeader, pNormalHeaderFileBuff);
	if (0 != iRet)
	{
		OmnAlarm << "constructHeaderLinkListFromNormalHeaderFile failed" << enderr;
		return false;
	}
	char* pBuff = 0;
	u64 header_file_start_docid = getStartDocidOfNormalHeaderFileByDocid(start_docid);
	u64 ullOldHeaderFilerawfid = mCurrentOldHeaderFile->getRawFileID();
	u64 offset = 0;
	for (map_header_t::iterator it = mapHeader.begin();it !=mapHeader.end();it++)
	{
		iRet = saveOldHeadersToFile(rdata, it->second, ullOldHeaderFilerawfid, offset);
		if (0 != iRet)
		{
			OmnAlarm << "saveOldHeadersToFile failed for docid:" << (*(it->second.begin()))->getDocid() <<  enderr;
			return false;
		}
		(*(it->second.begin()))->setNextHeader(ullOldHeaderFilerawfid, offset);

		pBuff = pNormalHeaderFileBuff->data() +
				((*(it->second.begin()))->getDocid() - header_file_start_docid) * mHeaderInCacheAndHeaderFileSize;
		iRet = pHeader->serializeToCacheAndHeaderFileBuff(pBuff);
		if (0 != iRet)
		{
			OmnAlarm << "serializeToFileBuff failed for docid:" << (*(it->second.begin()))->getDocid() <<  enderr;
			return false;
		}
	}
	iRet = mRawSE->overwriteRawFile(rdata, mSiteID, mCubeID, mAseID,
			rawfid, pNormalHeaderFileBuff->data(), illHeaderFileSize);
	if (0 != iRet)
	{
//		AosLogError(rdata, false, AosErrmsgId::eWriteHeaderFile)
//			<< AosFieldName::eRawFileId << rawfid
//			<< AosFieldName::eReturnValue << iRet << enderr;
		OmnAlarm << "overwriting failed on rawfile with rawfid:" << rawfid << " iRet=" << iRet << enderr;
		return false;
	}
	*/
	return true;
}


u32
AosHeaderFileMgr::getNormalHeaderFileSeqnoByDocid(const u64 docid)
{
	if (0 == docid)
	{
		return 0;
	}
	return (docid - 1) / mHeadersPerFile + 1;
}



u64
AosHeaderFileMgr::getNormalHeaderFileRawfidByDocid(const u64 docid)
{
	if (0 == docid)
	{
		return 0;
	}
	return eHeaderFileID_start + getNormalHeaderFileSeqnoByDocid(docid);
}


u64
AosHeaderFileMgr::getNormalHeaderOffsetByDocid(const u64 docid)
{
	if (0 == docid)
	{
		return 0;
	}
	return (docid - 1) % mHeadersPerFile * mHeaderInCacheAndHeaderFileSize;
}


u64
AosHeaderFileMgr::getStartDocidOfNormalHeaderFileByDocid(const u64 docid)
{
	if (0 == docid)
	{
		return 0;
	}
	return (docid - 1) / mHeadersPerFile * mHeadersPerFile + 1;
}


u32
AosHeaderFileMgr::getFirstOldHeaderFileSeqnoByEpochUnit(const u32 epochUnit)
{
	return getMetafileSeqnoByEpochUnit(epochUnit) + eOHFIDsReservedForEachUnit;
}


u64
AosHeaderFileMgr::getEndDocidOfNormalHeaderFileByDocid(const u64 docid)
{
	if (0 == docid)
	{
		return 0;
	}
	return getStartDocidOfNormalHeaderFileByDocid(docid) + mHeadersPerFile - 1;
}


int
AosHeaderFileMgr::constructHeaderLinkListFromHCL(		//sorted by timestamp,
		AosRundata		*rdata,
		map_header_t&	map,
		AosBuffArray	*buff,
		i64				start,
		i64				end)
{
	HeaderChangeLogEntry* pHeaderChangeLogEntry = (HeaderChangeLogEntry*)buff->getEntry(start);
	u64	start_docid = pHeaderChangeLogEntry->ullDocID;
	aos_assert_rr(0 != start_docid, rdata, -1);
	pHeaderChangeLogEntry = (HeaderChangeLogEntry*)buff->getEntry(end);
	u64	end_docid = pHeaderChangeLogEntry->ullDocID;
	//start_docid and end_docid must be within the same header file
	aos_assert_rr(getEndDocidOfNormalHeaderFileByDocid(start_docid) >= end_docid, rdata, -2);
	int iRet = 0;
	map_header_t::iterator it = map.begin();
	for (i64 pos = start;pos <= end; pos++)
	{
		AosBlobHeaderPtr pHeader = OmnNew AosBlobHeader(0, mHeaderCustomDataSize);
		iRet = pHeader->unserializeFromACLandHCLBuff(buff->getEntry(pos));
		if (0 != iRet)
		{
			OmnAlarm << "unserializeFromChangeLogBuff failed" << enderr;
			return -3;
		}
		map[pHeader->getDocid()].push_back(pHeader);	//TODO:
	}
	for (it = map.begin();it != map.end();it++)
	{
		std::sort(it->second.begin(), it->second.end(), AosBlobHeader::compareTimestamp);
	}
	return 0;
}


int
AosHeaderFileMgr::saveOldHeadersToFile(
		AosRundata	*rdata,
		vec_t&		vec,	//this vector should contain the newest N headers, N >= 2
		u64&		rawfid,
		u64&		offset)
{
//	if (2 > vec.size())
//	{
//		OmnAlarm << "2 > vec.size():" << vec.size() << enderr;
//		return -1;
//	}
//	AosBuffPtr pOldHeaderFileBuff = OmnNew AosBuff((vec.size() - 1) * ((*vec.begin())->getHeaderInCacheAndHeaderFileLength()));
//	if (pOldHeaderFileBuff.isNull())
//	{
//		OmnAlarm << "create old header file buff failed for docid:" << (*vec.begin())->getDocid() << enderr;
//		return -2;
//	}
//	int iRet = 0;
//	if (mCurrentOldHeaderFile->getLength() > mOldHeaderFileSizeThreshold)
//	{
//		iRet = switchOldHeaderFile(rdata);
//		if (0 != iRet)
//		{
//			OmnAlarm << "switchOldHeaderFile failed, iRet=" << iRet << enderr;
//			return -3;
//		}
//		rawfid = mCurrentOldHeaderFile->getRawFileID();
//	}
//	char* p = pOldHeaderFileBuff->data();
//	u64 ullBuffOffset = 0;
//	vec_t::iterator it_last = vec.end();
//	it_last--;
//	for (vec_t::iterator it = vec.begin();it != it_last;it++)
//	{
//		(*it)->setNextHeader(rawfid, offset + ullBuffOffset + (*it)->getHeaderInCacheAndHeaderFileLength());
//		iRet = (*it)->serializeToCacheAndHeaderFileBuff(p + ullBuffOffset);
//		if (0 != iRet)
//		{
//			OmnAlarm << "serializeToHeaderFileBuff failed for docid:" << (*it)->getDocid() << enderr;
//			return -4;
//		}
//		ullBuffOffset += (*it)->getHeaderInCacheAndHeaderFileLength();
//	}
//	iRet = (*it_last)->serializeToCacheAndHeaderFileBuff(p + ullBuffOffset);
//	if (0 != iRet)
//	{
//		OmnAlarm << "serializeToHeaderFileBuff failed for docid:" << (*it_last)->getDocid() << enderr;
//		return -5;
//	}
//	iRet = mCurrentOldHeaderFile->append(rdata, pOldHeaderFileBuff->data(), pOldHeaderFileBuff->buffLen());
//	if (0 != iRet)
//	{
//		OmnAlarm << "mCurrentOldHeaderFile->append failed, iRet=" << iRet << enderr;
//		return -6;
//	}
	return 0;
}


AosBuffPtr
AosHeaderFileMgr::readNormalHeaderFile(
		AosRundata	*rdata,
		const u32 	ullNormalHeaderFileSeqno)	//i64 for pBuff->setDataLen
{
	u64 rawfid = getNormalHeaderFileRawfidBySeqno(ullNormalHeaderFileSeqno);
	if (!mRawSE->fileExists(rdata, mSiteID, mCubeID, mAseID, rawfid))
	{
		return NULL;
	}
	AosBuffPtr pBuff = OmnNew AosBuff(getNormalHeaderFileSize() AosMemoryCheckerArgs);
	if (pBuff.isNull())
	{
		OmnAlarm << "create buff obj for normal header file failed, rawfid:" << rawfid << enderr;
		return NULL;
	}
	memset(pBuff->data(), 0, pBuff->buffLen());
	int iRet = mRawSE->readRawFile(rdata, mSiteID, mCubeID, mAseID,
			rawfid, 0, getNormalHeaderFileSize(), pBuff, AosRawFile::eModifyOnly);	//overwriteRawFile will open the file as eModifyOnly
	if (0 != iRet)
		//TODO:for now, we just create the header file if it does not exist, judging by the result of reading
	{
//		AosRawFilePtr raw_file = mRawSE->createRawFile(rdata, mSiteID, mCubeID, mAseID,
//				rawfid, AosMediaType::eDisk, AosRawFile::eModifyOnly);	//overwriteRawFile will open the file as eModifyOnly
//		if (raw_file.isNull())
//		{
//			OmnAlarm << "header file creation failed on rawfid:"
//					<< rawfid << enderr;
//			return NULL;
//		}
//		pBuff->setDataLen(getNormalHeaderFileSize());
//		memset(pBuff->data(), 0, pBuff->buffLen());
		OmnAlarm << "mRawSE->readRawFile failed, iRet=" << iRet << " siteid:" << mSiteID
				<< " cubeid:" << mCubeID << " aseid:" << mAseID << " rawfid:" << rawfid << enderr;
		return NULL;
	}
	return pBuff;
}


int
AosHeaderFileMgr::constructHeaderLinkListFromNormalHeaderFile(
		AosRundata		*rdata,
		map_header_t&	map,
		AosBuffPtr		pNormalHeaderFileBuff)
{
	map_header_t::iterator it = map.begin();
	int iRet = 0;
	for (;it != map.end();it++)
	{
		AosBlobHeaderPtr ptemHeader = OmnNew AosBlobHeader(0, mHeaderCustomDataSize);
		iRet = ptemHeader->unserializeFromCacheAndHeaderFileBuff(pNormalHeaderFileBuff->data() +
			((*(it->second.begin()))->getDocid() - getStartDocidOfNormalHeaderFileByDocid((*(it->second.begin()))->getDocid()))
			* (*(it->second.begin()))->getHeaderInCacheAndHeaderFileLength());
		if (0 != iRet)
		{
			OmnAlarm << "unserializeFromHeaderFileBuff failed for docid:" << (*(it->second.begin()))->getDocid() << enderr;
			return -1;
		}
		vec_t::reverse_iterator rit = it->second.rbegin();
		if ((*rit)->getTimestamp() > ptemHeader->getTimestamp())	//normal header on disk is older than headers in list
		{
			it->second.push_back(ptemHeader);
		}
		//normal header should be within the list otherwise
	}
	return 0;
}


int
AosHeaderFileMgr::switchOldHeaderFile(AosRundata *rdata)
{
//	u64 rawfid = mCurrentOldHeaderFile->getRawFileID() + 1;
//	if (eOldHeaderFileID_end < rawfid)
//	{
//		OmnAlarm << "rawfid:" << rawfid << " exceeds boundary:" << (u64)eOldHeaderFileID_end << enderr;
//		return -1;
//	}
//	mCurrentOldHeaderFile = mRawSE->createRawFile(rdata, mSiteID, mCubeID, mSiteID,
//			rawfid, AosMediaType::eDisk, AosRawFile::eModifyOnly);
//	if (mCurrentOldHeaderFile.isNull())
//	{
//		OmnAlarm << "mRawSE->createRawFile failed for rawfid:" << rawfid << enderr;
//		return -1;
//	}
//	int iRet = 0;
//	if (0 != (iRet = saveOldHeaderFileSeqnosToMetafile(rdata)))
//	{
//		OmnAlarm << "saveLastLogFileIDToMetafile failed on rawfid:" << rawfid << ", iRet=" << iRet << enderr;
//		return -4;
//	}
	return 0;
}


int
AosHeaderFileMgr::saveOldHeaderFileSeqnosToMetafile(
		AosRundata				*rdata,
		const u32				ulSeqnoForFlushing,
		const u32				ulSeqnoForWorking,
		const std::vector<u32>	&vExistingSeqno)
{
	u32 ulMetaFile2Len = sizeof(MetaFileOldHeaderFileIDs) + sizeof(u32) * vExistingSeqno.size();
	AosBuffPtr buff = OmnNew AosBuff(ulMetaFile2Len AosMemoryCheckerArgs);
	aos_assert_rr(buff.notNull(), rdata, -1);
	memset(buff->data(), 0, buff->buffLen());
	buff->setDataLen(ulMetaFile2Len);
	MetaFileOldHeaderFileIDs* p = (MetaFileOldHeaderFileIDs*)buff->data();
	p->ulOldHeaderFileForFlushingSeqno = ulSeqnoForFlushing;
	p->ulOldHeaderFileForWorkingSeqno = ulSeqnoForWorking;
	buff->setCrtIdx((i64)sizeof(MetaFileOldHeaderFileIDs));
	for (std::vector<u32>::const_iterator it = vExistingSeqno.begin(); it != vExistingSeqno.end(); it++)
	{
		buff->setU32(*it);
	}
	u64 ullMetaFileID = getMetafileRawfidBySeqno(ulSeqnoForFlushing);
	if (getMetafileRawfidBySeqno(ulSeqnoForWorking) != ullMetaFileID)
	{
		OmnAlarm << "getMetafileRawfidBySeqno(ulSeqnoForWorking):" << getMetafileRawfidBySeqno(ulSeqnoForWorking)
				<< " != ullMetaFileID:" << ullMetaFileID << enderr;
		return -2;
	}
	int iRet = 0;
	if (0 != (iRet = mRawSE->overwriteRawFile(rdata, mSiteID, mCubeID,
			mAseID, ullMetaFileID, buff->data(), buff->dataLen())))
	{
		OmnAlarm << "rewrite meta file failed, eMetaFileID:" << ullMetaFileID << " iRet=" << iRet << enderr;
		return -3;
	}
	return 0;
}


int
AosHeaderFileMgr::getOldHeaderFileSeqnosByEpochUnitFromMetafile(
		AosRundata			*rdata,
		const u32			ulEpochUnit,
		u32					&ulSeqnoForFlushing,
		u32					&ulSeqnoForWorking,
		std::vector<u32>	&vExistingSeqno)
{
	u32 ulMetaFile2Len = sizeof(MetaFileOldHeaderFileIDs);
	AosBuffPtr buff = OmnNew AosBuff(ulMetaFile2Len AosMemoryCheckerArgs);
	aos_assert_rr(buff.notNull(), rdata, -1);
	memset(buff->data(), 0, buff->buffLen());
	MetaFileOldHeaderFileIDs* p = (MetaFileOldHeaderFileIDs*)buff->data();
	u64 ullMetaFileID = getMetafileRawfidByEpochUnit(ulEpochUnit);
	int iRet = 0;
	if (!mRawSE->fileExists(rdata, mSiteID, mCubeID, mAseID, ullMetaFileID))
	{
		AosRawFilePtr pMetaFile = mRawSE->createRawFile(rdata, mSiteID, mCubeID,
				mAseID, ullMetaFileID, AosMediaType::eDisk, AosRawFile::eModifyOnly);
		if (pMetaFile.isNull())
		{
			OmnAlarm << "create meta file failed for epoch day:" << ulEpochUnit << " rawfid:" << ullMetaFileID << enderr;
			return -2;
		}
		ulSeqnoForWorking = getFirstOldHeaderFileSeqnoByEpochUnit(ulEpochUnit);
		ulSeqnoForFlushing = ulSeqnoForWorking + 1;
		p->ulOldHeaderFileForFlushingSeqno = ulSeqnoForFlushing;
		p->ulOldHeaderFileForWorkingSeqno = ulSeqnoForWorking;
		vExistingSeqno.clear();
		iRet = pMetaFile->append(rdata, buff->data(), ulMetaFile2Len);
		if (0 != iRet)
		{
			OmnAlarm << "append date to meta file failed, iRet=" << iRet << " epoch day:" << ulEpochUnit
					<< " rawfid:" << ullMetaFileID << " append len:" << ulMetaFile2Len << enderr;
			return -3;
		}
		return 0;
	}
	else
	{
		AosRawFilePtr pMetaFile = mRawSE->getRawFile(rdata, mSiteID, mCubeID, mAseID, ullMetaFileID, AosRawFile::eReadWrite);
		if (pMetaFile.isNull())
		{
			OmnAlarm << "get meta file failed for epoch day:" << ulEpochUnit << " rawfid:" << ullMetaFileID << enderr;
			return -4;
		}
		iRet = pMetaFile->open(rdata, AosRawFile::eReadWrite);
		if (0 != iRet)
		{
			OmnAlarm << "pMetaFile->open failed, iRet=" << iRet << " rawfid:" << ullMetaFileID
					<< " open mode:" << AosRawFile::eReadWrite << enderr;
			return -5;
		}
		ulMetaFile2Len = pMetaFile->getLength();
		if (sizeof(MetaFileOldHeaderFileIDs) > ulMetaFile2Len)
		{
			OmnAlarm << "sizeof(MetaFileOldHeaderFileIDs):" << sizeof(MetaFileOldHeaderFileIDs)
					<< " > ulMetaFile2Len:" << ulMetaFile2Len << " rawfid:" << ullMetaFileID << enderr;
			return -6;
		}
		iRet = mRawSE->readRawFile(rdata, mSiteID, mCubeID,
					mAseID, ullMetaFileID, 0, ulMetaFile2Len, buff, AosRawFile::eReadWrite);
		p = (MetaFileOldHeaderFileIDs*)buff->data();
		ulSeqnoForFlushing = p->ulOldHeaderFileForFlushingSeqno;
		ulSeqnoForWorking = p->ulOldHeaderFileForWorkingSeqno;
		i64 illOffset = sizeof(MetaFileOldHeaderFileIDs);
		while(buff->dataLen() > illOffset)
		{
			vExistingSeqno.push_back(buff->getU32(illOffset, 0));
			illOffset += sizeof(u32);
		}
		return 0;
	}
}


u64
AosHeaderFileMgr::getMetafileRawfidBySeqno(const u32 seqno)
{
	return eOldHeaderFileID_start + (seqno & 0xFFFF0000);
}


bool
AosHeaderFileMgr::openLastOldHeaderFile(AosRundata	*rdata)
{
//	u64 last_fid = getLastOldHeaderFileSeqnosFromMetafile(rdata);
//	if (mRawSE->fileExists(rdata, mSiteID, mCubeID, mAseID, last_fid))
//	{
//		mCurrentOldHeaderFile = mRawSE->getRawFile(rdata, mSiteID, mCubeID, mAseID, last_fid, AosRawFile::eReadWrite);
//	}
//	else
//	{
//		mCurrentOldHeaderFile = mRawSE->createRawFile(rdata, mSiteID, mCubeID, mAseID,
//				last_fid, AosMediaType::eDisk, AosRawFile::eReadWrite);
//	}
//	aos_assert_rr(mCurrentOldHeaderFile.notNull(), rdata, false);
	return true;
}


int
AosHeaderFileMgr::overwriteNormalHeaderFile(
		AosRundata		*rdata,
		const u32		ulNormalHeaderFileSeqno,
		AosBuffPtr&		pBuff)
{
	if (pBuff.isNull())
	{
		OmnAlarm << "pBuff.isNull()" << enderr;
		return -1;
	}
	if (pBuff->dataLen() != getNormalHeaderFileSize())
	{
		OmnAlarm << "pBuff->dataLen():" << pBuff->dataLen()
				<< " != getNormalHeaderFileSize:" << getNormalHeaderFileSize() << enderr;
		return -2;
	}
	u64 ullRawfid = getNormalHeaderFileRawfidBySeqno(ulNormalHeaderFileSeqno);
	int iRet = mRawSE->overwriteRawFile(rdata, mSiteID, mCubeID, mAseID, ullRawfid, pBuff->data(), pBuff->dataLen());
	if (0 != iRet)
	{
		OmnAlarm << "mRawSE->overwriteRawFile failed, seqno:" << ulNormalHeaderFileSeqno
				<< " rawfid:" << ullRawfid << " pBuff->dataLen():" << pBuff->dataLen()
				<< ", iRet=" << iRet << enderr;
		return -3;
	}
	return 0;
}


AosRawFilePtr
AosHeaderFileMgr::getOldHeaderFileBySeqno(
    		AosRundata	*rdata,
    		const u32	ulSeqno)
{
	u64 ullRawfid = getOldHeaderFileRawfidBySeqno(ulSeqno);
	return mRawSE->getRawFile(rdata, mSiteID, mCubeID, mAseID, ullRawfid, AosRawFile::eModifyOnly);
}


u64
AosHeaderFileMgr::getOldHeaderFileRawfidBySeqno(const u32 seqno)
{
	return eOldHeaderFileID_start + seqno;
}


u64
AosHeaderFileMgr::getNormalHeaderFileRawfidBySeqno(const u32 seqno)
{
	return eHeaderFileID_start + seqno;
}


u64
AosHeaderFileMgr::getMetafileRawfidByTimestamp(const u64 timestamp)
{
	return eOldHeaderFileID_start + getMetafileSeqnoByTimestamp(timestamp);
}


u32
AosHeaderFileMgr::getMetafileSeqnoByTimestamp(const u64 timestamp)
{
	return getEpochUnitByTimestamp(timestamp) << 16;
}


u32
AosHeaderFileMgr::getNormalHeaderFileSize()
{
	return mHeaderInCacheAndHeaderFileSize * mHeadersPerFile;
}


int
AosHeaderFileMgr::serializeHeaderToNormalHeaderFileBuff(
			AosBlobHeaderPtr	&pHeader,
			AosBuffPtr			&pNormalHeaderFileBuff)
{
	if (pHeader.isNull())
	{
		OmnAlarm << "pHeader.isNull()" << enderr;
		return -1;
	}
	if (pNormalHeaderFileBuff.isNull())
	{
		OmnAlarm << "pNormalHeaderFileBuff.isNull" << enderr;
		return -2;
	}
	if (AosBlobHeader::unresolvedNextHeader == pHeader->getNextHeaderPtr() ||
			AosBlobHeader::noPrevHeader == pHeader->getNextHeaderPtr())
	{
		OmnAlarm << "illegeal next header pointer:" << pHeader->getNextHeaderPtr().toStr() << enderr;
		return -3;
	}
	return pHeader->serializeToCacheAndHeaderFileBuff(
			pNormalHeaderFileBuff->data() + getNormalHeaderOffsetByDocid(pHeader->getDocid()));
}


AosBlobHeaderPtr
AosHeaderFileMgr::unserializeHeaderFromNormalHeaderFileBuff(
		const u64	docid,
		AosBuffPtr	&pNormalHeaderFileBuff)
{
	AosBlobHeaderPtr pTempHeader = OmnNew AosBlobHeader(docid, mHeaderCustomDataSize);
	if (pTempHeader.isNull())
	{
		OmnAlarm << "failed to create header obj for docid:" << docid
				<< " mHeaderCustomDataSize:" << mHeaderCustomDataSize << enderr;
		return NULL;
	}
	int iRet = pTempHeader->unserializeFromCacheAndHeaderFileBuff(pNormalHeaderFileBuff->data() +
			(docid - getStartDocidOfNormalHeaderFileByDocid(docid)) * mHeaderInCacheAndHeaderFileSize);
	if (0 != iRet)
	{
		OmnScreen << "unserializeFromCacheAndHeaderFileBuff failed for docid:" << docid << ", iRet=" << iRet
				<< " happened at timestamp:" << OmnTime::getTimestamp() << endl;
		return NULL;
	}
	return pTempHeader;
}


int
AosHeaderFileMgr::overwriteOldHeader(
		AosRundata							*rdata,
		const AosBlobHeaderPtr				&header,
		const AosBlobHeader::HeaderPointer&	sNextHeader)
{
	AosRawFilePtr pOldHeaderFile = getOldHeaderFileBySeqno(rdata, sNextHeader.ulSeqno);
	if (pOldHeaderFile.isNull())
	{
		OmnAlarm << "getOldHeaderFileBySeqno failed, seqno:" << sNextHeader.ulSeqno << enderr;
		return -1;
	}
	int iRet = 0;
	pOldHeaderFile->lock();
	if (!pOldHeaderFile->isGood())
	{
		iRet = pOldHeaderFile->open(rdata, AosRawFile::eModifyOnly);
		if (0 != iRet)
		{
			OmnAlarm << "pOldHeaderFile->open failed, iRet=" << iRet << enderr;
			pOldHeaderFile->unlock();
			return -2;
		}
	}
	AosBuffPtr pBuff = OmnNew AosBuff(pOldHeaderFile->getLength() AosMemoryCheckerArgs);
	if (pBuff.isNull())
	{
		OmnAlarm << "can not create AosBuff obj, size:" << pOldHeaderFile->getLength() << enderr;
		pOldHeaderFile->unlock();
		return -3;
	}
	iRet = pOldHeaderFile->read(rdata, 0, pOldHeaderFile->getLength(), pBuff);
	if (0 != iRet)
	{
		OmnAlarm << "pOldHeaderFile->read failed, iRet=" << iRet << " rawfid:" << pOldHeaderFile->getRawFileID()
				<< " read len:" << pOldHeaderFile->getLength() << enderr;
		pOldHeaderFile->unlock();
		return -4;
	}
	iRet = header->serializeToCacheAndHeaderFileBuff(pBuff->data() + sNextHeader.ulOffset);
	if (0 != iRet)
	{
		OmnAlarm << "header->serializeToCacheAndHeaderFileBuff failed, iRet=" << iRet
				<< " docid:" << header->getDocid() << " offset:" << sNextHeader.ulOffset << enderr;
		pOldHeaderFile->unlock();
		return -5;
	}
	iRet = pOldHeaderFile->overwrite(rdata, pBuff->data(), pOldHeaderFile->getLength());
	if (0 != iRet)
	{
		OmnAlarm << "pOldHeaderFile->overwrite failed, iRet=" << iRet << " rawfid:"
				<< pOldHeaderFile->getRawFileID() << " write len:" << pOldHeaderFile->getLength() << enderr;
		pOldHeaderFile->unlock();
		return -6;
	}
	pOldHeaderFile->unlock();
	return 0;
}


bool
AosHeaderFileMgr::oldHeaderFileExists(
		AosRundata	*rdata,
		const u32	ulSeqno)
{
	return mRawSE->fileExists(rdata, mSiteID, mCubeID, mAseID, getOldHeaderFileRawfidBySeqno(ulSeqno));
}


u32
AosHeaderFileMgr::getEpochUnitByTimestamp(const u64 ullTimestamp)
{
	return (u32)(ullTimestamp / 1000000) / mCompactionTimeUnitInSec;
//	return OmnTime::getEpochDayByTimestamp(ullTimestamp);
}


u32
AosHeaderFileMgr::getEpochUnitBySeqno(const u32 ulSeqno)
{
	return ulSeqno >> 16;
}


u32
AosHeaderFileMgr::getOldHeaderFileSeqnoByRawfid(const u64 rawfid)
{
	return rawfid - eOldHeaderFileID_start;
}


u64
AosHeaderFileMgr::getMetafileRawfidByEpochUnit(const u32 ulEpochUnit)
{
	return eOldHeaderFileID_start + getMetafileSeqnoByEpochUnit(ulEpochUnit);
}


u32
AosHeaderFileMgr::getMinAvailableSeqnoOfEpochUnit(
		const u32				ulEpochUnit,
		const u32				ulSeqnoForFlushing,
		const u32				ulSeqnoForWorking,
		const std::vector<u32>	&vExistingSeqno)
{
	u32 ulMinSeqnoOfEpochUnit = getFirstOldHeaderFileSeqnoByEpochUnit(ulEpochUnit);
	u32 ulMaxSeqnoOfEpochUnit = getMetafileSeqnoByEpochUnit(ulEpochUnit) + 65535;
	u32 ulMaxSeqno = ulSeqnoForFlushing > ulSeqnoForWorking ? ulSeqnoForFlushing : ulSeqnoForWorking;
	if (vExistingSeqno.size() == 0)
	{
		return ulMaxSeqno + 1;
	}
	std::vector<u32>::const_iterator it = vExistingSeqno.begin();
	for (u32 seqno = ulMinSeqnoOfEpochUnit; seqno <= ulMaxSeqnoOfEpochUnit; seqno++)
	{
		if (*it == seqno)
		{
			it++;
			if (it != vExistingSeqno.end())
			{
				continue;
			}
			else if (ulSeqnoForFlushing != seqno + 1 && ulSeqnoForWorking != seqno + 1)
			{
				return seqno + 1;
			}
			else
			{
				return ulMaxSeqno + 1;
			}
		}
		else if (*it > seqno)
		{
			if (ulSeqnoForFlushing != seqno && ulSeqnoForWorking != seqno)
			{
				return seqno;
			}
			else
			{
				it++;
				if (it != vExistingSeqno.end())
				{
					continue;
				}
				else if (ulSeqnoForFlushing != seqno + 1 && ulSeqnoForWorking != seqno + 1)
				{
					return seqno + 1;
				}
				else
				{
					return ulMaxSeqno + 1;
				}
			}
		}
		else
		{
			OmnAlarm << "vExistingSeqno not sorted or there is unexpected data in the vector."
					<< " *it:" << *it << " seqno:" << seqno << enderr;
			OmnString s = "";
			for (it = vExistingSeqno.begin(); it != vExistingSeqno.end(); it++)
			{
				s << *it << "\n";
			}
			OmnScreen << "dumping vExistingSeqno:" << endl << s << endl;
			return 0;
		}
	}
	OmnAlarm << "It should never come here, epoch day:" << ulEpochUnit << enderr;
	OmnString s = "";
	for (it = vExistingSeqno.begin(); it != vExistingSeqno.end(); it++)
	{
		s << *it << "\n";
	}
	OmnScreen << "dumping vExistingSeqno:" << endl << s << endl;
	return 0;
}


u32
AosHeaderFileMgr::getMetafileSeqnoByEpochUnit(const u32 ulEpochUnit)
{
	return ulEpochUnit << 16;
}


bool
AosHeaderFileMgr::isOldHeaderFileMetaFileSeqno(const u32 seqno)
{
	return seqno % 65536 == 0;
}


int
AosHeaderFileMgr::recoverOldHeader(
		AosRundata			*rdata,
		AosBlobHeaderPtr	&pHeader)
{
//	AosRawFilePtr pOldHeaderFile;
//	AosBuffPtr pBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
//	u64 ullRawfid = getOldHeaderFileRawfidBySeqno(pHeader->getSelfPos().ulSeqno);
//	int iRet = 0;
//	if (!mRawSE->fileExists(rdata, mSiteID, mCubeID, mAseID, ullRawfid))
//	{
//		pOldHeaderFile = mRawSE->createRawFile(rdata, mSiteID, mCubeID, mAseID, ullRawfid,
//				AosMediaType::eDisk, AosRawFile::eModifyOnly);
//		if (pOldHeaderFile.isNull())
//		{
//			OmnAlarm << "createRawFile failed, rawfid:" << ullRawfid << " site id:" << mSiteID
//					<< " cube id:" << mCubeID << " aseid:" << mAseID << enderr;
//			return -1;
//		}
//		if (pBuff->buffLen() <= (i64)(pHeader->getSelfPos().ulOffset + pHeader->getHeaderInCacheAndHeaderFileLength()))
//		{
//			if (!pBuff->expandMemory1(pHeader->getSelfPos().ulOffset + pHeader->getHeaderInCacheAndHeaderFileLength()))
//			{
//				OmnAlarm << "pBuff->expandMemory1 failed, try to expand to size:"
//						<< pHeader->getSelfPos().ulOffset + pHeader->getHeaderInCacheAndHeaderFileLength() << enderr;
//			}
//		}
//		memset(pBuff->data(), 0, pHeader->getSelfPos().ulOffset + pHeader->getHeaderInCacheAndHeaderFileLength());
//	}
//	else
//	{
//		pOldHeaderFile = getOldHeaderFileBySeqno(rdata, pHeader->getSelfPos().ulSeqno);
//		if (pOldHeaderFile.isNull())
//		{
//			OmnAlarm << "getOldHeaderFileBySeqno failed, seqno:" << pHeader->getSelfPos().ulSeqno << enderr;
//			return -2;
//		}
//		if (!pOldHeaderFile->isGood())
//		{
//			iRet = pOldHeaderFile->open(rdata, AosRawFile::eModifyOnly);
//			if (0 != iRet)
//			{
//				OmnAlarm << "pOldHeaderFile->open failed, iRet=" << iRet << enderr;
//				return -3;
//			}
//		}
//		iRet = pOldHeaderFile->read(rdata, 0, pOldHeaderFile->getLength(), pBuff);
//		if (0 != iRet)
//		{
//			OmnAlarm << "pOldHeaderFile->read failed, iRet=" << iRet << " rawfid:" << pOldHeaderFile->getRawFileID()
//					<< " read len:" << pOldHeaderFile->getLength() << enderr;
//			return -4;
//		}
//		if (pBuff->buffLen() <= (i64)(pHeader->getSelfPos().ulOffset + pHeader->getHeaderInCacheAndHeaderFileLength()))
//		{
//			u32 ulOldBuffLen = (u32)pBuff->buffLen();
//			if (!pBuff->expandMemory1(pHeader->getSelfPos().ulOffset + pHeader->getHeaderInCacheAndHeaderFileLength()))
//			{
//				OmnAlarm << "pBuff->expandMemory1 failed, try to expand to size:"
//						<< pHeader->getSelfPos().ulOffset + pHeader->getHeaderInCacheAndHeaderFileLength() << enderr;
//			}
//			memset(pBuff->data() + ulOldBuffLen, 0,
//					pHeader->getSelfPos().ulOffset + pHeader->getHeaderInCacheAndHeaderFileLength() - ulOldBuffLen);
//		}
//	}
//	iRet = pHeader->serializeToCacheAndHeaderFileBuff(pBuff->data() + pHeader->getSelfPos().ulOffset);
//	if (0 != iRet)
//	{
//		OmnAlarm << "serializeToCacheAndHeaderFileBuff failed, iRet=" << iRet
//				<< " offset:" << pHeader->getSelfPos().ulOffset << enderr;
//		pHeader->dump();
//		return -5;
//	}
//	iRet = pOldHeaderFile->overwrite(rdata, pBuff->data(), pBuff->buffLen());
//	if (0 != iRet)
//	{
//		OmnAlarm << "pOldHeaderFile->overwrite failed, iRet=" << iRet << " rawfid:"
//				<< pOldHeaderFile->getRawFileID() << " write len:" << pBuff->buffLen() << enderr;
//		return -6;
//	}
	return 0;
}
