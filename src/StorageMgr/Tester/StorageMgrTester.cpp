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
//
// Modification History:
// 2010/10/24	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "StorageMgr/Tester/StorageMgrTester.h"

#include "alarm_c/alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "DocFileMgr/DocFileMgr.h"
#include "DocFileMgr/FileGroupMgr.h"
#include "DocFileMgr/StorageApp.h"
#include "Porting/Sleep.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "StorageMgr/StorageMgr.h"
#include "StorageMgr/DevLocation.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

bool	mStop = false;

// Ketty this test can use DocFileMgr/Tester.
#if 0
void aosSignalHandler(int value)
{
	if(value != 14)	return;
	
	cout << "To stop the FileGroupMgr!" << endl;
	mStop = true;	
	
	OmnSleep(6);
	AosStorageMgr::getSelf()->stop();
	cout << "-------------Finish!!" << endl;
	//exit(0);
}


AosStorageMgrTester::AosStorageMgrTester()
{
	::signal(SIGALRM, aosSignalHandler);
	
	mName = "StorageMgrTester";
	mInfoLock = OmnNew OmnMutex();
	for(u32 i=0; i<eTotalDBNum; i++)
	{
		mSeqno[i] = OmnNew u32[eMaxDocs];
		mOffset[i] = OmnNew u32[eMaxDocs];
		mRepeat[i] = OmnNew int[eMaxDocs];
		mPatternLen[i] = OmnNew int[eMaxDocs];
	
		memset(mSeqno[i], 0, sizeof(u32) * eMaxDocs);
		memset(mOffset[i], 0, sizeof(u32) * eMaxDocs);
		memset(mRepeat[i], 0, sizeof(int) * eMaxDocs);
		memset(mPatternLen[i], 0, sizeof(int) * eMaxDocs);
	
		mPatterns[i]= OmnNew char*[eMaxDocs];
		for(u32 j=0; j< eMaxDocs; j++)
		{
			mPatterns[i][j] = OmnNew char[eMaxPatternLen];
			memset(mPatterns[i][j], 0, sizeof(char) * eMaxPatternLen);

			mEntryLocks[i][j] = OmnNew OmnMutex();
		}
		
		mNumLocks[i] = OmnNew OmnMutex();
	}
	memset(mNumDocs, 0, sizeof(int) * eTotalDBNum);
}


AosStorageMgrTester::~AosStorageMgrTester()
{
	for(u32 i=0; i<eTotalDBNum; i++)
	{
		delete [] mSeqno[i];
		delete [] mOffset[i];
		delete [] mRepeat[i];
		delete [] mPatternLen[i];	
		for(u32 j=0; j< eMaxDocs; j++)
		{
			delete [] mPatterns[i][j];
		}
	}
}


bool 
AosStorageMgrTester::start()
{
	cout << "Start AosStorageMgr Tester ..." << endl;
	init();

	// start thread
	OmnThreadedObjPtr thisPtr(this, false);
	OmnThreadPtr thread[eThreadNum];
	for(u32 i=0; i<eThreadNum; i++)
	{
		thread[i] = OmnNew OmnThread(thisPtr, "Thrd", i, true, true, __FILE__, __LINE__);
		thread[i]->start();
	}
	
	while(1)
	{
		OmnSleep(5 * 60);
		mInfoLock->lock();
		OmnScreen << "---------------------Information--------" << "\n"
				<< "ReadNum:" << mReadNum << "; "
				<< "CreatedNum:" << mCreatedNum << "; "
				<< "ModifiedNum:" << mModifiedNum << "; "
				<< "DeletedNum:" << mDeletedNum << "; "
				<< "TotalCreatedNum:" << mTotalCreatedNum << "; "
				<< "TotalModifiedNum:" << mTotalModifiedNum << "; "
				<< "TotalDeletedNum:" << mTotalDeletedNum << "; "
				<< endl;
		mReadNum = 0;
		mCreatedNum = 0;
		mModifiedNum = 0;
		mDeletedNum = 0;
		mInfoLock->unlock();
	}

	return true;
}


void
AosStorageMgrTester::init()
{
	AosXmlTagPtr config = OmnApp::getAppConfig();
	aos_assert(config);

	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	mStorageApp = OmnNew AosStorageApp(AOSSTRKEY_IIL_STORAGE_APP, rdata);
		
	u32 virtual_server_id = 0;
	u32 appid = 0;
	AosDevLocationPtr location = OmnNew AosDevLocation();
	AosDocFileMgrObjPtr docFileMgr; 
	
	for(u32 i=0; i<eVirtualServerNum; i++)
	{
		virtual_server_id = i;
		for(u32 j=0; j< eAppNum; j++)
		{
			appid = j;
			
			docFileMgr = mStorageApp->retrieveAppData(virtual_server_id, appid, location, rdata);
			if(!docFileMgr)
			{
				docFileMgr = mStorageApp->createAppData(virtual_server_id, appid, location, rdata);
			}
			OmnString prefix = "data";
			prefix << "_" << docFileMgr->getId();
			docFileMgr->setFileNamePrefix(prefix);
		}
	}
	
	mCreatedNum = 0;
	mModifiedNum = 0;
	mDeletedNum = 0;
	mReadNum = 0;
	mTotalCreatedNum = 0;
	mTotalModifiedNum = 0;
	mTotalDeletedNum = 0;
	mTotalReadNum = 0;
}


bool
AosStorageMgrTester::threadFunc(
		  OmnThrdStatus::E &state,
		  const OmnThreadPtr &thread)
{
	u32 tid = thread->getLogicId();
	//u32 trys = 100000;
	
	u64 i=0;
	//while(state == OmnThrdStatus::eActive && i<trys)
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin); 
	AosDevLocationPtr location = OmnNew AosDevLocation();
	while(state == OmnThrdStatus::eActive)
	{
		if ((i % 1000) == 0)
		{
			// Print statistics information
		}
		if(mStop) break;
		basicTest(rdata, location);
		i++;
	}
	
	OmnScreen << "Thread:" << tid
		<< "; Existing!	" 
		<< "Total Num:" << eThreadNum 
		<< endl;
	
	thread->stop();
	return true;
}


bool 
AosStorageMgrTester::basicTest(
		const AosRundataPtr &rdata,
		const AosDevLocationPtr &location)
{
	// 1. random chose the app_id and pick the docFileMgr
	//u32 virtual_server_id = 0;
	u32 virtual_server_id = rand() % eVirtualServerNum;
	u32 appid = rand() % eAppNum;
	//OmnScreen << "basicTest:" 	
	//		<< "virtual_server_id:" << virtual_server_id << "; "
	//		<< "appid:" << appid<< "; "
	//		<< endl;
	AosDocFileMgrObjPtr docFileMgr = mStorageApp->retrieveAppData(virtual_server_id, appid, location, rdata);
	aos_assert_r(docFileMgr, false);

	// 2. random determain the opration
	bool rslt;
	//int idx = rand() % 100;
	int idx = 10;
	if (idx < eCreateWeight)
	{
		mInfoLock->lock();
		mCreatedNum++;
		mInfoLock->unlock();
		rslt = createDoc(docFileMgr, rdata);
	}
	else if(idx < eModifyWeight + eCreateWeight)
	{
		mInfoLock->lock();
		mModifiedNum++;
		mInfoLock->unlock();
		rslt = modifyDoc(docFileMgr, rdata);
	}
	else if(idx < eDeleteWeight + eModifyWeight + eCreateWeight)
	{
		mInfoLock->lock();
		mDeletedNum++;
		mInfoLock->unlock();
		rslt = deleteDoc(docFileMgr, rdata);
	}
	else
	{
		mInfoLock->lock();
		mReadNum++;
		mInfoLock->unlock();
		readDoc(docFileMgr, rdata);
	}

	return true;
}


bool
AosStorageMgrTester::createDoc(
		const AosDocFileMgrObjPtr &docFileMgr,
		const AosRundataPtr &rdata)
{
	u32 docFileMgrId = docFileMgr->getId();
	u32 dfm_id2 = docFileMgrId - eDocFileMgrIdStart;
	if(mNumDocs[dfm_id2]>= eMaxDocs)
	{
		OmnScreen << "Max docs reached!" << endl;
		return true;
	}

	// 1. Determine pattern length and repeate
	u32 pattern_len = rand() % (eMaxPatternLen-eMinPatternLen) + eMinPatternLen;
	u32 repeat = pickRepeat();

	// 2. Set the contents
	char * data = OmnNew char[eMaxRepeat * eMaxPatternLen + 1];
	char pattern[eMaxPatternLen];
	setContents(data, repeat, pattern_len, pattern);
	
	// 3. Create the doc
	u32 seqno = 0;
	u64 offset = 0;
	bool rslt = docFileMgr->saveDoc(seqno, offset, pattern_len*repeat, data, rdata);
	aos_assert_r(rslt, false);
	
	mNumLocks[dfm_id2]->lock();
	u32 crtNum = mNumDocs[dfm_id2]++;
	mEntryLocks[dfm_id2][crtNum]->lock();
	mNumLocks[dfm_id2]->unlock();

	mSeqno[dfm_id2][crtNum] = seqno; 
	mOffset[dfm_id2][crtNum] = offset; 
	mRepeat[dfm_id2][crtNum] = repeat; 
	mPatternLen[dfm_id2][crtNum] = pattern_len; 
	memcpy(mPatterns[dfm_id2][crtNum], pattern, pattern_len );
	mEntryLocks[dfm_id2][crtNum]->unlock();
	OmnDelete [] data;
	
	OmnScreen << "createDoc;"
			<< "docFileMgrId:" << docFileMgrId << "; "
			<< "seqno:" << seqno << "; "
			<< "offset:" << offset << "; "
			<< "size:" << pattern_len*repeat << "; "
			<< "pattern_len:" << pattern_len << "; "
			<< "repeat:" << repeat << "; "
			<< "crtNum:" << crtNum 
			<< endl;
	return true;
}


bool
AosStorageMgrTester::modifyDoc(
		const AosDocFileMgrObjPtr &docFileMgr,
		const AosRundataPtr &rdata)
{
	u32 docFileMgrId = docFileMgr->getId();
	u32 dfm_id2 = docFileMgrId - eDocFileMgrIdStart;
	
	mNumLocks[dfm_id2]->lock();
	u32 crtNum = mNumDocs[dfm_id2];
	if(crtNum == 0)
	{
		mNumLocks[dfm_id2]->unlock();
		return true;
	}
	u32 idx = rand() % crtNum;
	mEntryLocks[dfm_id2][idx]->lock();
	mNumLocks[dfm_id2]->unlock();
	
	u32 seqno = mSeqno[dfm_id2][idx];	
	u64 offset= mOffset[dfm_id2][idx];	

	// 1. change pattern length and repeat
	u32 pattern_len = rand() % (eMaxPatternLen-eMinPatternLen) + eMinPatternLen;
	u32 repeat = pickRepeat();
	
	// 2. modify the contents
	char * data = OmnNew char[eMaxRepeat * eMaxPatternLen + 1];
	char pattern[eMaxPatternLen];
	setContents(data, repeat, pattern_len, pattern);

	// 3. modify the doc and change the array
	OmnScreen << "modifyDoc;"
			<< "docFileMgrId:" << docFileMgrId << "; "
			<< "seqno:" << seqno << "; "
			<< "offset:" << offset << "; "
			<< "size:" << pattern_len*repeat << "; "
			<< "crtNum:" << idx << endl;
	bool rslt = docFileMgr->saveDoc(seqno, offset, pattern_len * repeat, data, rdata);
	aos_assert_rl(rslt, mEntryLocks[dfm_id2][idx], false);
	aos_assert_rl(offset != 0, mEntryLocks[dfm_id2][idx], false);
	
	mSeqno[dfm_id2][idx] = seqno; 
	mOffset[dfm_id2][idx] = offset; 
	mRepeat[dfm_id2][idx] = repeat; 
	mPatternLen[dfm_id2][idx] = pattern_len; 
	memcpy(mPatterns[dfm_id2][idx], pattern, pattern_len );
	mEntryLocks[dfm_id2][idx]->unlock();

	OmnDelete [] data;
	return true;
}


bool
AosStorageMgrTester::deleteDoc(
		const AosDocFileMgrObjPtr &docFileMgr,
		const AosRundataPtr &rdata)
{
	u32 docFileMgrId = docFileMgr->getId();
	u32 dfm_id2 = docFileMgrId - eDocFileMgrIdStart;
	
	mNumLocks[dfm_id2]->lock();
	if(mNumDocs[dfm_id2] == 0)
	{
		mNumLocks[dfm_id2]->unlock();
		return true;
	}
	u32 crtNum = mNumDocs[dfm_id2]--;
	int idx = rand() % crtNum;
	mEntryLocks[dfm_id2][idx]->lock();
	if(idx != (int)crtNum-1) mEntryLocks[dfm_id2][crtNum-1]->lock();	
	mNumLocks[dfm_id2]->unlock();
	
	u32 seqno = mSeqno[dfm_id2][idx];	
	u64 offset= mOffset[dfm_id2][idx];	
	OmnScreen << "deleteDoc;"
			<< "docFileMgrId:" << docFileMgrId << "; "
			<< "seqno:" << seqno << "; "
			<< "offset:" << offset << "; "
			<< "crtNum:" << idx << "; "
			<< "totalNum:" << crtNum
			<< endl;
	bool rslt = docFileMgr->deleteDoc(seqno, offset, rdata);
	aos_assert_rl(rslt, mEntryLocks[dfm_id2][idx], false);
	
	if(idx != (int)crtNum-1)
	{
		mSeqno[dfm_id2][idx] = mSeqno[dfm_id2][crtNum -1]; 
		mOffset[dfm_id2][idx] = mOffset[dfm_id2][crtNum -1]; 
		mRepeat[dfm_id2][idx] = mRepeat[dfm_id2][crtNum -1]; 
		mPatternLen[dfm_id2][idx] = mPatternLen[dfm_id2][crtNum -1]; 
		memcpy(mPatterns[dfm_id2][idx], mPatterns[dfm_id2][crtNum -1], mPatternLen[dfm_id2][crtNum -1]);
	}
	mSeqno[dfm_id2][crtNum -1] = 0; 
	mOffset[dfm_id2][crtNum -1] = 0; 
	mRepeat[dfm_id2][crtNum -1] = 0; 
	mPatternLen[dfm_id2][crtNum -1] = 0; 
	memset(mPatterns[dfm_id2][crtNum -1], 0, sizeof(char) * eMaxPatternLen);
	if(idx != (int)crtNum-1) mEntryLocks[dfm_id2][crtNum-1]->unlock();
	mEntryLocks[dfm_id2][idx]->unlock();
	
	return true;
}


bool
AosStorageMgrTester::readDoc(
		const AosDocFileMgrObjPtr &docFileMgr,
		const AosRundataPtr &rdata)
{
	// This only can run one thread. Because the crtNum
	
	u32 docFileMgrId = docFileMgr->getId();
	u32 dfm_id2 = docFileMgrId - eDocFileMgrIdStart;

	if(mNumDocs[dfm_id2]== 0)
	{
		return true;
	}

	char * data = OmnNew char[eMaxRepeat * eMaxPatternLen + 1];
	//int trys = rand() % mNumDocs[dfm_id2];
	//for (u32 i=0; i<trys; i++)
	//{
		mNumLocks[dfm_id2]->lock();
		u32 crtNum = mNumDocs[dfm_id2];
		int idx = rand() % crtNum;
		mEntryLocks[dfm_id2][idx]->lock();
		mNumLocks[dfm_id2]->unlock();
	
		u32 seqno = mSeqno[dfm_id2][idx];
		u64 offset = mOffset[dfm_id2][idx];
		int repeat= mRepeat[dfm_id2][idx];
		int pattern_len= mPatternLen[dfm_id2][idx];
	
		OmnScreen << "readDoc;"
				<< "docFileMgrId:" << docFileMgrId << "; "
				<< "seqno:" << seqno << "; "
				<< "offset:" << offset << endl;
		bool rslt = docFileMgr->readDoc(seqno, offset, data, pattern_len * repeat, rdata);
		aos_assert_r(rslt, false);
		for (int k=0; k<repeat; k++)
		{
			int ret = memcmp(&data[k*pattern_len], mPatterns[dfm_id2][idx], pattern_len);
			aos_assert_rl(ret == 0, mEntryLocks[dfm_id2][idx], false);
		}
		mEntryLocks[dfm_id2][idx]->unlock();
	//}
	OmnDelete [] data;
	return true;
}


void
AosStorageMgrTester::setContents(
		char *data,
		const u32 repeat,
		const u32 pattern_len,
		char *pattern)
{
	for(u32 i=0; i<pattern_len; i++)
	{
		pattern[i] = (char)(rand() % 74 + 48);
	}

	for(u32 i=0; i<repeat; i++)
	{
		memcpy(&data[i*pattern_len], pattern, pattern_len);
	}
}


u32
AosStorageMgrTester::pickRepeat()
{
	u32 vv = rand() % 100;
	u32 weight = eLevel1Weight;
	if (vv < weight) return (rand() % eRepeatLevel1) + 1;
	weight += eLevel2Weight;
	if (vv < weight) return (rand() % eRepeatLevel2) + 1;
	weight += eLevel3Weight;
	if (vv < weight) return (rand() % eRepeatLevel3) + 1;
	weight += eLevel4Weight;
	if (vv < weight) return (rand() % eRepeatLevel4) + 1;
	
	return (rand() % eRepeatLevel5) + 1;
}
#endif
