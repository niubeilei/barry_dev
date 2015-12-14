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
// This data scanner has only one file. Normally the file resides on the 
// local machine, but it is also possible that the file is remote.
//
// Modification History:
// 2013/11/21: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataScanner/Jimos/DataScannerConnector.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "Util/UtUtil.h"
#include "Util/DiskStat.h"
#include "Debug/Debug.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDataScannerConnector_0(const AosRundataPtr &rdata, const u32 version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDataScannerConnector(version);
		aos_assert_rr(jimo, rdata, 0);
		rdata->setJimo(jimo);
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


AosDataScannerConnector::AosDataScannerConnector(const u32 version)
:
AosDataScanner(AOSDATASCANNER_CUBE, version)
{
}


AosDataScannerConnector::AosDataScannerConnector(
		const AosRundataPtr &rdata, 
		const OmnString &objid)
:
AosDataScanner(AOSDATASCANNER_CUBE, 0)
{
	if (!initFile(rdata, objid))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosDataScannerConnector::~AosDataScannerConnector()
{
}

/*
bool
AosDataScannerConnector::initFile(
		const AosRundataPtr &rdata,
		const OmnString &objid)
{
	AosXmlTagPtr doc = AosGetDocByObjid(objid, rdata);
	aos_assert_rr(doc, rdata, false);
	//return initFile(rdata, doc);
	return config(rdata, doc);
}
*/

bool
AosDataScannerConnector::config(const AosRundataPtr &rdata, const AosXmlTagPtr &worker_doc)
{
	//	<scanner type="file" ...>
	//		<data_cube ...
	//			zky_physicalid="xxxx"
	//			zky_readsize="xxxx"
	//			zky_record_len="xxxx"
	//			zky_max_records="xxxx"
	//			zky_max_tasks="xxxx"
	//			zky_fullname="../data/voice.txt"
	//			zky_startpos="0"
	//			zky_length="-1">
	//		</data_cube>
	//		path
	// </scanner>
	
	aos_assert_rr(worker_doc, rdata, false);
//	mStartPos = worker_doc->getAttrInt("start_pos", 0);
//	mReadBlockSize = worker_doc->getAttrInt64("read_block_size", eDftReadBlockSize);
	mLock = OmnNew OmnMutex(true); 
	mLock2 = OmnNew OmnMutex(true); 
	mRundata = rdata->clone(AosMemoryCheckerArgsBegin); 
	mCondVar = OmnNew OmnCondVar();
	mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
	mInComplete = false;
	mIsFinished = false;
	mOffset = 0;
	mRemainingBuff = 0;
	mSeqno = 0;
	mCallBackNum = 0;
	mCrtSeqno = 0;
	//mBuffCacheNum = worker_doc->getAttrInt("buff_cache_num", eDftBuffCacheNum);
	mBuffCacheNum = 1;//worker_doc->getAttrInt("buff_cache_num", eDftBuffCacheNum);

	AosXmlTagPtr cube_worker_doc = worker_doc->getFirstChild("dataconnector");
	OmnTagFuncInfo << " datacube is: " << cube_worker_doc->toString() << endl; 
	aos_assert_r(cube_worker_doc, false);
	mDataConnector = AosCreateDataConnector(rdata.getPtr(), cube_worker_doc);
	if (!mDataConnector)
	{
		mDiskError = true;
		return false;
	}
	aos_assert_rr(mDataConnector, rdata, false);

	//Jozhi 2015/08/22
	AosDataConnectorCallerObjPtr thisptr(this, false);
	mDataConnector->setCaller(thisptr);

	mDataConnector->setTaskDocid(mTaskDocid);
	mDataConnector->config(rdata, cube_worker_doc);


	mConfig = worker_doc;	// Ketty 2013/12/30
	return true;
}


int 	
AosDataScannerConnector::getPhysicalId() const
{
	return mDataConnector->getPhysicalId();
}


int64_t
AosDataScannerConnector::getTotalSize() const
{
	return mDataConnector->getFileLength();
}


AosDataScannerObjPtr
AosDataScannerConnector::clone(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosDataScannerConnector(rdata, conf);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object: " << conf->toString() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool
AosDataScannerConnector::getNextBlock(
		AosBuffDataPtr &buff_data,
		const AosRundataPtr &rdata)
{
	// It reads in the next block. The scanner has a number of files. 
	// 'mStartPos' is the current position from which the next read starts reading
	// 'mFileLength' is the size of the current file. 
	// 'mReadBlockSize' is the read block size.
	//
	// If the current file was completely read, it moves on to the next file.
	// If no more file to read, it returns null. 
	// If the current file has some contents to read, it reads either 
	// the remaining contents up to 'mReadBlockSize'. 
	
	buff_data = 0;
	mLock2->lock();
	mLock->lock();
OmnTagFuncInfo << "data getnextblock mSeqno : " << mSeqno << " mCrtSeqno : " << mCrtSeqno << " mCallBackNum : " << mCallBackNum << " quesize : " << mBuffQueue.size() << endl;
	if (mRemainingBuff && !mInComplete)
	{
		buff_data = mRemainingBuff;
		mRemainingBuff = 0;
		buff_data->setCrtIdx(mOffset);
		mLock->unlock();
OmnTagFuncInfo << mInComplete << " mSeqno : " << mSeqno << " mCrtSeqno : " << mCrtSeqno << " mCallBackNum : " << mCallBackNum << endl;
		return true;
	}
	
	int num_tries = 1000;
	while (mBuffQueue.size() <= 0)
	{
		if (mIsFinished && mSeqno == mCallBackNum) 
		{
			mLock->unlock();
			mLock2->unlock();
OmnTagFuncInfo << mInComplete << " mSeqno : " << mSeqno << " mCrtSeqno : " << mCrtSeqno << " mCallBackNum : " << mCallBackNum << endl;
			return true;
		}

		// Ken Lee,2014/08/16
		if (num_tries-- <= 0)
		{
			mLock->unlock();
			mLock2->unlock();
			OmnAlarm << "trys error" << enderr;
			return false;
		}

		bool timeout = false;
		mCondVar->timedWait(mLock, timeout, 1);
		if (mRemainingBuff && !mInComplete)
		{
			buff_data = mRemainingBuff;
			mRemainingBuff = 0;
			buff_data->setCrtIdx(mOffset);
			mLock->unlock();
			OmnTagFuncInfo << mInComplete << " mSeqno : " << mSeqno << " mCrtSeqno : " << mCrtSeqno << " mCallBackNum : " << mCallBackNum << endl;
			return true;
		}
	}
	
	SortedBuff sb = mBuffQueue.top();
	while (1)
	{
		if (mCrtSeqno == sb.mSeqno) break;

		bool timeout = false;
		mCondVar->timedWait(mLock, timeout, 1);

		if (mRemainingBuff && !mInComplete)
		{
			buff_data = mRemainingBuff;
			mRemainingBuff = 0;
			buff_data->setCrtIdx(mOffset);
			mLock->unlock();
			OmnTagFuncInfo << mInComplete << " mSeqno : " << mSeqno << " mCrtSeqno : " << mCrtSeqno << " mCallBackNum : " << mCallBackNum << endl;
			return true;
		}


		if (mBuffQueue.size() <= 0)
		{
			aos_assert_rl(mIsFinished, mLock, false);
			aos_assert_rl(mSeqno == mCallBackNum, mLock, false);
			mLock->unlock();
			mLock2->unlock();
			return true;
		}

		sb = mBuffQueue.top();
	}
	
	mCrtSeqno++;
	mBuffQueue.pop();
	if (!mIsFinished)
	{
		//Jozhi 2015/08/22
		AosDataConnectorCallerObjPtr thisptr(this, false);
		mDataConnector->setCaller(thisptr);

		mDataConnector->readData(mSeqno++, rdata.getPtr());
	}

	AosBuffDataPtr buffdata = sb.mBuffData;
	if (!mRemainingBuff)
	{
	    buff_data = buffdata;
OmnTagFuncInfo << mInComplete << " mSeqno : " << mSeqno << " mCrtSeqno : " << mCrtSeqno << " mCallBackNum : " << mCallBackNum << endl;
	}
	else
	{
		aos_assert_rl(mInComplete, mLock, false);
		aos_assert_rl(mRemainingBuff, mLock, false);
		aos_assert_rl(mRemainingBuff->hasMore(), mLock, false);

		//OmnString ss(mRemainingBuff->data(), mRemainingBuff->dataLen());
		//OmnScreen << "ken data set mRemainingBuff last data:" << ss << endl;
	
		//Jackie add metadata and adjust record position
		AosMetaDataPtr metadata = buffdata->getMetadata();
	//	int file_offset = metadata->getFileOffset();
	//	int64_t start_offset = file_offset - mRemainingBuff->dataLen();
	//	metadata->setStartOffset(start_offset);
		metadata->moveToCrt();
		mRemainingBuff->setMetadata(metadata);

		mRemainingBuff->setCrtIdx(mRemainingBuff->dataLen());
		mRemainingBuff->appendBuff(buffdata->getBuff());
		if (buffdata->isEOF())
		{
			mRemainingBuff->setEOF();

			//OmnScreen << "ken data set mRemainingBuff EOF true, queue size:" << mBuffQueue.size() << endl;
		}
		buff_data = mRemainingBuff;
		mRemainingBuff = 0;
OmnTagFuncInfo << mInComplete << " mSeqno : " << mSeqno <<  " mCrtSeqno : " << mCrtSeqno << " mCallBackNum : " << mCallBackNum << endl;
	}
	aos_assert_rl(!mOffset, mLock, false);
	buff_data->setCrtIdx(mOffset);

	mLock->unlock();
//	OmnString ss(buff_data->getBuff()->data(), 80);
//	OmnTagFuncInfo << ss << endl;
	return true;
}


bool
AosDataScannerConnector::setRemainingBuff(
		const AosBuffDataPtr &buffdata,
		const bool incomplete,
		const int64_t proc_size,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	mInComplete = incomplete;

	aos_assert_rl(!mRemainingBuff, mLock, false);
	if (incomplete)
	{
		//OmnScreen << "ken data, incomplete is true" << endl;

		if (buffdata->isEOF())
		{
			mLock->unlock();
			mLock2->unlock();
			return true;
		}
		
		// new buff. 
		int64_t offset = buffdata->getCrtIdx();
		int len = buffdata->dataLen() - offset;
		aos_assert_rl(offset >= 0, mLock, false);
		aos_assert_rl(len > 0, mLock, false);

		mRemainingBuff = OmnNew AosBuffData();
		AosBuffPtr buff = OmnNew AosBuff(eDefaultBuffSize AosMemoryCheckerArgs);
		aos_assert_rl(len < buff->buffLen(), mLock, false);
		buff->setBuff(buffdata->data()+ offset, len);
		buff->reset();
		mRemainingBuff->setBuff(buff);
		mRemainingBuff->setDataLen(len);
		mOffset = 0;
//OmnScreen << " 11111111111111 " << mInComplete << " mSeqno : " << mSeqno << " mCrtSeqno : " << mCrtSeqno << " mCallBackNum : " << mCallBackNum << endl;
		mLock->unlock();
		mLock2->unlock();

		//OmnString ss(data, len);
		//OmnScreen << "ken data, last mRemainingBuff:" << ss << endl;
		return true;
	}

	if (buffdata->hasMore())
	{
		mRemainingBuff = buffdata;
		mOffset = buffdata->getCrtIdx();
//OmnScreen << " 22222222222222 " << mInComplete << " mSeqno : " << mSeqno << " mCrtSeqno : " << mCrtSeqno << " mCallBackNum : " << mCallBackNum << endl;
		
		mLock->unlock();
		mLock2->unlock();
		return true;
	}

//OmnScreen << " 33333333333333 " << mInComplete << " mSeqno : " << mSeqno << " mCrtSeqno : " << mCrtSeqno << " mCallBackNum : " << mCallBackNum << endl;
	
	mOffset = 0;
	mLock->unlock();
	mLock2->unlock();
	return true;
}

bool
AosDataScannerConnector::startReadData(const AosRundataPtr &rdata)
{
	aos_assert_r(mDataConnector, false);
	
	AosDataConnectorCallerObjPtr thisptr(this, false);
	mDataConnector->setCaller(thisptr);
	mDataConnector->start(rdata.getPtr());

	for(int i=0; i<mBuffCacheNum; i++)
	{
		mLock->lock();
		u64 seqno = mSeqno++;
		mLock->unlock();

		OmnTagFuncInfo << " read data from datacube" << endl; 
		mDataConnector->readData(seqno, rdata.getPtr());
	}
	return true;
}

void
AosDataScannerConnector::callBack(
		const u64 &reqId,
		const AosBuffDataPtr &buffdata,
		bool isFinished)
{
	if (!buffdata || buffdata->dataLen() <= 0)
	{
		mLock->lock();
		if (isFinished && !mIsFinished)
		{
			mIsFinished = isFinished;
		}
		mCallBackNum++;
	//OmnScreen << "ken data callback, reqId:" << reqId << ", mCallBackNum" << mCallBackNum << ",queue size:" << mBuffQueue.size() <<  " mIsFinished : " << mIsFinished << endl; 
		mLock->unlock();
		return;
	}

	mLock->lock();
	aos_assert_l((int)reqId >= mCrtSeqno, mLock);
	SortedBuff sb(buffdata, reqId);
	mBuffQueue.push(sb);
	if (isFinished && !mIsFinished)
	{
		mIsFinished = isFinished;
	}
	mCallBackNum++;
	mCondVar->signal();

	//OmnScreen << "ken data callback, reqId:" << reqId << ", data len:" << buffdata->dataLen()
	//	<< ", isEOF" << buffdata->isEOF() << ",queue size:" << mBuffQueue.size() << endl; 

	mLock->unlock();

}


bool 
AosDataScannerConnector::serializeTo(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(buff, rdata, false);
	aos_assert_rr(mDataConnector, rdata, false);
//	buff->setU32(mStartPos);
	mDataConnector->serializeTo(rdata.getPtr(), buff);

	return true;
}


bool 
AosDataScannerConnector::serializeFrom(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(buff, rdata, false);
//	mStartPos = buff->getInt64(0);
	mDataConnector = AosCreateDataConnectorFromBuff(rdata.getPtr(), buff);
	aos_assert_rr(mDataConnector, rdata, false);
	return true;
}

bool
AosDataScannerConnector::setValueBuff(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	return mDataConnector->setValueBuff(buff, rdata.getPtr());
}


AosJimoPtr 
AosDataScannerConnector::cloneJimo() const
{
	try
	{
		return OmnNew AosDataScannerConnector(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}

bool 
AosDataScannerConnector::reset(const AosRundataPtr &rdata)
{
	mInComplete = false;
	mIsFinished = false;
	mOffset = 0;
	mRemainingBuff = 0;
	mSeqno = 0;
	mCallBackNum = 0;
	mCrtSeqno = 0;

	AosXmlTagPtr cube_worker_doc = mConfig->getFirstChild("dataconnector");
	OmnTagFuncInfo << " datacube is: " << cube_worker_doc->toString() << endl; 
	aos_assert_r(cube_worker_doc, false);
	mDataConnector = AosCreateDataConnector(rdata.getPtr(), cube_worker_doc);
	aos_assert_rr(mDataConnector, rdata, false);
	mDataConnector->setTaskDocid(mTaskDocid);
	mDataConnector->config(rdata, cube_worker_doc);

	return true;
}
