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
// This dataset is defined by a query. When used, it uses the query
// to retrieve the data. A record scanner is used to loop through the
// results. 
//
// Modification History:
// 2014/05/07 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "Dataset/Jimos/DatasetByQueryLeftJoin.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DataRecord/RecordJoin.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDatasetByQueryLeftJoin_0(
 		const AosRundataPtr &rdata,
		const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDatasetByQueryLeftJoin(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosDatasetByQueryLeftJoin::AosDatasetByQueryLeftJoin(const int version)
:
AosDatasetByQuery(AOSDATASET_QUERY_INNER_JOIN, version)
{
}


AosDatasetByQueryLeftJoin::~AosDatasetByQueryLeftJoin()
{
}


AosJimoPtr
AosDatasetByQueryLeftJoin::cloneJimo() const
{
	try
	{
		return OmnNew AosDatasetByQueryLeftJoin(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool
AosDatasetByQueryLeftJoin::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc)
{
	aos_assert_r(worker_doc, false);

	bool rslt = AosDatasetByQuery::config(rdata, worker_doc);
	aos_assert_r(rslt, false);

	rslt = checkIsIndexQuery(rdata, worker_doc);
	aos_assert_r(rslt, false);

	mJoinFieldLeft = worker_doc->getAttrStr("join_field_left");
	aos_assert_r(mJoinFieldLeft != "", false);
	
	mJoinFieldRight = worker_doc->getAttrStr("join_field_right");
	aos_assert_r(mJoinFieldRight != "", false);

	mJoinIILName = worker_doc->getAttrStr("join_iilname");
	aos_assert_r(mJoinIILName != "", false);

	AosXmlTagPtr table1 = worker_doc->getFirstChild("table1");
	aos_assert_r(table1, false);

	mScanDataset1 = AosCreateDataset(rdata.getPtr(), table1);
	aos_assert_r(mScanDataset1, false);
	rslt = mScanDataset1->config(rdata, table1);
	aos_assert_r(rslt, false);

	AosXmlTagPtr table2 = worker_doc->getFirstChild("table2");
	aos_assert_r(table2, false);

	AosDatasetObjPtr scanDataset2 = AosCreateDataset(rdata.getPtr(), table2);
	aos_assert_r(scanDataset2, false);
	rslt = scanDataset2->config(rdata, table2);
	aos_assert_r(rslt, false);

	mScanDataset2Cfg = table2->clone(AosMemoryCheckerArgsBegin);

	return true;
}


bool
AosDatasetByQueryLeftJoin::checkIsIndexQuery(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	mIsIndexQuery = false;
	return true;
}


bool
AosDatasetByQueryLeftJoin::sendStart(const AosRundataPtr &rdata)
{
	mQueryStartTime = OmnGetTimestamp();

	aos_assert_r(mScanDataset1, false);
	bool rslt = mScanDataset1->sendStart(rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosDatasetByQueryLeftJoin::sendFinish(const AosRundataPtr &rdata)
{
	return true;
}

	
bool
AosDatasetByQueryLeftJoin::nextRecordset(
		const AosRundataPtr &rdata, 
		AosRecordsetObjPtr &recordset)
{
	recordset = 0;

	if (mPageSize <= 0)
	{
		mQueryFinishTime = OmnGetTimestamp();
		return true;
	}

	if (mDataReaded) 
	{
		mQueryFinishTime = OmnGetTimestamp();
		return true;
	}

	aos_assert_r(mDataRecord, false);
	aos_assert_r(mScanDataset1, false);

	bool rslt = false;
	AosDataRecordObj * rcd1;
	AosDataRecordObj * rcd2;
	AosDataRecordObjPtr record1, record2;
	AosRecordsetObjPtr rcd_set1, rcd_set2;
	AosDatasetObjPtr scan_dataset2;
	OmnString key, left_key, right_key;
	AosValueRslt value_rslt, value_rslt_left, value_rslt_right;
	bool outofmem = false;
	u32	size = mFieldDef.size();
	bool need_next_rcd2 = true;
	vector<AosDataRecordObjPtr> records;

	AosRecordJoin * join_record = OmnNew AosRecordJoin(AosRecordJoin::eOutJoin AosMemoryCheckerArgs);

	mDataRecord->clear();

	while (1)
	{
		rslt = mScanDataset1->nextRecordset(rdata, rcd_set1);
		aos_assert_r(rslt, false);

		if (AosRecordsetObj::checkEmpty(rcd_set1)) break;
	
		vector<OmnString> keys;
		while (1)
		{
			rslt = rcd_set1->nextRecord(rdata.getPtrNoLock(), rcd1);
			aos_assert_r(rslt, false);

			if (!rcd1) break;

			rslt = rcd1->getFieldValue(mJoinFieldLeft, value_rslt, false, rdata.getPtrNoLock());
			aos_assert_r(rslt, false);

			key = value_rslt.getStr();
			key.normalizeWhiteSpace(true, true);

			keys.push_back(key);
		}

		if (keys.empty()) break;

		rcd_set1->resetReadIdx();

		AosBuffPtr buff;
		rslt = AosGetKeyedStrValues(mJoinIILName, keys, true, 0, buff, rdata);
		aos_assert_r(rslt && buff, false);

		scan_dataset2 = AosCreateDataset(rdata.getPtr(), mScanDataset2Cfg);
		aos_assert_r(scan_dataset2, false);
		rslt = scan_dataset2->config(rdata, mScanDataset2Cfg);
		aos_assert_r(rslt, false);

		buff->reset();
		rslt = scan_dataset2->setValueBuff(buff, rdata);
		aos_assert_r(rslt, false);
		rslt = scan_dataset2->sendStart(rdata);
		aos_assert_r(rslt, false);
		rslt = scan_dataset2->nextRecordset(rdata, rcd_set2);
		aos_assert_r(rslt, false);

		need_next_rcd2 = true;

		while (1)
		{
			rslt = rcd_set1->nextRecord(rdata.getPtrNoLock(), rcd1);
			aos_assert_r(rslt, false);

			if (need_next_rcd2)
			{
				rslt = rcd_set2->nextRecord(rdata.getPtrNoLock(), rcd2);
				aos_assert_r(rslt, false);
			}

			if (!rcd1 || !rcd2)
			{
				aos_assert_r(!rcd1 && !rcd2, false);
				break;
			}

			rslt = rcd1->getFieldValue(mJoinFieldLeft, value_rslt_left, false, rdata.getPtrNoLock());
			aos_assert_r(rslt, false);

			rslt = rcd2->getFieldValue(mJoinFieldRight, value_rslt_right, false, rdata.getPtrNoLock());
			aos_assert_r(rslt, false);

			left_key = value_rslt_left.getStr();
			left_key.normalizeWhiteSpace(true, true);

			right_key = value_rslt_right.getStr();
			right_key.normalizeWhiteSpace(true, true);

			records.clear();			
			if (left_key != right_key)
			{
				need_next_rcd2 = false;				
				record1 = rcd1;
				records.push_back(record1);
			}
			else
			{
				need_next_rcd2 = true;
				record1 = rcd1;
				record2 = rcd2;
				records.push_back(record1);
				records.push_back(record2);
			}

			rslt = join_record->setRecords(records, rdata.getPtrNoLock());
			aos_assert_r(rslt, false);

			if (mWhereCond)
			{
				AosDataRecordObjPtr record = join_record;
				rslt = mWhereCond->evalCond(record, rdata);
				if (!rslt) continue;
			}

			mTotalValues++;
			if (mNeedIgnoreNum > 0)
			{
				mNeedIgnoreNum--;
				continue;
			}
			if (mNumValues >= mPageSize && mGetTotal)
			{
				continue;
			}

			for (u32 i=0; i<size; i++)
			{
				rslt = mFieldDef[i].mOExpr->getValue(rdata.getPtrNoLock(), dynamic_cast<AosDataRecordObj *>(join_record), value_rslt);
				if (rslt)
				{
					rslt = mDataRecord->setFieldValue(mFieldDef[i].mCidx, value_rslt, outofmem, rdata.getPtrNoLock());
					aos_assert_r(rslt, false);
					aos_assert_r(!outofmem, false);
				}
			}
			mDataRecord->flush(true);

			mNumValues++;

			if (mNumValues >= mPageSize && !mGetTotal)
			{
				break;
			}
		}
	}

	recordset = AosRecordsetObj::createRecordsetStatic(rdata.getPtrNoLock(), mDataRecord);

	mQueryFinishTime = OmnGetTimestamp();

	return true;
}
	

bool
AosDatasetByQueryLeftJoin::getRsltInfor(
		AosXmlTagPtr &infor,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mScanDataset1, false);
	bool rslt = true;
	if (mIsIndexQuery)
	{
		rslt = mScanDataset1->getRsltInfor(infor, rdata);
	}
	else
	{
		OmnString str = "<Contents";
		str << " total=\"" << mTotalValues << "\""
			<< " start_idx=\"" << mStartIdx << "\""
			<< " num=\"" << mNumValues << "\""
			<< "/>";

		infor = AosXmlParser::parse(str AosMemoryCheckerArgs);
		aos_assert_r(infor, false);
	}
	aos_assert_r(rslt, false);
	aos_assert_r(infor, false);

	OmnString time_str;
	time_str << (mQueryFinishTime - mQueryStartTime) / 1000.0 << "ms";
	infor->setAttr("time", time_str);

	return true;
}

