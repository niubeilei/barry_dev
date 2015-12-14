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
// 2014/09/01 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "Dataset/Jimos/DatasetByQueryUnion.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDatasetByQueryUnion_0(
 		const AosRundataPtr &rdata,
		const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDatasetByQueryUnion(version);
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


AosDatasetByQueryUnion::AosDatasetByQueryUnion(const int version)
:
AosDatasetByQuery(AOSDATASET_QUERY_NEST, version),
mNeedSet(false)
{
}


AosDatasetByQueryUnion::~AosDatasetByQueryUnion()
{
}


AosJimoPtr
AosDatasetByQueryUnion::cloneJimo() const
{
	try
	{
		return OmnNew AosDatasetByQueryUnion(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool
AosDatasetByQueryUnion::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc)
{
	aos_assert_r(worker_doc, false);

	bool rslt = AosDatasetByQuery::config(rdata, worker_doc);
	aos_assert_r(rslt, false);

	rslt = checkIsIndexQuery(rdata, worker_doc);
	aos_assert_r(rslt, false);

	mScanDatasets.clear();
	mUnionAll.clear();

	AosXmlTagPtr tables = worker_doc->getFirstChild("tables");
	aos_assert_r(tables, false);
	
	bool union_all = true;
	AosDatasetObjPtr dataset;
	AosXmlTagPtr table = tables->getFirstChild("table");
	while (table)
	{
		dataset = AosCreateDataset(rdata.getPtr(), table);
		aos_assert_r(dataset, false);
		rslt = dataset->config(rdata, table);
		aos_assert_r(rslt, false);
			
		union_all = table->getAttrBool("union_all", true);

		mScanDatasets.push_back(dataset);
		mUnionAll.push_back(union_all);

		if (!union_all) mNeedSet = true;

		table = tables->getNextChild("table");
	}

	aos_assert_r(mScanDatasets.size() > 0, false);

	return true;
}
	

bool
AosDatasetByQueryUnion::checkIsIndexQuery(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	mIsIndexQuery = false;

	bool rslt = AosDatasetByQuery::initUnIndexQuery(rdata, def);
	aos_assert_r(rslt, false);

	if (!mIsIndexQuery)
	{
		def->setAttr("start_idx", "0");
		def->setAttr("psize", "10000000");
	}

	return true;
}


bool
AosDatasetByQueryUnion::sendStart(const AosRundataPtr &rdata)
{
	mQueryStartTime = OmnGetTimestamp();

	aos_assert_r(mScanDatasets.size() > 0, false);

	bool rslt = true;
	for (u32 i=0; i<mScanDatasets.size(); i++)
	{
		rslt = mScanDatasets[i]->sendStart(rdata);
		aos_assert_r(rslt, false);
	}

	return true;
}


bool
AosDatasetByQueryUnion::sendFinish(const AosRundataPtr &rdata)
{
	return true;
}

	
bool
AosDatasetByQueryUnion::nextRecordset(
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
	aos_assert_r(mScanDatasets.size() > 0, false);

	AosDataRecordObj * rcd;
	AosRecordsetObjPtr rcd_set;
	AosValueRslt value_rslt;
	bool outofmem = false;
	bool rslt = true;

	mNumValues = 0;
	mTotalValues = 0;
	u32 size = mFieldDef.size();
	mDataRecord->clear();
	u32 idx = 0;
	AosDatasetObjPtr dataset = mScanDatasets[idx];
	bool union_all;
	mUnionSet.clear();

	while (1)
	{
		rslt = dataset->nextRecordset(rdata, rcd_set);
		aos_assert_r(rslt, false);

		if (AosRecordsetObj::checkEmpty(rcd_set))
		{
			idx++;
			if (idx < mScanDatasets.size())
			{
				dataset = mScanDatasets[idx];
				union_all = mUnionAll[idx];
				continue;
			}
			break;
		}
	
		while (1)
		{
			rslt = rcd_set->nextRecord(rdata.getPtrNoLock(), rcd);
			aos_assert_r(rslt, false);

			if (!rcd) break;

			if (mNeedSet)
			{
				OmnString ss(rcd->getData(rdata.getPtrNoLock()), rcd->getMemoryLen());
				aos_assert_r(ss != "", false);

				if (!union_all && mUnionSet.count(ss) > 0)
				{
					continue;
				}
					
				mUnionSet.insert(ss);
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
				rslt = mFieldDef[i].mOExpr->getValue(rdata.getPtrNoLock(), rcd, value_rslt);
				aos_assert_r(rslt, false);
					
				rslt = mDataRecord->setFieldValue(mFieldDef[i].mCidx, value_rslt, outofmem, rdata.getPtrNoLock());
				aos_assert_r(rslt, false);
				aos_assert_r(!outofmem, false);
			}
			mDataRecord->flush(true);

			mNumValues++;

			if (mNumValues >= mPageSize && !mGetTotal)
			{
				break;
			}
		}
	}

	mDataReaded = true;

	recordset = AosRecordsetObj::createRecordsetStatic(rdata.getPtrNoLock(), mDataRecord);

	mQueryFinishTime = OmnGetTimestamp();

	return true;
}


bool
AosDatasetByQueryUnion::getRsltInfor(
		AosXmlTagPtr &infor,
		const AosRundataPtr &rdata)
{
	OmnString str = "<Contents";
	str << " total=\"" << mTotalValues << "\""
		<< " start_idx=\"" << mStartIdx << "\""
		<< " num=\"" << mNumValues << "\""
		<< "/>";

	infor = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(infor, false);

	OmnString time_str;
	time_str << (mQueryFinishTime - mQueryStartTime) / 1000.0 << "ms";
	infor->setAttr("time", time_str);

	return true;
}

