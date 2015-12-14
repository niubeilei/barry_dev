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
// 15 May 2015 created by White
////////////////////////////////////////////////////////////////////////////
#include "BlobSE/BlobSEAPI.h"

#include "Alarm/Alarm.h"
#include "API/AosApiS.h"
#include "BlobSE/BlobHeader.h"
#include "BlobSE/BlobSE.h"
#include "DfmUtil/DfmDoc.h"
#include "UtilData/BlobSEReqEntry.h"
#include "UtilData/ModuleId.h"
#include "XmlUtil/XmlTag.h"

extern "C"
{
AosJimoPtr AosCreateJimoFunc_AosBlobSEAPI_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosBlobSEAPI(rdata, 0);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}

AosBlobSEAPI::AosBlobSEAPI(
		const AosRundataPtr	&rdata,
		const AosXmlTagPtr	&conf)
{
	mRundata = rdata;
	mConf = conf;
}

AosBlobSEAPI::AosBlobSEAPI(
		const AosRundataPtr	&rdata,
		int					cubeId,
		const u64	  		aseId,
		const u32			header_custom_data_size)
:
mLock(OmnNew OmnMutex()),
mLastAppliedOprID(0),
mLastAppendedOprID(0)
{
	mRundata = rdata;
	try
	{
		//get blobSE configuration
		AosXmlTagPtr conf = OmnApp::getAppConfig();
		aos_assert(conf);

		AosXmlTagPtr blobse_conf = conf->getFirstChild("BlobSE");
		//blobse_conf->setAttr("cubeid",getCubeId());
		blobse_conf->setAttr("cubeid", cubeId);
		blobse_conf->setAttr("aseid", aseId);
		blobse_conf->setAttr("header_custom_data_size", header_custom_data_size);

		mConf = conf;
		mBlobSE = OmnNew AosBlobSE(rdata.getPtrNoLock(),
				AosDfmDoc::cloneDoc((AosDfmDocType::E)1), conf);
	}
	catch(const OmnExcept &e)
	{
		OmnAlarm << "Failed to create AosRawFile obj: " << e.getErrmsg() << enderr;
	}
	catch(...)
	{
		OmnAlarm << "unknown exception catched during BlobSE obj creation " << enderr;
	}
	if (mBlobSE.isNull())
	{
		OmnAlarm << "Failed to create AosBlobSE obj" << enderr;
		return;
	}
	mHeaderDeque.clear();
}

AosBlobSEAPI::~AosBlobSEAPI()
{
	OmnDelete mLock;
}

AosJimoPtr
AosBlobSEAPI::cloneJimo() const
{
	// return OmnNew AosBlobSEAPI(mRundata);
    AosJimoPtr thisptr((AosBlobSEAPI*)this, false);
    return thisptr;
}

bool
AosBlobSEAPI::appendEntry(
		const AosRundataPtr	&rdata,
		const u32			ulTermID,
		const u64			ullOprID,
		const AosBuffPtr	&pBuff)
{
	if (pBuff.isNull())
	{
		OmnAlarm << "pBuff.isNull(), termid:" << ulTermID << " oprid:" << ullOprID << enderr;
		return false;
	}
	AosBlobSEReqEntryPtr pBlobSEReqEntry = OmnNew AosBlobSEReqEntry(mBlobSE->getConfig().header_custom_data_size);
	if (pBlobSEReqEntry.isNull())
	{
		OmnAlarm << "create AosBlobSEReqEntry obj failed, termid:" << ulTermID << " oprid:" << ullOprID << enderr;
		return false;
	}
	int iRet = pBlobSEReqEntry->unserializeFromBuff(pBuff);
	if (0 != iRet)
	{
		OmnAlarm << "pBlobSEReqEntry->unserializeFromBuff failed, iRet=" << iRet << enderr;
		return false;
	}
	AosBlobHeaderPtr pHeader = OmnNew AosBlobHeader(0, mBlobSE->getConfig().header_custom_data_size);
	if (pHeader.isNull())
	{
		OmnAlarm << "create AosBlobHeader obj failed, termid:" << ulTermID << " oprid:" << ullOprID << enderr;
		return false;
	}
	switch (pBlobSEReqEntry->getReqTypeAndResult())
	{
	case AosBlobSEReqEntry::eSave:
		OmnScreen << "saving doc with oprid:" << ullOprID << " docid:" << pBlobSEReqEntry->getDocid() << " this@" << this << endl;
		iRet = mBlobSE->saveDoc(rdata, ulTermID, ullOprID, pBlobSEReqEntry.getPtrNoLock(), pHeader);
		if (0 != iRet)
		{
			OmnAlarm << "mBlobSE->saveDoc failed, iRet=" << iRet << " termid:" << ulTermID << " oprid:" << ullOprID << enderr;
			return false;
		}
		pHeader->setOprType(AosBlobHeader::eDocOperation);
		break;
	case AosBlobSEReqEntry::eDelete:
		iRet = mBlobSE->deleteDoc(rdata, ulTermID, ullOprID, pBlobSEReqEntry.getPtrNoLock(), pHeader);
		if (0 != iRet)
		{
			OmnAlarm << "mBlobSE->deleteDoc failed, iRet=" << iRet << " termid:" << ulTermID << " oprid:" << ullOprID << enderr;
			return false;
		}
		pHeader->setOprType(AosBlobHeader::eDocOperation);
		break;
	case AosBlobSEReqEntry::eCreateSnapshot:
		iRet = mBlobSE->saveSnapshotOprEntry(rdata.getPtrNoLock(), ulTermID, ullOprID,
				pBlobSEReqEntry->getSnapshotID(), AosBlobHeader::eCreateSnapshot);
		if (0 != iRet)
		{
			OmnAlarm << "mBlobSE->saveSnapshotOprEntry failed, iRet=" << iRet << " oprid:" << ullOprID << enderr;
			return false;
		}
		pHeader->setSnapshotID(pBlobSEReqEntry->getSnapshotID());
		pHeader->setOprType(AosBlobHeader::eCreateSnapshot);
		break;
	case AosBlobSEReqEntry::eCommitSnapshot:
		iRet = mBlobSE->saveSnapshotOprEntry(rdata.getPtrNoLock(), ulTermID, ullOprID,
				pBlobSEReqEntry->getSnapshotID(), AosBlobHeader::eCommitSnapshot);
		if (0 != iRet)
		{
			OmnAlarm << "mBlobSE->saveSnapshotOprEntry failed, iRet=" << iRet << " termid:" << ulTermID << " oprid:" << ullOprID << enderr;
			return false;
		}
		pHeader->setSnapshotID(pBlobSEReqEntry->getSnapshotID());
		pHeader->setOprType(AosBlobHeader::eCommitSnapshot);
		break;
	case AosBlobSEReqEntry::eCancelSnapshot:
		iRet = mBlobSE->saveSnapshotOprEntry(rdata.getPtrNoLock(), ulTermID,
				ullOprID, pBlobSEReqEntry->getSnapshotID(), AosBlobHeader::eCancelSnapshot);
		if (0 != iRet)
		{
			OmnAlarm << "mBlobSE->saveSnapshotOprEntry failed, iRet=" << iRet << " termid:" << ulTermID << " oprid:" << ullOprID << enderr;
			return false;
		}
		pHeader->setSnapshotID(pBlobSEReqEntry->getSnapshotID());
		pHeader->setOprType(AosBlobHeader::eCancelSnapshot);
		break;
	case AosBlobSEReqEntry::eNotExist:
		pHeader->setOprType(AosBlobHeader::eNoOp);
		break;
	default:
		OmnAlarm << "unsupported request type:" << pBlobSEReqEntry->getReqTypeAndResult() << enderr;
		return false;
	}
	iRet = saveHeaderToCache(ullOprID, pHeader);
	if (0 != iRet)
	{
		OmnAlarm << "saveHeaderToCache failed, iRet=" << iRet << " termid:" << ulTermID << " oprid:" << ullOprID << enderr;
		return false;
	}
	return true;
}


bool
AosBlobSEAPI::getEntry(
		const AosRundataPtr	&rdata,
		const u64			ullOprID,
		u32					&ulTermID,
		AosBuffPtr			&pBuff)
{
	if (pBuff.isNull())
	{
		OmnAlarm << "pBuff.isNull()" << enderr;
		return false;
	}
	mLock->lock();
	if (ullOprID > mLastAppendedOprID)
	{
		mLock->unlock();
		OmnAlarm << "ullOprID:" << ullOprID << " > mLastAppendedOprID:" << mLastAppendedOprID << enderr;
		return false;
	}
	mLock->unlock();
	return 0 == mBlobSE->getEntryByOprID(rdata, ullOprID, ulTermID, pBuff);
}


bool
AosBlobSEAPI::apply(
		const AosRundataPtr	&rdata,
		const u64			ullOprID)
{
	AosBlobHeaderPtr pHeader = OmnNew AosBlobHeader(0, mBlobSE->getConfig().header_custom_data_size);
	int iRet = 0;

	//OmnScreen << "enter blobseapi apply. ullOprID =  " << ullOprID << " this@" << this << endl;
	iRet = getHeaderFromCacheByOprID(ullOprID, pHeader);
	if (0 != iRet)
	{
		OmnAlarm << "getHeaderFromCacheByOprID failed, iRet=" << iRet << " oprid:" << ullOprID << enderr;
		return false;
	}
	//pHeader->dump();
	switch (pHeader->getOprType())
	{
	case AosBlobHeader::eDocOperation:
		iRet = mBlobSE->updateHeader(rdata.getPtrNoLock(), pHeader);
		if (0 != iRet)
		{
			OmnAlarm << "mBlobSE->updateHeader failed, iRet=" << iRet << " oprid:" << ullOprID << enderr;
			return false;
		}
		break;

	case AosBlobHeader::eCreateSnapshot:
		iRet = mBlobSE->createSnapshot(rdata, pHeader->getSnapshotID());
		if (0 != iRet)
		{
			OmnAlarm << "mBlobSE->createSnapshot failed, iRet=" << iRet << " snap id:" << pHeader->getSnapshotID() << enderr;
			return false;
		}
		break;

	case AosBlobHeader::eCommitSnapshot:
		iRet = mBlobSE->commitSnapshot(rdata, pHeader->getSnapshotID());
		if (0 != iRet)
		{
			OmnAlarm << "mBlobSE->commitSnapshot failed, iRet=" << iRet << " snap id:" << pHeader->getSnapshotID() << enderr;
			return false;
		}
		break;

	case AosBlobHeader::eCancelSnapshot:
		iRet = mBlobSE->cancelSnapshot(rdata, pHeader->getSnapshotID());
		if (0 != iRet)
		{
			OmnAlarm << "mBlobSE->cancelSnapshot failed, iRet=" << iRet << " snap id:" << pHeader->getSnapshotID() << enderr;
			return false;
		}
		break;

	case AosBlobHeader::eNoOp:
		OmnScreen << "applying no-op operation by only printing log, logid:" << ullOprID << endl;
		break;

	default:
		OmnAlarm << "unsupported request type:" << pHeader->getOprType() << enderr;
		return false;
	}
	return true;
}


bool
AosBlobSEAPI::readDoc(
		const AosRundataPtr	&rdata,
		const u64			ullAseID,		//TODO:place holder
		const u64			ullSnapID,
		const u64 			ullDocid,
		const u64 			ullTimestamp,
		AosBuffPtr			&pBodyBuff,
		AosBuffPtr			&pHeaderCustomData)
{
//	if (pBuff.isNull())
//	{
//		OmnAlarm << "pBuff.isNull()" << enderr;
//		return false;
//	}
	AosBlobSEReqEntryPtr pBlobSEReqEntry = OmnNew AosBlobSEReqEntry(mBlobSE->getConfig().header_custom_data_size);
	if (pBlobSEReqEntry.isNull())
	{
		OmnAlarm << "create AosBlobSEReqEntry obj failed" << enderr;
		return false;
	}
//	int iRet = pBlobSEReqEntry->unserializeFromBuff(pBuff);
//	if (0 != iRet)
//	{
//		OmnAlarm << "pBlobSEReqEntry->unserializeFromBuff failed, iRet=" << iRet << enderr;
//		return -3;
//	}
//	if (AosBlobSEReqEntry::eRead != pBlobSEReqEntry->getReqTypeAndResult())
//	{
//		OmnAlarm << "AosBlobSEReqEntry::eRead != pBlobSEReqEntry->getReqType():" << pBlobSEReqEntry->getReqTypeAndResult() << enderr;
//	}
	//OmnScreen << "trying to read docid:" << ullDocid << " this@" << this << endl;
	pBlobSEReqEntry->setSnapshotID(ullSnapID);
	pBlobSEReqEntry->setDocid(ullDocid);
	pBlobSEReqEntry->setTimestamp(ullTimestamp);
	bool bNeedsSuppressReadError = needsSuppressReadError(ullAseID);
	int iRet = mBlobSE->readDoc(rdata, pBlobSEReqEntry.getPtrNoLock());
	if (0 != iRet)
	{
		if (!bNeedsSuppressReadError)
		{
			OmnAlarm << "mBlobSE->readDoc failed, iRet=" << iRet << " docid:" << pBlobSEReqEntry->getDocid()
					<< " timestamp:" << ullTimestamp << enderr;
			return false;
		}
		else
		{
			OmnScreen << "mBlobSE->readDoc failed, iRet=" << iRet << " docid:" << pBlobSEReqEntry->getDocid()
					<< " timestamp:" << ullTimestamp << endl;
			return true;
		}
	}
	pBodyBuff = pBlobSEReqEntry->getBodyBuff();
	pHeaderCustomData = pBlobSEReqEntry->getHeaderCustomDataBuff();
//	iRet = pBlobSEReqEntry->serializeToBuff(pBuff);
//	if (0 != iRet)
//	{
//		OmnAlarm << "pBlobSEReqEntry->serializeToBuff failed, iRet=" << iRet << enderr;
//		return -5;
//	}
	return true;
}


int
AosBlobSEAPI::saveHeaderToCache(
		const u64				ullOprID,
		const AosBlobHeaderPtr&	pHeader)
{
	if (pHeader.isNull())
	{
		OmnAlarm << "pHeader.isNull(), oprid:" << ullOprID << enderr;
		return -1;
	}
	mLock->lock();
	if (ullOprID != mLastAppendedOprID + 1)
	{
		mLock->unlock();
		OmnAlarm << "ullOprID:" << ullOprID << " != mLastAppendedOprID:" << mLastAppendedOprID << " + 1" << enderr;
		return -2;
	}
	pHeader->setOprID(ullOprID);
	mLastAppendedOprID = ullOprID;
	mHeaderDeque.push_back(pHeader);
	mLock->unlock();
	return 0;
}


int
AosBlobSEAPI::getHeaderFromCacheByOprID(
		const u64			ullOprID,
		AosBlobHeaderPtr&	pHeader)
{
	if (pHeader.isNull())
	{
		OmnAlarm << "pHeader.isNull(), oprid:" << ullOprID << enderr;
		return -1;
	}
	mLock->lock();
	if (ullOprID != mLastAppliedOprID + 1)
	{
		mLock->unlock();
		OmnAlarm << "ullOprID:" << ullOprID << " != mLastAppliedOprID:" << mLastAppliedOprID << " + 1" << enderr;
		return -2;
	}
	if (mLastAppliedOprID + 1 > mLastAppendedOprID)
	{
		mLock->unlock();
		OmnAlarm << "mLastAppliedOprID:" << mLastAppliedOprID << " + 1 > mLastAppendedOprID:" << mLastAppendedOprID << enderr;
		return -3;
	}
	mLastAppliedOprID = ullOprID;
	pHeader = mHeaderDeque.front();
	if (pHeader->getOprID() != ullOprID)
	{
		OmnAlarm << "pHeader->getOprID():" << pHeader->getOprID() << " != ullOprID:" << ullOprID << enderr;
		mLock->unlock();
		return -4;
	}
	mHeaderDeque.pop_front();
	mLock->unlock();
	return 0;
}


int
AosBlobSEAPI::doCompationAsRaftLeader()
{
	return mBlobSE->doCompationAsRaftLeader();
}


int
AosBlobSEAPI::shutdown(const AosRundataPtr	&rdata)
{
	return mBlobSE->shutdown(rdata.getPtrNoLock());
}


bool
AosBlobSEAPI::genBlobSEData(
		AosBuffPtr &buff,
		const u64 snap_id,
		const u64 docid,
		const AosBlobSEReqEntry::ReqTypeAndResult reqType,
		const AosBuffPtr &docstr,
		AosBuffPtr pCustomeData)
{
	aos_assert_r(buff, false);
	AosBlobSEReqEntryPtr req;
	if (pCustomeData.notNull())
	{
		req = OmnNew AosBlobSEReqEntry(pCustomeData->dataLen());
	}
	else
	{
		req = OmnNew AosBlobSEReqEntry(0);
	}
	u64 ullTimestamp = OmnTime::getTimestamp();
	req->setTimestamp(ullTimestamp);
	//hardcode logID to 1 for now
	req->setOprID(1);	//FIXME:can not hard code this
	req->setSnapshotID(snap_id);
	req->setDocid(docid);
	//req->setReqTypeAndResult(AosBlobSEReqEntry::eSave);
	req->setReqTypeAndResult(reqType);
	switch(reqType)
	{
		case AosBlobSEReqEntry::eSave:
			//set header data
			req->setHeaderCustomDataBuff(pCustomeData);

			//set body data
			req->setBodyBuff(docstr);
			break;

		case AosBlobSEReqEntry::eDelete:
			break;

		default:
			break;
	}
	//generate the final data buff
	int iRet = req->serializeToBuff(buff);
	if (0 != iRet)
	{
		OmnAlarm << "req->serializeToBuff failed, iRet=" << iRet << enderr;
		return false;
	}
	return true;
}


bool
AosBlobSEAPI::removeEntry(
		const AosRundataPtr	&rdata,
		const u64			ullOprID)
{
	mLock->lock();
	if (ullOprID != mLastAppendedOprID)
	{
		mLock->unlock();
		OmnAlarm << "ullOprID:" << ullOprID << " != mLastAppendedOprID:" << mLastAppendedOprID << enderr;
		return false;
	}
	AosBlobHeaderPtr pHeader = mHeaderDeque.back();
	if (pHeader->getOprID() != ullOprID)
	{
		mLock->unlock();
		OmnAlarm << "pHeader->getOprID():" << pHeader->getOprID() << " != ullOprID:" << ullOprID << enderr;
		return false;
	}
	int iRet = mBlobSE->removeEntry(rdata.getPtrNoLock(), pHeader);
	if (0 != iRet)
	{
		mLock->unlock();
		OmnAlarm << "removeEntry failed, iRet=" << iRet << enderr;
		pHeader->dump();
		return false;
	}
	mLastAppendedOprID--;
	mHeaderDeque.pop_back();
	mLock->unlock();
	return true;
}


bool
AosBlobSEAPI::init(
		const AosRundataPtr&	rdata,
		const u64				ullLastAppliedOprID)
{
	mLastAppliedOprID = ullLastAppliedOprID;
	int iRet = mBlobSE->init(rdata.getPtrNoLock(), ullLastAppliedOprID);
	if (0 != iRet)
	{
		OmnAlarm << "blobse init failed, iRet=" << iRet << " last applied oprid:" << ullLastAppliedOprID << enderr;
		return false;
	}
	mLastAppendedOprID = mBlobSE->getLastAppendOprIDFromActiveLogFile();
	OmnScreen << "ullLastAppliedOprID:" << ullLastAppliedOprID << " mLastAppendedOprID:" << mLastAppendedOprID << endl;
	iRet = recoverHeaders(rdata);
	if (0 != iRet)
	{
		OmnAlarm << "generateHeaders failed, iRet=" << iRet << enderr;
		return false;
	}
	return true;
}


int
AosBlobSEAPI::recoverHeaders(const AosRundataPtr& rdata)
{
	int iRet = 0;
	AosBuffPtr pBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	AosBlobHeaderPtr pHeader;
	for (u64 ullOprID = mLastAppliedOprID + 1; ullOprID <= mLastAppendedOprID; ullOprID++)
	{
		iRet = mBlobSE->generateHeaderForOprID(rdata.getPtrNoLock(), ullOprID, pHeader);
		if (0 != iRet)
		{
			OmnAlarm << "mBlobSE->generateHeaderForOprID failed, iRet=" << iRet << " oprid:" << ullOprID << enderr;
			return -1;
		}
		if (pHeader->getOprID() != ullOprID)
		{
			OmnAlarm << "pHeader->getOprID():" << pHeader->getOprID() << " != ullOprID:" << ullOprID << enderr;
			return -2;
		}
		mHeaderDeque.push_back(pHeader);
	}
	return 0;
}


bool
AosBlobSEAPI::needsSuppressReadError(u64 eAseID)
{
	switch (eAseID)
	{
	case AosModuleId::eDoc:
	case AosModuleId::eIIL:
	case AosModuleId::eConfigMgr:
	case AosModuleId::eAccessRcd:
	case AosModuleId::eBinaryDoc:
	case AosModuleId::eGroupDoc:
	case AosModuleId::eLocalBitmapStore:
	case AosModuleId::eBitmapTree:
	case 100:
		return true;
		break;
	default:
		break;
	}
	return false;
}


OmnString
AosBlobSEAPI::getBaseDir()
{
	return mBlobSE->getBaseDir();
}
