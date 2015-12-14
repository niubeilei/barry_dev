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
// 04/24/2013 Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#include "DataScanner/MultiFileScanner.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "Util/UtUtil.h"
#include "XmlUtil/SeXmlParser.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/NetFileCltObj.h"
#include "SEInterfaces/ActionCaller.h"
#include "NetFile/NetFile.h"
#include "Actions/ActImportDocFixed.h"


OmnThreadPtr 		AosMultiFileScanner::smThread = 0;
OmnMutexPtr			AosMultiFileScanner::smLock = OmnNew OmnMutex();	
OmnCondVarPtr		AosMultiFileScanner::smCondVar = OmnNew OmnCondVar();
queue<MultiFileScannerReq>			AosMultiFileScanner::smQueue;


AosMultiFileScanner::AosMultiFileScanner(const bool flag)
:
AosDataScanner(AOSDATASCANNER_FILE, AosDataScannerType::eFile, flag),
mMemLock(OmnNew OmnMutex()),
mMemCondVar(OmnNew OmnCondVar()),
mTotalReceivedSize(0),
mActions(0),
mRundata(0)
{
}


AosMultiFileScanner::AosMultiFileScanner(const AosRundataPtr &rdata)
:
AosDataScanner(AOSDATASCANNER_FILE, AosDataScannerType::eFile, false),
mMemLock(OmnNew OmnMutex()),
mMemCondVar(OmnNew OmnCondVar()),
mTotalReceivedSize(0),
mActions(0),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	smLock->lock();
	if (!smThread)
	{
		OmnThreadedObjPtr thisptr(this, false); 
		smThread = OmnNew OmnThread(thisptr, "MultiFileScannerThrd", 0, false, true, __FILE__, __LINE__);
		smLock->unlock();

		smThread->start(); 
	}
	else
	{
		smLock->unlock();
	}
}


void
AosMultiFileScanner::setFiles(const vector<AosNetFileObjPtr> &vv)
{
	mFiles = vv;	
	AosFileReadListenerPtr thisptr(this, false);
	for (u32 i = 0; i < mFiles.size(); i++)
	{
		mFiles[i]->setCaller(thisptr);
	}
}	


AosMultiFileScanner::~AosMultiFileScanner()
{
	//OmnScreen << "multi fiel scanner deleted" << endl;
}
	

bool
AosMultiFileScanner::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while(state == OmnThrdStatus::eActive)
	{
		smLock->lock();
		if (smQueue.empty())
		{
			smCondVar->wait(smLock);
			smLock->unlock();
			continue;
		}
//OmnScreen << "MultiFileScannerReq MultiFileScannerReq MultiFileScannerReq smQueue.size: " << smQueue.size() << endl;
		MultiFileScannerReq req = smQueue.front();
		smQueue.pop();
		smLock->unlock();

		readFile(req);
	}
	return true;
}


bool
AosMultiFileScanner::readFile(const MultiFileScannerReq &req)
{
	u64 reqId = req.reqId; 

	AosNetFileObjPtr file = req.file;
	aos_assert_r(file, false);

	AosRundataPtr rdata = req.rdata;
	aos_assert_r(rdata, false);

	bool rslt = file->readData(reqId, req.blockSize, rdata.getPtr());
	if (!rslt)
	{
		//for bug trace
		//{
		//	aos_assert_r(mTask, false);
		//	u64 task_docid = mTask->getTaskDocid();
		//	OmnScreen << "=====================read file error , task_docid: " << task_docid << endl;
		//	AosXmlTagPtr task_doc = AosGetDocByDocid(task_docid, rdata);
		//	aos_assert_r(task_doc, false);
		//	OmnCout << "task_doc: " << task_doc->toString() << endl;
		//}
		AosFileReadListenerPtr scanner = file->getCaller();
		aos_assert_r(scanner, false);

		AosDiskStat disk_stat;
		disk_stat.setServerIsDown(true);

		scanner->fileReadCallBack(reqId, req.blockSize, true, disk_stat);
		return true;
	}
	return true;
}


bool
AosMultiFileScanner::signal(const int threadLogicId)
{
	return true;
}


bool
AosMultiFileScanner::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


void
AosMultiFileScanner::addRequest(const u64 &reqId, const int64_t &blocksize)
{ 	
	aos_assert(mRundata);
	aos_assert(reqId < mFiles.size());
	AosNetFileObjPtr file = mFiles[reqId];
	aos_assert(file);

	aos_assert(blocksize < eMaxMemory);
	while(1)
	{
		mMemLock->lock();
		if (mTotalReceivedSize + blocksize > eMaxMemory)
		{
			//OmnScreen << "wait wait wait wait wait: " << mTotalReceivedSize << " , " << reqId << endl;
			mMemCondVar->wait(mMemLock);
			mMemLock->unlock();
			continue;
		}

		mTotalReceivedSize += blocksize;
		mMemLock->unlock();

		smLock->lock();
		MultiFileScannerReq req;
		req.reqId = reqId;
		req.blockSize = blocksize;
		req.file = file;
		req.rdata = mRundata;

		smQueue.push(req);

		smCondVar->signal();
		smLock->unlock();
		break;
	}
}


void
AosMultiFileScanner::fileReadCallBack(
		const u64 &reqId, 
		const int64_t &expected_size, 
		const bool &finished, 
		const AosDiskStat &disk_stat)
{
	//if (disk_stat.serverIsDown())
	if (disk_stat.getError() > 0)
	{
		OmnScreen << "=====================disk error1111111111111111 : " << endl;
		mDiskError = true;                                                        
	}
	aos_assert(mActions);
	mActions->callBack(reqId, expected_size, finished);
}


AosBuffPtr
AosMultiFileScanner::getNextBlock(const u64 &reqId, const int64_t &expected_size)
{
	aos_assert_r(reqId < mFiles.size(), 0);

	AosNetFileObjPtr file = mFiles[reqId];
	aos_assert_r(file, 0);

	AosBuffPtr buff = file->getBuff();
	aos_assert_r(buff, 0);

	mMemLock->lock();
	mTotalReceivedSize -= expected_size;
	mMemCondVar->signal();
	mMemLock->unlock();
	return buff;
}


void
AosMultiFileScanner::destroyedMember()
{
	OmnScreen << "destroyedMember destroyedMember destroyedMember destroyedMember destroyedMember !!!!" << endl;
	OmnScreen << "jozhi =======destroyedMember, mActins: " << ((AosActImportDocFixed*)(mActions.getPtr())) << " , this: " << this << endl;
	mActions = 0;
	mFiles.clear();
}


AosJimoPtr
AosMultiFileScanner::cloneJimo()  const
{
	try
	{
		return OmnNew AosMultiFileScanner(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosDataScannerObjPtr
AosMultiFileScanner::clone(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosMultiFileScanner(rdata);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object: " << conf->toString() << enderr;
		return 0;
	}
}


int
AosMultiFileScanner::getPhysicalId() const
{
	OmnShouldNeverComeHere;
	return -1;
}

