////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 2014-11-24 Created by White Wu
////////////////////////////////////////////////////////////////////////////
#include "RawSE/RawSE.h"

#include "ErrorMgr/ErrmsgId.h"
#include "RawSE/PartitionIDMgr.h"
#include "RawSE/RawfidMgr.h"
#include "RawSE/RawFile.h"
#include "RawSE/RawFileHeader.h"
#include "Rundata/Rundata.h"
#include "SEUtil/EnumFieldName.h"
#include "Util/OmnNew.h"
#include "Thread/LockGuard.h"

AosRawSE::AosRawSE(AosXmlTagPtr	&conf)
:
mLockRaw(OmnNew OmnMutex()),
mLock(mLockRaw),
mRawfidMgr(OmnNew AosRawfidMgr()),
mPartitionIDMgr(OmnNew AosPartitionIDMgr(conf)),
mRawFileHeader(OmnNew AosRawFileHeader()),
mpHead(NULL),
mpTail(NULL)
{
	mRawFileMap.clear();
	if (!config())
	{
		mIsReady = false;
	}
	else
	{
		mIsReady = true;
	}
}


AosRawSE::~AosRawSE()
{
}


AosRawFilePtr
AosRawSE::createRawFile(
		AosRundata					*rdata,
		const u32					siteid,
		const u32					cubeid,
		const u64					aseid,
		const AosMediaType::Type	media_type,
		const AosRawFile::Type		eType,
		u64 						&rawfid)
{
	// This function creates a new raw file. This is done by the following:
	// 	1. Pick a partition
	// 	2. Allocate a rawfid,
	//	3. Create the file
	//	4. Append the contents to the file, if any
	//	5. Create the header
	//	6. Return the smart pointer of the raw file object

	u32 partition_id = mPartitionIDMgr->pickPartition(rdata, media_type);
	rawfid = mRawfidMgr->allocateRawfid(rdata, partition_id);
	return createRawFile(rdata, siteid, cubeid, aseid, rawfid, media_type, eType);
}


AosRawFilePtr
AosRawSE::createRawFile(
		AosRundata					*rdata,
		const u32					siteid,
		const u32					cubeid,
		const u64					aseid,
		const u64 					rawfid,
		const AosMediaType::Type	media_type,
		const AosRawFile::Type		eType)
{
	/*
	 * this function creates the raw file with rawfid specified by the caller
	 */
	AosRawFilePtr raw_file = getRawFile(rdata, siteid, cubeid, aseid, rawfid, eType);
	int iRet = 0;
	if(raw_file.notNull())
	{
		if (0 != (iRet = raw_file->create(rdata)))
		{
			OmnAlarm << "can not create raw file, rawfid is '" << rawfid << "'" << enderr;
			return NULL;
		}
		if (!mRawFileHeader->updateHeader(rdata, rawfid))
		{
			OmnAlarm << "can not update raw file header, rawfid is '" << rawfid << "'" << enderr;
			return NULL;
		}
		//TODO:may need some further checking
	}
	else
	{
//		rdata->setError() << "getRawFile failed.";
//		AosLogError(rdata, false, AosErrmsgId::eInternalError)
//			<< AosEnumFieldName::eRawFileId << rawfid << enderr;
		OmnAlarm << "getRawFile failed for rawfid '" << rawfid << "' siteid:'"
				<< siteid << "' cubeid:'" << cubeid << "' aseid:'" << aseid  << "'" << enderr;
		return NULL;
	}
	return raw_file;
}


int
AosRawSE::appendData(
		AosRundata				*rdata,
		const u32				siteid,
		const u32				cubeid,
		const u64				aseid,
		const u64 				rawfid,
		const char *const 		data,
		const u64 				length,
		const AosRawFile::Type	eType)
{
	//this function tries to append data to a certain file identified by rawfid
	AosRawFilePtr raw_file = getRawFile(rdata, siteid, cubeid, aseid, rawfid, eType);

	if(raw_file.notNull())
	{
		raw_file->lock();
		int iRet = raw_file->append(rdata, data, length);
		if (0 != iRet)
		{
			OmnAlarm << "raw_file->append failed, iRet=" << iRet << " rawfid:" << rawfid << " siteid:"
				<< siteid << " cubeid:" << cubeid << " aseid:" << aseid << " append len:" << length << enderr;
			raw_file->unlock();
			return -1;
		}
		raw_file->unlock();
		return 0;
	}
	else
	{
//		rdata->setError() << "can not get raw file ptr for appending";
//		AosLogError(rdata, false, AosErrmsgId::eInternalError)
//			<< AosEnumFieldName::eRawFileId << rawfid << enderr;
		OmnAlarm << "getRawFile failed for rawfid '" << rawfid << "' siteid:'"
				<< siteid << "' cubeid:'" << cubeid << "' aseid:'" << aseid  << "'" << enderr;
		return -2;
	}
}


int
AosRawSE::readRawFile(
		AosRundata				*rdata,
		const u32				siteid,
		const u32				cubeid,
		const u64				aseid,
		const u64 				rawfid,
		const u64				offset,
		const u64				length,
		AosBuffPtr				&buff,
		const AosRawFile::Type	eType,
		const bool				bShowAlarm)
{
	//this function tries to read some data from a certain file identified by rawfid
	AosRawFilePtr raw_file = getRawFile(rdata, siteid, cubeid, aseid, rawfid, eType);
	if(raw_file.notNull())
	{
		raw_file->lock();
		int iRet = raw_file->read(rdata, offset, length, buff, bShowAlarm);
		if (0 != iRet)
		{
			if (bShowAlarm)
			{
				OmnAlarm << "raw_file->read failed, iRet=" << iRet << " rawfid:" << rawfid << " siteid:"
					<< siteid << " cubeid:" << cubeid << " aseid:" << aseid << " read len:" << length
					<< " read offset:" << offset << " file len:" << raw_file->getLength() << enderr;
			}
			else
			{
				OmnScreen << "raw_file->read failed, iRet=" << iRet << " rawfid:" << rawfid << " siteid:"
					<< siteid << " cubeid:" << cubeid << " aseid:" << aseid << " read len:" << length
					<< " read offset:" << offset << " file len:" << raw_file->getLength() << endl;
			}
			raw_file->unlock();
			return -1;
		}
		raw_file->unlock();
		return 0;
	}
	else
	{
//		rdata->setError() << "can not get raw file ptr for reading";
//		AosLogError(rdata, false, AosErrmsgId::eInternalError)
//			<< AosEnumFieldName::eRawFileId << rawfid << enderr;
		OmnAlarm << "getRawFile failed for rawfid '" << rawfid << "' siteid:'"
				<< siteid << "' cubeid:'" << cubeid << "' aseid:'" << aseid  << "'" << enderr;
		return -2;
	}
}


AosRawFilePtr
AosRawSE::getRawFile(
		AosRundata				*rdata,
		const u32				siteid,
		const u32				cubeid,
		const u64				aseid,
		const u64				rawfid,
		const AosRawFile::Type	eType)
{
	//this function returns a raw file object with a smart pointer
	//if the file is in the cache, then just return it
	//create one otherwise
//	OmnScreen << "getting rawfile obj, siteid:" << siteid << " cubeid:" << cubeid << " aseid:" << aseid
//			<< " rawfid:" << rawfid << endl;
//	LockGuard(g, mLockRaw);
//	itr_t it = mRawFileMap.find(AosRawFile::Identifier(siteid, cubeid, aseid, rawfid));
//	if (mRawFileMap.end() == it)
//	{
		try
		{
			AosRawFilePtr raw_file = OmnNew AosRawFile(rdata, mPartitionIDMgr.getPtrNoLock(), siteid, cubeid, aseid, rawfid,
					mConfig.level1size, mConfig.level2size, mConfig.level3size, eType);
			if (raw_file.isNull())
			{
				OmnAlarm << "can not create raw file obj with rawfid:'" << rawfid << "' siteid:'"
						<< siteid << "' cubeid:'" << cubeid << "' aseid:'" << aseid  << "'" << enderr;
				return NULL;
			}
//			OmnScreen << "file addr @" << raw_file.getPtrNoLock() << endl;
//			mRawFileMap[AosRawFile::Identifier(siteid, cubeid, aseid, rawfid)] = raw_file;
//			it = mRawFileMap.find(AosRawFile::Identifier(siteid, cubeid, aseid, rawfid));
			return raw_file;
		}
		catch(const OmnExcept &e)
		{
			OmnAlarm << "Failed to create AosRawFile obj: " << e.getErrmsg() << enderr;
			return NULL;
		}
		catch(...)
		{
			OmnAlarm << "something is wrong." << enderr;
			return NULL;
		}
//	}
//	else
//	{
//		it->second->lock();
//		if (it->second->isGood() && eType != it->second->getType())
//			//TODO:should any file can be opened as readonly? if yes, it has to be reopened. what about the cost?
//		{
//			OmnScreen << "trying to get file with rawfid '" << rawfid << "' siteid:'"
//					<< siteid << "' cubeid:'" << cubeid << "' aseid:'" << aseid
//					<< "' with type of '" << it->second->getType() << "' as type of '" << eType << "'" << endl;
//			int iRet = it->second->close();
//			if (0 != iRet)
//			{
//				OmnAlarm << "rawfile close failed, iRet=" << iRet << " rawfid '" << rawfid << "' siteid:'"
//					<< siteid << "' cubeid:'" << cubeid << "' aseid:'" << aseid << enderr;
//				it->second->unlock();
//				return NULL;
//			}
//			it->second->setType(eType);
//	//		iRet = it->second->open(rdata, eType);
//	//		if (0 != iRet)
//	//		{
//	//			OmnAlarm << "rawfile open failed, iRet=" << iRet << " rawfid '" << rawfid << "' siteid:'"
//	//				<< siteid << "' cubeid:'" << cubeid << "' aseid:'" << aseid << " type:" << eType << enderr;
//	//			mLockRaw->unlock();
//	//			return NULL;
//	//		}
//		}
//		it->second->unlock();
//	}
//	moveToHeadInDLinkedList(rdata, it->second);
//	return it->second;
}


bool
AosRawSE::config()
{
	mConfig.level1size = 10;
	mConfig.level2size = 10;
	mConfig.level3size = 10;
	mConfig.cacheSize = 2000;
	return true;
}


int
AosRawSE::deleteFile(
		AosRundata				*rdata,
		const u32				siteid,
		const u32				cubeid,
		const u64				aseid,
		const u64 				rawfid,
		const AosRawFile::Type	eType)
{
	//this function deletes file from media
	AosRawFilePtr raw_file = getRawFile(rdata, siteid, cubeid, aseid, rawfid, eType);
	aos_assert_rr(raw_file.notNull(), rdata, -1);
	raw_file->lock();
	int iRet = raw_file->del(rdata);
	raw_file->unlock();
	if (0 == iRet)
	{
		mLockRaw->lock();
		if (0 != (iRet = agingOut(rdata, raw_file)))
		{
			OmnAlarm << "agingOut failed, iRet=" << iRet << enderr;
			mLockRaw->unlock();
			return -2;
		}
		mLockRaw->unlock();
		return 0;
	}
	else
	{
		OmnAlarm << "can not delete rawfile '" << raw_file->getFileNameWithFullPath() << "' with rawfile id:'" << rawfid
			  << "' siteid '" << siteid << "' cubeid '" << cubeid << "' aseid '" << aseid << "' iRet=" << iRet << enderr;
		return -3;
	}

}


int
AosRawSE::agingOut(
		AosRundata		*rdata,
		AosRawFilePtr	pRawFile)
{
	//this function does the aging out
//	dumpRawfileMap(__FUNCTION__);
//	aos_assert_rr(pRawFile.notNull(), rdata, -1);
//	aos_assert_rr((mpHead.notNull() && mpTail.notNull()), rdata, -2);
//	OmnScreen << "aging out rawfile, siteid:" << pRawFile->getSiteID() << " cubeid:" << pRawFile->getCubeID()
//			<< " aseid:" << pRawFile->getAseID() << " rawfid:" << pRawFile->getRawFileID() << endl;
//	mRawFileMap.erase(AosRawFile::Identifier(pRawFile->getSiteID(), pRawFile->getCubeID(), pRawFile->getAseID(), pRawFile->getRawFileID()));
//	if (mpHead == mpTail)	//there is only one element in the list
//	{
//		mpHead = mpTail = NULL;	//clean them
//		return 0;
//	}
//	if (mpHead == pRawFile)	//aging out the first one
//	{
//		mpHead = mpHead->mNext;
//	}
//	if (mpTail == pRawFile)	//aging out the last one
//	{
//		mpTail = mpTail->mPrev;
//	}
//	aos_assert_rr((pRawFile->mPrev.notNull() && pRawFile->mNext.notNull()), rdata, -3);
//	pRawFile->mPrev->mNext = pRawFile->mNext;
//	pRawFile->mNext->mPrev = pRawFile->mPrev;
	return 0;
}


int
AosRawSE::overwriteRawFile(
		AosRundata			*rdata,
		const u32			siteid,
		const u32			cubeid,
		const u64			aseid,
		const u64			rawfid,
		const char *const 	data,
		const u64 			length)
{
	//this function overwrites the rawfile identified by rawfid
	AosRawFilePtr raw_file = getRawFile(rdata, siteid, cubeid, aseid, rawfid, AosRawFile::eModifyOnly);
	if(raw_file.notNull())
	{
		raw_file->lock();
		int iRet = raw_file->overwrite(rdata, data, length);
		if (0 != iRet)
		{
			OmnAlarm << "raw_file->overwrite failed, iRet=" << iRet << " rawfid:" << rawfid << " siteid:"
				<< siteid << " cubeid:" << cubeid << " aseid:" << aseid << " overwrite len:" << length << enderr;
			raw_file->unlock();
			return -1;
		}
		raw_file->unlock();
		return 0;
	}
	else
	{
//		rdata->setError() << "can not get raw file ptr for overwriting";
//		AosLogError(rdata, false, AosErrmsgId::eInternalError)
//			<< AosEnumFieldName::eRawFileId << rawfid << enderr;
		OmnAlarm << "getRawFile failed for overwriting, rawfid '" << rawfid << "' siteid:'"
				<< siteid << "' cubeid:'" << cubeid << "' aseid:'" << aseid  << "'" << enderr;
		return -2;
	}
}


bool
AosRawSE::fileExists(
		AosRundata	*rdata,
		const u32	siteid,
		const u32	cubeid,
		const u64	aseid,
		const u64	rawfid)
{
	try
	{
		AosRawFilePtr raw_file = getRawFile(rdata, siteid, cubeid, aseid, rawfid, AosRawFile::eReadWrite);
			//TODO:type should not be AosRawFile::eModifyOnly, redesign it
		if (raw_file.isNull())
		{
			OmnAlarm << "can not create raw file obj with rawfid:'" << rawfid << "' siteid:'"
					<< siteid << "' cubeid:'" << cubeid << "' aseid:'" << aseid  << "'" << enderr;
			return false;
		}
		else
		{
			return OmnFile::fileExist(raw_file->getFileNameWithFullPath());
		}
	}
	catch(const OmnExcept &e)
	{
		OmnAlarm << "Failed to create AosRawFile obj: " << e.getErrmsg() << enderr;
		return false;
	}
	return false;
}


int
AosRawSE::moveToHeadInDLinkedList(
		AosRundata		*rdata,
		AosRawFilePtr	pRawFile)
{
	//this function does the aging in
//	aos_assert_rr(pRawFile.notNull(), rdata, -1);
//	if (mpHead.isNull() || mpTail.isNull())		//this double linked list is empty
//	{
//		mpHead = pRawFile;
//		mpTail = pRawFile;
//	}
//	if (pRawFile->mNext.notNull() && pRawFile->mPrev.notNull())	//not first inserted
//	{
//		pRawFile->mNext->mPrev = pRawFile->mPrev;
//		pRawFile->mPrev->mNext = pRawFile->mNext;
//	}
//	pRawFile->mNext = mpHead;
//	mpHead->mPrev = pRawFile;
//	mpHead = pRawFile;
//	pRawFile->mPrev = mpTail;
//	mpTail->mNext = pRawFile;
////	OmnScreen << "mRawFileMap.size():" << mRawFileMap.size() << endl;
//	if (mConfig.cacheSize < mRawFileMap.size())
//	{
//		agingOut(rdata, mpTail);
//	}
	return 0;
}


void
AosRawSE::dumpRawfileMap(const char* pCaller)
{
	OmnScreen << "dumping Rawfile map in " << pCaller << " with size:" << mRawFileMap.size() << endl;
	for (itr_t it = mRawFileMap.begin(); it != mRawFileMap.end(); it++)
	{
		it->second->dumpInfo();
	}
}


OmnString AosRawSE::getBaseDir()
{
	return mPartitionIDMgr->getBaseDir();
}
