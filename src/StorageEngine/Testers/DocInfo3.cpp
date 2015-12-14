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
// 2012/04/04 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "StorageEngine/Testers/DocInfo3.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "API/AosApiG.h"
#include "DocTrans/BatchFixedStartTrans.h"
#include "DocTrans/BatchFixedFinishTrans.h"
#include "DocTrans/BatchFixedTrans.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"


AosDocInfo3::AosDocInfo3(const u32 serverid, const u64 &job_docid, const u64 &job_serverid)
:
mLock(OmnNew OmnMutex()),
mRawData(0),
mDocids(0),
mDocSize(-1),
mNumDocs(0),
mMaxDocs(0),
mServerId(serverid),
mLastSizeid(0),
mTotalNumDocs(0),
mJobServerId(job_serverid),
mJobDocid(job_docid)
{
}


AosDocInfo3::~AosDocInfo3()
{
	OmnDelete [] mDocids;
	OmnDelete [] mRawData; 
	mDocids = 0;
	mRawData = 0;
	//OmnScreen << "delete docinfo:" << mServerId << endl;
}
		

bool
AosDocInfo3::createMemory(
		const u64 &buff_len,
		const int doc_size)
{
	aos_assert_r(buff_len > 0, false);
	aos_assert_r(doc_size > 0, false);
	
	mLock->lock();
	mDocSize = doc_size;
	mMaxDocs = buff_len / doc_size;
	
	if(mRawData) OmnDelete [] mRawData;
	mRawData = OmnNew char[buff_len + 10]; 
	memset(mRawData, 0, buff_len + 10);
	
	if(mDocids) OmnDelete [] mDocids;
	mDocids = OmnNew u64[mMaxDocs];
	memset(mDocids, 0, sizeof(u64) * mMaxDocs);
	
	mLock->unlock();
	return true;
}

	
bool 
AosDocInfo3::addDoc(
		const char *doc, 
		const int len, 
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	// Docs are buff-ed based on virtual_id. Docs belonging to the same
	// virtual ID are saved in the same buff.
	aos_assert_r(len == mDocSize, false);
	aos_assert_r(docid, false);

	u64 sizeid = AosGetSizeIdByDocid(docid);
	aos_assert_r(sizeid, false);

	mLock->lock();
	if (mNumDocs > 0 && (mNumDocs >= mMaxDocs || mLastSizeid != sizeid))
	{
		// Need to send the transaction to DocServer.
		bool rslt = sendPriv(rdata);
		aos_assert_rl(rslt, mLock, false);
	}

	mLastSizeid = sizeid;

	mDocids[mNumDocs] = docid;
	memcpy(&mRawData[mNumDocs* mDocSize], doc, mDocSize);
	mNumDocs++;
	mTotalNumDocs++;

	mLock->unlock();
	return true;
}

	
bool
AosDocInfo3::sendPriv(const AosRundataPtr &rdata)
{
	// It sends the buff 'vid' to the backend server. Its format is:
	// 	number of docs			u32
	// 	docids[...]				an array of u64
	// 	offsets[...]			an array of u32
	// 	buff
	aos_assert_r(mNumDocs > 0, false);
		
	/*
	// Create the transaction
	OmnString docstr = "<trans ";
	docstr << AOSTAG_TYPE << "=\"" << AOSDOCSVRPROCNAME_BATCHFIXED << "\" "
		   << AOSTAG_JOB_DOCID << "=\"" << mJobDocid << "\" "
		   << AOSTAG_NUM_DOCS << "=\"" << mNumDocs << "\" "
		   << AOSTAG_RECORD_SIZE << "=\"" << mDocSize << "\" "
		   << AOSTAG_SIZEID << "=\"" << mLastSizeid << "\" "
		   << AOSTAG_BUFF_SIZE << "=\"" << (mDocSize * mMaxDocs + 10) << "\">";

	// Append 'docids'
	u32 len = mNumDocs * sizeof(u64);
	docstr << "<docids><![BDATA[" << len << ":";
	docstr.append((char *)mDocids, len);
	docstr << "]]></docids>";

	len = mNumDocs * mDocSize;
	docstr << "<buffs><![BDATA[" << len << ":";
	docstr.append(mRawData, len);
	docstr << "]]></buffs>";
	docstr << "</trans>";

	AosDocClient::getSelf()->addReqToServer(rdata, docstr, mServerId, false);
*/

	u32 docid_len = mNumDocs * sizeof(u64);
	AosBuffPtr docid_buff = OmnNew AosBuff(docid_len AosMemoryCheckerArgs);
	docid_buff->setBuff((char *)mDocids, docid_len);

	u32 raw_len = mNumDocs * mDocSize;
	AosBuffPtr raw_data = OmnNew AosBuff(raw_len AosMemoryCheckerArgs);
	raw_data->setBuff(mRawData, raw_len);

	AosTransPtr trans = OmnNew AosBatchFixedTrans(
		mServerId, mJobDocid, 1, mNumDocs, 
		mDocSize, mLastSizeid, docid_buff, raw_data, false, false);
	AosSendTrans(rdata, trans);	

	mNumDocs = 0;
	memset(mDocids, 0, sizeof(u64) * mMaxDocs);
	memset(mRawData, 0, mDocSize * mMaxDocs);
	return true;
}

	
bool
AosDocInfo3::sendStart(const AosRundataPtr &rdata)
{
	mLock->lock();
/*	
	OmnString docstr = "<trans ";
	docstr << AOSTAG_TYPE << "=\"" << AOSDOCSVRPROCNAME_BATCHFIXEDSTART << "\" "
		   << AOSTAG_JOB_DOCID << "=\"" << mJobDocid << "\" "
		   << AOSTAG_JOBSERVERID << "=\"" << mJobServerId << "\" "
		   << AOSTAG_RECORD_SIZE << "=\"" << mDocSize << "\"/>";
	
	AosXmlTagPtr resp;
	bool rslt = AosDocClient::getSelf()->addReqToServer(rdata, docstr, resp, mServerId, false);
	aos_assert_rl(rslt && resp, mLock, false);
	
	rslt = resp->getAttrBool("rslt");
	aos_assert_rl(rslt, mLock, false);

	*/
	AosTransPtr trans = OmnNew AosBatchFixedStartTrans(
		mServerId,  mJobDocid, 1,
		mJobServerId, false, true);

	AosBuffPtr resp;
	bool timeout;
	bool rslt = AosSendTrans(rdata, trans, timeout, resp);	
	aos_assert_rl(rslt && !timeout && resp, mLock, false);	
	
	rslt = resp->getU8(0);
	aos_assert_rl(rslt, mLock, false);

	OmnScreen << "send docinfo start, jobdocid:" << mJobDocid
		<< ", serverid:" << mServerId
		<< ", size:" << mDocSize << endl;
	mLock->unlock();
	return true;
}



bool
AosDocInfo3::sendFinish(const AosRundataPtr &rdata)
{
	// Call this function after finishing the assembling. It 
	// sends all the buffs. 
	mLock->lock();
	if (mNumDocs > 0)
	{
		bool rslt = sendPriv(rdata);
		aos_assert_rl(rslt, mLock, false);
	}
/*	
	OmnString docstr = "<trans ";
	docstr << AOSTAG_TYPE << "=\"" << AOSDOCSVRPROCNAME_BATCHFIXEDFINISH << "\" "
		   << AOSTAG_JOB_DOCID << "=\"" << mJobDocid << "\" "
		   << "total_num_docs=\"" << mTotalNumDocs << "\" "
		   << AOSTAG_RECORD_SIZE << "=\"" << mDocSize << "\"/>";

	AosDocClient::getSelf()->addReqToServer(rdata, docstr, mServerId, false);
	
	*/

	AosTransPtr trans = OmnNew AosBatchFixedFinishTrans(
		mServerId, mJobDocid, 1,
		mTotalNumDocs, false, false);
	AosSendTrans(rdata, trans);	
	OmnScreen << "send docinfo finish, jobdocid:" << mJobDocid
		<< ", serverid" << mServerId
		<< ", num_docs:" << mTotalNumDocs
		<< ", size:" << mDocSize << endl;
	
	mLock->unlock();
	return true;
}

