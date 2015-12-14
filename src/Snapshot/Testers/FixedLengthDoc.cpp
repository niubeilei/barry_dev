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
#include "Snapshot/Testers/FixedLengthDoc.h"

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
#include "DocTrans/BatchFixedTrans.h"
#include "StorageEngine/SengineDocInfoFixed.h"

AosFixedLengthDoc::AosFixedLengthDoc(
		const AosXmlTagPtr &def,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
:
mLock(OmnNew OmnMutex()),
mCrtSizeid(0),
mDocSize(0),
mDocid(0),
mNumDocids(0),
mId(1),
mStartDocid(0),
mEndDocid(0),
mTaskDocid(task_docid)
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
	
	mDocSize = mRecordDoc->getAttrU64(AOSTAG_LENGTH, 0);
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
	docid = getNextDocid(rdata);
	aos_assert_r(docid, false);

	if (mStartDocid == 0) mStartDocid = docid;
	mEndDocid = docid;

	OmnString str = composeEntry(docid);
	const char *doc = str.data(); 
	
	mLock->lock();
	int vid = AosGetCubeId(AosXmlDoc::getOwnDocid(docid));
	aos_assert_rl(vid >= 0 && vid < mNumVirtuals, mLock, false);
	if (mIndex[vid] >= eMaxDocs)
	{
		bool rslt = sendRequestPriv(rdata, vid);
		aos_assert_rl(rslt, mLock, false);
		mIndex[vid] = 0;
	}

	if (mFirstDocid[vid] == 0) mFirstDocid[vid]= docid;

	u64 local_docid = AosGetLocalIdByDocid(docid) / mNumVirtuals;
//OmnScreen << "===== local_docid: " << local_docid << ";" << endl;
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
	if(mIndex[vid] <= 0) return true; 
	//aos_assert_rr(mIndex[vid] > 0, rdata, false);
		
	/*
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
	*/
	u32 len = mIndex[vid] * sizeof(u64);
	AosBuffPtr docid_buff = OmnNew AosBuff(len AosMemoryCheckerArgs); 
	docid_buff->setBuff((char *)mDocids[vid], len);

	aos_assert_r(mIndex[vid] > 0 && docid_buff->dataLen() / sizeof(u64) == (u32)mIndex[vid], false);

	len = mIndex[vid] * mDocSize;
	AosBuffPtr buff = OmnNew AosBuff(len AosMemoryCheckerArgs); 
	buff->setBuff(mRawData[vid], len);

//u64 *docids = (u64 *)docid_buff->data();
//for (int i = 0; i< mIndex[vid]; i++)
//{
//	OmnScreen << "localid:" << docids[i] << ";" << endl;
//}



	aos_assert_r(mSnapIds.count(vid) != 0 , false);
	u64 snap_id = mSnapIds[vid];
	AosTransPtr trans = OmnNew AosBatchFixedTrans(
			        0, vid, mCrtSizeid, mIndex[vid],
					mDocSize, docid_buff, buff, 
					snap_id, mTaskDocid);
	AosSendTransAsyncResp(rdata, trans);

	memset(mDocids[vid], 0, sizeof(u64) * eMaxDocs);
	memset(mGlobalDocids[vid], 0, sizeof(u64) * eMaxDocs);
	mFirstDocid[vid]= 0;
	return true;
}

bool
AosFixedLengthDoc::createData(const AosRundataPtr &rdata)
{
	bool rslt = appendEntry(rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosFixedLengthDoc::sendRequest(const AosRundataPtr &rdata)
{
	mLock->lock();
	for (int vid = 0; vid < mNumVirtuals; vid++)
	{
		bool rslt = sendRequestPriv(rdata, vid);
		aos_assert_rl(rslt, mLock, false);
		mIndex[vid] = 0;
	}
	mLock->unlock();
	return true;
}


bool
AosFixedLengthDoc::checkData(
		const AosRundataPtr &rdata, 
		const u64 &start_docid, 
		const u64 &end_docid)
{
	u64 idx = start_docid;
	while (idx <= end_docid)
	{
		u64 did = idx;
		AosXmlTagPtr dd = AosDocClient::getSelf()->getDocByDocid(did, rdata);
		aos_assert_r(dd, false);
		OmnString ss = dd->getNodeText("str");
		aos_assert_r(ss != "", false);
		OmnString docidstr(ss.data(), 18);
		u64 id = atoll(docidstr.data());
		aos_assert_r(id == did, false);
		idx ++;
	}
	return true;
}
