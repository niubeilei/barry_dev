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
#include "BlobSE/TesterSnapShot/BlobSETester.h"

#include "Alarm/Alarm.h"
#include "BlobSE/BlobSE.h"
#include "UtilData/BlobSEReqEntry.h"
#include "RawSE/RawSE.h"
#include "BlobSE/TesterSnapShot/LocalSnapshot.h"
#include "Debug/Debug.h"
#include "DfmUtil/DfmDoc.h"
#include "Porting/Sleep.h"
#include "Porting/TimeOfDay.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "BlobSE/BlobSESnapShotMgr.h"
#include "BlobSE/BlobSESnapShot.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
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
	mRundata = OmnNew AosRundata();
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
	mHeader = OmnNew AosDfmDocHeader(0, 0, 0, OmnNew AosBuff());
	mDoc = AosDfmDoc::cloneDoc(mDocType);
	try
	{
		// Ketty 2015/04/05
		//mBlobSE = OmnNew AosBlobSE(mRundata.getPtrNoLock(), mHeader, mDoc, gApp_config);
		mBlobSE = OmnNew AosBlobSE(mRundata.getPtrNoLock(), mDoc, gApp_config);

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
	mFile = OmnNew OmnFile();
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
	mCrtSec = OmnTime::getCrtSec();
	OmnThreadedObjPtr thisptr(this, false);
	if (!loadSeed())
	{
		OmnAlarm << "loadSeed failed." << enderr;
		return false;
	}
	AosXmlTagPtr config = gApp_config->getFirstChild();
	config = config->getFirstChild(mName.data());
	
	//new snapshotMgr
	AosRawSEPtr rawse = OmnNew AosRawSE();                                                
	mSnapShotMgr = OmnNew AosBlobSESnapshotMgr(mBlobSE, rawse);

	//OmnString docs_left_file = config->getAttrStrSimp("docs_left", "");
	//OmnString docs_deleted_file = config->getAttrStrSimp("docs_deleted", "");

	int batch_size = 1;
	int mBactch_id = 0;

	oneBatchTests();

	/*for (int i=1; i<batch_size; i++)
	{
		runThreader(thisptr, i);
	}*/

	return true;
}


bool
AosBlobSETester::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	u64 start_docid = 1;
	u32 num_docids = 0;
	oneBatchTests();
	
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
AosBlobSETester::oneBatchTests()
{
	int p = 10;
	mLock->lock();
	mBatch_id++;
	mLock->unlock();
	//switch(mBatch_id)
	switch(2)
	{
	case 1:
		if(!testModifyDocWithRaftInterface(mBatch_id+20))
		{
			OmnAlarm << "testModifyDocWithRaftInterface failed." << enderr;
		}
		break;

	case 2:
		if(!testMoreThread(p))
		if(!testThreeSnapshot())
		{
			OmnAlarm << "testThreeSnapshot failed." << enderr;
		}
		break;

	case 3:
		//if(!testMoreThread(i))
		if(!testDocAndHeader())
		{
			OmnAlarm << "testDocAndHeader failed." << enderr;
		}
		break;
	}

	return true;

}


bool
AosBlobSETester::testCreateDoc(const u64 &docid)
{
	// This function creates a raw file.
	OmnScreen << "Test No." << mTestCount << ":doc creation." << endl;

	// Ketty 2015/04/07
	if(docid > mMaxDocid) mMaxDocid = docid;

	u64 snap_id = 0;	//TODO:
	u64 trans_id = 0;	//TODO:
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
AosBlobSETester::testModifyDoc()
{
	return true;
	OmnScreen << "Test No." << mTestCount++ << ":doc modification." << endl;
	u64 snap_id = 0;	//TODO:
	u64 trans_id = 0;	//TODO:
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
	bool rslt = mBlobSE->saveDoc(mRundata, snap_id, doc, trans_id, ullTimestamp);
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
AosBlobSETester::testDeleteDoc()
{
	OmnScreen << "Test No." << mTestCount << ":doc deletion." << endl;
	int iRet = 0;
	u64 snap_id = 0;	//TODO:
	u64 trans_id = 0;	//TODO:
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
AosBlobSETester::testReadDoc()
{
//	randomly pick a file;
//	randomly determine read position
//	randomly determine read length;
	OmnScreen << "Test No." << mTestCount << ":reading doc." << endl;
	u64 snap_id = 0;	//TODO:
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
	AosBuffPtr content = OmnNew AosBuff(10000);
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
		doc = mBlobSE->readDoc(mRundata.getPtrNoLock(), snap_id, docid, 0);	//TODO:
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
		doc = mBlobSE->readDoc(mRundata.getPtrNoLock(), snap_id, docid, 0);	//TODO:
		if (doc.isNull())
		{
			OmnAlarm << "Failed to read doc with docid:'" << docid
					<< "' snap_id '" << snap_id << "'" << enderr;
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
		//OmnScreen << "seed[" << i << "]:'" << gSeedMap[i] << "'" << endl;
	}
	return true;
}


AosBuffPtr
AosBlobSETester::createData(
		const u64	docid,
		const u32	seed_id)
{
	AosBuffPtr buff = OmnNew AosBuff();
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
	return buff;
}


bool
AosBlobSETester::testRestart()
{
	//we only check the docid in log files for now
	u64 snap_id = 0;	//TODO:
	AosDfmDocPtr doc;
	u32 uFailed = 0;
	u64 ullTimestamp = 0;
	for (std::vector<u64>::iterator it = mvDoc.begin();it != mvDoc.end();it++)
	{
		doc = mBlobSE->readDoc(mRundata, snap_id, *it, ullTimestamp);
		if (doc.isNull())
		{
			OmnAlarm << "Failed to read doc with docid:" << *it	<< " snap_id:" << snap_id
					<< " timestamp:" << ullTimestamp << enderr;
			uFailed++;
		}
	}
	OmnScreen << "mvDoc.size():" << mvDoc.size() << " uFailed=" << uFailed << endl;
	uFailed = 0;
	for (std::vector<u64>::iterator it = mvDeletedDoc.begin();it != mvDeletedDoc.end();it++)
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
	OmnScreen << "mvDeletedDoc.size():" << mvDeletedDoc.size() << " uFailed=" << uFailed << endl;
	return 0 == uFailed;
}


bool
AosBlobSETester::testCompactionAsRaftLeader()
{
	int iRet = mBlobSE->doCompationAsRaftLeader();
	if (0 != iRet)
	{
		OmnAlarm << "mBlobSE->doCompationAsRaftLeader, iRet=" << iRet << enderr;
		return false;
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
	rslt = mBlobSE->commitSnapshot(mRundata, opr_id);
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
	u64 target_snap_id, submit_snap_id;
	bool rslt = submitLocalSnapshot(target_snap_id,
			submit_snap_id);
	aos_assert_r(rslt, false);
	if(target_snap_id == 0)
	{
		return true;
	}

	AosTransId trans_id;
	//rslt = mBlobSE->submitSnapshot(mRundata,
	//		target_snap_id, submit_snap_id);
	//aos_assert_r(rslt, false);

	//rslt = sanitycheck();
	//aos_assert_r(rslt, false);


	OmnScreen << "submit Snapshot finish; "
		<< "; target_snap_id:" << target_snap_id
		<< "; submit_snap_id:" << submit_snap_id
		<< endl;

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
	u64 trans_id = 0;	//TODO:
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
			snap->getSnapId(), dfm_doc, opr_id, timestamp);	//TODO:
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
	u64 trans_id = 0;	//TODO:
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


bool
AosBlobSETester::runThreader(OmnThreadedObjPtr thisPtr ,int num)
{
	mLock->lock();
	// run more thread
	OmnThreadedObjPtr thisptr(this, false);
	mThread = OmnNew OmnThread(thisptr, "blobSETester", num, true, true, __FILE__, __LINE__);      
	//mStatus = eStart;                                                                   
	mThread->start();
	mLock->unlock();

	return true;
}


bool
AosBlobSETester::testThreeSnapshot()
{
	const AosRundataPtr rdata = mRundata;
	u64 snapshot_id = 1;
	u32	buff_size = 10;
	bool finish = false;
	//AosRawSEPtr rawse = OmnNew AosRawSE();                                                
	//if(!mSnapShotMgr)
	//{
	//	mSnapShotMgr = OmnNew AosBlobSESnapshotMgr(mBlobSE, rawse);
	//}
	bool rslt = false;
	
	rslt = mSnapShotMgr->createSnapshot(mRundata, snapshot_id);
	aos_assert_r(rslt, false);

	mLock->lock();
	for(u64 i=10001; i<=10003; i++)
	{
		const u64 snap_id = snapshot_id;
		//build the header
		const u64 docid = i;
		AosBlobHeaderPtr header = OmnNew AosBlobHeader(i, buff_size);
		header->setDocid(docid);
		header->setSnapshotID(snap_id);
		const AosBlobHeaderPtr chead = header;

		aos_assert_r(rslt, false);
		mSnapShotMgr->saveDoc(rdata, chead, finish);

		buff_size++;
	}
	mLock->unlock();

	snapshot_id = 2;
	rslt = mSnapShotMgr->createSnapshot(mRundata, snapshot_id);
	aos_assert_r(rslt, false);
	mLock->lock();
	for(u64 i=10002; i<=10004; i++)
	{
		const u64 snap_id = snapshot_id;
		//build the header
		const u64 docid = i;
		AosBlobHeaderPtr header = OmnNew AosBlobHeader(i, buff_size);
		header->setDocid(docid);
		header->setSnapshotID(snap_id);
		const AosBlobHeaderPtr chead = header;

		aos_assert_r(rslt, false);
		mSnapShotMgr->saveDoc(rdata, chead, finish);

		buff_size++;
	}
	mLock->unlock();

	snapshot_id = 3;
	rslt = mSnapShotMgr->createSnapshot(mRundata, snapshot_id);
	aos_assert_r(rslt, false);
	mLock->lock();
	for(u64 i=10003; i<=10005; i++)
	{
		const u64 snap_id = snapshot_id;
		//build the header
		const u64 docid = i;
		AosBlobHeaderPtr header = OmnNew AosBlobHeader(i, buff_size);
		header->setDocid(docid);
		header->setSnapshotID(snap_id);
		const AosBlobHeaderPtr chead = header;

		aos_assert_r(rslt, false);
		mSnapShotMgr->saveDoc(rdata, chead, finish);

		buff_size++;
	}

	//before commit and check the values
	map<u64, AosBlobSESnapshotPtr> snapshots = mSnapShotMgr->getSnapshots();
	aos_assert_r((snapshots.size() == 3), false);

	vector<AosBlobSESnapshotPtr> canceledSnapshots = mSnapShotMgr-> getCanceledSnapshots();
	aos_assert_r((canceledSnapshots.size()==0), false);

	vector<AosBlobSESnapshotPtr> committedSnapshots = mSnapShotMgr->getCommittedSnapshots();
	aos_assert_r((committedSnapshots.size()==0), false);
	mLock->unlock();

	//commit tow snapshots and cancel one snapshot
	for(u64 i=1; i<=3; i++)
	{
		if(i == 3)
			mSnapShotMgr->cancelSnapshot(rdata, i);
		else
			mSnapShotMgr->commitSnapshot(rdata, i);
	}
	
	mLock->lock();
	//after commit check the values
	snapshots = mSnapShotMgr->getSnapshots();
	aos_assert_r((snapshots.size() == 0), false);

	canceledSnapshots = mSnapShotMgr-> getCanceledSnapshots();
	aos_assert_r((canceledSnapshots.size()==1), false);

	committedSnapshots = mSnapShotMgr->getCommittedSnapshots();
	aos_assert_r((committedSnapshots.size()==2), false);

	hash_map<u64, vector<u64> > docidSnapshotMap = mSnapShotMgr->getDocidSnapshotMap();

	hash_map<u64, vector<u64> >::iterator itr;
	vector<u64> snapIdList;
	for(u64 i=10001; i<=10005; i++)
	{
		itr = docidSnapshotMap.find(i);
		if(i == 10005)
		{
			//for check i == 10005 snapIdList == 0
			aos_assert_r(itr==docidSnapshotMap.end(), false);
		}
		else
		{
			snapIdList = itr->second;
		}

		if(i == 10001)
		{
			aos_assert_r(snapIdList.size() == 1,false);
		}
		else if(i == 10002)
		{
			aos_assert_r(snapIdList.size() == 2,false);
		}
		else if(i == 10003)
		{
			aos_assert_r(snapIdList.size() == 2,false);
		}
		else if(i == 10004)
		{
			aos_assert_r(snapIdList.size() == 1,false);
		}
	}
	mLock->unlock();

	return true;	
}


bool
AosBlobSETester::saveDoc(const AosRundataPtr rdata, 
							AosBlobHeaderPtr header, 
							u64 snapshot_id, u64 doc_id, 
							u64 rawfid, u64 offset, u64 len)
{	
	bool finish = false;
	const u64 snap_id = snapshot_id;
	//build the header
	const u64 docid = doc_id;
	header->setSnapshotID(snap_id);
	header->setDocid(docid);
	header->setBodyRawfid(rawfid);
	header->setBodyOffset(offset);
	header->setBodyLen(len);
	const AosBlobHeaderPtr chead = header;

	bool rslt = mSnapShotMgr->saveDoc(rdata, chead, finish);
	aos_assert_r(rslt, false);
	
	return true;
}

bool
AosBlobSETester::readDoc(const AosRundataPtr rdata, 
							AosBlobHeaderPtr header, 
							u64 snapshot_id, u64 doc_id,
							u64 rawid, u64 offset, 
							u64 len, u64 timestamp)
{	
	const u64 ctimestamp = timestamp;
	bool rslt;
	const u64 snap_id = snapshot_id;
	const u64 docid = doc_id;
	rslt = mSnapShotMgr->readDoc(rdata, snap_id, docid, header, ctimestamp);
	aos_assert_r(rslt, false);

	//check value
	if(header.isNull())
		return true;

	AosBlobSESnapshotPtr snapShot;
	if(mSnapShotMgr->isCommitted(snap_id))
	{
		if(snap_id == 0)
		{
			vector<AosBlobSESnapshotPtr> committedList = mSnapShotMgr->getCommittedSnapshots();
			for(u64 i=0; i<committedList.size(); i++)
			{
				AosBlobSESnapshotPtr snapshot = committedList[i];
				map<u64, AosBlobHeaderPtr> docidHeaderMap = snapshot->getHeaderMap();
				map<u64, AosBlobHeaderPtr>::iterator itr = docidHeaderMap.find(doc_id);
				if(itr != docidHeaderMap.end())
				{
					AosBlobHeaderPtr header = docidHeaderMap[doc_id];
					u64	filId = 0;
					u64	off_set = 0;
					u64	length = 0;
					filId = header->getBodyRawfid();
					aos_assert_r(filId==rawid, false);

					off_set = header->getBodyOffset();
					aos_assert_r(off_set==offset, false);

					length = header->getBodyLen();
					aos_assert_r(length==len, false);
				
				}
			}
			return true;
		}
		else
		{
			map<u64, AosBlobSESnapshotPtr> committedSnapshotsMap = mSnapShotMgr->getCommittedSnapshotsMap();
			map<u64, AosBlobSESnapshotPtr>::iterator itr = committedSnapshotsMap.find(snap_id);
			if(itr != committedSnapshotsMap.end())
			{
				snapShot = itr->second;
			}
		}
	}
	else
		snapShot = mSnapShotMgr->getSnapshotById(snap_id);


	map<u64, AosBlobHeaderPtr> docidHeaderMap = snapShot->getHeaderMap();
	map<u64, AosBlobHeaderPtr>::iterator itr = docidHeaderMap.find(doc_id);
	if(itr != docidHeaderMap.end())
	{
		AosBlobHeaderPtr header = docidHeaderMap[doc_id];
		u64	filId = 0;
		u64	off_set = 0;
		u64	length = 0;
		filId = header->getBodyRawfid();
		aos_assert_r(filId==rawid, false);

		off_set = header->getBodyOffset();
		aos_assert_r(off_set==offset, false);

		length = header->getBodyLen();
		aos_assert_r(length==len, false);
	
	}
	return true;
}


bool
AosBlobSETester::testDocAndHeader()
{
	const AosRundataPtr rdata = mRundata;
	u64 snapshot_id = 1;
	u64	doc_id = 10001;
	u32	buff_size = 10;
	bool finish = false;
	u64 rawfid = 1;
	u64	offset = 100;
	u64	len = 10;
	u64	timestamp = OmnGetTimestamp();
	/*AosRawSEPtr rawse = OmnNew AosRawSE();                                                
	if(!mSnapShotMgr)
	{
		mSnapShotMgr = OmnNew AosBlobSESnapshotMgr(mBlobSE, rawse);
	}
	*/
	//build header
	AosBlobHeaderPtr header = OmnNew AosBlobHeader(doc_id, buff_size);

	////////////////////////////////////////////////////////////////////////
	///////////////         create snapshot first           ////////////////
	////////////////////////////////////////////////////////////////////////
	bool rslt = mSnapShotMgr->createSnapshot(mRundata, snapshot_id);
	
	rslt = saveDoc(rdata, header, snapshot_id, doc_id, rawfid, offset, len);
	aos_assert_r(rslt, false);
	while(1)
	{
		snapshot_id = 0;
		rslt = readDoc(rdata, header, snapshot_id, doc_id, rawfid, offset, len, timestamp);
		aos_assert_r(rslt, false);

		snapshot_id = 1;
		rslt = readDoc(rdata, header, snapshot_id, doc_id, rawfid, offset, len, timestamp);
		aos_assert_r(rslt, false);
		break;
	}

	rawfid++;
	offset+=100;
	len+=10;

	//modify doc
	rslt = saveDoc(rdata, header, snapshot_id, doc_id, rawfid, offset, len);
	aos_assert_r(rslt, false);

	//check value
	rslt = readDoc(rdata, header, snapshot_id, doc_id, rawfid, offset, len, timestamp);
	aos_assert_r(rslt, false);

	//commit snapshot
	const u64 snap_id = snapshot_id;
	rslt = mSnapShotMgr->commitSnapshot(rdata, snap_id);
	aos_assert_r(rslt, false);
	//check value
	while(1)
	{
		snapshot_id = 0;
		rslt = readDoc(rdata, header, snapshot_id, doc_id, rawfid, offset, len, timestamp);
		aos_assert_r(rslt, false);

		snapshot_id = 1;
		rslt = readDoc(rdata, header, snapshot_id, doc_id, rawfid, offset, len, timestamp);
		aos_assert_r(rslt, false);
		break;
	}
	
	//check old header
	if(mSnapShotMgr->isCommitted(snap_id));
	{
		AosBlobSESnapshotPtr snapshot;
		map<u64, AosBlobSESnapshotPtr> committedSnapshotsMap = mSnapShotMgr->getCommittedSnapshotsMap();
		map<u64, AosBlobSESnapshotPtr>::iterator itr = committedSnapshotsMap.find(snap_id);
		if(itr != committedSnapshotsMap.end())
		{
			snapshot = itr->second;
		}
		vector<AosBlobHeaderPtr> headerList = snapshot->getDeletedHeaderList();
		
		for(u64 i=0; i< headerList.size(); i++)
		{
			rslt = readDoc(rdata, headerList[i], snapshot_id, doc_id, rawfid, offset, len, timestamp);
			aos_assert_r(rslt, false);
		}
	}

	////////////////////////////////////
	//create snapshot second////////////
	////////////////////////////////////
	snapshot_id++;
	rslt = mSnapShotMgr->createSnapshot(mRundata, snapshot_id);
	aos_assert_r(rslt, false);
	//snapshot = mSnapShotMgr->getSnapshotById(snapshot_id);
	//check value
	while(1)
	{
		snapshot_id = 0;
		rslt = readDoc(rdata, header, snapshot_id, doc_id, rawfid, offset, len, timestamp);
		aos_assert_r(rslt, false);

		snapshot_id = 2;
		rslt = readDoc(rdata, header, snapshot_id, doc_id, rawfid, offset, len, timestamp);
		aos_assert_r(rslt, false);
		break;
	}

	//append entry
	rslt = saveDoc(rdata, header, snapshot_id, doc_id, rawfid, offset, len);
	aos_assert_r(rslt, false);
	//commit snapshot
	rslt = mSnapShotMgr->commitSnapshot(rdata, snapshot_id);
	aos_assert_r(rslt, false);
	
	//check value
	while(1)
	{
		snapshot_id = 0;
		rslt = readDoc(rdata, header, snapshot_id, doc_id, rawfid, offset, len, timestamp);
		aos_assert_r(rslt, false);

		snapshot_id = 2;
		rslt = readDoc(rdata, header, snapshot_id, doc_id, rawfid, offset, len, timestamp);
		aos_assert_r(rslt, false);
		break;
	}

	//check all data structures
	map<u64, AosBlobSESnapshotPtr> snapshotMap = mSnapShotMgr->getSnapshots();
	aos_assert_r(snapshotMap.size()==0, false);

	vector<AosBlobSESnapshotPtr> canceledSnapshotList = mSnapShotMgr->getCanceledSnapshots();
	aos_assert_r(canceledSnapshotList.size()==0, false);

	vector<AosBlobSESnapshotPtr> committedSnapshotList = mSnapShotMgr->getCommittedSnapshots();
	aos_assert_r(committedSnapshotList.size()==2, false);

	hash_map<u64, vector<u64> > docidSnapshotMap = mSnapShotMgr->getDocidSnapshotMap();
	hash_map<u64, vector<u64> >::iterator itr = docidSnapshotMap.find(doc_id);
	if(itr != docidSnapshotMap.end())
	{
		vector<u64> snapIdList = itr->second;
		aos_assert_r(snapIdList.size()==2, false);
	}

	return true;
}
	


bool
AosBlobSETester::testModifyDocWithRaftInterface(u64 ullSnapshotID)
{
	u64 ullOprID = 10;
	//u64 ullSnapshotID = 4;
	//ullSnapshotID = 4;
	u64	docid = 10001;
	u32	buff_size = 10;
	bool finish = false;
	u64 rawfid = 1;
	u64	offset = 100;
	u64	len = 10;
	u64	timestamp = OmnGetTimestamp();

	u64 CustomDataSize = 10;

	//lock
	mLock->lock();
	AosBlobHeaderPtr header = OmnNew AosBlobHeader(0, CustomDataSize);
	if (header.isNull())
	{
		OmnAlarm << "create AosBlobHeader obj failed, oprid:" << ullOprID << enderr;
		return -4;
	}

	AosBlobSEReqEntryPtr blobSEReqEntry = OmnNew AosBlobSEReqEntry(header->getCustomDataSize());
	if (blobSEReqEntry.isNull())
	{
		OmnAlarm << "create AosBlobSEReqEntry obj failed, oprid:" << ullOprID << enderr;
		return -2;
	}

	//to set config
	u64 ullTimestamp = OmnTime::getTimestamp();
	blobSEReqEntry->setTimestamp(ullTimestamp);
	blobSEReqEntry->setOprID(ullOprID);
	blobSEReqEntry->setSnapshotID(ullSnapshotID);
	blobSEReqEntry->setDocid(docid);
	blobSEReqEntry->setReqTypeAndResult(AosBlobSEReqEntry::eSave);
	AosBuffPtr headerCustomData = OmnNew AosBuff(mBlobSE->getConfig().header_custom_data_size);
	memset(headerCustomData->data(), 0, headerCustomData->buffLen());
	blobSEReqEntry->setHeaderCustomDataBuff(headerCustomData);
	AosBuffPtr bodyDataBuff = createData(docid);
	if (bodyDataBuff.isNull())
	{
		OmnAlarm << "createData failed, docid:" << docid << enderr;
		return false;
	}
	blobSEReqEntry->setBodyBuff(bodyDataBuff);
	mLock->unlock();

	//append snapshot
	const AosRundataPtr rdata = mRundata;
	int iRet = mBlobSE->createSnapshot(rdata, ullSnapshotID);
	if(iRet != 0)
	{
		OmnAlarm << "create snapshot failed, iRet=" << iRet << 
			"snapshot ID" << ullSnapshotID << enderr;
		return false;
	}

	//save Doc
	iRet = mBlobSE->saveDoc(rdata, ullOprID, blobSEReqEntry.getPtrNoLock(), header);
	if (iRet != 0)
	{
		OmnAlarm << "mBlobSE->saveDoc failed, iRet=" << iRet << " oprid:" << ullOprID << enderr;
		return false;
	}
	iRet = mBlobSE->updateHeader(mRundata.getPtr(), header);
	if(iRet != 0)
	{
		OmnAlarm << "save doc failed, rslt_num=" << iRet << " doc_id " << header->getDocid();
		return false;
	}

	//read Doc
	blobSEReqEntry->setReqTypeAndResult(AosBlobSEReqEntry::eRead);
	iRet = mBlobSE->readDoc(rdata, blobSEReqEntry.getPtrNoLock());
	if (iRet != 0)
	{
		OmnAlarm << "mBlobSE->readDoc failed, iRet=" << iRet << enderr;
		return false;
	}
	
	//commit snapshot
	iRet = mBlobSE->commitSnapshot(rdata, ullSnapshotID);
	if(iRet != 0)
	{
		OmnAlarm << "commit snapshot failed, iRet=" << iRet << 
			"snapshot ID" << ullSnapshotID << enderr;
		return false;
	}

	//read Doc
	iRet = mBlobSE->readDoc(rdata, blobSEReqEntry.getPtrNoLock());
	if (iRet != 0)
	{
		OmnAlarm << "mBlobSE->readDoc failed, iRet=" << iRet << enderr;
		return false;
	}
	else
	{
		//check value
		u32 seed_id = 0;
		u32 repeat_time = 0;
		p_seed_repeat_t* seedPair;
		itr_t it;
		mLock->lock();
		AosBuffPtr content = OmnNew AosBuff(10000);                              
		aos_assert_rr(content.notNull(), mRundata.getPtrNoLock(), false);
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
		AosBuffPtr doc_buff = blobSEReqEntry->getBodyBuff();
		if (doc_buff.isNull())
		{
			OmnAlarm << "can not get body buff for docid:" << docid << enderr;
			return false;
		}
		if (content->dataLen() != doc_buff->dataLen())
		{
			OmnAlarm << "content length does not match for docid:" << docid
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
		mLock->unlock();
	}
//for give a snapshot id that can not create snapshot 
//will requst a alarm
/*
   ullSnapshotID = 10;
   blobSEReqEntry->setSnapshotID(ullSnapshotID);
   iRet = mBlobSE->readDoc(rdata, blobSEReqEntry.getPtrNoLock());
   if(iRet != 0)
   {
   OmnAlarm << "read doc failed, iRet=" << iRet 
   << "snapshot id " << ullSnapshotID <<enderr;
   return false;
   }

   return true;
   */
	return true;
}

bool
AosBlobSETester::testMoreThread(u64 snapshot_id)
{
	const AosRundataPtr rdata = mRundata;
	u64	doc_id = 10001;
	u32	buff_size = 10;
	bool finish = false;
	u64 rawfid = 1;
	u64	offset = 100;
	u64	len = 10;
	u64	timestamp = OmnGetTimestamp();
	/*
	if(!mSnapShotMgr)
	{
		AosRawSEPtr rawse = OmnNew AosRawSE();                                                
		mSnapShotMgr = OmnNew AosBlobSESnapshotMgr(mBlobSE, rawse);
	}
	*/
	mLock->lock();
	//build header
	AosBlobHeaderPtr header = OmnNew AosBlobHeader(doc_id, buff_size);
	mLock->unlock();

	////////////////////////////////////////////////////////////////////////
	///////////////         create snapshot first           ////////////////
	////////////////////////////////////////////////////////////////////////
	//for(u64 i=0; i<5; i++)
	//{
		bool rslt = mSnapShotMgr->createSnapshot(mRundata, snapshot_id);
		
		rslt = saveDoc(rdata, header, snapshot_id, doc_id, rawfid, offset, len);
		aos_assert_r(rslt, false);

		rawfid++;
		offset+=100;
		len+=10;

		//modify doc
		rslt = saveDoc(rdata, header, snapshot_id, doc_id, rawfid, offset, len);
		aos_assert_r(rslt, false);

		//check value
		rslt = readDoc(rdata, header, snapshot_id, doc_id, rawfid, offset, len, timestamp);
		aos_assert_r(rslt, false);

		//commit snapshot
		const u64 snap_id = snapshot_id;
		rslt = mSnapShotMgr->commitSnapshot(rdata, snap_id);
		aos_assert_r(rslt, false);

		//snapshot_id++;
	//}
	return true;
}




