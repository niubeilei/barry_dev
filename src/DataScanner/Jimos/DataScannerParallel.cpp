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
#include "DataScanner/Jimos/DataScannerParallel.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "Util/UtUtil.h"
#include "Util/DiskStat.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDataScannerParallel_0(const AosRundataPtr &rdata, const u32 version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDataScannerParallel(version);
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


AosDataScannerParallel::AosDataScannerParallel(const u32 version)
:
AosDataScanner(AOSDATASCANNER_PARALLEL, version)
{
}


AosDataScannerParallel::AosDataScannerParallel(
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


AosDataScannerParallel::~AosDataScannerParallel()
{
	for(size_t i=0; i<mConnectors.size(); i++)
	{
		OmnDelete	mConnectors[i];
	}
}


bool
AosDataScannerParallel::config(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc)
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
	mLock = OmnNew OmnMutex(); 
	mCondVar = OmnNew OmnCondVar();
	mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
	mReqId = -1;
	mNoConnector = false;
	mCallBackNum = 0;
	mReadNum = 0;
	mDataConnectorNum = 0;
	mFinishedNum = 0;
	mReadSize = 0;
	mProcSize = 0;

	AosXmlTagPtr cube_worker_doc = worker_doc->getFirstChild("dataconnector");
	aos_assert_r(cube_worker_doc, false);
	
	mDataConnector = AosCreateDataConnector(rdata.getPtr(), cube_worker_doc);
	aos_assert_rr(mDataConnector, rdata, false);
	mDataConnector->setTaskDocid(mTaskDocid);
	mDataConnector->config(rdata, cube_worker_doc);

	mConnectorNum = worker_doc->getAttrInt("cubes_num", eDftConnectorNum);
	aos_assert_r(mConnectorNum > 0, false);

	for(int i=0; i<mConnectorNum; i++)
	{
		AosDataConnectorObjPtr cube_file = mDataConnector->nextDataConnector();
		if (!cube_file)
		{
			mLock->lock();
			mNoConnector = true;
			mLock->unlock();
			break;
		}
		cube_file->setTargetReporter(this);
		ConnectorBuff * cube_buff = OmnNew ConnectorBuff(cube_file);
		mConnectors.push_back(cube_buff);
		__sync_fetch_and_add(&mDataConnectorNum, 1);
	}

	mConnectorNum = mConnectors.size();
	mConfig = worker_doc;	// Ketty 2013/12/30
	return true;
}


int 	
AosDataScannerParallel::getPhysicalId() const
{
	return mDataConnector->getPhysicalId();
}


int64_t
AosDataScannerParallel::getTotalSize() const
{
	return mDataConnector->getFileLength();
}


AosDataScannerObjPtr
AosDataScannerParallel::clone(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosDataScannerParallel(rdata, conf);
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
AosDataScannerParallel::getNextBlock(
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
	mLock->lock();
	while (mReqIds.size() <= 0)
	{
		if (mNoConnector && mCallBackNum == mReadNum && mDataConnectorNum == mFinishedNum && mReadSize == mProcSize)
		{
			mLock->unlock();
			return true;
		}
		bool timeout = false;
		mCondVar->timedWait(mLock, timeout, 1);
	}

	int reqid = mReqIds.front();
	mReqIds.pop();
	aos_assert_rl((int)reqid < mConnectorNum, mLock, false);
	mLock->unlock();

//	mReqId = reqid;

	ConnectorBuff * cube_buff = mConnectors[reqid];

	// Chen Ding, 2015/11/10
	aos_assert_r(cube_buff->mCrtBuff->getReqId() == reqid, false);

	/*
	aos_assert_r(cube_buff, false);
	if ((cube_buff->mRemainingBuff) && !(cube_buff->mInComplete))
	{
		aos_assert_r(!cube_buff->mCrtBuff, false);
		buff_data = cube_buff->mRemainingBuff;
		buff_data->setCrtIdx(cube_buff->mOffset);
		buff_data->setReqId(reqid);
		cube_buff->mRemainingBuff = 0;
		return true;
	}
	*/

	aos_assert_r(cube_buff->mCrtBuff, false);

	if (!cube_buff->mRemainingBuff)
	{
	    buff_data = cube_buff->mCrtBuff;
		mConnectors[reqid]->mProcSize += buff_data->dataLen() - buff_data->getCrtIdx();
	}
	else
	{
		aos_assert_r(cube_buff->mRemainingBuff, false);
		aos_assert_r(cube_buff->mRemainingBuff->hasMore(), false);
		buff_data = cube_buff->mRemainingBuff;                       

		/*
		//Crystal add metadata and adjust record position            
		AosMetaDataPtr metadata = cube_buff->mCrtBuff->getMetadata();
                                                             
		int file_offset = metadata->getFileOffset();                 
		int64_t start_offset = file_offset - buff_data->dataLen();   
		metadata->setStartOffset(start_offset);                      
		*/

		//AosMetaDataPtr metadata = cube_buff->mCrtBuff->getMetadata();
		//metadata->moveToCrt();
		//buff_data->setMetadata(metadata);                            

		i64 remaining_length = buff_data->dataLen();
		AosMetaDataPtr metadata = cube_buff->mCrtBuff->getMetadata();
		metadata->moveToBack(remaining_length);
		buff_data->setMetadata(metadata);                            

		buff_data->setCrtIdx(buff_data->dataLen());
		buff_data->appendBuff(cube_buff->mCrtBuff->getBuff());
		if(cube_buff->mCrtBuff->isEOF())
		{
			buff_data->setEOF();
		}
		cube_buff->mRemainingBuff = 0;
		mConnectors[reqid]->mProcSize += cube_buff->mCrtBuff->dataLen() - cube_buff->mCrtBuff->getCrtIdx();
	}
	buff_data->setCrtIdx(cube_buff->mOffset);
	buff_data->setReqId(reqid);

	cube_buff->mCrtBuff = 0;
	
	//readData
	return true;
}


bool
AosDataScannerParallel::setRemainingBuff(
		const AosBuffDataPtr &buffdata,
		const bool incomplete,
		const int64_t proc_size,
		const AosRundataPtr &rdata)
{
	int reqid = buffdata->getReqId();
	aos_assert_r(reqid >= 0, false);
	aos_assert_r(reqid < mConnectorNum, false);
	ConnectorBuff * cube_buff = mConnectors[reqid];
	aos_assert_r(cube_buff, false);
	aos_assert_r(proc_size >= 0, false);
	cube_buff->mInComplete = incomplete;
	//cube_buff->mRemainingBuff = buffdata;

//OmnScreen << this << " setremain reqId: " << reqid << " , " << cube_buff  << endl;
	aos_assert_r(!cube_buff->mRemainingBuff, false);
	aos_assert_r(!cube_buff->mCrtBuff, false);
	if (incomplete)
	{
		if (cube_buff->mIsFinished)
		{
			OmnAlarm << " should never come here !" << enderr;
			return true;
			/*
			///get next cube
			cube_buff->mConnectorFile = mDataConnector->nextDataConnector();   
			if(cube_buff->mConnectorFile)
			{
				cube_buff->reset();
				AosDataConnectorCallerObjPtr thisptr(this, false);
				cube_buff->mConnectorFile->setTaskDocid(mTaskDocid);
				cube_buff->mConnectorFile->setCaller(thisptr);
				__sync_fetch_and_add(&mReadNum, 1);
				cube_buff->mConnectorFile->readData(reqid, rdata.getPtr());
				__sync_fetch_and_add(&mDataConnectorNum, 1);
				mLock->lock();
				mProcSize += proc_size;
				mLock->unlock();
				return true;
			}

			mLock->lock();
			mNoConnector = true;
			mProcSize += proc_size;
			mLock->unlock();
			return true;
			*/
		}
		
		// new buff. 
		int64_t offset = buffdata->getCrtIdx();
		int len = buffdata->dataLen() - offset;
		aos_assert_r(offset >= 0, false);
		aos_assert_r(len > 0, false);

		cube_buff->mRemainingBuff = OmnNew AosBuffData();
		AosBuffPtr buff = OmnNew AosBuff(eDefaultBuffSize AosMemoryCheckerArgs);
		aos_assert_r(len < buff->buffLen(), false);
		cube_buff->mRemainingBuff->setBuff(buff);
		char *data = cube_buff->mRemainingBuff->data();
		const char* data_source = buffdata->data();
		memcpy(data, &data_source[offset], len);
		cube_buff->mRemainingBuff->setDataLen(len);
		cube_buff->mRemainingBuff->setReqId(reqid);
		cube_buff->mOffset = 0;

		aos_assert_r(cube_buff->mConnectorFile, false);
		__sync_fetch_and_add(&mReadNum, 1);
		cube_buff->mConnectorFile->readData(reqid, rdata.getPtr());
//OmnScreen << this << " readData reqid : " << reqid << " , " << mCallBackNum << " , " << mReadNum << endl;
		mLock->lock();
		mProcSize += proc_size;
		mLock->unlock();
		return true;
	}

	if (buffdata->hasMore())
	{
		OmnAlarm << " should never come here !" << enderr;
		return true;
		/*
		cube_buff->mRemainingBuff = buffdata;
		cube_buff->mOffset = buffdata->getCrtIdx();

		mLock->lock();
		mReqIds.push(reqid);
		mProcSize += proc_size;
		mLock->unlock();
		return true;
		*/
	}
	
	cube_buff->mOffset = 0;
	if (cube_buff->mIsFinished)
	{
		///get next cube
		mLock->lock();
		cube_buff->mConnectorFile = mDataConnector->nextDataConnector();  
		mLock->unlock();
		if(cube_buff->mConnectorFile)
		{
			cube_buff->reset();
			AosDataConnectorCallerObjPtr thisptr(this, false);
			cube_buff->mConnectorFile->setTaskDocid(mTaskDocid);
			cube_buff->mConnectorFile->setCaller(thisptr);
			__sync_fetch_and_add(&mReadNum, 1);
			cube_buff->mConnectorFile->readData(reqid, rdata.getPtr());
			__sync_fetch_and_add(&mDataConnectorNum, 1);
			mLock->lock();
			mProcSize += proc_size;
			mLock->unlock();
			return true;
		}

		mLock->lock();
		mNoConnector = true;
		mProcSize += proc_size;
		mLock->unlock();
		return true;
	}
	aos_assert_r(cube_buff->mConnectorFile, false);
	__sync_fetch_and_add(&mReadNum, 1);
	cube_buff->mConnectorFile->readData(reqid, rdata.getPtr());
	mLock->lock();
	mProcSize += proc_size;
	mLock->unlock();
	return true;
}

bool
AosDataScannerParallel::startReadData(const AosRundataPtr &rdata)
{
	aos_assert_r(mDataConnector, false);
	AosDataConnectorCallerObjPtr thisptr(this, false);
	ConnectorBuff * cubeBuff;
	for(size_t i=0; i<mConnectors.size(); i++)
	{
		cubeBuff = mConnectors[i];
		aos_assert_r(cubeBuff->mConnectorFile, false);
		cubeBuff->mConnectorFile->setTaskDocid(mTaskDocid);
		cubeBuff->mConnectorFile->setCaller(thisptr);
		__sync_fetch_and_add(&mReadNum, 1);
		cubeBuff->mConnectorFile->readData(i, rdata.getPtr());
	}
	return true;
}

void
AosDataScannerParallel::callBack(
		const u64 &reqId,
		const AosBuffDataPtr &buffdata,
		bool isFinished)
{
	aos_assert((int)reqId < mConnectorNum);
	if (!buffdata || buffdata->dataLen() <= 0)
	{
		if(!isFinished)
		{
			OmnAlarm << " callBack error " << enderr;
			isFinished = true;
		}
	
		ConnectorBuff * cube_buff = mConnectors[reqId];
		__sync_fetch_and_add(&mFinishedNum, 1);
		cube_buff->mIsFinished = isFinished;
			
		mLock->lock();
		cube_buff->mConnectorFile = mDataConnector->nextDataConnector();   
		mLock->unlock();
		if(cube_buff->mConnectorFile)
		{
			cube_buff->reset();
			AosDataConnectorCallerObjPtr thisptr(this, false);
			cube_buff->mConnectorFile->setTaskDocid(mTaskDocid);
			cube_buff->mConnectorFile->setCaller(thisptr);
			__sync_fetch_and_add(&mReadNum, 1);
			cube_buff->mConnectorFile->readData(reqId, mRundata.getPtr());
			__sync_fetch_and_add(&mDataConnectorNum, 1);
		}
		
		mLock->lock();
		mCallBackNum++;
		mLock->unlock();
		return;
	}

	buffdata->setReqId(reqId);
	mConnectors[reqId]->mCrtBuff = buffdata;
	mConnectors[reqId]->mDataLen += buffdata->dataLen();
	if (isFinished && !mConnectors[reqId]->mIsFinished)
	{
		__sync_fetch_and_add(&mFinishedNum, 1);
		if(mConnectors[reqId]->mConnectorFile)
		{
		}
	}
	mConnectors[reqId]->mIsFinished = isFinished;

	//aos_assert((int)reqId == buffdata->getReqId());
	mLock->lock();
	mReadSize += buffdata->dataLen();
	mReqIds.push(reqId);
	mCondVar->signal();
	mCallBackNum++;
	mLock->unlock();

}



bool 
AosDataScannerParallel::serializeTo(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;

	aos_assert_rr(buff, rdata, false);
	aos_assert_rr(mDataConnector, rdata, false);
	mDataConnector->serializeTo(rdata.getPtr(), buff);

	return true;
}


bool 
AosDataScannerParallel::serializeFrom(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;

	aos_assert_rr(buff, rdata, false);
//	mStartPos = buff->getInt64(0);
	mDataConnector = AosCreateDataConnectorFromBuff(rdata.getPtr(), buff);
	aos_assert_rr(mDataConnector, rdata, false);
	return true;
}


AosJimoPtr 
AosDataScannerParallel::cloneJimo() const
{
	try
	{
		return OmnNew AosDataScannerParallel(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}
