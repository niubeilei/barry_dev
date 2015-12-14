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
#include "StorageEngine/Testers/ResultSyncTester.h"

#include "API/AosApi.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "StorageEngine/Testers/ResultUtilTester.h"
#include "DocClient/DocidShufflerMgr.h"
#include "DocClient/DocClient.h"
#include "SEInterfaces/DataBlobObj.h"
#include "SEInterfaces/DataCacherObj.h"
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


AosResultSyncTester::AosResultSyncTester(
		const OmnString &scanner_id, 
		const AosRundataPtr &rdata)
:
mScannerId(scanner_id),
mToTalDocs(0),
mLock(OmnNew OmnMutex()),
mFieldStr("")
{
}



AosResultSyncTester::~AosResultSyncTester()
{
}


bool 
AosResultSyncTester::basicTest()
{
	cout << "Start  ..." << mScannerId << endl;
	AosRundataPtr rdata = OmnApp::getRundata()->clone(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(100);
	rdata->setUserid(307);
	docShuffle(mScannerId, rdata);
	return true;
}


bool
AosResultSyncTester::docShuffle(
		const OmnString &scanner_id,
		const AosRundataPtr &rdata)
{
	vector<AosQueryBitmapObjPtr> bitmaps; 
	aos_assert_r(createBitmapByOnePhy(bitmaps, rdata), false);
	mFieldStr = "xml";
	if (rand()%2 == 1)
	{
		mFieldStr = "text";
	}
OmnScreen << "---------------------------- mFieldStr: " << mFieldStr << endl;
	OmnString docstr = "<fnames >";
	docstr << "<fname zky_data_type=\"string\" type=\"" << mFieldStr << "\" >"
		<< "str" << "</fname> </fnames>";
	AosXmlParser parser;
	AosXmlTagPtr fnames = parser.parse(docstr, "" AosMemoryCheckerArgs);

	AosResultUtilTester::sendStart(scanner_id, fnames, rdata);
	AosResultUtilTester::sendData(scanner_id, bitmaps, 0, rdata);

	int total_num_data_msgs = bitmaps.size();
	AosResultUtilTester::sendFinished(scanner_id, rdata);
	aos_assert_r(retrieveData(mScannerId, rdata), false);
	return true;
}


bool
AosResultSyncTester::initDocids(const u64 &start_docid, const int &num_docs)
{
	AosDocids id;
	id.start_docid = start_docid;
	id.num_docs = num_docs;
	mDocids.push_back(id);
}


bool
AosResultSyncTester::createBitmapByOnePhy(
		vector<AosQueryBitmapObjPtr> &bitmaps,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mDocids.size() > 0, false);
	mToTalDocs = 0;
	for (u32 j = 0; j < mDocids.size(); j++)
	{
		int k = bitmaps.size();
		bitmaps.push_back(OmnNew AosQueryBitmap());
		AosDocids id = mDocids[j];
		u64 start_docid = id.start_docid;
		u64 num = id.num_docs;
		for (u64 i = 0; i < num; i++)
		{
			bitmaps[k]->appendDocid(start_docid);
			start_docid ++;
		}
		mToTalDocs += num;
	}
	OmnScreen << "---------------- scannerid: " << mScannerId << " , total_num_docs : " << mToTalDocs << endl;
	return true;
}


bool
AosResultSyncTester::retrieveData(
		const OmnString &scanner_id,
		const AosRundataPtr &rdata)
{
	aos_assert_r(getDocs(scanner_id, rdata), false);
	return true;
}


bool
AosResultSyncTester::getDocs(
		const OmnString &scanner_id,
		const AosRundataPtr &rdata)
{
	int serverid = 0;
	for (; serverid< AosGetNumPhysicals(); serverid++)
	{
		while (1)
		{
			AosBuffPtr buff = AosDocClient::getSelf()->batchGetDocs(scanner_id, serverid, rdata);
			if (!buff || buff->dataLen() <= 0) break;
			buff->reset();
			parseBuff(rdata, buff);
		}
	}

	check();
	return true;
}


bool
AosResultSyncTester::parseBuff(
		const AosRundataPtr &rdata,
		const AosBuffPtr &buff)
{
	if (mFieldStr == "text")
	{
		return formatText(buff, rdata);
	}
	return formatAll(buff, rdata);
}


bool
AosResultSyncTester::formatText(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(buff && buff->dataLen() > 0, false);
	OmnScreen << "----------- scannerid: " << mScannerId << " , data Len : " << buff->dataLen() << " , mFieldStr: " << mFieldStr << endl;
	map<u64, int>::iterator itr;
	while (buff->getCrtIdx()< buff->dataLen())
	{
		u32 record_len = buff->getU32(0);
		u64 docid = buff->getU64(0);
		aos_assert_r(docid && record_len, false);

		OmnString str;
		str = buff->getOmnStr("");
		aos_assert_r(str != "", false);
		AosDocType::E type = AosDocType::getDocType(docid);
		if(type == AosDocType::eFixedLength)
		{
			OmnString sep = ":";
			vector<OmnString> vv;
			AosStrSplit::splitStrBySubstr(str.data(), sep.data(), vv, 100); 
			aos_assert_r(vv.size() == 5, false);

			OmnString docidstr;
			docidstr << docid;
			OmnString docidstr1;
			docidstr1 << docid*424;

			aos_assert_r(docidstr == vv[0], false);
			OmnString a = "12345abcde";
			aos_assert_r(a == vv[1], false);
			OmnString b = "xyzrsw98765";
			aos_assert_r(b == vv[2], false);
			aos_assert_r(docidstr1== vv[3], false);
			aos_assert_r(docidstr == vv[4], false);
		}
		else
		{
			OmnString a = "ksjdflskjdflksjdlfkjsdl";
			aos_assert_r(str == a, false);
		}

		mLock->lock();
		map<u64, int>::iterator itr;
		itr = mMap.find(docid);
		aos_assert_rl(itr == mMap.end(), mLock, false);
		mMap.insert(make_pair(docid, 1));
		mLock->unlock();
	}
}


bool
AosResultSyncTester::formatAll(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(buff && buff->dataLen() > 0, false);
OmnScreen << "----------- scannerid: " << mScannerId << " , data Len : " << buff->dataLen() << " , mFieldStr: " << mFieldStr << endl;
	map<u64, int>::iterator itr;
	while (buff->getCrtIdx()< buff->dataLen())
	{
		u32 record_len = buff->getU32(0);
		u64 docid = buff->getU64(0);
		aos_assert_r(docid && record_len, false);

		char data[record_len +1];
		bool rslt = buff->getBuff(data, record_len);
		data[record_len] = 0;
		aos_assert_r(rslt, false);
		AosDocType::E type = AosDocType::getDocType(docid);
		if(type == AosDocType::eFixedLength)
		{

			OmnString sep = ":";
			vector<OmnString> vv;
			AosStrSplit::splitStrBySubstr(data, sep.data(), vv, 100); 
			aos_assert_r(vv.size() == 5, false);

			OmnString docidstr;
			docidstr << docid;
			OmnString docidstr1;
			docidstr1 << docid*424;

			aos_assert_r(docidstr == vv[0], false);
			OmnString a = "12345abcde";
			aos_assert_r(a == vv[1], false);
			OmnString b = "xyzrsw98765";
			aos_assert_r(b == vv[2], false);
			aos_assert_r(docidstr1== vv[3], false);
			aos_assert_r(docidstr == vv[4], false);
		}

		mLock->lock();
		map<u64, int>::iterator itr;
		itr = mMap.find(docid);
		aos_assert_rl(itr == mMap.end(), mLock, false);
		mMap.insert(make_pair(docid, 1));
		mLock->unlock();
	}
}


bool
AosResultSyncTester::check()
{
OmnScreen << "------------------  Finished scanner_id:  " << mScannerId 
	<< " , mTotalDocs: " << mToTalDocs << " , map.size: " << mMap.size()<< endl;
	aos_assert_r(mDocids.size() > 0, false);
	mLock->lock();
	map<u64, int>::iterator itr;
	for (u32 j = 0; j < mDocids.size(); j++)
	{
		AosDocids id = mDocids[j];
		u64 start_docid = id.start_docid;
		u64 num = id.num_docs;
		for (u32 i = 0; i < num; i++)
		{
			itr = mMap.find(start_docid);
			aos_assert_rl(itr != mMap.end(), mLock, false);
			start_docid ++;
		}
	}
	mLock->unlock();
	return true;
}


