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
// 6 May 2015 created by White
////////////////////////////////////////////////////////////////////////////
#include "BlobSE/OprIDCache.h"

#include "Alarm/Alarm.h"
#include "BlobSE/BlobSE.h"
#include "BlobSE/RawfidRange.h"
#include "Util/Buff.h"
#include "Util/OmnNew.h"
AosOprIDCache::AosOprIDCache(
		AosRundata			*rdata,
		AosRawSE			*rawse,
		const AosXmlTagPtr	&conf)
:
mLock(OmnNew OmnMutex()),
mLockRaw(mLock.getPtrNoLock()),
mRdata(rdata)
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
	config = conf->getFirstChild("OprIDCache", true);
	if (config.isNull())
	{
		rdata->setErrmsg("can not find config tag named HeaderFile");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	mOprIDsPerFile = config->getAttrU32("oprids_per_file", eOprIDsPerFile);
	if (eOprIDsPerFile == mOprIDsPerFile)
	{
		OmnScreen << "mOprIDsPerFile set to default value:" << mOprIDsPerFile
				<< ", please make sure this is what you want." << endl;
	}
	mActiveOprIDFileBuffer = OmnNew AosBuff(getOprIDFileSize() AosMemoryCheckerArgs);
	if (mActiveOprIDFileBuffer.isNull())
	{
		OmnAlarm << "can not create mActiveOprIDFileBuffer obj, buff len:" << getOprIDFileSize() << enderr;
		rdata->setErrmsg("can not create mActiveOprIDFileBuffer obj");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	if (mRawSE->fileExists(mRdata, mSiteID, mCubeID, mAseID, eOprIDFileID_start + 1))
	{
		mActiveOprIDFile = mRawSE->getRawFile(mRdata, mSiteID, mCubeID, mAseID,
				eOprIDFileID_start + 1, AosRawFile::eReadWrite);
		if (mActiveOprIDFile.isNull())
		{
			OmnAlarm << "can not get rawfile obj with rawfid:" << eOprIDFileID_start << enderr;
			rdata->setErrmsg("can not get rawfile obj");
			OmnThrowException(rdata->getErrmsg());
			return;
		}
		int iRet = mActiveOprIDFile->read(mRdata, 0, getOprIDFileSize(), mActiveOprIDFileBuffer);
		if (0 != iRet)
		{
			OmnAlarm << "mActiveOprIDFile->read failed, iRet=" << iRet
					<< " rawfid:" << mActiveOprIDFile->getRawFileID()
					<< " read len:" << getOprIDFileSize() << enderr;
			rdata->setErrmsg("mActiveOprIDFile->read failed");
			OmnThrowException(rdata->getErrmsg());
			return;
		}
	}
	else
	{
		mActiveOprIDFile = mRawSE->getRawFile(mRdata, mSiteID, mCubeID, mAseID,
				eOprIDFileID_start + 1, AosRawFile::eModifyOnly);
		if (mActiveOprIDFile.isNull())
		{
			OmnAlarm << "can not get rawfile obj with rawfid:" << eOprIDFileID_start << enderr;
			rdata->setErrmsg("can not get rawfile obj");
			OmnThrowException(rdata->getErrmsg());
			return;
		}
		memset(mActiveOprIDFileBuffer->data(), 0, mActiveOprIDFileBuffer->buffLen());
	}
	mActiveOprIDFileBuffer->setDataLen(getOprIDFileSize());
}


AosOprIDCache::~AosOprIDCache()
{
}


int
AosOprIDCache::saveOprID(
		AosRundata	*rdata,
		const u64	ullOprId,
		const u64	ullRawfid,
		const u64	ullOffset,
		const u64	ullLen)
{
	if (0 == ullOprId)
	{
		OmnAlarm << "0 == ullOprId" << enderr;
		return 0;
	}
	int iRet = 0;
	mLockRaw->lock();
	if (getOprIDFileSeqnoByRawfid(mActiveOprIDFile->getRawFileID()) != getOprIDFileSeqnoByOprID(ullOprId))
	{
		iRet = switchOprIDFileAndBuff(getOprIDFileSeqnoByOprID(ullOprId));
		if (0 != iRet)
		{
			OmnAlarm << "switchOprIDFile failed, iRet=" << iRet << enderr;
			mLockRaw->unlock();
			return -1;
		}
	}
	OprIDEntry* pEntry = (OprIDEntry*)(mActiveOprIDFileBuffer->data() + getOffsetByOprID(ullOprId));
	pEntry->ullRawfid = ullRawfid;
	pEntry->ullOffset = ullOffset;
	pEntry->ullLen = ullLen;
	iRet = overwriteOprIDFile();
	if (0 != iRet)
	{
		OmnAlarm << "overwriteOprIDFile failed, iRet=" << iRet
				<< " rawfid:" << mActiveOprIDFile->getRawFileID()
				<< " writelen:" << mActiveOprIDFileBuffer->dataLen() << enderr;
		mLockRaw->unlock();
		return -1;
	}
	mLockRaw->unlock();
	return 0;
}


int
AosOprIDCache::removeOprID(
		AosRundata	*rdata,
		const u64	ullOprId)
{
	OmnNotImplementedYet;
	return 0;
}


int
AosOprIDCache::getRawfidAndOffsetAndLenByOprID(
		const u64	ullOprId,
		u64			&ullRawfid,
		u64			&ullOffset,
		u64			&ullLen)
{
	int iRet = 0;
	mLockRaw->lock();
	if (getOprIDFileSeqnoByRawfid(mActiveOprIDFile->getRawFileID()) != getOprIDFileSeqnoByOprID(ullOprId))
	{
		iRet = switchOprIDFileAndBuff(getOprIDFileSeqnoByOprID(ullOprId));
		if (0 != iRet)
		{
			OmnAlarm << "switchOprIDFile failed, iRet=" << iRet << enderr;
			mLockRaw->unlock();
			return -1;
		}
	}
	OprIDEntry* pEntry = (OprIDEntry*)(mActiveOprIDFileBuffer->data() + getOffsetByOprID(ullOprId));
	ullRawfid = pEntry->ullRawfid;
	ullOffset = pEntry->ullOffset;
	ullLen = pEntry->ullLen;
	if (0 == ullRawfid)
	{
		OmnAlarm << "0 == ullRawfid:" << ullRawfid << enderr;
		mLockRaw->unlock();
		return -2;
	}
	mLockRaw->unlock();
	return 0;
}


u32
AosOprIDCache::getOprIDFileSeqnoByOprID(const u64 ullOprID)
{
	if (0 == ullOprID)
	{
		return 0;
	}
	return (ullOprID - 1) / mOprIDsPerFile + 1;
}


int
AosOprIDCache::overwriteOprIDFile()
{
	return mActiveOprIDFile->overwrite(mRdata, mActiveOprIDFileBuffer->data(), mActiveOprIDFileBuffer->dataLen());
}


int
AosOprIDCache::switchOprIDFileAndBuff(const u32 ulSeqno)
{
	int iRet = overwriteOprIDFile();
	if (0 != iRet)
	{
		OmnAlarm << "overwriteOprIDFile failed, iRet=" << iRet
				<< " rawfid:" << mActiveOprIDFile->getRawFileID()
				<< " writelen:" << mActiveOprIDFileBuffer->dataLen() << enderr;
		return -1;
	}
	mActiveOprIDFile = mRawSE->getRawFile(mRdata, mSiteID, mCubeID, mAseID,
			getOprIDFileRawfidBySeqno(ulSeqno), AosRawFile::eModifyOnly);
	if (mActiveOprIDFile.isNull())
	{
		OmnAlarm << "mRawSE->getRawFile failed, siteid:" << mSiteID << " cubeid:" << mCubeID
				<< " aseid:" << mAseID << " rawfid:" << getOprIDFileRawfidBySeqno(ulSeqno)
				<< " type:" << AosRawFile::eModifyOnly << enderr;
		return -2;
	}
	if (mRawSE->fileExists(mRdata, mSiteID, mCubeID, mAseID, getOprIDFileRawfidBySeqno(ulSeqno)))
	{
		iRet = mActiveOprIDFile->read(mRdata, 0, getOprIDFileSize(), mActiveOprIDFileBuffer);
		if (0 != iRet)
		{
			OmnAlarm << "mActiveOprIDFile->read failed, iRet=" << iRet << " read len:" << getOprIDFileSize()
					<< " rawfid:" << mActiveOprIDFile->getRawFileID() << enderr;
			return -3;
		}
	}
	else
	{
		memset(mActiveOprIDFileBuffer->data(), 0, mActiveOprIDFileBuffer->buffLen());
	}
	mActiveOprIDFileBuffer->setDataLen(getOprIDFileSize());
	return 0;
}


u32
AosOprIDCache::getOprIDFileSeqnoByRawfid(const u64 ullRawfid)
{
	return ullRawfid - eOprIDFileID_start;
}


u32
AosOprIDCache::getOffsetByOprID(const u64 ullOprID)
{
	if (0 == ullOprID)
	{
		return 0;
	}
	return (ullOprID - 1) % mOprIDsPerFile * sizeof(OprIDEntry);
}


u64
AosOprIDCache::getOprIDFileRawfidBySeqno(const u32 ulSeqno)
{
	return eOprIDFileID_start + ulSeqno;
}


u32
AosOprIDCache::getOprIDFileSize()
{
	return mOprIDsPerFile * sizeof(OprIDEntry);
}


int
AosOprIDCache::shutdown()
{
	OmnScreen << "AosOprIDCache is shutting down..." << endl;
	int iRet = overwriteOprIDFile();
	if (0 != iRet)
	{
		OmnAlarm << "overwriteOprIDFile failed, iRet=" << iRet
				<< " rawfid:" << mActiveOprIDFile->getRawFileID()
				<< " writelen:" << mActiveOprIDFileBuffer->dataLen() << enderr;
		return -1;
	}
	OmnScreen << "AosOprIDCache has shutdown." << endl;
	return 0;
}


int
AosOprIDCache::getNextOprIDEntry(
		AosRundata	*rdata,
		OprIDEntry	&sOprIDEntry)
{
	return 0;
}


bool
AosOprIDCache::oprHasHeader(OprIDEntry &sOprIDEntry)
{
	switch (sOprIDEntry.ucType)
	{
	case eDocOperation:
		return true;
		break;
	case eSnapShot:
		return false;
		break;
	default:
		OmnAlarm << "Unkown OprType:" << sOprIDEntry.ucType << enderr;
		return true;	//to trigger more alarms
		break;
	}
	OmnShouldNeverComeHere;
	return true;	//to trigger more alarms
}
