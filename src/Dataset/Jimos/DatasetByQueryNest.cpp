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
#include "Dataset/Jimos/DatasetByQueryNest.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDatasetByQueryNest_0(
 		const AosRundataPtr &rdata,
		const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDatasetByQueryNest(version);
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


AosDatasetByQueryNest::AosDatasetByQueryNest(const int version)
:
AosDatasetByQuery(AOSDATASET_QUERY_NEST, version)
{
}


AosDatasetByQueryNest::~AosDatasetByQueryNest()
{
}


AosJimoPtr
AosDatasetByQueryNest::cloneJimo() const
{
	try
	{
		return OmnNew AosDatasetByQueryNest(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool
AosDatasetByQueryNest::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc)
{
	aos_assert_r(worker_doc, false);

	bool rslt = AosDatasetByQuery::config(rdata, worker_doc);
	aos_assert_r(rslt, false);

	rslt = checkIsIndexQuery(rdata, worker_doc);
	aos_assert_r(rslt, false);
	
	// jimodb-671 Phil
#if 0
	AosXmlTagPtr table = worker_doc->getFirstChild("table");
	aos_assert_r(table, false);

	mScanDataset = AosCreateDataset(rdata.getPtr(), table);
#endif
	mScanDataset = mInputDataset;
	aos_assert_r(mScanDataset, false);
	
	// jimodb-1373
	mScanDataset->setPageSize(mPageSize); 
#if 0
	rslt = mScanDataset->config(rdata, table);
	aos_assert_r(rslt, false);
#endif

	return true;
}
	

bool
AosDatasetByQueryNest::checkIsIndexQuery(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	mIsIndexQuery = false;

	if (!mIsIndexQuery)
	{
		def->setAttr("start_idx", "0");
		// jimodb-1372
		if (def->getAttrStr("psize") == AOSTAG_NOT_A_NUMBER)
		{
			def->setAttr("psize", "10000000");
		}
		//def->setAttr("psize", "10000000");
	}

	bool rslt = AosDatasetByQuery::initUnIndexQuery(rdata, def);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosDatasetByQueryNest::sendStart(const AosRundataPtr &rdata)
{
	mQueryStartTime = OmnGetTimestamp();
#if 0
	aos_assert_r(mScanDataset, false);
	bool rslt = mScanDataset->sendStart(rdata);
	aos_assert_r(rslt, false);
#endif

	return true;
}


bool
AosDatasetByQueryNest::sendFinish(const AosRundataPtr &rdata)
{
	return true;
}

	
bool
AosDatasetByQueryNest::nextRecordset(
		const AosRundataPtr &rdata, 
		AosRecordsetObjPtr &recordset)
{
	recordset = 0;

	if (mPageSize <= 0)
	{
		mQueryFinishTime = OmnGetTimestamp();
		return true;
	}

	aos_assert_r(mDataRecord, false);
	aos_assert_r(mScanDataset, false);

	aos_assert_r(!mIsIndexQuery, false);

	bool rslt = true;
	if (mUsingFieldValues)
	{
		rslt = nextRecordsetUsingFieldValues(rdata);
	}
	else
	{
		rslt = nextRecordsetNoIndex(rdata);
	}
	aos_assert_r(rslt, false);

	recordset = AosRecordsetObj::createRecordsetStatic(rdata.getPtrNoLock(), mDataRecord);

	mQueryFinishTime = OmnGetTimestamp();

	return true;
}


bool
AosDatasetByQueryNest::nextRecordsetNoIndex(const AosRundataPtr &rdata)
{
	aos_assert_r(mDataRecord, false);
	aos_assert_r(mScanDataset, false);

	AosDataRecordObj * rcd;
	AosRecordsetObjPtr rcd_set;
	AosValueRslt value_rslt;
	bool outofmem = false;
	bool rslt = true;
	bool flag = false;

	mNumValues = 0;
	mTotalValues = 0;
	u32 size = mFieldDef.size();
	mDataRecord->clear();

	while (1)
	{
		rslt = mScanDataset->nextRecordset(rdata, rcd_set);
		aos_assert_r(rslt, false);

		if (AosRecordsetObj::checkEmpty(rcd_set)) break;
	
		while (1)
		{
			rslt = rcd_set->nextRecord(rdata.getPtrNoLock(), rcd);
			aos_assert_r(rslt, false);

			if (!rcd) break;

			if (mWhereCond)
			{
				AosDataRecordObjPtr record = rcd;
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
				// jimodb-1372
				flag = 1;
				break;
			}
		}
		if (flag)
			break;
	}

	return true;
}


bool
AosDatasetByQueryNest::nextRecordsetUsingFieldValues(const AosRundataPtr &rdata)
{
	if (mDataReaded) 
	{
		return true;
	}

	aos_assert_r(mDataRecord, false);
	aos_assert_r(mScanDataset, false);

	AosDataRecordObj * rcd;
	AosRecordsetObjPtr rcd_set;
	AosValueRslt value_rslt;
	bool outofmem = false;
	bool rslt = true;
	bool flag =false;

	mNumValues = 1;
	mTotalValues = 1;
	u32 size = mFieldNameExprs.size();
	mDataRecord->clear();

	while (1)
	{
		rslt = mScanDataset->nextRecordset(rdata, rcd_set);
		aos_assert_r(rslt, false);

		if (AosRecordsetObj::checkEmpty(rcd_set)) break;
	
		while (1)
		{
			rslt = rcd_set->nextRecord(rdata.getPtrNoLock(), rcd);
			aos_assert_r(rslt, false);

			if (!rcd) break;

			if (mWhereCond)
			{
				AosDataRecordObjPtr record = rcd;
				rslt = mWhereCond->evalCond(record, rdata);
				if (!rslt) continue;
			}

			vector<AosValueRslt> v; 
			for (u32 i=0; i<size; i++)
			{
				if (mFieldNameExprs[i])
				{
					rslt = mFieldNameExprs[i]->getValue(rdata.getPtrNoLock(), rcd, value_rslt);
					aos_assert_r(rslt, false);
				}
				else
				{
					aos_assert_r(i < mFieldDef.size(), false);
					aos_assert_r(mFieldDef[i].mOname == "*", false);

					value_rslt.setI64(mTotalValues);
				}
				v.push_back(value_rslt);
			}
			mFieldValues.push_back(v);

			// jimodb-1372
			mNumValues ++;
			if (mNumValues > mPageSize) 
			{
				flag = 1;
				break;
			}
		}
		if(flag)
			break;
	}

	rslt = groupby(rdata);
	aos_assert_r(rslt, false);

	rslt = orderby(rdata);
	aos_assert_r(rslt, false);

	mNumValues = 0;
	mTotalValues = mFieldValues.size();
	size = mFieldDef.size();
	mDataRecord->clear();

	for (u32 i=mStartIdx; i<mTotalValues; i++)
	{
		for (u32 j=0; j<size; j++)
		{
			rslt = mDataRecord->setFieldValue(mFieldDef[j].mCidx, mFieldValues[i][j], outofmem, rdata.getPtrNoLock());
			aos_assert_r(rslt, false);
			aos_assert_r(!outofmem, false);
		}
		mDataRecord->flush(true);
		mNumValues++;
	}

	mDataReaded = true;
	return true;
}


bool
AosDatasetByQueryNest::getRsltInfor(
		AosXmlTagPtr &infor,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mScanDataset, false);
	bool rslt = true;
	if (mIsIndexQuery)
	{
		rslt = mScanDataset->getRsltInfor(infor, rdata);
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

