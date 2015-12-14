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
// 09/20/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DataScanner/Testers3/FixedLengthDoc.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DocClient/DocClient.h"
#include "Random/RandomUtil.h"
#include "DocServer/ProcNames.h"
#include "Rundata/Rundata.h"
#include "QueryRslt/QueryRslt.h"
#include "QueryRslt/QueryRsltMgr.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/QueryContextObj.h"

AosFixedLengthDoc::AosFixedLengthDoc(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
:
mLock(OmnNew OmnMutex()),
mCrtSizeid(0),
mDocSize(0),
mDocid(0),
mNumDocids(0),
mId(1),
mStartDocid(0),
mCheck(false)
{
	if (!config(def))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosFixedLengthDoc::~AosFixedLengthDoc()
{
	for (int i = 0; i<mNumVirtuals; i++)
	{
		OmnDelete [] mDocids[i];
		OmnDelete [] mRawData[i];
		OmnDelete [] mGlobalDocids[i];
		mDocids[i] = 0;
		mRawData[i] = 0;
		mGlobalDocids[i] = 0;
	}
}

bool
AosFixedLengthDoc::config(const AosXmlTagPtr &def)
{
	mNumVirtuals = AosGetNumCubes();
	aos_assert_r(mNumVirtuals, false);

	mNumServers = AosGetNumPhysicals();
	aos_assert_r(mNumServers, false);

	mRecordDoc = def;
	aos_assert_r(mRecordDoc, false);
	
	mDocSize = mRecordDoc->getAttrU64("record_len", 0);
	aos_assert_r(mDocSize, false);
	for (u32 i = 0; i< (u32)mNumVirtuals; i++)
	{
		mIndex[i] = 0;
		mFirstDocid[i] = 0;
		mDocids[i] = OmnNew u64[eMaxDocs];
		memset(mDocids[i], 0, sizeof(u64) * eMaxDocs);
		mGlobalDocids[i] = OmnNew u64[eMaxDocs];
		memset(mGlobalDocids[i], 0, sizeof(u64) * eMaxDocs);
		int size = mDocSize * eMaxDocs;
		mRawData[i] = OmnNew char[size +10];
OmnScreen << "size :" << size << endl;
	}

	return true;
}

u64
AosFixedLengthDoc::getNextDocid(const AosRundataPtr &rdata)
{
	mLock->lock();
	if (mNumDocids <= 0)
	{
		getDocids(mDocid, mNumDocids, rdata);
	}
	aos_assert_rl(mDocid > 0, mLock, 0);
	aos_assert_rl(mNumDocids > 0, mLock, 0);
	u64 docid = mDocid++;
	mNumDocids--;
	mLock->unlock();
	return docid;
}


bool
AosFixedLengthDoc::getDocids(
		u64 &docid,
		int &num_docids,
		const AosRundataPtr &rdata)
{
	if (mCrtSizeid == 0) mCrtSizeid = AosCreateSizeId(mDocSize, mRecordDoc, rdata);
OmnScreen << "mCrtSizeid :" << mCrtSizeid << endl;
	bool overflow = false;
	bool rslt = AosGetDocids(mCrtSizeid, docid, num_docids, overflow, rdata);
	aos_assert_r(rslt, false);
	if (overflow)
	{
		 AosGetDocids(mCrtSizeid, docid, num_docids, overflow, rdata);
	}
	if (mStartDocid == 0) mStartDocid = docid;
	aos_assert_r(!overflow, false);

	aos_assert_r(docid > 0, false);
	aos_assert_r(num_docids > 0, false);
	return true;
}

bool
AosFixedLengthDoc::appendEntry(
		const AosRundataPtr &rdata)
{
	u64 docid = 0;
//if (rand() % 50 != 8)
//{
	docid = getNextDocid(rdata);
//}
//if (docid == 0)
//{
//	int s = rand() % 6;
//	int i = 0;
//	while (i++<s)
//	{
//		int vid = rand() % mNumVirtuals;
//		if (mIndex[vid] >= eMaxDocs)
//		{
//			mLock->lock();
//			bool rslt = sendRequestPriv(rdata, vid);
//			aos_assert_rl(rslt, mLock, false);
//			mIndex[vid] = 0;
//			mLock->unlock();
//		}
//
//		int pos = mIndex[vid];
//		aos_assert_rl(pos < eMaxDocs, mLock, false);
//		mDocids[vid][pos] = 0;
//		mGlobalDocids[vid][pos] = 0;
//		mIndex[vid] ++;
//
//	}
//	return true;
//}
	aos_assert_r(docid, false);

	OmnString str = composeEntry(docid);
	const char *doc = str.data(); 
	
	mLock->lock();
	int vid = AosGetVirtualIDByDocid(docid);
	aos_assert_rl(vid >= 0 && vid < mNumVirtuals, mLock, false);
	if (mIndex[vid] >= eMaxDocs)
	{
		bool rslt = sendRequestPriv(rdata, vid);
		aos_assert_rl(rslt, mLock, false);
		mIndex[vid] = 0;
	}

	if (mFirstDocid[vid] == 0) mFirstDocid[vid]= docid;
	u64 local_docid = (docid & 0xffffff) / mNumVirtuals;
	int pos = mIndex[vid];
	aos_assert_rl(pos < eMaxDocs, mLock, false);
	mDocids[vid][pos] = local_docid; 
	mGlobalDocids[vid][pos] = docid; 
	int offset = pos * mDocSize;
	aos_assert_rl(offset + mDocSize <= mDocSize * eMaxDocs, mLock, false);
	memcpy(&mRawData[vid][offset], doc, mDocSize);
	mIndex[vid] ++;
	mLock->unlock();
	return true;
}


OmnString
AosFixedLengthDoc::composeEntry(const u64 &docid)
{
	//char data[4];
	//*(u64 *)data = docid; 
	//OmnString str(data, sizeof(u64));
	//str << OmnRandom::lowcaseStr(787, 787);
	//str << OmnRandom::lowcaseStr(mDocSize - sizeof(u64), mDocSize - sizeof(u64));
	//if (mString == "")
	//{
	//	mString = OmnRandom::lowcaseStr(mDocSize - sizeof(u64), mDocSize - sizeof(u64));
	//}
	mString = OmnRandom::lowcaseStr(mDocSize -18, mDocSize -18);
	OmnString ss;
	ss << docid;
	OmnString str;
	str << ss << mString;
	return str;
}



bool
AosFixedLengthDoc::sendRequestPriv(const AosRundataPtr &rdata, const int &vid)
{
	// It sends the buff 'vid' to the backend server. Its format is:
	// 	number of docs			u32
	// 	docids[...]				an array of u64
	// 	offsets[...]			an array of u32
	// 	buff
	aos_assert_rr(mIndex[vid] > 0, rdata, false);
		
	// Create the transaction
	OmnString docstr = "<trans ";
	OmnString type = (mDocSize <= 0) ? AOSDOCSVRPROCNAME_BATCHVARIABLE : AOSDOCSVRPROCNAME_BATCHFIXED;
	docstr << AOSTAG_TYPE		 << "=\"" << type << "\" "
		   << AOSTAG_SIZEID 	 << "=\"" << mCrtSizeid << "\" "
		   << AOSTAG_VIRTUAL_ID	 << "=\"" << vid << "\" "
		   << AOSTAG_FIRST_DOCID << "=\"" << mFirstDocid[vid] << "\" "
		   << AOSTAG_RECORD_SIZE << "=\"" << mDocSize << "\" "
		   << AOSTAG_NUM_DOCS	 << "=\"" << mIndex[vid] << "\">";

	// Append 'docids'
	u32 len = mIndex[vid] * sizeof(u64);
	docstr << "<docids><![BDATA[" << len << ":";
	docstr.append((char *)mDocids[vid], len);
	docstr << "]]></docids>";

	len = mIndex[vid] * mDocSize;
	docstr << "<buffs><![BDATA[" << len << ":";
	docstr.append(mRawData[vid], len);
	docstr << "]]></buffs>";
	docstr << "</trans>";

	// Send the buff
	u64 vvid = vid + AosGetNumCubes() * (rand() % 100 + 1);
	aos_assert_r(vvid > 0, false);

	bool rslt = AosDocClient::getSelf()->addReqPublic(rdata, docstr, vvid, false);
	aos_assert_r(rslt, false);
	OmnScreen << "doc batch send, sizeid: " << mCrtSizeid << ", vid:" << vid << endl;

	memset(mDocids[vid], 0, sizeof(u64) * eMaxDocs);
	memset(mGlobalDocids[vid], 0, sizeof(u64) * eMaxDocs);
	mFirstDocid[vid]= 0;
//OmnSleep(1);
	return true;
}

bool
AosFixedLengthDoc::createData(const AosRundataPtr &rdata, bool finish, int num_docs)
{
	bool rslt = appendEntry(rdata);
	aos_assert_r(rslt, false);

	mLock->lock();
	if (finish)
	{
		for (int vid = 0; vid < mNumVirtuals; vid++)
		{
			bool rslt = sendRequestPriv(rdata, vid);
			aos_assert_rl(rslt, mLock, false);
			mIndex[vid] = 0;
		}

		checkData(rdata, num_docs);
	}
	mLock->unlock();
	return true;
}

bool
AosFixedLengthDoc::checkData(const AosRundataPtr &rdata, int num_docs)
{
	if (!mCheck) return true;
	u64 idx = mStartDocid;
	int nn = 1;
	while (idx < (num_docs + mStartDocid))
	{
if (nn %20 == 0) 
{
	nn = 0;
	OmnScreen << " docid :" << idx << endl;
}
		u64 did = idx;
		AosXmlTagPtr dd = AosDocClient::getSelf()->getDocByDocid(did, rdata);
		aos_assert_r(dd, false);
		OmnString ss = dd->getNodeText("str");
		aos_assert_r(ss != "", false);
		OmnString docidstr(ss.data(), 18);
		u64 id = atoll(docidstr.data());
		aos_assert_r(id == did, false);
		//idx = idx + eMaxDocs;
		idx ++;
nn ++;
	}
	return true;
}
