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
// result return mode:
// a. if 'mCallBack' is  "true"  send a data to client 
//
// b. if 'mCallBack is 'false"   returns the data to  the caller
//
// Modification History:
// 10/19/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DocServer/DocBatchReader.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApiG.h"
#include "DocServer/Ptrs.h"
#include "DocServer/DocReader.h"
#include "DocServer/DocDelete.h"
#include "DocServer/DocBatchReaderMgr.h"
#include "DocServer/DocBatchReaderReq.h"
#include "Porting/Sleep.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/BitmapObj.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadPool.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/Buff.h"


static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("doc_reader", __FILE__, __LINE__);


AosDocBatchReader::AosDocBatchReader(
		const OmnString &scanner_id,
		const AosDocBatchReaderReqPtr &request,
		const AosRundataPtr &rdata)
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mThread(0),
mRundata(0),
mScannerId(scanner_id),
mFinished(false),
mCrtVidIdx(0),
mTotalReceivedSize(0),
mCallBack(false),
mCallBackServerId(-1),
mNumCallBackDataMsgs(0)
{
	mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
	mBlockSize = request->block_size;

	mQueueSize = request->queue_size <= eMaxSize ? request->queue_size:eMaxSize; 
	mReadPolicy = toEnum(request->read_policy_str);
	if (!isValid(mReadPolicy))
	{
		mReadPolicy = eAllClientsReady;
	}

	mBatchType = toEnumByBatchType(request->batch_type_str);
	if (!isValidByBatchType(mBatchType))
	{
		mBatchType = eBatchRead; 
	}

	if (mBatchType == eBatchDelete)
	{
		mBlockSize = 0;
		mQueueSize = 0;
	}

	// <fnames>
	// <fname zky_data_type= "string" type="attr|text|xml" />
	// <fname zky_data_type= "string" type="attr|text|xml" />
	//  .....
	// </fnames>
	AosXmlTagPtr fnames = request->field_names;
	if (!fnames)
	{
		FieldDef field;
		field.type = eXml;
		mFields.push_back(field);
	}
	else
	{
		AosXmlTagPtr child = fnames->getFirstChild();
		aos_assert(child);
		FieldType type = toEnumField(child->getAttrStr(AOSTAG_TYPE)); 
		while(child)
		{
			FieldDef field;
			if (type == eXml)
			{
				field.type = type;
				mFields.push_back(field);
			}
			else
			{
				field.type = toEnumField(child->getAttrStr(AOSTAG_TYPE));
			 	OmnString data_type = child->getAttrStr(AOSTAG_DATA_TYPE, "");
				field.data_type = AosDataType::toEnum(data_type);
				field.fname = child->getNodeText();
				if (field.fname != "")
				{
					mFields.push_back(field);
				}
			}
			child = fnames->getNextChild();
		}
	}
}


AosDocBatchReader::~AosDocBatchReader()
{
	AosDocBatchReaderMgr::getSelf()->readerDestroyed(mTotalReceivedSize);
}


bool
AosDocBatchReader::start()
{
	OmnThreadedObjPtr thisptr(this, false);                        
	mThread = OmnNew OmnThread(thisptr, "docbatchreader", 0, false, true, __FILE__, __LINE__);
	mThread->start();
	return true;
}


bool
AosDocBatchReader::signal(const int threadLogicId)
{
	return true;
}


bool
AosDocBatchReader::checkThread(OmnString &err,	const int thrdLogicId) const
{
	return true;
}



bool
AosDocBatchReader::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if (mFinished)
		{
			OmnScreen <<"scanner_id : " << mScannerId << " finish finish finish !" << endl;
			//state = OmnThrdStatus::eStop;
			state = OmnThrdStatus::eExit;
			mLock->unlock();
			return true;
		}

//		if (mDataQueue.size() >= mQueueSize)
//		{
//			mCondVar->wait(mLock);
//			mLock->unlock();
//			continue;
//		}

		switch (mReadPolicy)
		{
		case eAllClientsReady:
			{
				 if (!isReadyToReadLocked())
				 {
					 // Not ready to read yet. Need to wait.
					 mCondVar->wait(mLock);
					 //thread->wait();
					 mLock->unlock();
					 continue;
				 }
				 switch (mBatchType)
				 {
				 case eBatchRead:
					  {
						    if (mDataQueue.size() >= mQueueSize)
							{
								mCondVar->wait(mLock);
								mLock->unlock();
								continue;
							}

				 	  		int num_blocks = mQueueSize - mDataQueue.size();
					 		readDocsLocked(num_blocks, mRundata);
							break;
					  }
				 case eBatchDelete:
					  {
						  	deleteDocsLocked(mRundata);
						  	break;
					  }
				 default:
					  break;
				 }
				 mCondVar->signal();
				 mLock->unlock();
				 continue;
			}

		default:
			 break;
		}

		OmnAlarm << "Incorrect read policy: " << mReadPolicy << enderr;
		OmnSleep(5);
		mLock->unlock();
	}
	return true;
}


bool
AosDocBatchReader::readDocsLocked(
		const int &num_blocks,
		const AosRundataPtr &rdata)
{
	aos_assert_r(num_blocks > 0, false);
	aos_assert_r(mBlockSize > 0, false);
	aos_assert_r(mScannerId != "", false);
	AosDocReaderPtr runner;
	vector<OmnThrdShellProcPtr> runners;
	u32 init_vid_idx = mCrtVidIdx;

	vector<u32> total_vids;
	AosGetLocalVirtualIds(total_vids);
	while (1)
	{
		u32 virtual_id = total_vids[mCrtVidIdx];
		if (mBitmaps.count(virtual_id) != 0 && mBitmaps[virtual_id])
		{
			runner = OmnNew AosDocReader(
					virtual_id, mScannerId, mBlockSize, 
					mBitmaps[virtual_id], mIndex[virtual_id], 
					mFields, rdata);
			runners.push_back(runner);

			if (runners.size() >= (u32)num_blocks) break;
		}

		mCrtVidIdx ++;
		if (mCrtVidIdx >= total_vids.size()) mCrtVidIdx = 0; 
		if (mCrtVidIdx == init_vid_idx) break;
	}

	mCrtVidIdx ++;
	if (mCrtVidIdx >= total_vids.size()) mCrtVidIdx = 0; 
	if (runners.size() <= 0) return true;

	mLock->unlock();

	aos_assert_r(sgThreadPool, false);
	sgThreadPool->procSync(runners);
	aos_assert_r(runners.size() <= (u32)num_blocks, false);

	mLock->lock();
	for (u32 i = 0; i < runners.size(); i++)
	{
		runner = (AosDocReader *)runners[i].getPtr();
		u32 vid = runner->getVirtualId();
		if (mIndex.count(vid) != 0)
		{
			mIndex[vid]= runner->getBitmapIndex();
			aos_assert_r(mBitmaps.count(vid) != 0, false);
			if (mIndex[vid] < 0) mBitmaps[vid] = 0;
		}

		AosBuffPtr buff = runner->getBuff();
		aos_assert_r(buff, false);
		if (buff->dataLen() <= 0) continue; 
		mDataQueue.push(buff);

		callBackClientLocked(rdata);
	}
	return true;
}


bool
AosDocBatchReader::isReadyToReadLocked()
{
	return allFinishedLocked();	
}


bool
AosDocBatchReader::allFinishedLocked()
{
	if (mClientMap.size() <= 0)
	{
		return true;
	}
	return false;
}


bool
AosDocBatchReader::setStartClient(
		const u32 &client_id,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	OmnScreen << "DocBatchReader-Start-Client scanner_id: " << mScannerId  << " , client_id" << client_id << endl;
	if (mClientMap.count(client_id) != 0)
	{
		OmnAlarm << "scannerid :" << mScannerId << " , client_id: " << client_id << enderr;
	}
	AosStatus status;
	mClientMap.insert(make_pair(client_id, status));
	mLock->unlock();
	return true;
}


bool
AosDocBatchReader::setFinishedClient(
		const u32 &client_id,
		const int total_num_data_msgs,
		const AosRundataPtr &rdata)
{
	// This function is called when its client sent a finished 
	// message. 
	mLock->lock();
	OmnScreen << "DocBatchReader-Finished-Client scanner_id: " << mScannerId  << " , client_id: " << client_id << endl;
	map<u32, AosStatus>::iterator itr = mClientMap.find(client_id);
	if (itr == mClientMap.end())
	{
		// It can be an error or a racing condition. If the finish
		// message is received before the start message (which
		// is unlikely but possible), it is not an error. 
		mLock->unlock();
		OmnAlarm << "Received a finish without a start: " << client_id << enderr;
		return false;
	}

	OmnScreen << "DocBatchReader-Finished-Client scanner_id: " << mScannerId  << " , client_id: " << client_id 
		<< " , total_num_data_msgs: " << total_num_data_msgs << " , num_data_msg: " << itr->second.num_data_msgs << endl;

	aos_assert_rr(itr->second.status == AosStatus::eStart, rdata, false);
	itr->second.status = AosStatus::eFinished;
	itr->second.total_num_data_msgs = total_num_data_msgs;
	if (itr->second.num_data_msgs < total_num_data_msgs)
	{
		// This means the finish message is received while some more data 
		// messages are still on the way. It needs to wait until all the
		// data messages are received.
		mLock->unlock();
		return true;
	}
	bool is_check_finished = true; 
	map<u32, AosStatus>::iterator it;
	for (it = mClientMap.begin(); it != mClientMap.end(); it ++)
	{
		if (it->second.num_data_msgs != 0 || it->second.total_num_data_msgs != 0)
		{
			is_check_finished = false;
			break;
		}
	}
	// This means that this class has received all the data requests
	// from this cleint. This client can be removed from the map.
	mClientMap.erase(client_id);
	if (allFinishedLocked())
	{
		//mCondVar->signal();
		mCondVar->broadcastSignal();
	}
	mLock->unlock();
	if (is_check_finished)
	{
		AosDocBatchReaderPtr thisptr(this, false);
		AosDocBatchReaderMgr::getSelf()->isFinishedLocked(mScannerId, thisptr, rdata);
	}
	return true;
}



bool
AosDocBatchReader::shuffle(
		const AosBitmapObjPtr &bitmap,
		const u32 &client_id,
		const AosRundataPtr &rdata)
{
	// This function is called when a data message is received from 
	// a client for this reader.
	// It shuffles docids in 'bitmap' to mBitmaps based on virtual IDs.
	if (mCallBack && !mThread)
	{
		start();
	}
	mLock->lock();
	mTotalReceivedSize += bitmap->getMemorySize();
	map<u32, AosStatus>::iterator itr = mClientMap.find(client_id);
	aos_assert_rl(itr != mClientMap.end(), mLock, false);
	itr->second.num_data_msgs++;

	vector<u32> total_vids;
	AosGetLocalVirtualIds(total_vids);
	aos_assert_r(total_vids.size() > 0, false);
	aos_assert_r(total_vids.size() >= 1, false);

	u32 vid;
	u64 docid;
	while ((docid = bitmap->nextDocid()))
	{
		vid = AosGetCubeId(AosXmlDoc::getOwnDocid(docid));
		aos_assert_r(vid >= total_vids[0] && vid <= total_vids[total_vids.size()-1], false);
		if (mBitmaps.count(vid) == 0)
		{
			// AosBitmapObjPtr bmap = AosBitmapObj::getBitmapStatic();
			AosBitmapObjPtr bmap = AosGetBitmap();
			mBitmaps.insert(make_pair(vid, bmap));
			mIndex.insert(make_pair(vid, 0));
		}
		mBitmaps[vid]->appendDocid(docid);
	}

	if (itr->second.status == AosStatus::eFinished)
	{
		// This means that the finish message was received before
		// this data message. It needs to check whether it has
		// received all the data messages from the client.
		if (itr->second.num_data_msgs == itr->second.total_num_data_msgs)
		{
			// It has received all the messages.
			mClientMap.erase(client_id);
			if (allFinishedLocked())
			{
				//mCondVar->signal();
				mCondVar->broadcastSignal();
			}
		}
	}
	mLock->unlock();
	return true;
}



bool
AosDocBatchReader::isFinished(const AosRundataPtr &rdata) 
{
	mLock->lock();
	if (!allFinishedLocked())
	{
		mLock->unlock();
		return false;
	}
	if (mDataQueue.size() != 0)
	{
		mLock->unlock();
		return false;
	}

	vector<u32> total_vids;
	AosGetLocalVirtualIds(total_vids);
	for (u32 i = 0; i < total_vids.size(); i++)
	{
		u32 vid = total_vids[i];
		if (mBitmaps.count(vid) == 0) 
		{
			aos_assert_r(mIndex.count(vid) == 0, false);
			OmnScreen << "vid(" << vid << ")" <<" is Null" << endl;
			continue;
		}
		aos_assert_r(mIndex.count(vid) != 0, false);

		if (mIndex[vid] >= 0) 
		{
			mLock->unlock();
			return false;
		}
	}
	mFinished = true;
	mLock->unlock();
	return true;
}

/////////////////////////// Read Doc ////////////////////////

bool
AosDocBatchReader::callBackClientLocked(const AosRundataPtr &rdata)
{
	if (!mCallBack) return true;
	AosBuffPtr buff = mDataQueue.front();
	mDataQueue.pop();
	mNumCallBackDataMsgs ++;
	mLock->unlock();

	aos_assert_r(mCallBackServerId >= 0, false);
	AosDocBatchReaderMgr::getSelf()->callBackClient(mScannerId, 
			buff, mCallBackServerId, mNumCallBackDataMsgs, rdata);
	mLock->lock();
	return true;
}


bool
AosDocBatchReader::retrieveDocs(
		const OmnString &scanner_id,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	// This function reads in a block of data and returns the data to 
	// the caller. If nothing to be read, it sets the data to null and 
	// returns.
	aos_assert_r(!mCallBack, false);
	if (!mThread) start();
	aos_assert_r(mThread, false);

	switch (mReadPolicy)
	{
	case eReadFromAllVirtuals:
		 return readFromAllVirtuals(resp_buff, rdata);

	case eAllClientsReady:
		 return readWhenAllClientsReady(resp_buff, rdata);

	default:
		 break;
	}
	return false;
}


bool
AosDocBatchReader::readWhenAllClientsReady(
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	// This function checks whether all clients are ready for reading. 
	// If not, it should wait. 
	//
	// When all clients are ready, it checks whether there are data available.
	// If not, it needs to wait. Otherwise, it gets the data and returns.
	resp_buff = waitOnReadyToRead(rdata);
	return true;
}


AosBuffPtr
AosDocBatchReader::waitOnReadyToRead(const AosRundataPtr &rdata)
{
	while (1)
	{
		mLock->lock();
		if (mDataQueue.size() <= 0)
		{
			// No data yet. Need to wait
			mCondVar->wait(mLock);
if (!allFinishedLocked())
{
OmnScreen << "allFinishedLocked allFinishedLocked allFinishedLocked mScannerId: " << mScannerId << endl;
}
			mLock->unlock();
			continue;
		}

		// This means the reader is ready to read. 
		AosBuffPtr buff = mDataQueue.front();
		mDataQueue.pop();
		mCondVar->signal();
		mLock->unlock();
		return buff;
	}
	return 0;
}

bool
AosDocBatchReader::readFromAllVirtuals(
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	// We will implement this function later. Linda, 2012/10/19
	OmnShouldNeverComeHere;
	return false;
	/*mLock->lock();
	int num_virs = mNumVirtuals / AosGetNumPhysicals();
	aos_assert_r(num_virs, false);
	u64 block_size = (mBlockSize > 0 ? mBlockSize:eDftBlockSize)/ num_virs; 
	if (block_size > eMaxBlockSize) block_size = eDftBlockSize;
	AosDocReaderPtr runner; 
	vector<OmnThrdShellProcPtr> runners;
	for (int i = 0; i < mNumVirtuals; i++)
	{
		if (!mBitmaps[i]) continue;
		runner = OmnNew AosDocReader(i, mScannerId, block_size, mBitmaps[i], mIndex[i], rdata);
		runners.push_back(runner);
	}

//	OmnThreadShellMgr::getSelf()->procSync(runners);

	for (u32 i = 0; i < runners.size(); i++)
	{
		runner = (AosDocReader *)runners[i].getPtr();
		int vid = runner->getVirtualId();
		mIndex[vid]= runner->getBitmapIndex();
		if (mIndex[vid] < 0)
		{
			mBitmaps[vid] = 0;
		}
		AosBuffPtr buff = runner->getBuff();
		aos_assert_rl(buff, mLock, false);
		if (buff->dataLen() <= 0) continue; 
		resp_buff->addBuff(buff);
	}
	mLock->unlock();
	return true;
	*/
}
////////////////////////////// Read Doc ///////////////////////////////

///////////////////////////////// delete ///////////////////////////
bool
AosDocBatchReader::deleteDocsLocked(const AosRundataPtr &rdata)
{
	aos_assert_r(mScannerId != "", false);
	AosDocDeletePtr runner;
	vector<OmnThrdShellProcPtr> runners;
	vector<u32> total_vids;
	AosGetLocalVirtualIds(total_vids);
	for (u32 i = 0; i < total_vids.size(); i++)
	{
		u32 vid = total_vids[i];
		if (mBitmaps[vid])
		{
			runner = OmnNew AosDocDelete(
					vid, mScannerId, mBitmaps[vid], 
					mIndex[vid], rdata);
			runners.push_back(runner);
		}
	}

	mLock->unlock();

	aos_assert_r(sgThreadPool, false);
	sgThreadPool->procSync(runners);

	mLock->lock();
	for (u32 i = 0; i < runners.size(); i++)
	{
		runner = (AosDocDelete *)runners[i].getPtr();
		u32 vid = runner->getVirtualId();
		if (mIndex.count(vid) != 0)
		{
			mIndex[vid]= runner->getBitmapIndex();
			aos_assert_r(mBitmaps.count(vid) != 0, false);
			if (mIndex[vid] < 0) mBitmaps[vid] = 0;
		}
	}
	return true;
}


bool
AosDocBatchReader::deleteDocs(
		const OmnString &scanner_id,
		const AosRundataPtr &rdata)
{
	if (!mThread) start();
	aos_assert_r(mThread, false);

	switch (mReadPolicy)
	{
	case eReadFromAllVirtuals:
		 OmnNotImplementedYet;
		 return false;

	case eAllClientsReady:
		 return waitOnReadyToReturn();

	default:
		 break;
	}
	return false;
}


bool
AosDocBatchReader::waitOnReadyToReturn()
{
	while (1)
	{
		mLock->lock();
		if (isDeleteFinished())
		{
			mLock->unlock();
			break;
		}
OmnScreen << "wait!!!!!!!!!!!!!!!!!!!" << endl;
		mCondVar->wait(mLock);
		mLock->unlock();
	}
	return true;
}

bool
AosDocBatchReader::isDeleteFinished()
{
	vector<u32> total_vids;
	AosGetLocalVirtualIds(total_vids);
	for (u32 i = 0; i < total_vids.size(); i++)
	{
		u32 vid = total_vids[i]; 
		if (mBitmaps.count(vid) == 0)
		{
			aos_assert_r(mIndex.count(vid) == 0, false);
			OmnScreen << "vid(" << vid << ")" <<" Null" << endl;
			continue;
		}
		aos_assert_r(mIndex.count(vid) != 0, false);
		if (mIndex[vid] >= 0) 
		{
			return false;
		}
	}
	return true;
}

////////////////////////////// delete /////////////////////////////
#endif
