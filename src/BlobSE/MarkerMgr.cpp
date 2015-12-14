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
// 11 Aug 2015 created by White
////////////////////////////////////////////////////////////////////////////
#include "BlobSE/MarkerMgr.h"

#include "Alarm/Alarm.h"
#include "BlobSE/Marker.h"
#include "BlobSE/RawfidRange.h"
#include "Debug/Except.h"

AosMarkerMgr::AosMarkerMgr(
		AosRundata	*rdata,
		AosBlobSE	*blobSE,
		AosRawSE	*rawSE)
{
	mMarkerFileBuff = OmnNew AosBuff(sizeof(AosMarker::MarkerFile) AosMemoryCheckerArgs);
	memset(mMarkerFileBuff->data(), 0, mMarkerFileBuff->buffLen());
	mMarkerFileBuff->setDataLen(sizeof(AosMarker::MarkerFile));
	if (rawSE->fileExists(rdata, blobSE->getConfig().siteid, blobSE->getConfig().cubeid,
			blobSE->getConfig().aseid, eMarkerFileID))
	{
		mMarkerFile = rawSE->getRawFile(rdata, blobSE->getConfig().siteid, blobSE->getConfig().cubeid,
				blobSE->getConfig().aseid, eMarkerFileID, AosRawFile::eReadWrite);
		if (mMarkerFile.isNull())
		{
			OmnAlarm << "open marker file failed, rawfid:" << eMarkerFileID << " siteid:" << blobSE->getConfig().siteid
					<< " cubeid:" << blobSE->getConfig().cubeid << " aseid:" << blobSE->getConfig().aseid << enderr;
			rdata->setErrmsg("open marker file failed.");
			OmnThrowException(rdata->getErrmsg());
			return;
		}
		int iRet = mMarkerFile->read(rdata, 0, sizeof(AosMarker::MarkerFile), mMarkerFileBuff, true);
		if (0 != iRet)
		{
			OmnAlarm << "mMarkerFile->read failed, iRet=" << iRet << " read len:"
					<< sizeof(AosMarker::MarkerFile) << enderr;
			rdata->setErrmsg("mMarkerFile->read failed.");
			OmnThrowException(rdata->getErrmsg());
			return;
		}
	}
	else
	{
		mMarkerFile = rawSE->createRawFile(rdata, blobSE->getConfig().siteid, blobSE->getConfig().cubeid,
				blobSE->getConfig().aseid, eMarkerFileID, AosMediaType::eDisk, AosRawFile::eReadWrite);
		if (mMarkerFile.isNull())
		{
			OmnAlarm << "open marker file failed, rawfid:" << eMarkerFileID << " siteid:" << blobSE->getConfig().siteid
					<< " cubeid:" << blobSE->getConfig().cubeid << " aseid:" << blobSE->getConfig().aseid << enderr;
			rdata->setErrmsg("open marker file failed.");
			OmnThrowException(rdata->getErrmsg());
			return;
		}
	}
	mMarkerFileSt = (AosMarker::MarkerFile*)mMarkerFileBuff->data();
}


AosMarkerMgr::~AosMarkerMgr()
{
}


int
AosMarkerMgr::saveMarkerToMarkerFile(
		AosRundata	*rdata,
		AosMarker	&marker)
{
	switch (marker.getType())
	{
	case AosMarker::eHeaderStartToSave:
		mMarkerFileSt->sHeaderStartToSave.sHead.ucType = AosMarker::eHeaderStartToSave;
		mMarkerFileSt->sHeaderStartToSave.sHead.ullOprID = marker.getOprID();
		mMarkerFileSt->sHeaderStartToSave.sHead.ullRawfid = marker.getRawfid();
		mMarkerFileSt->sHeaderStartToSave.sHead.ullOffset = marker.getOffset();
		break;
	case AosMarker::eHeaderSaved:
		mMarkerFileSt->sHeaderSaved.sHead.ucType = AosMarker::eHeaderSaved;
		mMarkerFileSt->sHeaderSaved.sHead.ullOprID = marker.getOprID();
		mMarkerFileSt->sHeaderSaved.sHead.ullRawfid = marker.getRawfid();
		mMarkerFileSt->sHeaderSaved.sHead.ullOffset = marker.getOffset();
		break;
	case AosMarker::eCompactionStart:
		mMarkerFileSt->sCompactionStart.sHead.ucType = AosMarker::eCompactionStart;
		mMarkerFileSt->sCompactionStart.sHead.ullOprID = marker.getOprID();
		mMarkerFileSt->sCompactionStart.sHead.ullRawfid = marker.getRawfid();
		mMarkerFileSt->sCompactionStart.sHead.ullOffset = marker.getOffset();
		break;
	case AosMarker::eCompactionFinished:
		mMarkerFileSt->sCompactionFinished.sHead.ucType = AosMarker::eCompactionStart;
		mMarkerFileSt->sCompactionFinished.sHead.ullOprID = marker.getOprID();
		mMarkerFileSt->sCompactionFinished.sHead.ullRawfid = marker.getRawfid();
		mMarkerFileSt->sCompactionFinished.sHead.ullOffset = marker.getOffset();
		break;
	case AosMarker::eStartToProcessHeader:
		mMarkerFileSt->sCompactionHeader.sHead.ucType = AosMarker::eStartToProcessHeader;
	case AosMarker::eProcessHeaderFinished:
		mMarkerFileSt->sCompactionHeader.sHead.ucType = AosMarker::eProcessHeaderFinished;
		mMarkerFileSt->sCompactionHeader.sHead.ullOprID = marker.getOprID();
		mMarkerFileSt->sCompactionHeader.sHead.ullRawfid = marker.getRawfid();
		mMarkerFileSt->sCompactionHeader.sHead.ullOffset = marker.getOffset();
		mMarkerFileSt->sCompactionHeader.ulSeqnoBeingCompacted = marker.getSeqnoBeingCompacted();
		mMarkerFileSt->sCompactionHeader.ulSeqnoCompacted = marker.getSeqnoCompacted();
		break;
	case AosMarker::eStartToProcessBody:
		mMarkerFileSt->sCompactionBody.sHead.ucType = AosMarker::eStartToProcessBody;
	case AosMarker::eProcessBodyFinished:
		mMarkerFileSt->sCompactionBody.sHead.ucType = AosMarker::eProcessBodyFinished;
		mMarkerFileSt->sCompactionBody.sHead.ullOprID = marker.getOprID();
		mMarkerFileSt->sCompactionBody.sHead.ullRawfid = marker.getRawfid();
		mMarkerFileSt->sCompactionBody.sHead.ullOffset = marker.getOffset();
		mMarkerFileSt->sCompactionBody.ullBodyRawfidBeingCompacted = marker.getBodyRawfidBeingCompacted();
		mMarkerFileSt->sCompactionBody.ullBodyRawfidCompacted = marker.getBodyRawfidCompacted();
		break;
	default:
		OmnAlarm << "Invalid marker type:" << marker.getType() << enderr;
		break;
	}
	return mMarkerFile->overwrite(rdata, mMarkerFileBuff->data(), mMarkerFileBuff->dataLen());
}


int
AosMarkerMgr::getRecoverStartLocation(
		AosRundata	*rdata,
		u64			&ullSnapshotSavedOprID,
		u64			&ullFlushDirtyHeaderOprID)
{
	/*
	 * if the snapshot marker comes before the flush dirty header marker, the opr ids between them should only be
	 * reprocessed by snapshot module
	 *         time line --->
	 * ------------*=========================*-------------------->
	 *       snapshot marker     flush dirty header marker
	 */
	if (mMarkerFileSt->sHeaderSaved.sHead.ullOprID >= mMarkerFileSt->sHeaderStartToSave.sHead.ullOprID)
	{
		ullFlushDirtyHeaderOprID = mMarkerFileSt->sHeaderSaved.sHead.ullOprID;
	}
	else
	{
		ullFlushDirtyHeaderOprID = mMarkerFileSt->sHeaderStartToSave.sHead.ullOprID;
	}
	ullSnapshotSavedOprID = 0;	//snapshot is not working right now.
	return 0;
}
