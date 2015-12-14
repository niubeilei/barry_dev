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
// 2015-5-5 Modified by Koala Ren
////////////////////////////////////////////////////////////////////////////
#include "BlobSE/TesterNew/BlobSETesterNew.h"

#include "Alarm/Alarm.h"
#include "BlobSE/BlobSE.h"
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
#include <arpa/inet.h>
#include <limits.h>
#include <set>
#include <stdlib.h>
#include <string.h>
#include <vector>

using namespace std;

extern AosXmlTagPtr			gApp_config;
extern i64					gTestDuration;


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
	mCreateWeight = config->getAttrInt("create_weight", 25);
	mModifyWeight = config->getAttrInt("modify_weight", 25);
	mReadWeight = config->getAttrInt("read_weight", 25);
	mDeleteWeight = config->getAttrInt("delete_weight", 25);

	mReadInvalidWeight = config->getAttrInt("read_invalid_weight", 10);
	mReadValidWeight = config->getAttrInt("read_valid_weight", 90);
	mReadDeletedWeight = config->getAttrInt("read_deleted_weight", 90);
	mNumOfThreads = config->getAttrU32("number_of_threads", 1);

	mFinishedTries = 0;
    mTestDurationSec = gTestDuration;
	mTestCount = 0;
	mReadTestCount = 0;
	mCreateTestCount = 0;
	mDelTestCount = 0;
	mModifyTestCount = 0;
	mReadFailedCount = 0;
	mCreateFailedCount = 0;
	mDelFailedCount = 0;
	mModifyFailedCount = 0;
	mNumFinished = 0;
	mCrtSec = 0;
	mMaxDocid = 0;
}


AosBlobSETester::~AosBlobSETester()
{
}


bool
AosBlobSETester::start()
{
	OmnThreadedObjPtr thisptr(this, false);

    AosXmlTagPtr config = gApp_config->getFirstChild();
	config = config->getFirstChild(mName.data());
	OmnScreen << "Weights for this trial" << endl
			<< "read:'" << mReadWeight << "'" << endl
			<< "create:'" << mCreateWeight << "'" << endl
			<< "modify:'" << mModifyWeight << "'" << endl
			<< "delete:'" << mDeleteWeight << "'" << endl;

	mCrtSec = OmnTime::getCrtSec();
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

    OmnScreen << "mTries = " << mTries << endl;
    OmnScreen << "mFinishedTries = " << mFinishedTries << endl;
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
    if (mTries <= 0) mTries = eDefaultTries;
    while(1)
    {
        mLock->lock();
        if(mFinishedTries < mTries)
        {
            mLock->unlock();
            if(mFinishedTries >= mTries)
            {
                break;
            }
            oneBatchTests(mBatchSize, start_docid, num_docids);
            mLock->lock();
            mFinishedTries++;
            mLock->unlock();
        }
        else
        {
            mLock->unlock();
            break;
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
AosBlobSETester::oneBatchTests(
		const int	batch_size,
		u64			&start_docid,
		u32			&num_docids)
{
	int testMinValue[] = {
            eCreateDocs,
            eModifyDocs,
            eReadDocs,
            eDeleteDocs
            };
	int testWeight[] = {
            mCreateWeight,
            mModifyWeight,
            mReadWeight,
            mDeleteWeight};

	bool rslt = false;
	for (int i=0; i<batch_size; i++)
	{
        switch(OmnRandom::intByRange(
					testMinValue, testMinValue, testWeight, 4))
		{
		case eCreateDocs:
             rslt = testCreateDoc();
             aos_assert_r(rslt, false);
             break;

		case eModifyDocs:
             rslt = testModifyDoc();
             aos_assert_r(rslt, false);
			 break;

		case eReadDocs:
			 rslt = testReadDoc();
             aos_assert_r(rslt, false);
			 break;

		case eDeleteDocs:
             rslt = testDeleteDoc();
             aos_assert_r(rslt, false);
			 break;

		default:
			 OmnShouldNeverComeHere;
			 return false;
		}
	}

	return true;
}


bool
AosBlobSETester::wayOfCreateDoc(AosXmlTagPtr &doc, OmnString &docStr)
{
    OmnString docContents;
    docContents = "<doc><![CDATA[docStr]]>  </doc>";//
    doc = AosXmlParser::parse(docContents);
    if(rand()%100 < 50)//50% create doc by jimoCreateDoc, and 50% create doc by jimoCreateBinaryDoc
    {
        doc = Jimo::jimoCreateDoc(mRundata.getPtrNoLock(), doc);
    }
    else
    {
        u32 buffSize = rand();
        AosBuffPtr buff = OmnNew AosBuff(buffSize);
        buff->setOmnStr(docStr);
        doc = Jimo::jimoCreateBinaryDoc(mRundata.getPtrNoLock(), docContents, true, buff);//
    }
    aos_assert_r(!(doc == false), false);
    return true;
}


bool
AosBlobSETester::wayOfDeleteDoc(u64 docid,const OmnString &objid)
{
    // if deletebinarydoc or deletedoc depend on the doctype
    //
    //
    bool rslt;
    if(rand()%100 < 50)
    {
        rslt  = Jimo::jimoDeleteDocByDocid(mRundata.getPtrNoLock(), docid);
    }
    else
    {
        rslt = Jimo::jimoDeleteDocByObjid(mRundata.getPtrNoLock(), objid);
    }
    return rslt;
}


bool
AosBlobSETester::wayOfReadDoc(AosXmlTagPtr &doc, u64 docid,const OmnString &objid)
{
    if(rand()%100 < 50)//50% create doc by jimoCreateDoc, and 50% create doc by jimoCreateBinaryDoc
    {
        doc = Jimo::jimoGetDocByDocid(mRundata.getPtrNoLock(), docid);
    }
    else
    {
        doc = Jimo::jimoGetDocByObjid(mRundata.getPtrNoLock(), objid);
    }
    if(doc == 0)
        return false;
    return true;
}


u64
AosBlobSETester::generateSnapshotID()
{
    // This function picks a snapshot ID.
    // If no snapshots yet, return 0;

    //OmnNotImplementedYet;
    return 0;
}


u64
AosBlobSETester::getDocID(u64 index)
{
    aos_assert_r(index >= 0, false);
    return mDocs[index][0].docid;
}


u64
AosBlobSETester::pickSnapshotID(u64 index)
{
    //pick up a snapshot from the history log
    aos_assert_r(index >= 0, false);
    int n = rand() % mDocs[index].size();
    return mDocs[index][n].snapshot;
}


u64
AosBlobSETester::pickTimestamp(u64 index)
{
    //pick up a snapshot from the history log
    aos_assert_r(index >= 0, false);
    int n = rand() % mDocs[index].size();
    return mDocs[index][n].timestamp;
}


AosBlobSETester::Operations
AosBlobSETester::getDocState(u64 index)
{
    //get the last operator from the history log
    //aos_assert_r(index >= 0, false);
    if(index <= 0)
        return eNotCreate;
    int n = mDocs[index].size()-1;
    return mDocs[index][n].oper;
}


bool
AosBlobSETester::generateContents(u32 seed_id,u32 repeat,OmnString &docStr)
{
    for(int i =0; i< repeat; i++)
    {
        docStr << seed_id;
    }
    if(docStr == NULL)
    {
        return false;
    }
    return true;
}


bool
AosBlobSETester::isDocEqualOriginal(u64 index, const AosXmlTagPtr &doc)
{
    //with docid we can rebuild the doc
    //and then compare to the doc we read
    //judge if they are equal
    aos_assert_r(index >= 0, false);
    OmnString docstr;
    u32 seed = mDocs[index][mDocs[index].size()-1].seed_id;
    u32 repeat = mDocs[index][mDocs[index].size()-1].repeat;
    generateContents(seed, repeat, docstr);
    if(docstr == doc->getNodeText())
    {
        return true;
    }
    return false;
}


bool
AosBlobSETester::testCreateDoc()
{
	// This function creates a new doc. The caller
    mCreateTestCount++;
    mTestCount++;
    OmnScreen << "Test No." << mTestCount << ":doc creation." << endl;

    u32 seed_id = rand();
	u64 ullTimestamp = OmnTime::getTimestamp();
    u32 repeat = rand()%100;
    u64 snapshot = 0;
    AosXmlTagPtr doc;

    OmnString docContents, docStr;
    bool rslt = generateContents(seed_id, repeat, docStr);
    aos_assert_r(rslt, false);
    rslt = wayOfCreateDoc(doc, docStr);
    aos_assert_r(rslt, false);
    u64 docid = doc->getAttrU64("zyk_docid",0);
    aos_assert_r(docid > 0, false);

    if(rand() % 100 < 10)//create with a snapshot
    {
        snapshot = generateSnapshotID();//
    }
    vector<DocEntry> vec;
    DocEntry docEntry;
    docEntry.docid = docid;
    docEntry.timestamp = ullTimestamp;
    docEntry.snapshot = snapshot;
    docEntry.repeat = repeat;
    docEntry.seed_id = seed_id;
    docEntry.oper = eCreateDocs;
    vec.push_back(docEntry);
    mDocs.push_back(vec);
    if(docid > mMaxDocid)
        mMaxDocid = docid;

    AosXmlTagPtr getDoc;
    rslt = wayOfReadDoc(getDoc, docid, "");//
    aos_assert_r(rslt, false);
    aos_assert_r(doc->getNodeText() == getDoc->getNodeText(), false);

    return true;
}


bool
AosBlobSETester::testModifyDoc()
{
    //90% modify exist doc, and 10% modify not exist doc
	mModifyTestCount++;
	mTestCount++;
    OmnScreen << "Test No." << mTestCount << ":doc Modification." << endl;
    aos_assert_r(mDocs.size() > 0, false);

    bool rslt;
	u64 docid;
    u64 ullTimestamp = OmnTime::getTimestamp();
    u64 snapshot = 0;
    u32 seed_id = rand();
    u32 repeat = rand()%100;

    OmnString docStr;
    rslt = generateContents(seed_id, repeat, docStr);
    aos_assert_r(rslt, false);
    AosXmlTagPtr doc;
    if(rand()%100 < 10)//modify a doc which not exist
    {
        docid = 2 *mMaxDocid;
        rslt = wayOfReadDoc(doc, docid, "");
        aos_assert_r(!rslt, false);
        return true;
    }
    u64 index = rand()% mDocs.size();
    docid = getDocID(index);
    Operations oper = getDocState(index);
    if(oper == eDeleteDocs || oper == eNotCreate)
    {
        OmnScreen << "this docid has been deleted or not been created yet! "<<endl;
        rslt = wayOfReadDoc(doc, docid,"");
        aos_assert_r(!rslt, false);
        return true;
    }
    rslt = wayOfReadDoc(doc, docid, "");
    aos_assert_r(rslt, false);
    doc->setNodeText(docStr, false);

    AosXmlTagPtr docOfRead;
    rslt = wayOfReadDoc(docOfRead, docid, "");
    aos_assert_r(rslt, false);
    //judge if the contents modified equal to the original doc
    rslt = isDocEqualOriginal(index, docOfRead);
    aos_assert_r(rslt, false);

    if(rand()%100 <10)//randomly generate snapshot
    {
        snapshot = generateSnapshotID();
    }
    DocEntry docEntry;
    docEntry.docid = docid;
    docEntry.timestamp = ullTimestamp;
    docEntry.snapshot = snapshot;
    docEntry.repeat = seed_id;
    docEntry.seed_id = repeat;
    docEntry.oper = eModifyDocs;
    mDocs[index].push_back(docEntry);

	return true;
}


bool
AosBlobSETester::testDeleteDoc()
{
    mDelTestCount++;
    mTestCount++;
    OmnScreen << "Test No." << mTestCount << ":doc delete." << endl;
    aos_assert_r(mDocs.size() > 0, false);
    bool rslt;
    u64 snapshot = 0;
    u64 ullTimestamp = OmnTime::getTimestamp();
    u64 docid;

    if(rand()%100 < 10)//delete an doc which not exist
    {
        docid =  2*mMaxDocid;
        rslt = wayOfDeleteDoc(docid, "");
        aos_assert_r(!rslt, false);
        return true;
    }
    u64 index = rand()%mDocs.size();
    Operations oper = getDocState(index);
    docid = getDocID(index);
    if(oper == eDeleteDocs || oper == eNotCreate)
    {
        rslt = wayOfDeleteDoc(docid, "");
        aos_assert_r(!rslt, false);
        return true;
    }
    rslt = wayOfDeleteDoc(docid, "");
    aos_assert_r(rslt, false);

    //read this doc after delete it, but actually, we can't read the contents successfully
    AosXmlTagPtr doc;
    rslt = wayOfReadDoc(doc, docid, "");
    aos_assert_r(rslt, false);

    if(rand()%100 <10)
    {
        snapshot = generateSnapshotID();
    }
    DocEntry docEntry;
    docEntry.docid = docid;
    docEntry.timestamp = ullTimestamp;
    docEntry.snapshot = snapshot;
    docEntry.repeat = 0;
    docEntry.seed_id = 0;
    docEntry.oper = eDeleteDocs;
    mDocs[index].push_back(docEntry);

    return true;
}


bool
AosBlobSETester::testReadDoc()
{
    //we can read docs by docid and objid, also we can assign a snapshot or timestamp to read a doc
    //but actually, we can't assign snapshot or timestamp now
	mReadTestCount++;
	mTestCount++;
    OmnScreen << "Test No." << mTestCount << ":doc read." << endl;

    bool rslt;
    u64 docid,index;
    u64 ullTimestamp = 0;
    u64 snapshot = 0;
    AosXmlTagPtr doc;
    if(mDocs.size() <= 0)
    {
        OmnScreen<< "no Docs have been created yet!"<<endl;
        docid = rand();
        rslt = wayOfReadDoc(doc, docid, "");
		aos_assert_r(!rslt, false);
        return true;
    }

	switch(OmnRandom::percent(mReadInvalidWeight, mReadValidWeight, mReadDeletedWeight))
	{
	case 0:	//read invalid docids
		docid = mMaxDocid*2 ;
		OmnScreen << "reading docid:" << docid << " as invalid reading" << endl;
        rslt = wayOfReadDoc(doc, docid, "");
        aos_assert_r(!rslt, false);
		break;

	case 1:	//read valid docids
		index = rand()% mDocs.size();
        docid = getDocID(index);
		OmnScreen << "reading docid:" << docid << " as valid reading" << endl;
        if(rand()%100 < 10)//read with snapshot
        {
            //select a snapshot
            snapshot = pickSnapshotID(docid);
        }
        else if(rand()%100 <10)//read with time
        {
            // Find a timestamp to read
           ullTimestamp = pickTimestamp(docid);
        }
  //      Operations oper = getDocState(index);
        if(getDocState(index) == eDeleteDocs || getDocState(index) == eNotCreate)
        {
            OmnScreen << "this docid has been deleted or not been created yet! "<<endl;
            rslt = wayOfReadDoc(doc, docid,"");
            aos_assert_r(!rslt, false);
            return true;
        }
        rslt = wayOfReadDoc(doc, docid, "");
        aos_assert_r(rslt, false);
        // Check whether the contents are correct.
        rslt = isDocEqualOriginal(docid,doc);
        aos_assert_r(rslt,false);
        break;

	default:
		OmnAlarm << "Invalid percentage!" << enderr;
		return false;
	}
    return true;
}





