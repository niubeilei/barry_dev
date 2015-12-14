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
#include "StorageEngine/Testers2/FixedLengthTester.h"

#include "SEInterfaces/DataAssemblerObj.h"
#include "SEInterfaces/Ptrs.h"
#include "Actions/Ptrs.h"
#include "Actions/ActImportDoc.h"
#include "TaskMgr/TaskData.h" 
#include "TaskMgr/Ptrs.h"
#include "DataAssembler/DocAssembler.h"
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
#include "Util/ValueRslt.h"


AosFixedLengthTester::AosFixedLengthTester(
		const AosXmlTagPtr &record_doc, 
		const int &recordlength,
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
mSaveDocFileMgrFinished(false),
mCreateFileFinished(false),
mNumObjid(0)
{
	clear(rdata);
	aos_assert(mDocAssembler);
}

AosFixedLengthTester::~AosFixedLengthTester()
{
}

bool 
AosFixedLengthTester::basicTest(const AosRundataPtr &rdata)
{
	vector<int> weights;
	int total = mWeightAddContents; 	weights.push_back(total); 
	total += mWeightCheckContents; 	 	weights.push_back(total);

	bool rslt = false;
	int opr = pickOperation(weights);
	opr = 0;
	switch (opr)
	{
		case 0:
			OmnScreen  <<" MMMMM: " << mTries++ << ", Add " << endl;
			rslt = addContents(rdata);
			break;

		case 1: 
			OmnScreen  <<" MMMMM: " << mTries++ << ", Check " << endl;
			rslt = saveAndCheck(rdata);
			break;

		default:
			OmnAlarm << "Invalid operator: " << opr << enderr;
			rslt = false;
			break;
	}
if (mTries % 1000 == 0) 
	rslt = saveAndCheck(rdata);
	if (!rslt)
	{
		OmnAlarm << "Failed operation: " << opr << enderr;
	}
	return true;
}


bool
AosFixedLengthTester::saveAndCheck(const AosRundataPtr &rdata)
{
	saveDoc(rdata);
	mDocAssembler->sendFinish(rdata);

	while(1)
	{
		if (mCreateFileFinished) break;
	}

	saveToFile(rdata);

	while(1)
	{
		if (mSaveDocFileMgrFinished) break;
	}

	int r = rand()%2;
	r = 1;
	if (r == 1)
	{
		bool rslt = checkResults(rdata);
		aos_assert_r(rslt, false);
	}

	mNumObjid ++;
	clear(rdata);
	return true;
}


void
AosFixedLengthTester::clear(const AosRundataPtr &rdata)
{
	mDocAssembler = 0;
	mDocAssembler = AosDataAssembler::createDocAssembler(     
			"vc", 0, mRecordDoc, rdata AosMemoryCheckerArgs);
	for (int serverid = 0; serverid < AosGetNumPhysicals(); serverid ++)
	{
		mTaskDocStr[serverid] = "";
	}
	mSaveDocFileMgrFinished = false;
	mCreateFileFinished = false;
}


bool
AosFixedLengthTester::addContents(const AosRundataPtr &rdata)
{
	u64 crt_docid = 0;
	u32 crt_sizeid = 0;
	int num_docids = 0;
	int num_ranges = OmnRandom::intByRange(
			1, 1, 100, 
			2, 5, 100, 
			6, 30, 100, 
			31, 200, 30);
num_ranges = 2;
	u64 req_num_docids = 0;
	AosDocids *dd;
	for (int i=0; i<num_ranges; i++)
	{
		// Pick a range
		req_num_docids  = rand() % mMaxDocidsPerTry;
		crt_docid = 0;
		crt_sizeid = 0;
		num_docids = 0;
		for ( int k = 0; k < req_num_docids ; k++)
		{
			AosValueRslt valueRslt;
			mDocAssembler->appendEntry(valueRslt, rdata);
			u64 docid = valueRslt.getDocid();
			u32 sizeid = AosGetSizeIdByDocid(docid); 

			if (crt_docid == 0)
			{
				if (mDocids.count(sizeid) == 0)
				{
					dd = OmnNew AosDocids(docid, num_docids, sizeid);
					aos_assert_r(dd, false);
					//mDocids[sizeid] = dd;
					mDocids.insert(make_pair(sizeid, dd));
					mSizeid.push_back(sizeid);
				}
				else
				{
					dd = mDocids[sizeid];
					aos_assert_r(dd, false);
				}
				num_docids ++;
			}
			else
			{
				if (crt_sizeid == sizeid)
				{
					aos_assert_r(dd, false);
					if (docid == crt_docid + 1)
					{
						num_docids ++;
					}
					else
					{
						// save pre num_docids
						dd->updateSaveNumId(num_docids);

						//save next start docid
						dd->setSaveDocids(docid);
						dd->updateSaveNumId(0);
					}
				}
				else
				{
					// save pre sizeid num docids
					dd->updateSaveNumId(num_docids);

					if ( mDocids.count(sizeid) == 0)
					{
						dd = OmnNew AosDocids(docid, num_docids, sizeid);
						//mDocids[sizeid] = dd;
						mDocids.insert(make_pair(sizeid, dd));
						mSizeid.push_back(sizeid);
					}
					aos_assert_r(dd, false);
					//else
					//{
					//	dd = mDocids[sizeid];
					//}
				}
			}
			crt_docid = docid;
			crt_sizeid = sizeid;
		}
	OmnScreen << "i : " << i << " , crt_sizeid: " << crt_sizeid << " ,  num_docids: " << num_docids << endl;
		aos_assert_r(dd, false);
		dd->updateSaveNumId(num_docids);
	}
	return true;
}


bool
AosFixedLengthTester::checkResults(const AosRundataPtr &rdata)
{
	int r = rand() % 15 +1;
	r = 2;
	for (int t = 0; t < r; t++)
	{
		int idx = rand()%mSizeid.size();
//idx = mSizeid.size() -1;
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

		int offset = rand()%(save_numid -1);
//offset = 0;
		u64 save_sdocid = dd->getSaveDocids(index);
		u64 start_docid = save_sdocid + offset;

		int nn = rand() % mNumChecksPerTry;
//nn = save_numid;
nn = 2000;
		if ((u32) nn > save_numid - offset)
		{
			nn = save_numid - offset;
		}
		OmnScreen <<"------Check------"  << nn 
		<< " : " << dd->crt_sizeid << " : " << dd->crt_index 
		<< " : " << index << " : " << save_sdocid 
		<< " : " << save_numid 	<< " : " << start_docid << endl;

		for (int i=0; i<nn; i++)
		{
			bool rslt = checkDoc(start_docid + i, rdata);
			if (i % 100 == 0)
			{
				OmnScreen << "docid : " << start_docid + i <<endl;
			}
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
	if (str == "")
	{
		OmnAlarm << "xxxxxxxx" << enderr;
		AosXmlTagPtr dd = AosDocClient::getSelf()->getDocByDocid(docid, rdata);
		OmnScreen << "docid : " << docid << endl;
	}

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
	for (int serverid = 0; serverid < AosGetNumPhysicals(); serverid ++)
	{
		int data_serverid = AosGetSelfServerId();
		OmnString objid = "vpd_data";
		objid << mNumObjid << "_"<< serverid << "_" << data_serverid;
		OmnString docstr = "<records ";
		docstr << "zky_objid =\"" << objid << "\" zky_public_doc=\"true\" "
			<< "id=\"" << serverid << "\" "
			 << "zky_public_ctnr=\"true\">"
			 << "<files>" << mTaskDocStr[serverid] << "</files>"
			 << "</records>";
		AosXmlTagPtr dd = AosDocClient::getSelf()->createDoc(docstr, true, rdata);
		aos_assert_r(dd, false);

		aos_assert_r(dd->getAttrStr(AOSTAG_OBJID, "") == objid, false);
		AosXmlTagPtr doc = AosDocClient::getSelf()->getDocByObjid(objid, rdata);
		aos_assert_r(doc, false);
		OmnScreen << "doc : " << doc->toString() << endl;
	}
	return true;
}


void
AosFixedLengthTester::saveToFile(const AosRundataPtr &rdata)
{
//	OmnString records = "<records> <files>";
//	records << mTaskDocStr;
//	records << "</files></records>";

//	AosXmlParser parser;
//	AosXmlTagPtr root = parser.parse(records, "" AosMemoryCheckerArgs);
//	AosXmlTagPtr sdoc = root->getFirstChild();
//	aos_assert(sdoc);
	for (int data_serverid = 0; data_serverid < AosGetNumPhysicals(); data_serverid ++)
	{
		int self_serverid = AosGetSelfServerId();
		OmnString objid = "vpd_data";
		objid << mNumObjid <<"_" << self_serverid << "_" << data_serverid;
	
		AosXmlTagPtr sdoc = AosDocClient::getSelf()->getDocByObjid(objid, rdata);
		aos_assert(sdoc);

		AosActImportDocPtr ipdoc = OmnNew AosActImportDoc(false);
		aos_assert(ipdoc);
		AosTaskDataObjPtr pp;
		aos_assert(ipdoc->run(0, sdoc, pp, rdata));
	}
}

void
AosFixedLengthTester::updateOutPut(
		const int serverid,
		const OmnString &key,
		const u32 sizeid,
		const int docsize,
		const u64 &file_id)
{
	mTaskDocStr[serverid] << "<" << AOSTAG_TASKDATA_TAGNAME << " "
			<< AOSTAG_PHYSICALID << "=\"" << AosGetSelfServerId() << "\" " 
			<< AOSTAG_SERVERID << "=\"" << serverid << "\" " 
			<< AOSTAG_SIZEID << "=\"" << sizeid << "\" "
			<< AOSTAG_RECORD_LEN << "=\"" << docsize << "\" "
			<< AOSTAG_KEY << "=\"" << "vc" << "\" "
			<< AOSTAG_STORAGE_FILEID << "=\"" << file_id << "\" "
			<< "/>";
}
