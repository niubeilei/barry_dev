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
// How to torture: 
// 1. Create a table of records randomly.
// 2. Determine one column as the 'driving' column. This is the matched key.
// 3. Separate one or more columns into one IIL. 
//   
//
// Modification History:
// 09/26/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DataScanner/Testers3/DocScannerTester.h"

#include "API/AosApi.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "DataScanner/Testers3/FixedLengthDoc.h"
#include "DataScanner/Testers3/NormalDoc.h"
#include "DataScanner/DataScanner.h"
#include "DataScanner/DocScanner.h"
#include "DocClient/DocidShufflerMgr.h"
#include "DocClient/DocClient.h"
#include "SEInterfaces/DataBlobObj.h"
#include "SEInterfaces/DataCacherObj.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/Ptrs.h"
#include "Porting/Sleep.h"
#include "SEUtil/DocTags.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "Thread/Ptrs.h"

static u64 smCounter = 0;

AosDocScannerTester::AosDocScannerTester(
		const OmnString &scanner_id, 
		const OmnString &objid,
		const bool &bb,
		const AosRundataPtr &rdata)
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mThread(0),
mNum(0),
mTotalDocs(0)
{
	mScannerId = scanner_id;
	mObjid = objid;
	mIsRetrieve = bb;	
	OmnThreadedObjPtr thisptr(this, false);
	mThread = OmnNew OmnThread(thisptr, "docbatchreader", 0, false, true);
	mThread->start();
}


bool
AosDocScannerTester::config(const AosRundataPtr &rdata)
{
	// Retrieve the record
	OmnString sep = "";
	AosDataScannerObjPtr scanner = AosDataScannerObj::createDocScannerStatic(
			        mScannerId, rdata);
	AosDataBlobObjPtr blob = AosDataBlobObj::createDataBlobStatic(
			        sep, rdata);
	mDataCacher = AosDataCacherObj::createDataCacher(scanner, blob, rdata);
	return true;
}


AosDocScannerTester::~AosDocScannerTester()
{
}

bool
AosDocScannerTester::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if (!mRun)
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}
		mRun = false;
		mLock->unlock();
		basicTest();
	}
	return true;
}


bool
AosDocScannerTester::signal(const int threadLogicId)
{
	return true;
}


bool
AosDocScannerTester::checkThread(OmnString &err,  const int thrdLogicId) const
{
	return true;
}


void
AosDocScannerTester::startScanner(bool b)
{
	mLock->lock();
	mRun = b;
	mCondVar->signal();
	mLock->unlock();
}

bool 
AosDocScannerTester::basicTest()
{
	cout << "Start  ..." << mScannerId << endl;
	AosRundataPtr rdata = OmnApp::getRundata()->clone(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(100);
	rdata->setUserid(307);
	mFlag = true;
	mBatchType = "";
	OmnString scanner_id = mScannerId;
	bool delete_flag = false;
	for (int i = 0; i< 1; i++)
	{
		OmnScreen << "read ---------------------------------" << endl;
		mBatchType = "read";
		scanner_id = "read0";
		aos_assert_r(docShuffle(scanner_id, rdata), false);
		OmnSleep(5);

		u64 startTime = OmnGetSecond();
		aos_assert_r(retrieveData(scanner_id , rdata), false);
		u32 delta = OmnGetSecond() - startTime;
		OmnScreen << " ======== num docs :"<< mTotal << " , Speed: " << delta << "s , : "
			<< delta / 60 << "m"<< endl;

		if (delete_flag)
		{
			OmnScreen << "delete ---------------------------------" << endl;
			mBatchType = "delete";
			scanner_id = "delete1";
			aos_assert_r(docShuffle(scanner_id, rdata), false);
			OmnSleep(5);
			aos_assert_r(deleteDocs(scanner_id, rdata), false);

			OmnScreen << "read ---------------------------------" << endl;
			mFlag = false;
			mBatchType = "read";
			scanner_id = "read2";
			aos_assert_r(docShuffle(scanner_id, rdata), false);
			OmnSleep(5);
			aos_assert_r(retrieveData(scanner_id, rdata), false);
		}

	}

	return true;
}


bool
AosDocScannerTester::docShuffle(
		const OmnString &scanner_id,
		const AosRundataPtr &rdata)
{
	aos_assert_r(initByOnePhy(rdata), false);
	vector<AosBitmapObjPtr> bitmaps; 
	aos_assert_r(createBitmapByOnePhy(bitmaps, rdata), false);
	aos_assert_r(sendStart(scanner_id, rdata), false);
	OmnSleep(1);
	aos_assert_r(sendData(scanner_id, bitmaps, rdata), false);
	OmnSleep(4);
	int total_num_data_msgs = bitmaps.size();
	aos_assert_r(sendFinished(scanner_id, total_num_data_msgs, rdata), false);
	OmnSleep(4);
	return true;
}

bool
AosDocScannerTester::initByOnePhy(const AosRundataPtr &rdata)
{
	aos_assert_r(mObjid != "", false);
	AosXmlTagPtr dd = AosDocClient::getSelf()->getDocByObjid(mObjid, rdata);	
	aos_assert_r(dd, false);
	AosXmlTagPtr records = dd->getFirstChild();
	aos_assert_r(records, false);
	int numsubs = records->getNumSubtags();
	aos_assert_r(numsubs, false);
	int i = 0;
	while(i<numsubs)
	{
		AosXmlTagPtr record1 = records->getChild(i);
		aos_assert_r(record1, false);
		u64 start_docid = record1->getAttrU64("start_docid", 0);
		aos_assert_r(start_docid, false);
		mStartDocid.push_back(start_docid);	
		u64 num_docs = record1->getAttrU64("num_docs", 0);
		mNumDocs.push_back(num_docs);	
		mTotalDocs += num_docs;
		i ++;
	}
	mTotal = 0;
	AosXmlTagPtr record = records->getFirstChild();
	while(record)
	{
		u64 num_docs = record->getAttrU64("num_docs", 0);
		mTotal += num_docs;
		record = records->getNextChild();
	}
	return true;
}


bool
AosDocScannerTester::initByTwoPhy(const AosRundataPtr &rdata)
{
	aos_assert_r(mObjid != "", false);
	AosXmlTagPtr dd = AosDocClient::getSelf()->getDocByObjid(mObjid, rdata);	
	aos_assert_r(dd, false);
	AosXmlTagPtr records = dd->getFirstChild();
	aos_assert_r(records, false);
	int numsubs = records->getNumSubtags();
	aos_assert_r(numsubs, false);
	if (AosGetSelfServerId() == 0)
	{
		int i = 0;
		while(i<(numsubs/2))
		{
			AosXmlTagPtr record1 = records->getChild(i);
			aos_assert_r(record1, false);
			u64 start_docid = record1->getAttrU64("start_docid", 0);
			aos_assert_r(start_docid, false);
			mStartDocid.push_back(start_docid);	
			u64 num_docs = record1->getAttrU64("num_docs", 0);
			mNumDocs.push_back(num_docs);	
			mTotalDocs += num_docs;
			i ++;
		}
	}
	else
	{
		int i = numsubs/2;
		while(i<numsubs)
		{
			AosXmlTagPtr record = records->getChild(i);
			aos_assert_r(record, false);
			u64 start_docid = record->getAttrU64("start_docid", 0);
			aos_assert_r(start_docid, false);
			mStartDocid.push_back(start_docid);	
			u64 num_docs = record->getAttrU64("num_docs", 0);
			mNumDocs.push_back(num_docs);	
			mTotalDocs += num_docs;
			i ++;
		}
	}

	mTotal = 0;
	AosXmlTagPtr record = records->getFirstChild();
	while(record)
	{
		u64 num_docs = record->getAttrU64("num_docs", 0);
		mTotal += num_docs;
		record = records->getNextChild();
	}
	return true;
}


bool
AosDocScannerTester::createBitmapByOnePhy(
		vector<AosBitmapObjPtr> &bitmaps,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mStartDocid.size() > 0, false);
	u64 count = 0; 
	u64 k = 0;
	u64 sum = rand() % (mTotalDocs/2);
	bitmaps.push_back(OmnNew AosBitmap());
	for (u32 j = 0; j < mStartDocid.size(); j++)
	{
		u64 start_docid = mStartDocid[j];
		u64 num = mNumDocs[j];
OmnScreen << "add  scanner_id: " << mScannerId << " , " << start_docid << " , " << num << endl;
		for (u64  i = 0; i < num; i++)
		{
if (start_docid % 7 == 1 && mFlag) 
{
	start_docid ++;
	continue;
}
			count ++;
			if (count >= sum) 
			{
				bitmaps.push_back(OmnNew AosBitmap());
				count = 0; 
				k++;
			}

			bitmaps[k]->appendDocid(start_docid);
			start_docid ++;
		}	
	}
	return true;
}



bool
AosDocScannerTester::createBitmapByTwoPhy(
		vector<AosBitmapObjPtr> &bitmaps,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mStartDocid.size() > 0, false);
	u64 count = 0; 
	u64 k = 0;
	u64 sum = rand() % (mTotalDocs/2);
	bitmaps.push_back(OmnNew AosBitmap());
	for (u32 j = 0; j < mStartDocid.size(); j++)
	{
		u64 start_docid = mStartDocid[j];
		u64 num = mNumDocs[j];
OmnScreen << "add  scanner_id: " << mScannerId << " , " << start_docid << " , " << num << endl;
		for (u64  i = 0; i < num; i++)
		{
if (start_docid % 7 == 1 && mFlag) 
{
	start_docid ++;
	continue;
}
			count ++;
			if (count >= sum) 
			{
				bitmaps.push_back(OmnNew AosBitmap());
				count = 0; 
				k++;
			}

			int server_id = start_docid % AosGetNumCubes() % AosGetNumPhysicals();	
			if (server_id == AosGetSelfServerId())
			{
				bitmaps[k]->appendDocid(start_docid);
			}
			start_docid ++;
		}	
	}
	return true;
}

bool
AosDocScannerTester::sendStart(
		const OmnString &scanner_id, 
		const AosRundataPtr &rdata)
{
	//bool rslt = AosDocidShufflerMgr::getSelf()->sendStart(scanner_id,  2, "", 2000000, "read", rdata);
	bool rslt = AosDocidShufflerMgr::getSelf()->sendStart(scanner_id,  2, "", 1000000, mBatchType, rdata);
	return rslt;
}


bool
AosDocScannerTester::sendData(
		const OmnString &scanner_id,
		const vector<AosBitmapObjPtr> &bitmaps,
		const AosRundataPtr &rdata)
{
OmnScreen << "bitmaps size:" << bitmaps.size() << endl;
	int num_thrds = 1;
	for (u32 i = 0; i < bitmaps.size(); i++)
	{
		bool rslt = AosDocidShufflerMgr::getSelf()->shuffle(scanner_id, bitmaps[i], num_thrds, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosDocScannerTester::sendFinished(
		const OmnString &scanner_id, 
		const int total_num_data_msgs,
		const AosRundataPtr &rdata)
{
	bool rslt = AosDocidShufflerMgr::getSelf()->sendFinished(scanner_id, total_num_data_msgs, rdata);
	return rslt;
}


bool
AosDocScannerTester::retrieveData(
		const OmnString &scanner_id,
		const AosRundataPtr &rdata)
{
	if (!mIsRetrieve) return true;
	aos_assert_r(getDocs(scanner_id, rdata), false);
//	aos_assert_r(getDocs(rdata), false);
//	aos_assert_r(check(rdata), false);
	mStartDocid.clear();
	mNumDocs.clear();
	mMap.clear();
	
OmnScreen << "======" << mScannerId << " END "<< "=======" << mStartDocid.size() << " , "<< mNumDocs.size() << " , " <<mMap.size() << " , " << mTotalDocs<< endl;
	return true;
}


//////////////////// Doc Cacher /////////////////////
bool
AosDocScannerTester::getDocs(const AosRundataPtr &rdata)
{
	aos_assert_r(config(rdata), false);
	aos_assert_r(mDataCacher, false);
	vector<AosDataCacherObjPtr> cacher;
	mDataCacher->split(cacher, rdata);

	u64 startTime = OmnGetSecond();
	for (u32 i = 0; i< cacher.size(); i++)
	{
		aos_assert_r(nextValue(cacher[i], rdata), false);
	}

	u32 delta = OmnGetSecond() - startTime;
	if (delta > 0)
	{
		OmnScreen << "Trying: " << smCounter << ". Speed: " << smCounter / delta << endl;
	}
	OmnScreen << "num docs :"<< smCounter << " , Speed: " << delta << endl; 
	return true;
}

bool
AosDocScannerTester::nextValue(
		const AosDataCacherObjPtr &cacher,
		const AosRundataPtr &rdata)
{
	char *data;
	int len;
	u64 docid = 0;
	while (1)
	{
		//data = cacher->nextValue(len, rdata);
		cacher->nextValue(&data, len, docid, rdata);
		if (!data) break;
		aos_assert_r(procData(data, len, docid, rdata), false);
	}
	return true;
}

bool
AosDocScannerTester::procData(
		char *data, 
		const u32 &record_len, 
		const u64 &docid,
		const AosRundataPtr &rdata)
{
//	u32 record_len = *(u32 *) data;
//	u64 docid = *(u64 *) &data[4];

//	aos_assert_r(len == (sizeof(u64) + sizeof(u32) + record_len), false);
	aos_assert_r(docid && record_len, false);

	AosDocType::E type = AosDocType::getDocType(docid);
	if (type == AosDocType::eFixedLength)
	{
		//u64 id = *(u64 *)&data[12];
		OmnString docidstr(data, 18);
		u64 id = atoll(docidstr.data());
		if (docid != id)
		{
			AosXmlTagPtr doc = AosDocClient::getSelf()->getDocByDocid(docid, rdata);
			if (doc)
			{
				if (docid != id)
				{
					AosXmlTagPtr doc = AosDocClient::getSelf()->getDocByDocid(docid, rdata);
OmnScreen << "Failed docid :" << docid << endl; 
				}
			}
		}
		aos_assert_r(docid == id, false);
	}
	else
	{
		AosXmlParser parser;
		AosXmlTagPtr xml = parser.parse(&data[18], record_len, "" AosMemoryCheckerArgs);
		u64 id = xml->getAttrU64(AOSTAG_DOCID, 0);
		aos_assert_r(docid == id, false);
	}
	smCounter ++;
	map<u64, int>::iterator itr;
	itr = mMap.find(docid);
	aos_assert_r(itr == mMap.end(), false);
	mMap.insert(make_pair(docid, 1));
	return true;
}

//////////////////////////////// Data Cacher //////////////////////

///////////////////////////////////////////////////////////////
bool
AosDocScannerTester::getDocs(
		const OmnString &scanner_id,
		const AosRundataPtr &rdata)
{
OmnScreen << "=========================" << mScannerId << "=======================" << endl;
	u64 distid = 1;
	for (; distid<= (u64)AosGetNumPhysicals(); distid++)
	{
		AosBuffPtr buff = read(scanner_id, distid, rdata);
		while (buff->dataLen() > 0)
		{
			//parseDoc(buff, rdata);
			buff->reset();
			buff = read(scanner_id, distid, rdata);
		}
	}
	return true;
}

AosBuffPtr
AosDocScannerTester::read(
		const OmnString &scanner_id,
		const u64 &distid,
		const AosRundataPtr &rdata)
{
	AosBuffPtr buff = AosDocClient::getSelf()->batchGetDocs(scanner_id, distid, rdata);
	return buff;
}

bool
AosDocScannerTester::parseDoc(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	//mBuff format is:
	//      docid           (8 bytes)
	//      doc_length      (4 bytes)
	//      doc contents    (variable)
	aos_assert_r(buff && buff->dataLen() > 0, false);
	map<u64, int>::iterator itr;
	while (buff->getCrtIdx()< buff->dataLen())
	{
		u32 record_len = buff->getU32(0);
		u64 docid = buff->getU64(0);
		aos_assert_r(docid && record_len, false);

		char data[record_len];
		bool rslt = buff->getBuff(data, record_len);
		aos_assert_r(rslt, false);
		AosDocType::E type = AosDocType::getDocType(docid);
		if (type == AosDocType::eFixedLength)
		{
			//u64 id = *(u64 *)data;
			OmnString docidstr(data, 18);
			u64 id = atoll(docidstr.data());
			if (docid % 7 == 1 && !mFlag)
			{
				aos_assert_r(docid == id ,false);
			}
			else
			{
				if (!mFlag)
				{
					//aos_assert_r(docid != id, false);
					aos_assert_r(id == 0, false);
				}
				else
				{
					aos_assert_r(docid == id, false);
				}
			}
		}
		else
		{
			AosXmlParser parser;
			AosXmlTagPtr xml = parser.parse(data, record_len, "" AosMemoryCheckerArgs);
			u64 id = xml->getAttrU64(AOSTAG_DOCID, 0);
			aos_assert_r(docid == id, false);
		}

		itr = mMap.find(docid);
		aos_assert_r(itr == mMap.end(), false);
		mMap.insert(make_pair(docid, 1));
	}
	return true;
}
///////////////////////////////////////////////////////////////

bool
AosDocScannerTester::check(const AosRundataPtr &rdata)
{
	aos_assert_r(initcheck(rdata), false);

	int failed = 0;
	int failed2 = 0;
	aos_assert_r(mMap.size() > 0, false);
	map<u64, int>::iterator itr;
	int num;
	u32 count = 0;
	for (u32 i = 0; i < mStartDocid.size(); i++)
	{
		u64 start_docid = mStartDocid[i];
		num = mNumDocs[i];
		for (int j = 0; j <num; j++)
		{
if (start_docid % 7 == 1 && mFlag)
{
	if (mBatchType == "delete")
	{
		AosXmlTagPtr doc = AosDocClient::getSelf()->getDocByDocid(start_docid, rdata);
		if (!doc)
		{
			OmnScreen << "Failed " << " , docid :" << start_docid
				<< ", sizeid :" << AosGetSizeIdByDocid(start_docid)<< endl;
		}
		aos_assert_r(doc, false);
	}
	start_docid ++;
	continue;
}
			int server_id = start_docid % AosGetNumCubes() % AosGetNumPhysicals();	
			if (server_id == AosGetSelfServerId())
			{
				itr = mMap.find(start_docid);
				if (itr == mMap.end())
				{
					AosXmlTagPtr doc = AosDocClient::getSelf()->getDocByDocid(start_docid, rdata);
					if (doc)
					{
						OmnScreen << "Failed " << " , docid :" << start_docid  
							<< ", sizeid :" << AosGetSizeIdByDocid(start_docid)<< endl;
						failed ++;
					}
					failed2 ++;
					if (failed2 %10000 == 0) OmnScreen << start_docid << ", "<< endl;;
				}
				//aos_assert_r(itr != mMap.end(), false);
				count ++;
			}
			start_docid ++;
		}
	}
	OmnScreen << "scanner_id: " << mScannerId << " , Failed num :" << failed  << " , count : " << count << " ,size: " << mMap.size() << endl;
	aos_assert_r(count  == mMap.size(), false);
	return true;
}

bool
AosDocScannerTester::initcheck(const AosRundataPtr &rdata)
{
	mNumDocs.clear();
	mStartDocid.clear();
	mTotalDocs = 0;
	AosXmlTagPtr dd = AosDocClient::getSelf()->getDocByObjid("vpd_data", rdata);	
	aos_assert_r(dd, false);
	AosXmlTagPtr records = dd->getFirstChild();
	aos_assert_r(records, false);
	AosXmlTagPtr record = records->getFirstChild();
	while(record)
	{
		u64 start_docid = record->getAttrU64("start_docid", 0);
		aos_assert_r(start_docid, false);
		mStartDocid.push_back(start_docid);	
		u64 num_docs = record->getAttrU64("num_docs", 0);
		mNumDocs.push_back(num_docs);	
		mTotalDocs += num_docs;
		record = records->getNextChild();
	}
	return true;
}


bool
AosDocScannerTester::deleteDocs(const OmnString &scanner_id, const AosRundataPtr &rdata)
{
	bool rslt = AosDocClient::getSelf()->deleteBatchDocs(rdata, scanner_id);	
	aos_assert_r(rslt, false);
	return true;
}

