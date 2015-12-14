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
// 1. It maintains a list of seeds of fixed length for each file.
// 2. When adding new data to a file, it randomly determines the number of
//    seeds to add: "0123456789". "222222222222" "33333"
//    files[0] = "xxxx";
//    files[1] = "xxxx";
//
// 3. vector<u64> mDeletedDocids[num_threads];
//    vector<u64> mAllocatedBlocks[num_threads];
//
//    mAllocatedBlocks[0] = [1000, 18000000, 36000000];
//    mAllocatedBlocks[1] = [1000, 18000000, 36000000];
//
// Modification History:
// 2015-1-8 Created by White
////////////////////////////////////////////////////////////////////////////
#include "BlobSE/Tester/BlobSETester.h"

#include "Alarm/Alarm.h"
#include "BlobSE/BlobSE.h"
#include "BlobSE/BlobSEAPI.h"
#include "BlobSE/Tester/LocalSnapshot.h"
#include "Debug/Debug.h"
#include "DfmUtil/DfmDoc.h"
#include "Porting/Sleep.h"
#include "Porting/TimeOfDay.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "UtilData/BlobSEReqEntry.h"
#include <arpa/inet.h>
#include <limits.h>
#include <set>
#include <stdlib.h>
#include <string.h>
#include <vector>

using namespace std;

extern AosXmlTagPtr			gApp_config;
extern i64					gTestDuration;
extern u32					gRandomSeed;
extern bool					gTestRaftAPI;
extern bool					gUnitTest;
AosBuffPtr					AosBlobSETester::ptrBuff = OmnNew AosBuff(eMaxBuffSize AosMemoryCheckerArgs);
int 						gBigDocWeight = 0;
int							gSmallDocWeight = 0;
u32							gSeedCount = 0;
std::map<u32, OmnString>	gSeedMap;				//seedID and seed content


AosBlobSETester::AosBlobSETester()
:
mLockRaw(OmnNew OmnMutex()),
mLock(mLockRaw)
{
	mName = "BlobSETester";
	mRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	AosXmlTagPtr config = gApp_config->getFirstChild();
	config = config->getFirstChild(mName.data());
	if (config.isNull())
	{
		mRundata->setErrmsg("BlobSETester can not configure itself.");
		OmnThrowException(mRundata->getErrmsg());
		return;
	}
	mTries = config->getAttrInt("tries", 100);
	mBatchSize = config->getAttrInt("batch_size", 10);
	mSiteID = config->getAttrU32("siteid", 10);
	mCubeID = config->getAttrU32("cubeid", 10);
	mAseID = config->getAttrU64("aseid", 10);
	gSeedCount = config->getAttrU32("seed_count", 100);
	mDocType = (AosDfmDocType::E)config->getAttrInt("doc_type", 1);
	mCreateWeight = config->getAttrInt("create_weight", 25);
	mModifyWeight = config->getAttrInt("modify_weight", 25);
	mReadWeight = config->getAttrInt("read_weight", 25);
	mDeleteWeight = config->getAttrInt("delete_weight", 25);

	// Ketty 2015/04/07
	mCreateSnapshotWeight = config->getAttrInt("create_snapshot_weight", 5);
	mCommitSnapshotWeight = config->getAttrInt("commit_snapshot_weight", 5);
	mRollbackSnapshotWeight = config->getAttrInt("rollback_snapshot_weight", 3);
	mSubmitSnapshotWeight = config->getAttrInt("submit_snapshot_weight", 2);
	mCreateDocToSnapWeight = config->getAttrInt("createdoc_to_snap_weight", 15);
	mModifyDocFromSnapWeight = config->getAttrInt("modifydoc_from_snap_weight", 20);
	mDeleteDocFromSnapWeight = config->getAttrInt("deletedoc_from_snap_weight", 10);
	mReadDocFromSnapWeight = config->getAttrInt("readdoc_from_snap_weight", 30);

	mReturnOnFail = config->getAttrBool("stop_on_fail", 25);
	mReadInvalidWeight = config->getAttrInt("read_invalid_weight", 10);
	mReadValidWeight = config->getAttrInt("read_valid_weight", 90);
	mReadDeletedWeight = config->getAttrInt("read_deleted_weight", 90);
	mNumOfThreads = config->getAttrU32("number_of_threads", 1);
	mDocidAllocateSize = config->getAttrU32("docid_allocate_size", 10000);
	gBigDocWeight = config->getAttrInt("big_doc_weight", 1);
	gSmallDocWeight = config->getAttrInt("small_doc_weight", 100);
	mJustCheckWhatLeft = config->getAttrBool("just_check_what_left", true);
	mDoc = AosDfmDoc::cloneDoc(mDocType);
	try
	{
		if(gTestRaftAPI)
		{
//			mBlobSEAPI = OmnNew AosBlobSEAPI(mRundata, gApp_config);
			//RaftStateMachine = new AosRaftStateMachineBlobSE(buff);
			//blob = (AosBlobSEAPI*)AosRaftStateMachine::createRaftStateMachineStatic(rdata, "AosBlobSEAPI", 1);
			AosXmlTagPtr blobse_conf = gApp_config->getFirstChild("BlobSE");
			mBlobSEAPI = new AosBlobSEAPI(mRundata, 0, 10, blobse_conf->getAttrU32("header_custom_data_size", 0));
		}
		else
		{
//			mBlobSE = OmnNew AosBlobSE(mRundata.getPtrNoLock(), mDoc, gApp_config);
		}
	}
	catch(const OmnExcept &e)
	{
		OmnAlarm << "Failed to create AosRawFile obj: " << e.getErrmsg() << enderr;
	}
	catch(...)
	{
		OmnAlarm << "unknown exception catched during BlobSE obj creation " << enderr;
	}
	if(gTestRaftAPI)
	{
		if (!mBlobSEAPI)
		{
			OmnAlarm << "Failed to create mBlobSEAPI obj" << enderr;
			return;
		}
		if (!mBlobSEAPI->init(mRundata, 0))
		{
			OmnAlarm << "mBlobSEAPI->init failed, last applied oprid:" << 0 << enderr;
			return;
		}
	}
	else
	{
		if (mBlobSE.isNull())
		{
			OmnAlarm << "Failed to create AosBlobSE obj" << enderr;
			return;
		}
	}

	mTestDurationSec = gTestDuration;
	mTestCount = 0;
	muFileCreated = 0;
	mReadTestCount = 0;
	mCreateTestCount = 0;
	mDelTestCount = 0;
	mModifyTestCount = 0;
	mReadFailedCount = 0;
	mCreateFailedCount = 0;
	mDelFailedCount = 0;
	mModifyFailedCount = 0;
	mLastAllocatedDocID = 1;
	mNumFinished = 0;
	mCrtSec = 0;
	mOprID = 1;
	mFile = OmnNew OmnFile(AosMemoryCheckerArgsBegin);
	if (mFile.isNull())
	{
		OmnAlarm << "mFile.isNull()" << enderr;
		return;
	}

	mMaxSnapshotNum = 20;		// Ketty 2015/04/05
	mMaxDocid = 0;				// Ketty 2015/04/05

}

AosBlobSETester::~AosBlobSETester()
{
}


bool
AosBlobSETester::start()
{
	OmnThreadedObjPtr thisptr(this, false);
//	mNumThreadsRunning = mNumOfThreads;
	if (!loadSeed())
	{
		OmnAlarm << "loadSeed failed." << enderr;
		return false;
	}
	AosXmlTagPtr config = gApp_config->getFirstChild();
	config = config->getFirstChild(mName.data());
	OmnString docs_left_file = config->getAttrStrSimp("docs_left", "");
	OmnString docs_deleted_file = config->getAttrStrSimp("docs_deleted", "");
	bool bBrandNewTest = false;
	if (!mFile->openFile1(docs_left_file, OmnFile::eReadWrite))
	{
		OmnScreen << "Brand new test" << endl;
		bBrandNewTest = true;
	}
	if (!bBrandNewTest)
	{
		OmnScreen << "not a brand new test, checking what's left" << endl;
		OmnString tmp = "";
		bool bFinished = false;
		while(1)
		{
			tmp = mFile->getLine(bFinished);
			if (bFinished)
			{
				break;
			}
			mvDoc.push_back(tmp.toU64(0));
		}
		if (!mFile->openFile1(docs_deleted_file, OmnFile::eReadWrite))
		{
			OmnScreen << "no deleted docs" << endl;
		}
		bFinished = false;
		while(1)
		{
			tmp = mFile->getLine(bFinished);
			if (bFinished)
			{
				break;
			}
			mvDeletedDoc.push_back(tmp.toU64(0));
		}
		testRestart();
		mvDoc.clear();
		if (mJustCheckWhatLeft)
		{
			return true;
		}
	}
	if (0 < gRandomSeed)
	{
		OmnScreen << "Seed for this trial: '" << gRandomSeed << "'" << endl;
		OmnRandom::setSeed(gRandomSeed);
	}
	else
	{
		u32 uTime = time(NULL);
		OmnScreen << "Seed for this trial: '" << uTime << "'" << endl;
		OmnRandom::setSeed(uTime);
	}
	mCrtSec = OmnTime::getCrtSec();
	OmnScreen << "Weights for this trial" << endl
			<< "read:'" << mReadWeight << "'" << endl
			<< "create:'" << mCreateWeight << "'" << endl
			<< "modify:'" << mModifyWeight << "'" << endl
			<< "delete:'" << mDeleteWeight << "'" << endl
			<< "create_snapshot:'" << mCreateSnapshotWeight << "'" << endl
			<< "commit_snapshot:'" << mCommitSnapshotWeight << "'" << endl
			<< "rolback_snapshot:'" << mRollbackSnapshotWeight << "'" << endl
			<< "submit_snapshot:'" << mSubmitSnapshotWeight << "'" << endl
			<< "create_to_snapshot:'" << mCreateDocToSnapWeight << "'" << endl
			<< "modify_from_snapshot:'" << mModifyDocFromSnapWeight << "'" << endl
			<< "delete_from_snapshot:'" << mDeleteDocFromSnapWeight << "'" << endl
			<< "read_from_snapshot:'" << mReadDocFromSnapWeight << "'" << endl;

	for (int i = 0; i < mNumOfThreads; i++)
	{
		mThreads[i] = OmnNew OmnThread(thisptr, "BlobSETester", i, false, false, __FILE__, __LINE__);
		mThreads[i]->start();
	}
	while (1)
	{
		mLockRaw->lock();
		if (mNumFinished < mNumOfThreads)
		{
			mLockRaw->unlock();
			OmnSleep(1);
		}
		else
		{
			mLockRaw->unlock();
			break;
		}
	}
	//save the docids left after the test
	if (!mFile->openFile1(docs_left_file, OmnFile::eWriteCreate))
	{
		OmnScreen << "can not open docid file '" << docs_left_file << "'" << endl;
	}
	else
	{
		OmnString tmp;
		for (std::vector<u64>::iterator it = mvDoc.begin();it != mvDoc.end();it++)
		{
			tmp = "";
			tmp << *it << "\n";
			mFile->append(tmp, true);
		}
		mFile->closeFile();
	}
	//save the deleted docids
	if (!mFile->openFile1(docs_deleted_file, OmnFile::eWriteCreate))
	{
		OmnScreen << "can not open docid file '" << docs_deleted_file << "'" << endl;
	}
	else
	{
		OmnString tmp;
		for (std::vector<u64>::iterator it = mvDeletedDoc.begin();it != mvDeletedDoc.end();it++)
		{
			tmp = "";
			tmp << *it << "\n";
			mFile->append(tmp, true);
		}
		mFile->closeFile();
	}
	int iRet = 0;
	if (gTestRaftAPI)
	{
		iRet = mBlobSEAPI->shutdown(mRundata);
		if (0 != iRet)
		{
			OmnAlarm << "mBlobSEAPI->shutdown failed, iRet=" << iRet << enderr;
		}
	}
	else
	{
		iRet = mBlobSE->shutdown(mRundata.getPtrNoLock());
		if (0 != iRet)
		{
			OmnAlarm << "blobse shutdown failed, iRet=" << iRet << enderr;
		}
	}
	OmnScreen << "Read Test Count:" << mReadTestCount << " Failed Count:" << mReadFailedCount << endl;
	OmnScreen << "Create Test Count:" << mCreateTestCount << " Failed Count:" << mCreateFailedCount << endl;
	OmnScreen << "Modify Test Count:" << mModifyTestCount << " Failed Count:" << mModifyFailedCount << endl;
	OmnScreen << "Del Test Count:" << mDelTestCount << " Failed Count:" << mDelFailedCount << endl;
	OmnScreen << "Total Test Count:" << mReadTestCount + mCreateTestCount + mModifyTestCount + mDelTestCount
			<< " Total Failed Count:" << mReadFailedCount + mCreateFailedCount + mModifyFailedCount + mDelFailedCount << endl;
	return true;
}


bool
AosBlobSETester::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	u64 start_docid = 1;
	u32 num_docids = 0;
	if (gUnitTest)
	{
		compactionUnitTest();
		mLock->lock();
		mNumFinished++;
		mLock->unlock();
		state = OmnThrdStatus::eExit;
		return true;
	}
	if (mTestDurationSec > 0)
	{
		// Run the torturer for 'mTestDuration' amount of time.
		while (OmnTime::getCrtSec() < mCrtSec + mTestDurationSec)
		{
			while (state == OmnThrdStatus::eActive)
			{
				if (OmnTime::getCrtSec() < mCrtSec + mTestDurationSec)
				{
					oneBatchTests(mBatchSize, start_docid, num_docids);
				}
				else
				{
					break;
				}
			}
		}
	}
	else
	{
		OmnScreen << "mTestDurationSec=" << mTestDurationSec << endl;
		if (mTries <= 0) mTries = eDefaultTries;
		for (int i=0; i<mTries; i += mBatchSize)
		{
			oneBatchTests(mBatchSize, start_docid, num_docids);
		}
	}
	mLock->lock();
	mNumFinished++;
	mLock->unlock();
	state = OmnThrdStatus::eExit;
	return true;
}


bool
AosBlobSETester::signal(const int threadLogicId)
{
	return true;
}


bool
AosBlobSETester::allocateDocid(
		u64	&start_docid,
		u32	&num_docids)
{
	mLockRaw->lock();
	u64 docid = mLastAllocatedDocID;
	mLastAllocatedDocID += mDocidAllocateSize;
	mLockRaw->unlock();
	start_docid = docid;
	num_docids = mDocidAllocateSize;
	return true;
}


bool
AosBlobSETester::oneBatchTests(
		const int	batch_size,
		u64			&start_docid,
		u32			&num_docids)
{
	int testMinValue[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
	int testWeight[] = {mCreateWeight, mModifyWeight, mReadWeight, mDeleteWeight,
					mCreateSnapshotWeight, mCommitSnapshotWeight, mRollbackSnapshotWeight,
					mSubmitSnapshotWeight, mCreateDocToSnapWeight, mModifyDocFromSnapWeight,
					mDeleteDocFromSnapWeight, mReadDocFromSnapWeight};

	bool rslt = false;
	for (int i=0; i<batch_size; i++)
	{
		//switch (OmnRandom::intByRange(
		//		0, 0, mCreateWeight,
		//		1, 1, mModifyWeight,
		//		2, 2, mReadWeight,

        //		3, 3, mDeleteWeight)),
		switch(OmnRandom::intByRange(
					testMinValue, testMinValue, testWeight, 12))
		{
		case 0:
			 mCreateTestCount++;
			 mTestCount++;
			 if (num_docids <= 0)
			 {
				allocateDocid(start_docid, num_docids);
//				if (1 < start_docid)
//				{
//					if (!testCompactionAsRaftLeader())
//					{
//						OmnAlarm << "testCompactionAsRaftLeader failed." << enderr;
//					}
//				}
			 }
			 aos_assert_r(num_docids > 0, false);
			 aos_assert_r(start_docid >= 0, false);
			 num_docids--;
			 if(gTestRaftAPI)
			 {
				 if (!testCreateDocWithRaftInterface(start_docid++, mOprID++))
				 {
					mCreateFailedCount++;
					OmnAlarm << "Test No." << mTestCount << " failed on doc creation, docid:" << start_docid
							<< " oprid:" << mOprID << enderr;
					return false;
				 }
			 }
			 else
			 {
				 if (!testCreateDoc(start_docid++))
				 {
					mCreateFailedCount++;
					OmnAlarm << "Test No." << mTestCount << " failed on doc creation, docid:" << start_docid << enderr;
					return false;
				 }
			 }
			 break;

		case 1:
			 mModifyTestCount++;
			 mTestCount++;
			 if(gTestRaftAPI)
			 {
				 if (!testModifyDocWithRaftInterface(mOprID))
				 {
					mModifyFailedCount++;
					OmnAlarm << "Test No." << mTestCount << " failed on doc modification, oprid:" << mOprID << enderr;
					return false;
				 }
			 }
			 else
			 {
				 if (!testModifyDoc())
				 {
					mModifyFailedCount++;
					OmnAlarm << "Test No." << mTestCount << " failed on doc modification." << enderr;
					return false;
				 }
			 }
			 break;

		case 2:
			 mReadTestCount++;
			 mTestCount++;
			 if(gTestRaftAPI)
			 {
				 if (!testReadDocWithRaftInterface())
				 {
					mReadFailedCount++;
					OmnAlarm << "Test No." << mTestCount << " failed on doc reading" << enderr;
					return false;
				 }
			 }
			 else
			 {
				 if (!testReadDoc())
				 {
					mReadFailedCount++;
					OmnAlarm << "Test No." << mTestCount << " failed on doc reading." << enderr;
					return false;
				 }
			 }
			 break;

		case 3:
			 mDelTestCount++;
			 mTestCount++;
			 if(gTestRaftAPI)
			 {
				 if (!testDeleteDocWithRaftInterface(mOprID))
				 {
					mDelFailedCount++;
					OmnAlarm << "Test No." << mTestCount << " failed on doc deletion." << enderr;
					return false;
				 }
			 }
			 else
			 {
				 if (!testDeleteDoc())
				 {
					mDelFailedCount++;
					OmnAlarm << "Test No." << mTestCount << " failed on doc deletion." << enderr;
					return false;
				 }
			 }
			 break;

		case 4:
			 mCreateSnapshotTestCount++;
			 mTestCount++;
			 if (!testCreateSnapshot())
			 {
				OmnAlarm << "Test No." << mTestCount << " failed on snapshot creation." << enderr;
				return false;
			 }
			 break;

		case 5:
			 mCommitSnapshotTestCount++;
			 mTestCount++;
			 if (!testCommitSnapshot())
			 {
				OmnAlarm << "Test No." << mTestCount << " failed on snapshot commition." << enderr;
				return false;
			 }
			 break;

		case 6:
			 mRollbackSnapshotTestCount++;
			 mTestCount++;
			 if (!testRollbackSnapshot())
			 {
				OmnAlarm << "Test No." << mTestCount << " failed on snapshot rollback." << enderr;
				return false;
			 }
			 break;

		case 7:
			 mSubmitSnapshotTestCount++;
			 mTestCount++;
			 if (!testSubmitSnapshot())
			 {
				OmnAlarm << "Test No." << mTestCount << " failed on snapshot submition." << enderr;
				return false;
			 }
			 break;

		case 8:
			 mCreateDocToSnapTestCount++;
			 mTestCount++;
			 if (num_docids <= 0)
			 {
				 allocateDocid(start_docid, num_docids);
			 }
			 aos_assert_r(num_docids > 0, false);
			 aos_assert_r(start_docid >= 0, false);
			 num_docids--;
			 if (!testCreateDocToSnap(start_docid++))
			 {
				OmnAlarm << "Test No." << mTestCount << " failed on create doc to snapshot." << enderr;
				return false;
			 }
			 break;

		case 9:
			 mModofyDocFromSnapTestCount++;
			 mTestCount++;
			 if (!testModifyDocFromSnap())
			 {
				OmnAlarm << "Test No." << mTestCount << " failed on modify doc from snapshot." << enderr;
				return false;
			 }
			 break;

		case 10:
			 mDeleteDocFromSnapTestCount++;
			 mTestCount++;
			 if (!testDeleteDocFromSnap())
			 {
				OmnAlarm << "Test No." << mTestCount << " failed on delete doc from snapshot." << enderr;
				return false;
			 }
			 break;

		case 11:
			 mReadDocFromSnapTestCount++;
			 mTestCount++;
			 if (!testReadDocFromSnap())
			 {
				OmnAlarm << "Test No." << mTestCount << " failed on read doc from snapshot." << enderr;
				return false;
			 }
			 break;

		default:
			 OmnShouldNeverComeHere;
			 break;
		}
	}

	return true;
}


bool
AosBlobSETester::testCreateDoc(const u64 docid)
{
	// This function creates a raw file.
	OmnScreen << "Test No." << mTestCount << ":doc creation." << endl;

	// Ketty 2015/04/07
	if(docid > mMaxDocid) mMaxDocid = docid;

	u64 snap_id = 0;
	u64 trans_id = 0;
	AosDfmDocPtr doc = mDoc->clone(docid);
	AosBuffPtr buff = createData(docid);
	if (buff.isNull())
	{
		OmnAlarm << "createData failed, docid:'" << doc->getDocid()
			<< "'" << enderr;
		return false;
	}
	doc->setBodyBuff(buff);
	doc->setOrigLen(buff->dataLen());
	u64 ullTimestamp = OmnTime::getTimestamp();
	bool rslt = mBlobSE->saveDoc(mRundata, snap_id, doc, trans_id, ullTimestamp);
	if (!rslt)
	{
		OmnAlarm << "doc creation failed, docid:" << doc->getDocid() << " snap_id:" << snap_id
				<< " timestamp:" << ullTimestamp << enderr;
		return false;
	}
	mvDoc.push_back(doc->getDocid());
	return true;
}


bool
AosBlobSETester::testCreateDocWithRaftInterface(
		const u64 docid,
		const u64 ullOprID)
{
	// This function creates a raw file.
	OmnScreen << "Test No." << mTestCount << ":doc creation, docid:" << docid << endl;
	// Ketty 2015/04/07
	if(docid > mMaxDocid) mMaxDocid = docid;
	u64 ullSnapshotID = 0;
	AosBlobSEReqEntryPtr pAosBlobSEReqEntry = OmnNew AosBlobSEReqEntry(mBlobSEAPI->getConfig().header_custom_data_size);
	u64 ullTimestamp = OmnTime::getTimestamp();
	pAosBlobSEReqEntry->setTimestamp(ullTimestamp);
	pAosBlobSEReqEntry->setOprID(ullOprID);
	pAosBlobSEReqEntry->setSnapshotID(ullSnapshotID);
	pAosBlobSEReqEntry->setDocid(docid);
	pAosBlobSEReqEntry->setReqTypeAndResult(AosBlobSEReqEntry::eSave);
	AosBuffPtr pHeaderCustomData = OmnNew AosBuff(mBlobSEAPI->getConfig().header_custom_data_size AosMemoryCheckerArgs);
	memset(pHeaderCustomData->data(), 125, pHeaderCustomData->buffLen());
	pHeaderCustomData->setDataLen(mBlobSEAPI->getConfig().header_custom_data_size);
	pAosBlobSEReqEntry->setHeaderCustomDataBuff(pHeaderCustomData);
	AosBuffPtr pBodyDataBuff = createData(docid);
	if (pBodyDataBuff.isNull())
	{
		OmnAlarm << "createData failed, docid:" << docid << enderr;
		return false;
	}
	pAosBlobSEReqEntry->setBodyBuff(pBodyDataBuff);
	AosBuffPtr pBuff = OmnNew AosBuff(5000 AosMemoryCheckerArgs);
	int iRet = pAosBlobSEReqEntry->serializeToBuff(pBuff);
	if (0 != iRet)
	{
		OmnAlarm << "pAosBlobSEReqEntry->serializeToBuff failed, iRet=" << iRet << " docid:" << docid
				<< " oprid:" << ullOprID << enderr;
		return false;
	}
	u32 ulTermID = 0;
	bool rslt = mBlobSEAPI->appendEntry(mRundata, ulTermID, ullOprID, pBuff);
	if (!rslt)
	{
		OmnAlarm << "mBlobSEAPI->appendEntry failed for doc creation, docid:" << docid << " snap_id:" << ullSnapshotID
				<< " timestamp:" << ullTimestamp << enderr;
		return false;
	}
	rslt = mBlobSEAPI->apply(mRundata, ullOprID);
	if (!rslt)
	{
		OmnAlarm << "mBlobSEAPI->apply failed" << " oprid:" << ullOprID << enderr;
		return false;
	}
	mvDoc.push_back(docid);
	return true;
}


bool
AosBlobSETester::testModifyDoc()
{
	return true;
	OmnScreen << "Test No." << mTestCount << ":doc modification." << endl;
	u64 snap_id = 0;
	u64 trans_id = 0;
	int iRet = 0;
	if (0 >= mvDoc.size())	//no file has been created yet
	{
		OmnScreen << "modification canceled because no doc is a" << endl;
		return true;
	}
	u64 uDocIndex = rand() % mvDoc.size();
	u64 docid = mvDoc[uDocIndex];
	AosDfmDocPtr doc = mDoc->clone(docid);
	itr_t it = mDocSeedMap.find(docid);
	if (mDocSeedMap.end() == it)
	{
		OmnAlarm << "can not find seed info in mDocSeedMap for docid:" << docid << enderr;
		return false;
	}
	u32 seed_id = it->second.first + 1;
	AosBuffPtr buff = createData(docid, seed_id);
	doc->setBodyBuff(buff);
	doc->setOrigLen(buff->dataLen());
	u64 ullTimestamp = OmnTime::getTimestamp();
	bool rslt = mBlobSE->saveDoc(mRundata, snap_id, mDoc, trans_id, ullTimestamp);
	if (!rslt)
	{
		OmnAlarm << "doc modification failed on saving, docid:'" << doc->getDocid() << " snap_id:" << snap_id
				<< " timestamp:" << ullTimestamp << enderr;
		return false;
	}
	doc = mBlobSE->readDoc(mRundata.getPtrNoLock(), snap_id, docid, 0);
	if (doc.isNull())
	{
		OmnAlarm << "Failed to read modified doc with docid:'" << docid
				<< "' snap_id '" << snap_id << "'" << enderr;
		return false;
	}
	AosBuffPtr newBuff = doc->getBodyBuff();
	i64 buffLen = buff->dataLen();
	i64 newBuffLen = newBuff->dataLen();
	if(buffLen != newBuffLen)
	{
		OmnAlarm << "The length of the buff of the saved doc is different from"
			<< " that of the buff of the doc that is wanted to be modified for the same docid:'" << docid << "'" << enderr;
		return false;
	}
	if(0 != memcmp(buff->data(), newBuff->data(), buffLen))
	{
		OmnAlarm << "The buff of the saved doc is different from the buff of the"
			<< " doc that is wanted to be modified for the same docid:'" << docid << "'" << enderr;
		return false;
	}
	return true;
}


bool
AosBlobSETester::testModifyDocWithRaftInterface(u64 &ullOprID)
{
	OmnScreen << "Test No." << mTestCount << ":doc modification." << endl;
	u64 snap_id = 0;
	if (0 >= mvDoc.size())	//no file has been created yet
	{
		OmnScreen << "modification canceled because no doc is available." << endl;
		return true;
	}
	u64 uDocIndex = rand() % mvDoc.size();
	u64 docid = mvDoc[uDocIndex];
	u64 ullSnapshotID = 0;
	itr_t it = mDocSeedMap.find(docid);
	if (mDocSeedMap.end() == it)
	{
		OmnAlarm << "can not find seed info in mDocSeedMap for docid:" << docid << enderr;
		return false;
	}
	OmnScreen << "modifying doc with docid:" << docid << endl;
	u32 seed_id = (it->second.first + 1) % gSeedMap.size();
	AosBuffPtr pBodyDataBuff = createData(docid, seed_id);
	AosBlobSEReqEntryPtr pAosBlobSEReqEntry = OmnNew AosBlobSEReqEntry(mBlobSEAPI->getConfig().header_custom_data_size);
	u64 ullTimestamp = OmnTime::getTimestamp();
	pAosBlobSEReqEntry->setTimestamp(ullTimestamp);
	pAosBlobSEReqEntry->setOprID(ullOprID);
	pAosBlobSEReqEntry->setSnapshotID(ullSnapshotID);
	pAosBlobSEReqEntry->setDocid(docid);
	pAosBlobSEReqEntry->setReqTypeAndResult(AosBlobSEReqEntry::eSave);
	AosBuffPtr pHeaderCustomData = OmnNew AosBuff(mBlobSEAPI->getConfig().header_custom_data_size AosMemoryCheckerArgs);
	memset(pHeaderCustomData->data(), 125, pHeaderCustomData->buffLen());
	pHeaderCustomData->setDataLen(mBlobSEAPI->getConfig().header_custom_data_size);
	pAosBlobSEReqEntry->setHeaderCustomDataBuff(pHeaderCustomData);
	pAosBlobSEReqEntry->setBodyBuff(pBodyDataBuff);
	AosBuffPtr pBuff = OmnNew AosBuff(5000 AosMemoryCheckerArgs);
	int iRet = pAosBlobSEReqEntry->serializeToBuff(pBuff);
	if (0 != iRet)
	{
		OmnAlarm << "pAosBlobSEReqEntry->serializeToBuff failed, iRet=" << iRet << " docid:" << docid
				<< " oprid:" << ullOprID << enderr;
		return false;
	}
	u32 ulTermID = 0;
	bool rslt = mBlobSEAPI->appendEntry(mRundata, ulTermID, ullOprID, pBuff);
	if (!rslt)
	{
		OmnAlarm << "mBlobSEAPI->appendEntry failed for doc modification, docid:" << docid << " snap_id:" << ullSnapshotID
				<< " timestamp:" << ullTimestamp << enderr;
		return false;
	}
	rslt = mBlobSEAPI->apply(mRundata, ullOprID);
	if (!rslt)
	{
		OmnAlarm << "mBlobSEAPI->apply failed" << " oprid:" << ullOprID << enderr;
		ullOprID++;
		return false;
	}
	ullOprID++;
	rslt = mBlobSEAPI->readDoc(mRundata, 0, ullSnapshotID, docid, 0, pBuff, pHeaderCustomData);	//get the newest one
	if (!rslt)
	{
		OmnAlarm << "Failed to read modified doc with docid:'" << docid
				<< "' snap_id '" << snap_id << "'" << enderr;
		return false;
	}
	i64 oldBodyBuffLen = pBodyDataBuff->dataLen();
	if(oldBodyBuffLen != pBuff->dataLen())
	{
		OmnAlarm << "The length of the buff of the saved doc is different from"
			<< " that of the buff of the doc that is wanted to be modified for the same docid:'" << docid << "'" << enderr;
		return false;
	}
	if(0 != memcmp(pBodyDataBuff->data(), pBuff->data(), oldBodyBuffLen))
	{
		OmnAlarm << "The buff of the saved doc is different from the buff of the"
			<< " doc that is wanted to be modified for the same docid:'" << docid << "'" << enderr;
		return false;
	}
	return true;
}


bool
AosBlobSETester::testDeleteDoc()
{
	OmnScreen << "Test No." << mTestCount << ":doc deletion." << endl;
	int iRet = 0;
	u64 snap_id = 0;
	u64 trans_id = 0;
	u64 ullTimestamp = OmnTime::getTimestamp();
	if (0 >= mvDoc.size())	//no doc has been created yet
	{
		OmnScreen << "deleting doc that does not exist, docid:" << mLastAllocatedDocID << endl;
		AosDfmDocPtr doc = mDoc->clone(mLastAllocatedDocID);
		if (mBlobSE->deleteDoc(mRundata, snap_id, doc, trans_id, ullTimestamp))
		{
			OmnAlarm << "result of deleting a doc does not exist should not be true." << enderr;
			return false;
		}
		else
		{
			return true;
		}
	}
	u64 uDocIndex = rand() % mvDoc.size();
	u64 docid = mvDoc[uDocIndex];
	AosDfmDocPtr doc = mDoc->clone(docid);
	OmnScreen << "deleting doc with docid:" << docid << endl;
	if (!mBlobSE->deleteDoc(mRundata, snap_id, doc, trans_id, ullTimestamp))
	{
		OmnAlarm << "deletion failed on docid:" << docid << " snap_id:" << snap_id
				<< " trans_id:" << trans_id << " timestamp:" << ullTimestamp << enderr;
		return false;
	}
	mvDeletedDoc.push_back(docid);
	mvDoc.erase(mvDoc.begin() + uDocIndex);
	itr_t mDocSeedMap_it = mDocSeedMap.find(docid);
	if(mDocSeedMap_it == mDocSeedMap.end())
	{
		OmnAlarm << "can not find pair for docid:'" << docid << "' in mDocSeedMap." << enderr;
		return false;
	}
	mDocSeedMap.erase(mDocSeedMap_it);
	//check the deletion result
	doc = mBlobSE->readDoc(mRundata, snap_id, docid, 0);
	if (doc.notNull())
	{
		OmnAlarm << "doc should not be read back after deletion on docid:'" << docid << "'." << enderr;
		return false;
	}
	doc = mBlobSE->readDoc(mRundata, snap_id, docid, 1);
	if (doc.isNull())
	{
		OmnAlarm << "Failed to read doc with docid:" << docid << " snap_id:" << snap_id
				<< " timestamp:" << 1 << enderr;
		return false;
	}
	return true;
}


bool
AosBlobSETester::testDeleteDocWithRaftInterface(u64 &ullOprID)
{
	OmnScreen << "Test No." << mTestCount << ":doc deletion." << endl;
	int iRet = 0;
	u64 docid = 0;
	u64 ullSnapshotID = 0;
	AosBlobSEReqEntryPtr pAosBlobSEReqEntry = OmnNew AosBlobSEReqEntry(mBlobSEAPI->getConfig().header_custom_data_size);
	u64 ullTimestamp = OmnTime::getTimestamp();
	pAosBlobSEReqEntry->setTimestamp(ullTimestamp);
	pAosBlobSEReqEntry->setSnapshotID(ullSnapshotID);
	pAosBlobSEReqEntry->setReqTypeAndResult(AosBlobSEReqEntry::eDelete);
	AosBuffPtr pBuff = OmnNew AosBuff(5000 AosMemoryCheckerArgs);
	u64 ullDocIndex = 0;
	if (0 >= mvDoc.size())	//no doc has been created yet
	{
//		docid = mLastAllocatedDocID;
//		OmnScreen << "deleting doc that does not exist, docid:" << docid << endl;
//		pAosBlobSEReqEntry->setDocid(docid);
		OmnScreen << "no doc to delete" << endl;
		return true;
	}
	else
	{
		ullDocIndex = rand() % mvDoc.size();
		docid = mvDoc[ullDocIndex];
		pAosBlobSEReqEntry->setDocid(docid);
		pAosBlobSEReqEntry->setOprID(ullOprID);
		OmnScreen << "deleting doc with docid:" << docid << endl;
	}
	iRet = pAosBlobSEReqEntry->serializeToBuff(pBuff);
	if (0 != iRet)
	{
		OmnAlarm << "pAosBlobSEReqEntry->serializeToBuff failed, iRet=" << iRet << " docid:" << docid << enderr;
		return false;
	}
	u32 ulTermID = 0;
	bool rslt = mBlobSEAPI->appendEntry(mRundata, ulTermID, ullOprID, pBuff);
	if (!rslt)
	{
		OmnAlarm << "mBlobSEAPI->appendEntry failed for doc deletion, docid:" << docid << " snap_id:" << ullSnapshotID
				<< " timestamp:" << ullTimestamp << enderr;
		ullOprID++;
		return false;
	}
	rslt = mBlobSEAPI->apply(mRundata, ullOprID);
	if (!rslt)
	{
		OmnAlarm << "mBlobSEAPI->apply failed" << " oprid:" << ullOprID << enderr;
		return false;
	}
	ullOprID++;
	if (0 < mvDoc.size())
	{
		mvDeletedDoc.push_back(docid);
		mvDoc.erase(mvDoc.begin() + ullDocIndex);
		itr_t mDocSeedMap_it = mDocSeedMap.find(docid);
		if(mDocSeedMap_it == mDocSeedMap.end())
		{
			OmnAlarm << "can not find pair for docid:'" << docid << "' in mDocSeedMap." << enderr;
			return false;
		}
		mDocSeedMap.erase(mDocSeedMap_it);
	}
	//check the deletion result
	AosBuffPtr pHeaderCustomData;
	rslt = mBlobSEAPI->readDoc(mRundata, 0, ullSnapshotID, docid, 0, pBuff, pHeaderCustomData);	//get the newest one
	if (!rslt)
	{
		OmnAlarm << "read failed on deleted doc, docid:" << docid << " snapid:" << ullSnapshotID
				<< " timestamp:" << 0 << enderr;
		return false;
	}
	if (pBuff.notNull())
	{
		OmnAlarm << "read result of a deleted doc should be null" << enderr;
		return false;
	}
	rslt = mBlobSEAPI->readDoc(mRundata, 0, ullSnapshotID, docid, 1, pBuff, pHeaderCustomData);	//get the oldest one
	if (!rslt)
	{
		OmnAlarm << "read failed on deleted doc, docid:" << docid << " snapid:" << ullSnapshotID
				<< " timestamp:" << 1 << enderr;
		return false;
	}
	if (pBuff.isNull())
	{
		OmnAlarm << "read failed on deleted doc, docid:" << docid << " snapid:" << ullSnapshotID
				<< " timestamp:" << 1 << enderr;
		return false;
	}
	return true;
}


bool
AosBlobSETester::testReadDoc()
{
//	randomly pick a file;
//	randomly determine read position
//	randomly determine read length;
	OmnScreen << "Test No." << mTestCount << ":reading doc." << endl;
	u64 snap_id = 0;
	AosDfmDocPtr doc;
	u64 docid = 0;
	if (0 >= mvDoc.size())	//no file has been created yet
	{
		OmnScreen << "0 == mvDoc.size():" << mvDoc.size() << endl;
		u64 docid = rand();
		doc = mBlobSE->readDoc(mRundata.getPtrNoLock(), snap_id, docid, 0);		//0 means to read the newest one
		if (doc.notNull())
		{
			OmnAlarm << "result of reading a doc that does not exist should be NULL on docid:'"
					<< docid << "'." << enderr;
			return false;
		}
		return true;
	}
	AosBuffPtr content = OmnNew AosBuff(10000 AosMemoryCheckerArgs);
	aos_assert_rr(content.notNull(), mRundata.getPtrNoLock(), false);
	itr_t it;
	p_seed_repeat_t* seedPair;
	u32 seed_id = 0;
	u32 repeat_time = 0;
	const OmnString sTemp = "";
	AosBuffPtr doc_buff;
	AosBuffPtr pHeaderCustomData;
	switch(OmnRandom::percent(mReadInvalidWeight, mReadValidWeight, mReadDeletedWeight))
	{
	case 0:	//read invalid docids
		docid = mLastAllocatedDocID * 3;
		OmnScreen << "reading docid:" << docid << " as invalid reading" << endl;
		doc = mBlobSE->readDoc(mRundata.getPtrNoLock(), snap_id, docid, 0);
		if (doc.notNull())
		{
			OmnAlarm << "result of reading a doc that does not exist should be NULL on docid:'"
					<< docid << "'." << enderr;
			return false;
		}
		break;

	case 1:	//read valid docids
		docid = mvDoc[rand() % mvDoc.size()];
		OmnScreen << "reading docid:" << docid << " as valid reading" << endl;
		doc = mBlobSE->readDoc(mRundata.getPtrNoLock(), snap_id, docid, 0);
		if (doc.isNull())
		{
			OmnAlarm << "Failed to read doc with docid:" << docid
					<< " snap_id: " << snap_id << " timestamp:" << 0 << enderr;
			return false;
		}
		it = mDocSeedMap.find(docid);
		if (mDocSeedMap.end() == it)
		{
			OmnAlarm << "can not find seed in seed map for docid:'" << docid << "'." << enderr;
			return false;
		}
		seedPair = &it->second;
		seed_id = seedPair->first;
		repeat_time = seedPair->second;
		for (int i = 0; i < repeat_time; ++i)
		{
			content->setBuff(gSeedMap[seed_id].data(), gSeedMap[seed_id].length());
		}
		doc_buff = doc->getBodyBuff();
		if (doc_buff.isNull())
		{
			OmnAlarm << "can not get body buff for docid:" << docid << enderr;
			return false;
		}
		if (content->dataLen() != doc_buff->dataLen())
		{
			OmnAlarm << "content length does match for docid:" << docid
					<< " content.length()=" << content->dataLen()
					<< " doc_buff->dataLen()=" << doc_buff->dataLen() << enderr;
			return false;
		}
		if (0 != memcmp(content->data(), doc_buff->data(), content->dataLen()))
		{
			OmnAlarm << "Read validation failed for doc with docid:'" << docid << "'" << enderr;
			OmnAlarm << "Content should be:'" << content->data() << "'" << enderr;
			OmnAlarm << "Content from file:'" << doc_buff->data() <<"' with dataLen:" << doc_buff->dataLen() << enderr;
			return false;
		}
		else
		{
	//		OmnScreen << "Read validation passed for file '" << rawfile->getFileNameWithFullPath() << "' with rawfile id:'" << raw_file.rawFileID
	//				  << "' siteid '" << raw_file.siteID << "' cubeid '" << raw_file.cubeID << "' aseid '" << raw_file.aseID << "'" << endl;
			return true;
		}
		break;

	case 2:	//read deleted docs

		break;
	default:
		OmnAlarm << "Invalid percentage!" << enderr;
		return false;
	}

	return true;
}


bool
AosBlobSETester::testReadDocWithRaftInterface()
{

	//	randomly pick a file;
	//	randomly determine read position
	//	randomly determine read length;
	OmnScreen << "Test No." << mTestCount << ":reading doc." << endl;
	u64 snap_id = 0;
	u64 docid = 0;
	u64 ullSnapshotID = 0;
	AosBlobSEReqEntryPtr pAosBlobSEReqEntry = OmnNew AosBlobSEReqEntry(mBlobSEAPI->getConfig().header_custom_data_size);
	u64 ullTimestamp = 0;
	AosBuffPtr pBuff = OmnNew AosBuff(5000 AosMemoryCheckerArgs);
	int iRet = 0;
	bool rslt = false;
	AosBuffPtr pHeaderCustomData;
	if (0 >= mvDoc.size())	//no file has been created yet
	{
		OmnScreen << "0 == mvDoc.size():" << mvDoc.size() << " abort." << endl;
		return true;
		docid = rand();
		OmnScreen << "0 == mvDoc.size():" << mvDoc.size() << " reading with docid:" << docid << endl;
		rslt = mBlobSEAPI->readDoc(mRundata, 0, ullSnapshotID, docid, ullTimestamp, pBuff, pHeaderCustomData);
		if (!rslt)
		{
			OmnAlarm << "return value of reading a doc that does not exist should not be 0 on docid:" << docid
					<< " timestamp:" << ullTimestamp << enderr;
			return false;
		}
		return true;
	}
	AosBuffPtr content = OmnNew AosBuff(10000 AosMemoryCheckerArgs);
	aos_assert_rr(content.notNull(), mRundata.getPtrNoLock(), false);
	itr_t it;
	p_seed_repeat_t* seedPair;
	u32 seed_id = 0;
	u32 repeat_time = 0;
	const OmnString sTemp = "";
	AosBuffPtr doc_buff;
	switch(OmnRandom::percent(mReadInvalidWeight, mReadValidWeight, mReadDeletedWeight))
	{
	case 0:	//read invalid docids
		docid = mLastAllocatedDocID * 3;
		OmnScreen << "reading docid:" << docid << " as invalid reading" << endl;
		rslt = mBlobSEAPI->readDoc(mRundata, 0, ullSnapshotID, docid, ullTimestamp, pBuff, pHeaderCustomData);
		if (rslt)
		{
			OmnAlarm << "return value of reading a doc that does not exist should not be 0 on docid:" << docid
					<< " timestamp:" << ullTimestamp << enderr;
			return false;
		}
		break;

	case 1:	//read valid docids
		docid = mvDoc[rand() % mvDoc.size()];
		OmnScreen << "reading docid:" << docid << " as valid reading" << endl;
		rslt = mBlobSEAPI->readDoc(mRundata, 0, ullSnapshotID, docid, ullTimestamp, pBuff, pHeaderCustomData);
		if (!rslt)
		{
			OmnAlarm << "failed to read doc with docid:" << docid
					<< " snapid:" << ullSnapshotID << " timestamp:" << ullTimestamp << enderr;
			return false;
		}
		it = mDocSeedMap.find(docid);
		if (mDocSeedMap.end() == it)
		{
			OmnAlarm << "can not find seed in seed map for docid:'" << docid << "'." << enderr;
			return false;
		}
		seedPair = &it->second;
		seed_id = seedPair->first;
		repeat_time = seedPair->second;
		for (int i = 0; i < repeat_time; ++i)
		{
			content->setBuff(gSeedMap[seed_id].data(), gSeedMap[seed_id].length());
		}
		if (pBuff.isNull())
		{
			OmnAlarm << "can not get body buff for docid:" << docid << enderr;
			return false;
		}
		if (content->dataLen() != pBuff->dataLen())
		{
			OmnAlarm << "content length does match for docid:" << docid
					<< " content.length()=" << content->dataLen()
					<< " doc_buff->dataLen()=" << pBuff->dataLen() << enderr;
			return false;
		}
		if (0 != memcmp(content->data(), pBuff->data(), content->dataLen()))
		{
			OmnAlarm << "Read validation failed for doc with docid:'" << docid << "'" << enderr;
			OmnAlarm << "Content should be:'" << content->data() << "'" << enderr;
			OmnAlarm << "Content from file:'" << pBuff->data() <<"' with dataLen:" << pBuff->dataLen() << enderr;
			return false;
		}
		else
		{
	//		OmnScreen << "Read validation passed for file '" << rawfile->getFileNameWithFullPath() << "' with rawfile id:'" << raw_file.rawFileID
	//				  << "' siteid '" << raw_file.siteID << "' cubeid '" << raw_file.cubeID << "' aseid '" << raw_file.aseID << "'" << endl;
			return true;
		}
		break;

	case 2:	//read deleted docs

		break;
	default:
		OmnAlarm << "Invalid percentage!" << enderr;
		return false;
	}

	return true;
}


u32
AosBlobSETester::pickSeed()
{
	return rand() % gSeedCount;
}


bool
AosBlobSETester::loadSeed()
{
	for(u32 i = 0; i < gSeedCount; i++)
	{
//		mSeedMap[i] = OmnRandom::getRandomStr("", 0, 30);
		gSeedMap[i] = OmnRandom::binaryStr(0, 30);
//		OmnScreen << "seed[" << i << "]:'" << gSeedMap[i] << "'" << endl;
	}
	return true;
}


AosBuffPtr
AosBlobSETester::createData(
		const u64	docid,
		const u32	seed_id)
{
	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	u32 seed = seed_id == 0 ? pickSeed(): seed_id;
	u32 repeat = 0;
	switch(OmnRandom::percent(gBigDocWeight, gSmallDocWeight))
	{
	case 0:	//big
		repeat = OmnRandom::nextInt1(30, 300);
		break;

	case 1:	//small
		repeat = OmnRandom::nextInt1(0, 30);
		break;

	default:
		OmnAlarm << "invalid percentage!" << enderr;
	}
	for (u32 i=0; i<repeat; i++)
	{
		buff->setBuff(gSeedMap[seed].data(), gSeedMap[seed].length());
	}
//	OmnScreen << "data created:'" << endl;
//	for(int i = 0;i < buff->dataLen();i++)
//	{
//		printf("%02X", buff->data()[i]);
//	}
//	printf("'\n'");
//	for(int i = 0;i < buff->dataLen();i++)
//	{
//		printf("%c", buff->data()[i]);
//	}
//	printf("'\n'");
//	OmnScreen << "' with seed:" << seed << " '" << mSeedMap[seed] << "'" << endl;
	mDocSeedMap[docid] = ::make_pair(seed, repeat);
	buff->setDataLen(gSeedMap[seed].length() * repeat);
	return buff;
}


bool
AosBlobSETester::testRestart()
{
	//we only check the docid in log files for now
	u64 snap_id = 0;
	AosDfmDocPtr doc;
	u32 uFailed = 0;
	u64 ullTimestamp = 0;
	AosBuffPtr pHeaderCustomData;
	int iRet = 0;
	bool rslt = false;
	for (std::vector<u64>::iterator it = mvDoc.begin();it != mvDoc.end();it++)
	{
		if(gTestRaftAPI)
		{
			AosBuffPtr pBuff;
			rslt = mBlobSEAPI->readDoc(mRundata, 0, snap_id, *it, ullTimestamp, pBuff, pHeaderCustomData);
			if (!rslt)
			{
				OmnAlarm << "Failed to read doc with docid:" << *it	<< " snap_id:" << snap_id
						<< " timestamp:" << ullTimestamp << enderr;
				uFailed++;
			}
		}
		else
		{
			doc = mBlobSE->readDoc(mRundata, snap_id, *it, ullTimestamp);
			if (doc.isNull())
			{
				OmnAlarm << "Failed to read doc with docid:" << *it	<< " snap_id:" << snap_id
						<< " timestamp:" << ullTimestamp << enderr;
				uFailed++;
			}
		}
	}
	OmnScreen << "mvDoc.size():" << mvDoc.size() << " uFailed=" << uFailed << endl;
	uFailed = 0;
	for (std::vector<u64>::iterator it = mvDeletedDoc.begin();it != mvDeletedDoc.end();it++)
	{
		if(gTestRaftAPI)
		{
			AosBuffPtr pBuff;
			ullTimestamp = 1;
			rslt = mBlobSEAPI->readDoc(mRundata, 0, snap_id, *it, ullTimestamp, pBuff, pHeaderCustomData);
			if (!rslt)
			{
				OmnAlarm << "Failed to read doc with docid:" << *it	<< " snap_id:" << snap_id
						<< " timestamp:" << ullTimestamp << enderr;
				uFailed++;
			}
			ullTimestamp = 0;
			rslt = mBlobSEAPI->readDoc(mRundata, 0, snap_id, *it, ullTimestamp, pBuff, pHeaderCustomData);
			if (!rslt)
			{
				OmnAlarm << "failed to read doc with docid:" << *it
						<< " snapid:" << snap_id << " timestamp:" << ullTimestamp << enderr;
				uFailed++;
			}
		}
		else
		{
			ullTimestamp = 1;
			doc = mBlobSE->readDoc(mRundata, snap_id, *it, ullTimestamp);
			if (doc.isNull())
			{
				OmnAlarm << "Failed to read doc with docid:" << *it	<< " snap_id:" << snap_id
						<< " timestamp:" << ullTimestamp << enderr;
				uFailed++;
			}
			ullTimestamp = 0;
			doc = mBlobSE->readDoc(mRundata, snap_id, *it, ullTimestamp);
			if (doc.notNull())
			{
				OmnAlarm << "result of reading a doc that has been deleted should be NULL docid:"
						<< *it << " snap_id:" << snap_id << " timestamp:" << ullTimestamp << enderr;
				uFailed++;
			}
		}
	}
	OmnScreen << "mvDeletedDoc.size():" << mvDeletedDoc.size() << " uFailed=" << uFailed << endl;
	return 0 == uFailed;
}


bool
AosBlobSETester::testCompactionAsRaftLeader()
{
//	return true;
	int iRet = 0;
	if (gTestRaftAPI)
	{
		iRet = mBlobSEAPI->doCompationAsRaftLeader();
		if (0 != iRet)
		{
			OmnAlarm << "mBlobSEAPI->doCompationAsRaftLeader failed, iRet=" << iRet << enderr;
			return false;
		}
	}
	else
	{
		iRet = mBlobSE->doCompationAsRaftLeader();
		if (0 != iRet)
		{
			OmnAlarm << "mBlobSE->doCompationAsRaftLeader failed, iRet=" << iRet << enderr;
			return false;
		}
	}
	return true;
}


bool
AosBlobSETester::testCreateSnapshot()
{
	if(mSnaps.size() >= mMaxSnapshotNum)	return true;

	OmnScreen << "start create Snapshot" << endl;

	u64 opr_id;
	u64 snap_id = mBlobSE->createSnapshot(
			mRundata, opr_id);
	aos_assert_r(snap_id, false);

	OmnScreen << "create snapshot;"
		<< "; snap_id:" << snap_id
		<< endl;

	AosLocalSnapshotPtr snap = OmnNew AosLocalSnapshot(
			snap_id, mMaxDocid);
	mSnaps.insert(make_pair(snap_id, snap));

	OmnScreen << "create Snapshot finish."
		<< "; snap_id:" << snap_id
		<< endl;
	return true;
}


bool
AosBlobSETester::testCommitSnapshot()
{
	u64 snap_id;
	bool rslt = commitLocalSnapshot(snap_id);
	aos_assert_r(rslt, false);
	if(snap_id == 0)
	{
		return true;
	}

	u64 opr_id;
	rslt = mBlobSE->commitSnapshot(mRundata, snap_id);
	aos_assert_r(rslt, false);

	//rslt = sanitycheck();
	//aos_assert_r(rslt, false);

	OmnScreen << "commit Snapshot finish; "
		<< "; snap_id:" << snap_id
		<< endl;
	return true;
}


bool
AosBlobSETester::commitLocalSnapshot(u64 &snap_id)
{
	snap_id = 0;
	AosLocalSnapshotPtr snap = randGetLocalSnapshot();
	if(!snap) return true;

	snap_id = snap->getSnapId();
	bool rslt = deleteLocalSnapshot(snap_id);
	aos_assert_r(rslt, false);

	OmnScreen << "start commit local Snapshot;"
		<< "; snap_id:" << snap_id
		<< endl;

	map<u64, AosLocalDoc *> &snap_docs = snap->getDocs();
	map<u64, AosLocalDoc *>::iterator m_itr;
	for(m_itr = snap_docs.begin(); m_itr != snap_docs.end(); m_itr++)
	{
		u64 docid = m_itr->first;
		AosLocalDoc * local_doc = m_itr->second;

		OmnScreen << "commit local doc;"
			<< "; snap_id:" << snap->getSnapId()
			<< "; docid:" << docid
			<< "; isDeleted:" << local_doc->isDeleted()
			<< "; seed:" << local_doc->getSeedId()
			<< "; repeat:" << local_doc->getRepeat()
			<< "; "
			<< endl;

		if(local_doc->isDeleted())
		{
			deleteFromLocalDb(docid);
			continue;
		}

		saveToLocalDb(docid, local_doc->getSeedId(),
				local_doc->getRepeat());
	}

	OmnScreen << "commit local Snapshot finish;"
		<< "; snap_id:" << snap_id
		<< endl;
	return true;
}


bool
AosBlobSETester::testRollbackSnapshot()
{
	u64 snap_id;
	bool rslt = rollbackLocalSnapshot(snap_id);
	aos_assert_r(rslt, false);
	if(snap_id == 0)
	{
		return true;
	}

	u64 opr_id;
	rslt = mBlobSE->cancelSnapshot(mRundata, snap_id);
	aos_assert_r(rslt, false);

	//rslt = sanitycheck();
	//aos_assert_r(rslt, false);

	OmnScreen << "rollBack Snapshot finish; "
		<< "; snap_id:" << snap_id
		<< endl;

	return true;
}


bool
AosBlobSETester::rollbackLocalSnapshot(u64 &snap_id)
{
	snap_id = 0;
	AosLocalSnapshotPtr snap = randGetLocalSnapshot();
	if(!snap) return true;

	snap_id = snap->getSnapId();

	bool rslt = deleteLocalSnapshot(snap_id);
	aos_assert_r(rslt, false);

	OmnScreen << "rollback local Snapshot finish;"
		<< "; snap_id:" << snap_id
		<< endl;

	return true;
}


bool
AosBlobSETester::testSubmitSnapshot()
{
//	u64 target_snap_id, submit_snap_id;
//	bool rslt = submitLocalSnapshot(target_snap_id,
//			submit_snap_id);
//	aos_assert_r(rslt, false);
//	if(target_snap_id == 0)
//	{
//		return true;
//	}
//
//	AosTransId trans_id;
//	rslt = mBlobSE->submitSnapshot(mRundata,
//			target_snap_id, submit_snap_id);
//	aos_assert_r(rslt, false);
//
//	//rslt = sanitycheck();
//	//aos_assert_r(rslt, false);
//
//
//	OmnScreen << "submit Snapshot finish; "
//		<< "; target_snap_id:" << target_snap_id
//		<< "; submit_snap_id:" << submit_snap_id
//		<< endl;

	return true;
}


bool
AosBlobSETester::submitLocalSnapshot(
		u64 &target_snap_id,
		u64 &submit_snap_id)
{
	target_snap_id = 0;
	submit_snap_id = 0;

	if(mSnaps.size() <=1)	return true;

	AosLocalSnapshotPtr target_snap = randGetLocalSnapshot();
	if(!target_snap) return true;

	AosLocalSnapshotPtr submit_snap;
	while(1)
	{
		submit_snap = randGetLocalSnapshot();
		if(!submit_snap) return true;

		if(submit_snap->getSnapId() != target_snap->getSnapId())	break;
	}

	target_snap_id = target_snap->getSnapId();
	submit_snap_id = submit_snap->getSnapId();

	bool rslt = deleteLocalSnapshot(submit_snap_id);
	aos_assert_r(rslt, false);

	OmnScreen << "start submit local Snapshot;"
		<< "; target_snap_id:" << target_snap_id
		<< "; submit_snap_id:" << submit_snap_id
		<< endl;

	target_snap->merge(submit_snap);

	OmnScreen << "submit local Snapshot finish; "
		<< "; target_snap_id:" << target_snap_id
		<< "; submit_snap_id:" << submit_snap_id
		<< endl;
	return true;
}


AosLocalSnapshotPtr
AosBlobSETester::randGetLocalSnapshot()
{
	if(mSnaps.size() == 0)	return 0;

	u32 idx = rand() % mSnaps.size();
	map<u32, AosLocalSnapshotPtr>::iterator itr = mSnaps.begin();
	for(u32 i=0; i<idx; i++)	itr++;

	AosLocalSnapshotPtr snap = itr->second;
	aos_assert_r(snap, 0);

	return snap;
}

bool
AosBlobSETester::deleteLocalSnapshot(const u32 snap_id)
{
	map<u32, AosLocalSnapshotPtr>::iterator itr = mSnaps.find(snap_id);
	aos_assert_r(itr != mSnaps.end(), false);

	mSnaps.erase(itr);

	return true;
}


bool
AosBlobSETester::testCreateDocToSnap(const u64 new_docid)
{
	AosLocalSnapshotPtr snap = randGetLocalSnapshot();
	if(!snap)
	{
		return true;
	}

	//if(!snap->hasMemory())
	//{
	//	return true;
	//}

	// Ketty 2015/04/07
	if(new_docid > mMaxDocid) mMaxDocid = new_docid;
	AosLocalDoc *local_doc = snap->addDoc(new_docid);

	AosDfmDocPtr dfm_doc = mDoc->clone(new_docid);
	dfm_doc->setBodyBuff(local_doc->getData());

	//vector<AosTransId> tids;
	u64 trans_id = 0;
	bool rslt = mBlobSE->saveDoc(mRundata,
			snap->getSnapId(), dfm_doc, trans_id);
	if(!rslt)
	{
		OmnAlarm << "error!" << enderr;
		return false;
	}

	OmnScreen << "createDoc to Snapshot; "
		<< "snap_id:" << snap->getSnapId() << "; "
		<< "docid:" << new_docid << "; "
		<< "size:" << local_doc->getDocSize() << "; "
		<< "pattern:" << local_doc->getPattern() << "; "
		<< "repeat:" << local_doc->getRepeat() << "; "
		<< endl;

	return true;
}


bool
AosBlobSETester::testModifyDocFromSnap()
{
	AosLocalSnapshotPtr snap = randGetLocalSnapshot();
	if(!snap)
	{
		return true;
	}

	AosLocalDoc *local_doc = snap->randModifyDoc();
	if(!local_doc)
	{
		return true;
	}

	u64 docid = local_doc->getDocid();
	AosDfmDocPtr dfm_doc = mDoc->clone(docid);
	dfm_doc->setBodyBuff(local_doc->getData());

	vector<AosTransId> tids;
	u64 opr_id = 0;
	u64 timestamp = 0;
	bool rslt = mBlobSE->saveDoc(mRundata,
			snap->getSnapId(), dfm_doc, opr_id, timestamp);
	if(!rslt)
	{
		OmnAlarm << "error!" << enderr;
		return false;
	}

	OmnScreen << "modifyDoc from Snapshot; "
		<< "snap_id:" << snap->getSnapId() << "; "
		<< "docid:" << docid << "; "
		<< "size:" << local_doc->getDocSize() << "; "
		<< "pattern:" << local_doc->getPattern() << "; "
		<< "repeat:" << local_doc->getRepeat() << "; "
		<< endl;

	return true;
}


bool
AosBlobSETester::testDeleteDocFromSnap()
{
	AosLocalSnapshotPtr snap = randGetLocalSnapshot();
	if(!snap)
	{
		return true;
	}

	AosLocalDoc * local_doc = snap->randDeleteDoc();
	if(!local_doc)
	{
		return true;
	}

	u64 docid = local_doc->getDocid();
	AosDfmDocPtr dfm_doc = mDoc->clone(docid);

	//vector<AosTransId> tids;
	u64 trans_id = 0;
	bool rslt = mBlobSE->deleteDoc(mRundata,
			snap->getSnapId(), dfm_doc, trans_id);
	if(!rslt)
	{
		OmnAlarm << "error!" << enderr;
		return false;
	}

	OmnScreen << "deleteDoc from Snapshot; "
		<< "snap_id:" << snap->getSnapId() << "; "
		<< "docid:" << docid << "; "
		<< "size:" << local_doc->getDocSize() << "; "
		<< "pattern:" << local_doc->getPattern() << "; "
		<< "repeat:" << local_doc->getRepeat() << "; "
		<< endl;

	return true;
}


bool
AosBlobSETester::testReadDocFromSnap()
{
	AosLocalSnapshotPtr snap = randGetLocalSnapshot();
	if(!snap)
	{
		return true;
	}

	u64 docid = snap->randPickDocid();
	if(!docid)	return true;

	bool rslt;
	bool new_flag = false;
	AosLocalDoc * local_doc = 0;
	local_doc = snap->readDoc(docid);
	if(!local_doc)
	{
		// readFrom local_db.
		bool exist = true;
		u32 seed_id = 0, repeat = 0;
		rslt = readFromLocalDb(docid,
				exist, seed_id, repeat);
		aos_assert_r(rslt, false);

		local_doc = OmnNew AosLocalDoc(docid,
				!exist, seed_id, repeat);
		new_flag = true;
	}
	aos_assert_r(local_doc, false);

	OmnScreen << "start readDoc from Snapshot; "
		<< "snap_id:" << snap->getSnapId() << "; "
		<< "docid:" << docid
		<< "; "
		<< endl;

	AosDfmDocPtr dfm_doc = mBlobSE->readDoc(mRundata,
			snap->getSnapId(), docid, true);
	aos_assert_r(dfm_doc, false);

	if (dfm_doc.notNull())
	{
		bool is_same = local_doc->isSameData(dfm_doc->getBodyBuff());
		if(!is_same)
		{
			OmnAlarm << "error!" << enderr;
			return false;
		}
	}
	else
	{
		if(!local_doc->isDeleted())
		{
			OmnAlarm << "error!" << enderr;
			return false;
		}
	}

	OmnScreen << "readDoc end from Snapshot; "
		<< "snap_id:" << snap->getSnapId() << "; "
		<< "docid:" << docid
		<< "; "
		<< endl;

	if(new_flag) OmnDelete local_doc;
	return true;
}


bool
AosBlobSETester::deleteFromLocalDb(const u64 docid)
{
	std::vector<u64>::iterator mvDoc_it = std::find(mvDoc.begin(), mvDoc.end(), docid);
	if(mvDoc_it == mvDoc.end())
	{
		OmnScreen << "Error! can not find docid:'" << docid << "' in mvDoc. maybe delete two times." << endl;
		return true;
	}
	mvDoc.erase(mvDoc_it);
	mvDeletedDoc.push_back(docid);

	itr_t mDocSeedMap_it = mDocSeedMap.find(docid);
	if(mDocSeedMap_it == mDocSeedMap.end())
	{
		OmnAlarm << "can not find pair for docid:'" << docid << "' in mDocSeedMap." << enderr;
		return false;
	}
	mDocSeedMap.erase(mDocSeedMap_it);

	OmnScreen << "commit delete doc finish;"
		<< "; docid:" << docid
		<< "; "
		<< endl;
	return true;
}

bool
AosBlobSETester::saveToLocalDb(
		const u64 docid,
		const u32 seed_id,
		const u32 repeat)
{
	itr_t crt_itr = mDocSeedMap.find(docid);
	if (crt_itr !=  mDocSeedMap.end())
	{
		crt_itr->second = make_pair(seed_id, repeat);
		OmnScreen << "commit modify doc finish;"
			<< "; docid:" << docid
			<< "; seed_id:" << seed_id
			<< "; repeat:" << repeat
			<< "; "
			<< endl;
		return true;
	}

	mDocSeedMap.insert(make_pair(docid, make_pair(seed_id, repeat)));
	mvDoc.push_back(docid);
	OmnScreen << "commit add doc;"
		<< "; docid:" << docid
		<< "; seed_id:" << seed_id
		<< "; repeat:" << repeat
		<< "; "
		<< endl;
	return true;
}


bool
AosBlobSETester::readFromLocalDb(
		const u64 docid,
		bool &exist,
		u32 &seed_id,
		u32 &repeat)
{
	itr_t crt_itr = mDocSeedMap.find(docid);
	if (crt_itr ==  mDocSeedMap.end())
	{
		exist = false;
		return true;
	}


	p_seed_repeat_t* seedPair = &crt_itr->second;

	exist = true;
	seed_id = seedPair->first;
	repeat = seedPair->second;
	return true;
}


/*
bool
AosBlobSETester::sanitycheck(const AosRundataPtr &rdata)
{
	OmnScreen << "start sanitycheck;" << endl;

	u32 check_did = 1;
	map<u64, AosDocInfoPtr>::iterator itr = mDocInfo.begin();
	AosDfmDocPtr dfm_doc;
	bool rslt;
	for(; itr != mDocInfo.end(); itr++)
	{
		AosDocInfoPtr doc_info = itr->second;
		doc_info->lock();

		u32 docid = doc_info->getDocid();
		for(; check_did < docid; check_did++)
		{
			OmnScreen << "readDoc sanitycheck not exist"
				<< "; crt_docid:" << check_did
				<< endl;

			dfm_doc = mDfm->readDoc(rdata, check_did, false);
			aos_assert_rl(!dfm_doc, doc_info->getLock(), false);
		}
		check_did++;

		OmnScreen << "readDoc sanitycheck"
			<< "; act_docid:" << docid
			<< endl;
		dfm_doc = mDfm->readDoc(rdata, docid);
		aos_assert_rl(dfm_doc, doc_info->getLock(), false);

		bool is_same = doc_info->isSameData(dfm_doc->getBodyBuff());
		aos_assert_rl(is_same, doc_info->getLock(), false);

		doc_info->unlock();
	}

	OmnScreen << "sanitycheck end;" << endl;
	return true;
}
*/


bool
AosBlobSETester::compactionUnitTest()
{
	OmnScreen << "doing compactionUnitTest." << endl;
	OmnScreen << "create 10000 docs." << endl;
	for(u64 ull = 1;ull <= 10000;ull++)
	{
		if (!testCreateDocWithRaftInterface(ull, ull))
		{
			OmnAlarm << "failed on doc creation, docid:" << ull << " oprid:" << ull << enderr;
			return false;
		}
	}
	OmnScreen << "wait for 100s." << endl;
	OmnSleep(100);
	OmnScreen << "modify them once." << endl;
	for(u64 ull = 1;ull <= 10000;ull++)
	{
		if (!testCreateDocWithRaftInterface(ull, ull + 10000))
		{
			OmnAlarm << "failed on doc creation, docid:" << ull << " oprid:" << ull + 10000 << enderr;
			return false;
		}
	}
	OmnScreen << "wait for another 100s." << endl;
	OmnSleep(100);
	OmnScreen << "modify them again." << endl;
	for(u64 ull = 1;ull <= 10000;ull++)
	{
		if (!testCreateDocWithRaftInterface(ull, ull + 20000))
		{
			OmnAlarm << "failed on doc creation, docid:" << ull << " oprid:" << ull + 20000 << enderr;
			return false;
		}
	}
	OmnScreen << "wait for another 100s." << endl;
	OmnSleep(100);
	OmnScreen << "modify them again." << endl;
	for(u64 ull = 1;ull <= 10000;ull++)
	{
		if (!testCreateDocWithRaftInterface(ull, ull + 30000))
		{
			OmnAlarm << "failed on doc creation, docid:" << ull << " oprid:" << ull + 30000 << enderr;
			return false;
		}
	}
	OmnScreen << "wait for another 10s." << endl;
	OmnSleep(10);
	OmnScreen << "test compaction." << endl;
	testCompactionAsRaftLeader();
}
