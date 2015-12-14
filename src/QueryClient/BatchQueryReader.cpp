////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 	Created: 05/09/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////

#if 0
#include "QueryClient/BatchQueryReader.h"
#include "QueryClient/BatchQuery.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Util/OmnNew.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include <API/AosApiR.h>
#include <API/AosApiC.h>
#include "Util/Buff.h"
#include "Util/File.h"
#include "DocClient/DocClient.h"
#include "SEInterfaces/BitmapObj.h"
#include "DocClient/DocidShufflerMgr.h"

#include <iostream>
#include <algorithm>


OmnSingletonImpl(AosBatchQueryReaderSingleton, AosBatchQueryReader, AosBatchQueryReaderSelf, "AosBatchQueryReader");


AosBatchQueryReader::AosBatchQueryReader()
:
mQueryQueueNums(0),
mDocsQueueSize(0),
mBatchQueryLock(OmnNew OmnMutex()),
mSendLock(OmnNew OmnMutex()),
mSendCondVar(OmnNew OmnCondVar()),
mRecieveLock(OmnNew OmnMutex()),
mRecieveCondVar(OmnNew OmnCondVar()),
mThreadStatus1(false),
mThreadStatus2(false)
{
}

AosBatchQueryReader::~AosBatchQueryReader()
{
}

bool 
AosBatchQueryReader::start()
{
    OmnThreadedObjPtr thisPtr(this, false); 
	if(!mSendThrd)
	{
		mSendThrd = OmnNew OmnThread(thisPtr, "BatchQueryThread", eSendThrd, true, true, __FILE__, __LINE__);
	}
	mSendThrd->start();
	if(!mRecieveThrd)
	{
		mRecieveThrd = OmnNew OmnThread(thisPtr, "BatchQueryThread", eRecieveThrd, true, true, __FILE__, __LINE__);
	}
	mRecieveThrd->start();
	return true;
}


bool 
AosBatchQueryReader::stop()
{
	return true;
}

bool
AosBatchQueryReader::signal(const int threadLogicId)
{
	switch(threadLogicId)
	{
		case eSendThrd :
			mSendLock->lock();
			mSendCondVar->signal();
			mSendLock->unlock();
			break;
		case eRecieveThrd :
			mRecieveLock->lock();
			mRecieveCondVar->signal();
			mRecieveLock->unlock();
			break;
		default:
			OmnAlarm << "Invalid thread Id ! " << enderr;
	}
	return true;
}

bool    
AosBatchQueryReader::checkThread111(OmnString &err, const int thrdLogicId)
{
	bool threadStatus;
	switch(thrdLogicId)
	{
		case eSendThrd :
			mSendLock->lock();
			threadStatus = mThreadStatus1;	
			mThreadStatus1 = false;
			mSendLock->unlock();
			break;
		case eRecieveThrd :
			mRecieveLock->lock();
			threadStatus = mThreadStatus2;
			mThreadStatus2 = false;
			mRecieveLock->unlock();
			break;
		default:
			OmnAlarm << "Invalid thread Id ! " << enderr;
	}
	return threadStatus;
}

bool
AosBatchQueryReader::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	AosRundataPtr rdata = OmnApp::getRundata();
	if (thread->getLogicId() == eSendThrd)	
	{
		send(state);
	}

	if (thread->getLogicId() == eRecieveThrd)	
	{
		recieve(state, rdata);
	}

	state = OmnThrdStatus::eIdle;
	return true;
}

bool 
AosBatchQueryReader::config(const AosXmlTagPtr &config)
{
	return true;
}

OmnString	
AosBatchQueryReader::getShufferIdFromQueryId(const OmnString &queryid, OmnString &shuffer_idx)
{
	OmnString shufferid = queryid;
	shufferid << shuffer_idx;
	return shufferid; 
}

OmnString	
AosBatchQueryReader::getQueryIdFromShufferId(const OmnString &shufferid)
{
	OmnString sub = "_QUERYID_";
	int index = shufferid.findSubString(sub, 0);
	OmnString queryid = shufferid.substr(0, index+8);
	return queryid;
}

bool 
AosBatchQueryReader::send(OmnThrdStatus::E &state)
{
	while (state == OmnThrdStatus::eActive)
	{
		mSendLock->lock();
		if(mQueryQueue.size() <= 0 )
		{
			mSendCondVar->wait(mSendLock);
			mThreadStatus1 = true;
			mSendLock->unlock();
			continue;
		}
		struct query_result qr = mQueryQueue.front();
		mQueryQueue.pop();
		AosBitmapObjPtr bitmap = qr.bitmap;
		mQueryQueueNums -= ((AosBitmap*)(bitmap.getPtr()))->getDocidCount();
		mSendCondVar->signal();
		mBatchQueryLock->lock();
		map<OmnString, AosBatchQueryPtr>::iterator itr = mBatchQueries.find(qr.queryid);
		if (itr == mBatchQueries.end())
		{
			//never happen
			OmnAlarm << "Never happen to failed retrieving the batch_query: " << qr.queryid << enderr;
			mRecieveLock->unlock();
			continue;
		}
		mBatchQueryLock->unlock();
		AosBatchQueryPtr bq = itr->second; 
		mSendCondVar->signal();
		//shuffer 
		AosRundataPtr sendrdata = bq->getRundata()->clone(AosMemoryCheckerArgsBegin);
		AosBatchQueryReaderPtr thisPtr(this, false);
		OmnString shufferid = getShufferIdFromQueryId(qr.queryid, qr.shuffer_idx);
		AosDocidShufflerMgr::getSelf()->sendStart(shufferid,  500000, qr.fnames, sendrdata);
		AosDocidShufflerMgr::getSelf()->shuffle(shufferid, qr.bitmap, 1, thisPtr, sendrdata);
		AosDocidShufflerMgr::getSelf()->sendFinished(shufferid, sendrdata);
		mSendLock->unlock();
	}
	return true;
}

bool 
AosBatchQueryReader::docClientCallback(const AosRundataPtr &rdata,const OmnString &shufferid, const AosBuffPtr &buff, const bool finished)
{
	OmnString queryid = getQueryIdFromShufferId(shufferid);
OmnScreen << "DocClient Call Back ,  shuffer id :" << shufferid << ",DocsQueue Size:" << mDocsQueueSize << ", BlockQueue Size:" << mBlockQueueSize << ",QueryQueueNums:" << mQueryQueueNums << ",BuffSize:" << getBuffSize(rdata)+buff->dataLen() << endl;
	mBatchQueryLock->lock();
	map<OmnString, AosBatchQueryPtr>::iterator itr = mBatchQueries.find(queryid);
	if (itr == mBatchQueries.end())
	{
		//never happen
		OmnAlarm << "Never happen to failed retrieving the batch_query: " << queryid << enderr;
		mRecieveLock->unlock();
		return false;
	}
	mBatchQueryLock->unlock();
	AosBatchQueryPtr bq = itr->second; 
	if(finished)
	{
		bq->setDataRecievedOver();
	}

	mRecieveLock->lock();
	if(mDocsQueueSize == eMaxDocsQueueSize)
	{
		mRecieveCondVar->wait(mRecieveLock);
	}
	mDocsQueueSize += buff->dataLen();
	struct query_docs qd;
	qd.queryid = queryid;
	qd.buff = buff;
	mDocsQueue.push_back(qd);
	mRecieveCondVar->signal();
	mRecieveLock->unlock();
	return true;
}

bool 
AosBatchQueryReader::recieve(OmnThrdStatus::E &state, AosRundataPtr &rdata)
{
	while (state == OmnThrdStatus::eActive)
	{
		mRecieveLock->lock();
		if(mDocsQueue.size() <= 0 )
		{
			mRecieveCondVar->wait(mRecieveLock);
			mThreadStatus2 = true;
			mRecieveLock->unlock();
			continue;
		}
		struct query_docs qd = mDocsQueue.front();
		mDocsQueue.pop_front();
		mDocsQueueSize -= qd.buff->dataLen();
		mRecieveCondVar->signal();
		mBatchQueryLock->lock();
		map<OmnString, AosBatchQueryPtr>::iterator itr = mBatchQueries.find(qd.queryid);
		if (itr == mBatchQueries.end())
		{
			//never happen
			OmnAlarm << "Never happen to failed retrieving the batch_query: " << qd.queryid << enderr;
			mRecieveLock->unlock();
			continue;
		}
		mBatchQueryLock->unlock();
		AosBatchQueryPtr bq = itr->second; 
		mRecieveLock->unlock();
		bq->procDocs(qd.buff, rdata);
		
	}
	return true;
}

bool 
AosBatchQueryReader::addBatchQuery(OmnString &queryid, AosBatchQueryPtr &batchquery, const AosRundataPtr &rdata)
{
	mBatchQueryLock->lock();
	mBatchQueries[queryid] = batchquery;
	mBatchQueryLock->unlock();
	return true;
}

int
AosBatchQueryReader::getBuffSize(const AosRundataPtr &rdata)
{
	mBatchQueryLock->lock();
	map<OmnString, AosBatchQueryPtr>::iterator itr = mBatchQueries.begin();
	AosBatchQueryPtr bq;
	int size = 0;
	while(itr != mBatchQueries.end())
	{
		bq = itr->second;
		size += bq->getBuffSize();	
		itr++;
	}
	mBatchQueryLock->unlock();
	return size;
}

bool 
AosBatchQueryReader::isFull(const AosRundataPtr &rdata)
{
	int size = getBuffSize(rdata);
	if(size >= eMaxBuffSize)
	{
		return true;
	}
	return false;
}

bool 
AosBatchQueryReader::addQueryRslt(OmnString &queryid, OmnString &shuffer_idx, AosQueryRsltObjPtr &queryrslt, const AosXmlTagPtr &fnames, const AosRundataPtr &rdata)
{
	mSendLock->lock();
	if(mQueryQueueNums == eMaxQueryQueueNums)
	{
		mSendCondVar->wait(mSendLock);
	}
	AosBitmapObjPtr bitmap = OmnNew AosBitmap();
	int total = queryrslt->getNumDocs();
	u64 * docids = queryrslt->getDocidsPtr();
	for(int i=0; i<total; i++)
	{
		u64 did = docids[i];
//test start
if(did <= 0)
{
		OmnScreen << "************docid : " << did << "************"<< endl;
}
//test end
		bitmap->appendDocid(did);
	}
//	bitmap->appendBlock(queryrslt->getDocidsPtr(), queryrslt->getNumDocs());
	mQueryQueueNums += ((AosBitmap*)(bitmap.getPtr()))->getDocidCount();
	bitmap->reset();
	struct query_result qr;
	qr.queryid = queryid;
	qr.shuffer_idx = shuffer_idx;
	qr.fnames = fnames;
	qr.bitmap = bitmap;
	mQueryQueue.push(qr);	
	mSendCondVar->signal();
	mSendLock->unlock();
	return true;
}

#endif
