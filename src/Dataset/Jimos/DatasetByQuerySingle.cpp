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
// 2014/04/29 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "Dataset/Jimos/DatasetByQuerySingle.h"

#include "Debug/Debug.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDatasetByQuerySingle_0(
 		const AosRundataPtr &rdata,
		const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDatasetByQuerySingle(version);
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


AosDatasetByQuerySingle::AosDatasetByQuerySingle(const int version)
:
AosDatasetByQuery(AOSDATASET_QUERY_SINGLE, version),
mSetScanData(false),
mIsCountAll(false),
mPickDocidsOnly(false)
{
}

AosDatasetByQuerySingle::~AosDatasetByQuerySingle()
{
}


AosJimoPtr
AosDatasetByQuerySingle::cloneJimo() const
{
	try
	{
		return OmnNew AosDatasetByQuerySingle(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool
AosDatasetByQuerySingle::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc)
{
	aos_assert_r(worker_doc, false);

	bool rslt = AosDatasetByQuery::config(rdata, worker_doc);
	aos_assert_r(rslt, false);

	rslt = checkIsIndexQuery(rdata, worker_doc);
	aos_assert_r(rslt, false);

	mQueryReq = AosQueryReqObj::createQueryStaticNew(worker_doc, rdata);
	aos_assert_r(mQueryReq, false);

	mScanDataset = NULL;
	if (mInputDataset)                         
	{
		//input is a dataset from subquery
		mScanDataset = mInputDataset;
	}
	else
	{
		AosXmlTagPtr inputdataset = worker_doc->getFirstChild("inputdataset");
		aos_assert_r(inputdataset, false);

		mScanDataset = AosCreateDataset(rdata.getPtr(), inputdataset);
		aos_assert_r(mScanDataset, false);
		rslt = mScanDataset->config(rdata, inputdataset);
		aos_assert_r(rslt, false);
	}

	mTableObjid = worker_doc->getAttrStr(AOSTAG_NAME);
	aos_assert_r(mTableObjid != "", false);

	mSetScanData = false;
	return true;
}


bool
AosDatasetByQuerySingle::checkIsIndexQuery(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	mIsIndexQuery = true;

	if (def->getAttrStr("psize") == AOSTAG_NOT_A_NUMBER)
	{
		mIsIndexQuery = false;
		def->setAttr("psize", "10000000");
	}

	// Ketty 2014/05/14
	AosXmlTagPtr groupby_conf = def->getFirstChild("groupby");
	if (groupby_conf)
	{
		OmnString grpby_tp = groupby_conf->getAttrStr("type");
		if (grpby_tp == "stat" || grpby_tp == "norm")
		{
			mIsIndexQuery = false;
		}
	}

	AosXmlTagPtr orderby_conf = def->getFirstChild("orderby");
	if (orderby_conf) mIsIndexQuery = false;

	if (mWhereCond) mIsIndexQuery = false;

	bool rslt = AosDatasetByQuery::initUnIndexQuery(rdata, def);
	aos_assert_r(rslt, false);

	if (!mIsIndexQuery)
	{
		def->setAttr("start_idx", "0");
		def->setAttr("psize", "10000000");

		if (mGroupByField.empty()
			&& !mGroupByAggregate.empty()
			&& !mWhereCond)
		{
			bool has_query_filter = false;
			AosXmlTagPtr cond = def->xpathGetFirstChild("conds/cond");
			aos_assert_r(cond, false);
			AosXmlTagPtr term = cond->getFirstChild();
			while (term)
			{
				AosXmlTagPtr query_filters = term->getFirstChild("query_filters");
				if (query_filters && query_filters->getNumSubtags() > 0)
				{
					has_query_filter = true;
				}
				term = cond->getNextChild();
			}

			size_t isCountAll_count = 0;
			for (size_t i=0; i<mGroupByAggregate.size(); i++)
			{
				if (mGroupByAggregate[i].mType == "count_all"
					&& mGroupByAggregate[i].mDistinct == false)
				{
					isCountAll_count++;
				}
			}
			if (!has_query_filter && isCountAll_count == mGroupByAggregate.size())
			{
				mIsCountAll = true;
				def->setAttr("psize", "10");
			}
		}
	}

	return true;
}


bool
AosDatasetByQuerySingle::sendStart(const AosRundataPtr &rdata)
{
	// This function starts the query processing. It is a sync
	// call. When the function returns, the query processing
	// should have finished.
	mQueryStartTime = OmnGetTimestamp();

	if (mPageSize <= 0)
	{
		return true;
	}

	aos_assert_r(mQueryReq, false);
	bool rslt = mQueryReq->runQuery(rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosDatasetByQuerySingle::sendFinish(const AosRundataPtr &rdata)
{
	return true;
}


bool
AosDatasetByQuerySingle::generateScanData(const AosRundataPtr &rdata)
{
	// This function assumes picking docid phase is finished. The
	// docids are in 'mQueryReq'. This function uses 'mScanDataset'
	// to retrieve docs from cubes. mScanDataset retrieves N records
	// each time.
	//
	// It is possible to use different dataset to replace 'mScanDataset'
	// to interrupt this normal query process.

	// Chen Ding, 2015/01/28
	mLock->lock();
	if (mPickDocidsOnly)
	{
		bool rslt = gatherDocids(rdata);
		mLock->unlock();
		return rslt;
	}

	if (mSetScanData)
	{
		mLock->unlock();
		return true;
	}

	aos_assert_rl(mQueryReq, mLock, false);
	aos_assert_rl(mScanDataset, mLock, false);

	u64* docids = 0;
	int64_t num = 0, total_num = 0, start_ix = 0;
	bool rslt = true;
	AosQueryRsltObjPtr query_rslt;
	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);

	int64_t total = mQueryReq->getTotal();
	mTotalValues = total;
	if (mIsCountAll)
	{
		if (total > 0)
		{
			rslt = mQueryReq->generateRslt(query_rslt, rdata);
			aos_assert_rl(rslt && query_rslt, mLock, false);

			num = query_rslt->getNumDocs();
			aos_assert_rl(num > 0, mLock, false);

			docids = query_rslt->getDocidsPtr();
			aos_assert_rl(docids, mLock, false);

			buff->setU64(docids[0]);
		}
	}
	else
	{
set<u64> DocidsSet;
		while (1)
		{
			rslt = mQueryReq->generateRslt(query_rslt, rdata);
			aos_assert_rl(rslt && query_rslt, mLock, false);

			num = query_rslt->getNumDocs();
			if (num <= 0)
			{
				break;
			}

			docids = query_rslt->getDocidsPtr();
			aos_assert_rl(docids, mLock, false);

//			rslt = buff->setU64s(docids, num);
//			aos_assert_r(rslt, false);
int ss = 10;
//filter same docid
for(int64_t i=0; i<num; i++)
{
	u64 id = docids[i];
	if (DocidsSet.count(id) == 0)
	{
		DocidsSet.insert(id);
		buff->setU64(id);
	}
	else
	{
		if (ss > 0)
		{
			ss--;
			OmnAlarm << "docid exist:" << id << enderr;
		}
		continue;
	}
}

			total_num += num;
			if (!mIsIndexQuery && total_num < total)
			{
				start_ix = mQueryReq->getStartIdx();
				mQueryReq->setStartIdx(start_ix + num);

				rslt = mQueryReq->runQuery(rdata);
				aos_assert_rl(rslt, mLock, false);
			}
			else
			{
				break;
			}
		}
	}

	if (buff->dataLen() > 0)
	{
		buff->reset();
		rslt = mScanDataset->setValueBuff(buff, rdata);
		aos_assert_rl(rslt, mLock, false);
	}

	rslt = mScanDataset->sendStart(rdata);
	aos_assert_rl(rslt, mLock, false);

	mSetScanData = true;
	mLock->unlock();

	return true;
}


bool
AosDatasetByQuerySingle::nextRecordset(
		const AosRundataPtr &rdata,
		AosRecordsetObjPtr &recordset)
{
	// The queried results are stored in mScanDataset. This function
	// retrieves one recordset.
	recordset = 0;

	if (mPageSize <= 0)
	{
		mQueryFinishTime = OmnGetTimestamp();
		return true;
	}

	rdata->setArg1(AOSTAG_TABLEOBJID, mTableObjid);

	aos_assert_r(mDataRecord, false);
	bool rslt = generateScanData(rdata);
	aos_assert_r(rslt, false);

	AosDataRecordObjPtr record = mDataRecord->clone(rdata.getPtrNoLock() AosMemoryCheckerArgs);

	if (mIsIndexQuery)
	{
		rslt = nextRecordsetIndex(record, rdata);
	}
	else if (mUsingFieldValues)
	{
		rslt = nextRecordsetUsingFieldValues(record, rdata);
	}
	else
	{
		rslt = nextRecordsetNoIndex(record, rdata);
	}

	aos_assert_r(rslt, false);

	recordset = AosRecordsetObj::createRecordsetStatic(
		rdata.getPtrNoLock(), record);
	mQueryFinishTime = OmnGetTimestamp();

	return true;
}


bool
AosDatasetByQuerySingle::nextRecordsetIndex(
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	aos_assert_r(record, false);
	aos_assert_r(mScanDataset, false);

	record->clear();

	AosRecordsetObjPtr rcd_set;
	bool rslt = mScanDataset->nextRecordset(rdata, rcd_set);
	aos_assert_r(rslt, false);

	if (AosRecordsetObj::checkEmpty(rcd_set)) return true;

	AosDataRecordObj * rcd;
	AosValueRslt value_rslt;
	bool outofmem = false;

	mNumValues = 0;
	u32 size = mFieldDef.size();

	while (1)
	{
		rslt = rcd_set->nextRecord(rdata.getPtrNoLock(), rcd);
		aos_assert_r(rslt, false);

		if (!rcd) break;

		for (u32 i=0; i<size; i++)
		{
			rslt = mFieldDef[i].mOExpr->getValue(
				rdata.getPtrNoLock(), rcd, value_rslt);
			aos_assert_r(rslt, false);

			rslt = record->setFieldValue(mFieldDef[i].mCidx,
				value_rslt, outofmem, rdata.getPtrNoLock());
			aos_assert_r(rslt, false);
			aos_assert_r(!outofmem, false);
		}
		record->flush(true);

		mNumValues++;
	}

	return true;
}


bool
AosDatasetByQuerySingle::nextRecordsetNoIndex(
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	aos_assert_r(record, false);
	aos_assert_r(mScanDataset, false);

	AosDataRecordObj * rcd;
	AosRecordsetObjPtr rcd_set;
	AosValueRslt value_rslt;
	bool outofmem = false;
	bool rslt = true;

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
				rslt = mFieldDef[i].mOExpr->getValue(
					rdata.getPtrNoLock(), rcd, value_rslt);
				aos_assert_r(rslt, false);

				rslt = record->setFieldValue(mFieldDef[i].mCidx,
					value_rslt, outofmem, rdata.getPtrNoLock());
				aos_assert_r(rslt, false);
				aos_assert_r(!outofmem, false);
			}
			record->flush(true);

			mNumValues++;

			if (mNumValues >= mPageSize && !mGetTotal)
			{
				break;
			}
		}

		if (mNumValues >= mPageSize && !mGetTotal)
		{
			break;
		}
	}

	return true;
}


bool
AosDatasetByQuerySingle::nextRecordsetUsingFieldValues(
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	if (mDataReaded)
	{
		return true;
	}

	aos_assert_r(record, false);
	aos_assert_r(mScanDataset, false);

	record->clear();
	bool outofmem = false;
	bool rslt = true;

	AosValueRslt value_rslt;

	if (mTotalValues == 0)
	{
		if (!mGroupByField.empty() || mStartIdx > 0)
		{
			mDataReaded = true;
			return true;
		}

		if (mGroupByAggregate.size() == 0)
		{
			mNumValues = 0;
			mDataReaded = true;
			record->clear();
			return true;
		}

		vector<AosValueRslt> vv;
		value_rslt.setNull();
		for (size_t i=0; i<mFieldNameExprs.size(); i++)
		{
			vv.push_back(value_rslt);
		}

		for (size_t i=0; i<mGroupByAggregate.size(); i++)
		{
			OmnString type = mGroupByAggregate[i].mType;
			u32 idx = mGroupByAggregate[i].mIdx;

			value_rslt.setNull();
			if (type == "count_all" || type == "count")
			{
				value_rslt.setI64((i64)0);
			}
			vv[idx] = value_rslt;
		}
		mFieldValues.push_back(vv);

		mNumValues = 1;
		mTotalValues = 1;
		record->clear();

		for (size_t i=0; i<mFieldDef.size(); i++)
		{
			rslt = record->setFieldValue(mFieldDef[i].mCidx,
				vv[i], outofmem, rdata.getPtrNoLock());
			aos_assert_r(rslt, false);
			aos_assert_r(!outofmem, false);
		}
		record->flush(true);

		mDataReaded = true;
		return true;
	}

	AosDataRecordObj * rcd;
	AosRecordsetObjPtr rcd_set;

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
			u32 size = mFieldNameExprs.size();
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
		}
	}

	if (!mIsCountAll)
	{
		rslt = groupby(rdata);
		aos_assert_r(rslt, false);

		rslt = orderby(rdata);
		aos_assert_r(rslt, false);
	}

	mNumValues = 0;
	mTotalValues = mFieldValues.size();
	u32 size = mFieldDef.size();
	record->clear();

	for (u32 i=mStartIdx; i<mTotalValues; i++)
	{
		for (u32 j=0; j<size; j++)
		{
			rslt = record->setFieldValue(mFieldDef[j].mCidx,
				mFieldValues[i][j], outofmem, rdata.getPtrNoLock());
			aos_assert_r(rslt, false);
			aos_assert_r(!outofmem, false);
		}
		record->flush(true);

		mNumValues++;

		if (mNumValues >= mPageSize)
		{
			break;
		}
	}

	mDataReaded = true;
	return true;
}


bool
AosDatasetByQuerySingle::getRsltInfor(
		AosXmlTagPtr &infor,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mQueryReq, false);
	bool rslt = true;
	if (mIsIndexQuery)
	{
		rslt = mQueryReq->getRsltInfor(infor, rdata);
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

