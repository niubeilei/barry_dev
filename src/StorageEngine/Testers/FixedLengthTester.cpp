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
// Modification History:
// 2013/03/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "StorageEngine/Testers/FixedLengthTester.h"

#include "StorageEngine/Testers/DocInfo3.h"
#include "StorageEngine/Testers/ResultAsyncTester.h"
#include "StorageEngine/Testers/ResultSyncTester.h"
#include "StorageEngine/Testers/Ptrs.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "XmlUtil/SeXmlParser.h"
#include "StorageEngine/Ptrs.h"
#include "StorageEngine/StorageEngineMgr.h"
#include "DocClient/DocClient.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/StrSplit.h"
#include "DocTrans/BatchFixedCleanTrans.h"


AosFixedLengthTester::AosFixedLengthTester(
		const AosXmlTagPtr &record_doc, 
		const int &recordlength,
		const u64 &start_jobid, 
		const AosRundataPtr &rdata)
:
mTries(0),
mSep(":"),
mField1("12345abcde"),
mField2("xyzrsw98765"),
mDocidRangeSize(eDftDocidRangeSize),
mMaxDocidsPerTry(eDftMaxDocidsPerTry),
mWeightAddContents(eDftWeightAddContents),
mWeightCheckContents(eDftWeightCheckContents),
mNumChecksPerTry(eDftNumChecksPerTry),
mRecordLength(recordlength),
mRecordDoc(record_doc),
mRecordDocid(0),
mTotalNumDocs(0),
mCrtJobId(start_jobid),
mSendStart(false),
mSendFinish(false),
mSendCleanData(false),
mStartCheck(false),
mCrtSizeid(0),
mAllTotalNumDocs(0)
{
	config();
	clear();
}

AosFixedLengthTester::~AosFixedLengthTester()
{
}



void
AosFixedLengthTester::clear()
{
	mCrtJobId ++;
	mSendStart = true;
	mSendFinish = true;
	mSendCleanData = true;
	mStartCheck = false;
	mTotalNumDocs = 0;
	mDocInfos.clear();
	mServer.clear();
	int numPhysicals = AosGetNumPhysicals();
	bool rslt;
	AosDocInfo3Ptr info;
	for (int i=0; i<numPhysicals; i++)
	{
		info = OmnNew AosDocInfo3(i, mCrtJobId, eDftServerId);
		aos_assert(info);

		// It is fixed size
		rslt = info->createMemory(eDftBuffLen, mRecordLength);
		aos_assert(rslt);

		mDocInfos.push_back(info);
		mServer.push_back(1);
	}

	for (int j = 0; j < 100000; j++)
	{
		mDocids.push_back(0);
	}
}

bool
AosFixedLengthTester::config()
{
	return true;
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_r(conf, false);
	AosXmlTagPtr tag = conf->getFirstChild("fixed_length_testers");
	if (!tag) return true;

	//mTries = tag->getAttrInt("tries", eDftTries);

	OmnString sep = tag->getAttrStr("separator");
	if (sep != "") mSep = sep;

	OmnString field = tag->getAttrStr("field1");
	if (field != "") mField1 = field;

	field = tag->getAttrStr("field2");
	if (field != "") mField2 = field;

	//int nn = tag->getAttrInt("num_ranges", -1);
	//if (nn > 0) mNumRanges = nn;
	int nn;

	nn = tag->getAttrInt("docid_range_size", -1);
	if (nn > 0) mDocidRangeSize = nn;

	nn = tag->getAttrInt("add_content_weight", -1);
	if (nn > 0) mWeightAddContents = nn;

	nn = tag->getAttrInt("add_check_weight", -1);
	if (nn > 0) mWeightCheckContents = nn;

	nn = tag->getAttrInt("max_docids_per_try", -1);
	if (nn > 0) mMaxDocidsPerTry = nn;

	nn = tag->getAttrInt("num_checks_per_try", -1);
	if (nn > 0) mNumChecksPerTry = nn;

	nn = tag->getAttrInt("record_length", -1);
	if (nn > 0) mRecordLength = nn;

	return true;
}



bool 
AosFixedLengthTester::basicTest(const AosRundataPtr &rdata)
{
	vector<int> weights;
	int total = mWeightAddContents; 	weights.push_back(total); 
	total += mWeightCheckContents; 	 	weights.push_back(total);

	bool rslt = false;
	int opr = pickOperation(weights);
	switch (opr)
	{
		case 0:
			//OmnScreen << mCrtJobId <<" MMMMM: " << mTries++ << ", Add contents  " << endl;
			if (mStartCheck) 
			{
				clear();
			}
			startData(rdata);
			rslt = addContents(rdata);
			break;

		case 1: 
			OmnScreen << mCrtJobId <<" MMMMM: " << mTries++ << ", Check " << mAllTotalNumDocs<< endl;
			rslt = true;
			if (mAllTotalNumDocs)
			{
				finishData(rdata);
				cleanData(rdata);
				while(1)
				{
					if (mStartCheck) 
					{
						break;
					}
				}
				rslt = checkResults(rdata);
			}
			break;

		default:
			OmnAlarm << "Invalid operator: " << opr << enderr;
			rslt = false;
			break;
	}

	if (!rslt)
	{
		OmnAlarm << "Failed operation: " << opr << enderr;
	}
	return true;
}

void
AosFixedLengthTester::finish(const AosRundataPtr &rdata)
{
	if (mTotalNumDocs)
	{
OmnScreen << "+++++++++ finish finish ++++++++" << endl;
		finishData(rdata);
		cleanData(rdata);
	}
	saveDoc(rdata);
}

bool
AosFixedLengthTester::addContents(const AosRundataPtr &rdata)
{
	u32 sizeid = 0;
	int num_ranges = OmnRandom::intByRange(
			1, 1, 100, 
			2, 5, 100, 
			6, 30, 100, 
			31, 200, 30);
	u64 req_num_docids = 0;
	for (int i=0; i<num_ranges; i++)
	{
		// Pick a range
		u32 num_docids = rand() % mMaxDocidsPerTry;

		AosDocids *dd = mDocids[mCrtSizeid];
		aos_assert_r(dd, false);
		u32 delta = dd->num_docids;
		if (sizeid == 0) sizeid = dd->crt_sizeid;
		if (delta == 0 || sizeid != dd->crt_sizeid)
		{
			// No more docids for this range. Do nothing.
OmnScreen << "continue!!!!!!!" << sizeid << " : " << dd->crt_sizeid << endl;
			if (delta == 0)
			{
				getNextBlockDocid(rdata);
			}
			continue;
		}

		if (delta <= num_docids)
		{
			num_docids = delta;
		}

		u64 start_docid = dd->start_docid;
		dd->start_docid =  start_docid + num_docids;
		dd->num_docids -= num_docids;
		dd->updateSaveNumId(num_docids);
		
		addContents(start_docid, num_docids, rdata);
		req_num_docids += num_docids;
	}
OmnScreen << "idx: " << mCrtSizeid << " : " << sizeid << 
" : " << num_ranges << " : " << req_num_docids << " : " << mTotalNumDocs <<  endl;
	mTotalNumDocs += req_num_docids;
	mAllTotalNumDocs += req_num_docids;
	return true;
}


bool
AosFixedLengthTester::addContents(
		const u64 &start_docid, 
		const u32 num_docids,
		const AosRundataPtr &rdata)
{
	OmnString str;
	char buff[mRecordLength];
	int numPhysicals = AosGetNumPhysicals();
	for (u32 i=0; i<num_docids; i++)
	{
		u64 docid = start_docid + i;
		int pid = AosGetPhysicalId(AosXmlDoc::getOwnDocid(docid));
		aos_assert_r(pid >= 0 && pid < numPhysicals, false);
		AosDocInfo3Ptr info = mDocInfos[pid];
		aos_assert_r(info, false);

		str = "";
		str << docid << mSep << mField1 
			<< mSep << mField2 << mSep 
			<< docid*424 << mSep << docid;

		aos_assert_r(str.length() <= mRecordLength, false);
		memset(buff, 0, mRecordLength);
		memcpy(buff, str.data(), str.length());
		info->addDoc(buff, mRecordLength, docid, rdata);
	}
	return true;
}


bool
AosFixedLengthTester::checkResults(const AosRundataPtr &rdata)
{
	bool is_batch_read = true;
	OmnString scanner_id;
	scanner_id << mCrtJobId;
	AosResultAsyncTesterPtr obj = 0;
	AosResultSyncTesterPtr obj1 = 0;
	int ran = rand()%3;
	if (ran == 0 || ran == 1)
	{
		obj = OmnNew AosResultAsyncTester(scanner_id, rdata);
		obj->initDocids(5010, 550);
	}
	else
	{
		obj1 = OmnNew AosResultSyncTester(scanner_id, rdata);
		obj1->initDocids(5010, 550);
	}

	int r = rand() % 15 +1;
	for (int t = 0; t < r; t++)
	{
		int idx = rand()%mSizeid.size();
		u64 sizeid = mSizeid[idx];
		AosDocids *dd = mDocids[sizeid];
		aos_assert_r(dd, false);
		aos_assert_r(sizeid == dd->crt_sizeid, false);

		int save_numid = 0;
		int index = 0;
		while(1)
		{
			index = rand() % (dd->crt_index+1);
			save_numid = dd->getSaveNumid(index);
			if (save_numid == 0 && dd->crt_index == 0) 
			{
				return true;
			}
			if (save_numid > 0) break;
		}

		int offset = rand()%save_numid;
offset = 0;
		u64 save_sdocid = dd->getSaveDocids(index);
		u64 start_docid = save_sdocid + offset;

		int nn = rand() % mNumChecksPerTry;
nn = save_numid;
		if ((u32) nn > save_numid - offset)
		{
			nn = save_numid - offset;
		}
		OmnScreen <<"------Check------"  << nn 
		<< " : " << dd->crt_sizeid << " : " << dd->crt_index 
		<< " : " << index << " : " << save_sdocid 
		<< " : " << save_numid 	<< " : " << start_docid << endl;

		if(is_batch_read) 
		{
			if (obj)
			{
				obj->initDocids(start_docid, nn);
			}
			else
			{
				obj1->initDocids(start_docid, nn);
			}
		}
		else
		{
			for (int i=0; i<nn; i++)
			{
				bool rslt = checkDoc(start_docid + i, rdata);
				aos_assert_r(rslt, false);
			}
		}
	}


	if (is_batch_read)
	{
		OmnScreen << "----------------- start DocReader" << endl;
		if (obj)
		{
			obj->basicTest();
		}
		else
		{
			obj1->basicTest();
		}
	}
	return true;
}



bool
AosFixedLengthTester::checkDoc(const u64 &docid, const AosRundataPtr &rdata)
{
	// This function checks the doc 'docid'. 
	AosXmlTagPtr doc = AosDocClient::getSelf()->getDocByDocid(docid, rdata);
	aos_assert_r(doc, false);
	OmnString str = doc->getNodeText("str");
	aos_assert_r(str != "", false);

	vector<OmnString> vv;
	AosStrSplit::splitStrBySubstr(str.data(), mSep.data(), vv, 100);
	aos_assert_r(vv.size() == 5, false);

	OmnString docidstr;
	docidstr << docid;
	OmnString docidstr1;
	docidstr1 << docid*424;

	aos_assert_r(docidstr == vv[0], false);
	aos_assert_r(mField1 == vv[1], false);
	aos_assert_r(mField2 == vv[2], false);
	aos_assert_r(docidstr1== vv[3], false);
	aos_assert_r(docidstr == vv[4], false);
	return true;
}


bool
AosFixedLengthTester::getNextBlockDocid(const AosRundataPtr &rdata)
{
	u64 docid = 0;
	int num_docids = 0;
	u32 sizeid = 0;
	int size = mDocids.size();
	bool overflow = false;
	aos_assert_r(size >= eStartSizeid, false); 
	if (mCrtSizeid == 0)
	{
		getMoreDocids(docid, num_docids, sizeid, overflow, rdata);
		AosDocids* id = OmnNew AosDocids(docid, num_docids, sizeid);
		mDocids[sizeid] = id;
		aos_assert_r(mDocids[sizeid]->crt_sizeid == sizeid, false);
		mCrtSizeid = sizeid;
		mSizeid.push_back(sizeid);
		return true;
	}
	
	aos_assert_r(size > eStartSizeid, false); 
	sizeid = mDocids[mCrtSizeid]->crt_sizeid;
	getMoreDocids(docid, num_docids, sizeid, overflow, rdata);
	if (mCrtSizeid == sizeid)
	{
		mDocids[mCrtSizeid]->start_docid = docid;
		mDocids[mCrtSizeid]->num_docids = num_docids;
		mDocids[mCrtSizeid]->crt_sizeid = sizeid;
		mDocids[mCrtSizeid]->setSaveDocids(docid);
		aos_assert_r(mCrtSizeid == sizeid, false);
		AosDocids *id = mDocids[mCrtSizeid];
		aos_assert_r(id->num_docids == num_docids, false);
		return true;
	}
	else
	{
		AosDocids* id = OmnNew AosDocids(docid, num_docids, sizeid);
		mDocids[sizeid] = id;
		aos_assert_r(mCrtSizeid < sizeid, false);
		mCrtSizeid = sizeid;
		mSizeid.push_back(sizeid);
	}
	return true;
}



bool
AosFixedLengthTester::getMoreDocids(
		u64 &docid,     
		int &num_docids,
		u32 &crt_sizeid,
		bool &overflow,
		const AosRundataPtr &rdata)
{
	num_docids = 0;
	aos_assert_r(mRecordDoc, false);

	if (crt_sizeid == 0)
	{
		if (mRecordDocid == 0)
		{
			crt_sizeid = AosCreateSizeId(mRecordLength, mRecordDoc, rdata); 
			mRecordDocid = mRecordDoc->getAttrU64(AOSTAG_DOCID, 0); 
		}
		else
		{
			crt_sizeid = AosCreateSizeId(mRecordLength, mRecordDocid, rdata);
		}
		aos_assert_r(crt_sizeid, false);
	}

	overflow = false;
	bool rslt = AosGetDocids(crt_sizeid, docid, num_docids, overflow, rdata);
	aos_assert_r(rslt, false);
	if (overflow)
	{
		// The current size overflows. Need to create a new sizeid. 
		u64 sizeid = AosCreateSizeId(mRecordLength, mRecordDocid, rdata); 
		aos_assert_r(sizeid, false);
		crt_sizeid = sizeid;
		num_docids = 0;
		rslt = AosGetDocids(crt_sizeid, docid, num_docids, overflow, rdata);
		aos_assert_r(rslt, false);
		if(overflow)
		{
			AosGetDocids(crt_sizeid, docid, num_docids, overflow, rdata);
		}
		aos_assert_r(!overflow, false);
	}
//OmnScreen << "==================== sizeid:" << crt_sizeid << " , docid: " << docid << " , num_docids: " << num_docids << endl;
	aos_assert_r(docid > 0, false);
	aos_assert_r(num_docids > 0, false);
	return true;
}


bool
AosFixedLengthTester::startData(const AosRundataPtr &rdata)
{
	if (!mSendStart) return true;
	mSendStart = false;
OmnScreen << " startData startData startData " << mCrtJobId << endl;
	AosDocInfo3Ptr info;
	bool rslt;
	for (u32 i=0; i<mDocInfos.size(); i++)
	{
		info = mDocInfos[i];
		aos_assert_r(info, false);
		rslt = info->sendStart(rdata);      
		aos_assert_r(rslt, false);
	}

	getNextBlockDocid(rdata);
}

bool
AosFixedLengthTester::finishData(const AosRundataPtr &rdata)
{
	if (!mSendFinish) return true;
	mSendFinish = false;
	AosDocInfo3Ptr info;
	bool rslt;
	for (u32 i=0; i<mDocInfos.size(); i++)
	{
		info = mDocInfos[i];
		aos_assert_r(info, false);
		rslt = info->sendFinish(rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}

bool
AosFixedLengthTester::cleanData(const AosRundataPtr &rdata)
{
	if (!mSendCleanData) return true;
	mSendCleanData = false;

OmnScreen << " cleanData cleanData cleanData " << mCrtJobId << endl;
	int numPhysicals = AosGetNumPhysicals();
	for (int i = 0; i<numPhysicals; i++)
	{
		AosTransPtr trans = OmnNew AosBatchFixedCleanTrans(
				i, mCrtJobId , 1, eDftServerId, false, false);

		bool rslt = AosSendTrans(rdata, trans);
		aos_assert_rr(rslt, rdata, false);
	}
	return true;
}


void
AosFixedLengthTester::docFinished()
{
	mStartCheck = true;
}

int
AosFixedLengthTester::pickOperation(const vector<int> &weights)
{
	// 'weights' is an array of weights:
	//      weight[0] = weight 0
	//      weight[1] = weight 0 + weight 1
	//      weight[2] = weight 0 + weight 1 + weight 2
	//      ...
	// This function randomly generates an integer [0, weight[size-1]].
	// It then returns the index of the first element:
	//      number <= weight[index]
	if (weights.size() <= 0) return 0;
	int total = weights[weights.size()-1];
	if (total <= 0) return 0;

	int nn = rand() % total;
	for (u32 i=0; i<weights.size(); i++)
	{
		if (nn <= weights[i]) return i;
	}

	return weights.size()-1;
}

bool
AosFixedLengthTester::saveDoc(const AosRundataPtr &rdata)
{
	OmnString docstr = "<records ";
	docstr << "zky_objid =\"" << "vpd_data" << "\" zky_public_doc=\"true\" "
		    << "zky_public_ctnr=\"true\">";
	for (u32 i = 0; i<mSizeid.size(); i++)
	{
		u32 sizeid = mSizeid[i];
		AosDocids *dd = mDocids[sizeid];
		int crt_index = dd->crt_index;	
		for (int j = 0; j<crt_index; j++)
		{
			u64 start_docid = dd->getSaveDocids(j);
			u64 num_docs = dd->getSaveNumid(j);
			docstr << "<record start_docid=\"" << start_docid << "\" "
				   << " num_docs=\"" << num_docs << "\" "
				   << "sizeid =\"" << sizeid << "\" />";
		}
	}
	docstr << "</records>";
	AosXmlTagPtr dd = AosDocClient::getSelf()->createDoc(docstr, true, rdata);
	aos_assert_r(dd, false);
	OmnScreen << "doc : " << dd->toString() << endl;
	aos_assert_r(dd->getAttrStr(AOSTAG_OBJID, "") == "vpd_data", false);
	AosXmlTagPtr doc = AosDocClient::getSelf()->getDocByObjid("vpd_data", rdata);
	aos_assert_r(doc, false);
	return true;
}

void
AosFixedLengthTester::updateServer(const u32 serverid)
{
	OmnScreen << "======================= serverid :" << serverid << endl;
	mServer[serverid] = 2;
}

bool
AosFixedLengthTester::checkServerFinished()
{
	int numPhysicals = AosGetNumPhysicals();
	for (int i=0; i<numPhysicals; i++)
	{
		if (mServer[i] == 1) return false;
	}
	return true;
}

