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
// 09/02/2013 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "Snapshot/Testers/DfmFixedDocTester.h"

#include "API/AosApi.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DocClientObj.h"
#include "Snapshot/Testers/FixedLengthDoc.h"

AosDfmFixedDocTester::AosDfmFixedDocTester()
{
}

AosDfmFixedDocTester::AosDfmFixedDocTester(
		const AosRundataPtr &rdata)
:
AosDfmTester("fixed"),
mLogicid(0),
mTaskDocid(0),
mRdata(rdata),
mTotalNumDocs(0),
mNumDocs(0),
mBatchAddNum(0),
mRecordLen(0)
{
	mTaskDocid = rand() % 39999 +1;
	mVirtualIds = AosGetTotalCubeIds();
	AosXmlTagPtr conf = config();
	mFixedObj = OmnNew AosFixedLengthDoc(conf, mTaskDocid, rdata);
}



AosDfmFixedDocTester::AosDfmFixedDocTester(
		const u32 logicid,
		const AosRundataPtr &rdata)
:
AosDfmTester("fixed"),
mLogicid(logicid),
mTaskDocid(0),
mRdata(rdata),
mTotalNumDocs(0),
mNumDocs(0),
mBatchAddNum(0),
mRecordLen(0)
{
	mTaskDocid = rand() % 39999 +1;
	mVirtualIds = AosGetTotalCubeIds();
	mRecordLen = rand() %1991 + 9;
	AosXmlTagPtr conf = config();
	mFixedObj = OmnNew AosFixedLengthDoc(conf, mTaskDocid, rdata);
}

AosDfmFixedDocTester::~AosDfmFixedDocTester()
{
}



bool
AosDfmFixedDocTester::createSnapshot()
{
	mSnapIds.clear();
	mFixedObj->resetDocid();
	mNumDocs = 0;
	mBatchAddNum ++;
	Status st;        
	st.start = 0;
	st.end = 0;
	st.sts = eInvalid;
	mSts.insert(make_pair(mBatchAddNum, st));

	OmnString ss;
	u64 snap_id = 0;
	for (u32 i = 0; i < mVirtualIds.size(); i++)
	{
		snap_id = AosDocClientObj::getDocClient()->createSnapshot(mRdata, mVirtualIds[i], AosDocType::eGroupedDoc, mTaskDocid);
		mSnapIds.insert(make_pair(mVirtualIds[i], snap_id));
		ss << ";virtual_id:" << mVirtualIds[i] << ";snap_id:" << snap_id << ";";
	}
	mFixedObj->setSnapIds(mSnapIds);
	OmnScreen << "BBBBBBBBBBBBBBBB logic:" << mLogicid << ";create snap:" << ss << endl;
}


bool
AosDfmFixedDocTester::rollbackSnapshot()
{
	aos_assert_r(mSts.count(mBatchAddNum) != 0, false);
	mSts[mBatchAddNum].start = mFixedObj->getStartDocid();
	mSts[mBatchAddNum].end = mFixedObj->getEndDocid();
	mSts[mBatchAddNum].sts = eRollBack;
	u64 snap_id = 0;
	map<int, u64>::iterator itr = mSnapIds.begin();
	OmnString ss;
	for (; itr != mSnapIds.end(); ++itr)
	{
		snap_id = itr->second;
		AosDocClientObj::getDocClient()->rollbackSnapshot(mRdata, itr->first, AosDocType::eGroupedDoc, snap_id, mTaskDocid); 
		ss << ";virtual_id:" << itr->first << ";snap_id:" << snap_id << ";";
	}
	OmnScreen << "BBBBBBBBBBBBBBBB logic:" << mLogicid << ";rollback snap:" << ss << endl;
}


bool
AosDfmFixedDocTester::commitSnapShot()
{
	aos_assert_r(mSts.count(mBatchAddNum) != 0, false);
	mSts[mBatchAddNum].start = mFixedObj->getStartDocid();
	mSts[mBatchAddNum].end = mFixedObj->getEndDocid();
	mSts[mBatchAddNum].sts = eCommit;

	u64 snap_id = 0;
	map<int, u64>::iterator itr = mSnapIds.begin();
	OmnString ss;
	for (; itr != mSnapIds.end(); ++itr)
	{
		snap_id = itr->second;
		AosDocClientObj::getDocClient()->commitSnapshot(mRdata, itr->first, AosDocType::eGroupedDoc, snap_id, mTaskDocid); 
		ss << ";virtual_id:" << itr->first << ";snap_id:" << snap_id << ";";
	}
	mTotalNumDocs += mNumDocs; 
	OmnScreen << "BBBBBBBBBBBBBBBB logic:" << mLogicid << ";commit snap:" << ss << endl;
}


bool
AosDfmFixedDocTester::createDocs()
{
	vector<u64> values;
	int r = rand() %100 +1;
	for (int i = 0; i < r; i++)
	{
		mFixedObj->createData(mRdata);
		mNumDocs ++;
	}
	mFixedObj->sendRequest(mRdata);
	return true;
}


bool
AosDfmFixedDocTester::basicTest()
{
	createSnapshot();
	int r = rand() % 100 +1;
	OmnScreen << "tries: " << r << endl;
	for (int i = 0; i < r; i++)
	{
		createDocs();
	}

	r = rand() %4;
	if (r)
	{
		commitSnapShot();
	}
	else
	{
		rollbackSnapshot();
	}
	OmnSleep(10);
	verify();
}


AosXmlTagPtr
AosDfmFixedDocTester::config()
{
	//int record_len = rand() %1991 + 9;
	OmnScreen << " record_len :" << mRecordLen << endl;
	OmnString docstr="<record type=\"fixbin\" ";
	docstr << " zky_name=\"doc\" "<< AOSTAG_LENGTH << "=\" " << mRecordLen << "\" " 
		<< " zky_objid=\"unicom_ac_config_" << mRecordLen << "\" "
		<< " zky_pctrs=\"sdoc_unicom_doc\" zky_public_doc=\"true\" zky_public_ctnr=\"true\">"
		<< "<datafields>"
		<< " <datafield type=\"str\" zky_name=\"str\" zky_rawformat=\"str\" "
		<< " zky_length=\"" << mRecordLen << "\" "
		<< " zky_offset=\"0\"/>"
		<< "</datafields>"
		<< " </record> ";

	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(docstr, "" AosMemoryCheckerArgs);
	//AosXmlTagPtr dd = AosCreateDoc(xml, mRdata);
	//aos_assert_r(dd, false);
	return xml;
}


bool
AosDfmFixedDocTester::verify()
{
	//map<int, Status>::iterator itr = mSts.begin();
	//for (; itr != mSts.end(); ++itr)
	map<int, Status>::iterator itr = mSts.find(mBatchAddNum);
	if (itr->second.sts == eCommit)
	{
		mFixedObj->checkData(mRdata, itr->second.start, itr->second.end);
	}
	else
	{
		for (u64 i = itr->second.start; i<= itr->second.end; i++)
		{
			AosXmlTagPtr dd = AosDocClientObj::getDocClient()->getDocByDocid(i, mRdata);
			aos_assert_r(dd, false);
			OmnString ss = dd->getNodeText("str");
			aos_assert_r(ss == "", false);
		}
	}
	return true;
}


bool
AosDfmFixedDocTester::serializeTo(const AosBuffPtr &buff)
{
	buff->setU32(mLogicid);
	buff->setU64(mRecordLen);
	buff->setU64(mTotalNumDocs);
	buff->setInt(mBatchAddNum);

	map<int, Status>::iterator itr = mSts.begin();
	for (; itr != mSts.end(); ++itr)
	{
		buff->setU64(itr->first);
		buff->setU64(itr->second.start);
		buff->setU64(itr->second.end);
		buff->setInt(itr->second.sts);
	}
	return true;
}


bool
AosDfmFixedDocTester::serializeFrom(const AosBuffPtr &buff)
{
	mLogicid = buff->getU32(0);
	mRecordLen = buff->getU64(0);
	mTotalNumDocs = buff->getU64(0);
	mBatchAddNum = buff->getInt(0);
	for (u64  i = 1; i<= mBatchAddNum; i++)
	{
		u64 kk = buff->getU64(0); 
		aos_assert_r(kk == i, false);
		Status st;        
		st.start = buff->getU64(0);
		st.end = buff->getU64(0);
		st.sts = (AosDfmFixedDocTester::E)buff->getInt(0);
		mSts.insert(make_pair(kk, st));
	}

	return true;
}
