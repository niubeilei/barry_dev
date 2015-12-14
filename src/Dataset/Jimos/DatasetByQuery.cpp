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
#include "Dataset/Jimos/DatasetByQuery.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "JQLExpr/ExprFieldName.h"
#include "JQLParser/JQLParser.h"
#include "Rundata/Rundata.h"
#include "SEUtil/JqlUtil.h"
#include "XmlUtil/XmlTag.h"


extern int AosParseJQL(char *data, const bool flag);
extern AosJQLParser gAosJQLParser;


AosDatasetByQuery::AosDatasetByQuery(
		const OmnString &type,
		const int version)
:
AosDataset(type, version),
mLock(OmnNew OmnMutex()),
mIsIndexQuery(true),
mGetTotal(false),
mStartIdx(0),
mPageSize(eDftPsize),
mTotalValues(0),
mNumValues(0),
mNeedIgnoreNum(0),
mGroupBy(false),
mGroupByNeedSort(false),
mOrderBy(false),
mUsingFieldValues(false),
mDataReaded(false)
{
	mInputDataset = NULL;
}


AosDatasetByQuery::~AosDatasetByQuery()
{
}


bool
AosDatasetByQuery::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	aos_assert_r(def, false);

	AosXmlTagPtr record_def = def->getFirstChild("outputrecord");
	aos_assert_r(record_def, false);

	mDataRecord = AosDataRecordObj::createDataRecordStatic(record_def, 0, rdata.getPtrNoLock() AosMemoryCheckerArgs);
	aos_assert_r(mDataRecord, false);

	AosXmlTagPtr fnames = def->getFirstChild("fnames");
	aos_assert_r(fnames, false);

	OmnString tmp_name, errormsg;
	AosXmlTagPtr fname = fnames->getFirstChild();
	while (fname)
	{
		fieldDef fd;
		fd.mOname = fname->getNodeText("oname");

		if (fd.mOname != "*")
		{
			tmp_name = fd.mOname;
			tmp_name << ";";
			fd.mOExpr = AosParseExpr(tmp_name, errormsg, (rdata.getPtr())); 
			if (!fd.mOExpr)
			{
				OmnAlarm << errormsg << enderr;
				return false;
			}
		}

		fd.mCname = fname->getNodeText("cname");
		aos_assert_r(fd.mCname != "", false);

		fd.mCidx = mDataRecord->getFieldIdx(fd.mCname, rdata.getPtrNoLock());
		aos_assert_r(fd.mCidx >= 0, false);

		mFieldDef.push_back(fd);

		if (fd.mOExpr)
		{
			mFieldNameExprs.push_back(fd.mOExpr->cloneExpr());
		}
		else
		{
			mFieldNameExprs.push_back(0);
		}

		fname = fnames->getNextChild();
	}
	aos_assert_r(!mFieldDef.empty(), false);

	mWhereCond = 0;
	AosXmlTagPtr where_def = def->getFirstChild("where");
	if (where_def)
	{
		AosCondType::E type = AosCondType::toEnum(where_def->getAttrStr(AOSTAG_ZKY_TYPE));
		if (AosCondType::isValid(type))
		{
			mWhereCond = AosConditionObj::getConditionStatic(where_def, rdata);
			aos_assert_r(mWhereCond, false);
		}
	}

	return true;
}


bool
AosDatasetByQuery::initUnIndexQuery(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	aos_assert_r(def, false);

	mGetTotal = def->getAttrBool("get_total", false);
	mStartIdx = def->getAttrInt64("start_idx", 0);
	mPageSize = def->getAttrInt64("psize", eDftPsize);
	mNeedIgnoreNum = mStartIdx;

	if (mIsIndexQuery) return true;
	AosXmlTagPtr group_tag = def->getFirstChild("groupby");
	bool rslt = parseGroupBy(rdata, group_tag);
	aos_assert_r(rslt, false);

	AosXmlTagPtr order_tag = def->getFirstChild("orderby");
	rslt = parseOrderBy(rdata, order_tag);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosDatasetByQuery::parseGroupBy(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	mGroupByField.clear();
	if (!def) return true;

	OmnString type = def->getAttrStr("type");
	if (type != "norm") return true;

	OmnString tmp_name, errormsg;
	AosExprObjPtr expr;

	u32 size = mFieldDef.size();
	for (u32 i=0; i<size; i++)
	{
		AosJqlUtil::unescape(mFieldDef[i].mOname);
		//bool aggrOpr = false;
		//if(strncmp(mFieldDef[i].mOname.data(), "aggr_concat", 11) == 0) aggrOpr = true;
		//mFieldDef[i].mOname = AosJqlUtil::stripFunc(mFieldDef[i].mOname);
		//tmp_name = AosJqlUtil::stripFunc(mFieldDef[i].mOname);
		/*if(aggrOpr)
		{
			int idx = mFieldDef[i].mOname.find(',', false);
			aos_assert_r(idx != -1, false);
			int len = mFieldDef[i].mOname.length();
			mFieldDef[i].mOname = OmnString(mFieldDef[i].mOname.data(), idx);
			mConcatChar = OmnString(mFieldDef[i].mOname.data() + idx + 2, len - idx - 3); //take out the quotes
		}*/

		tmp_name = mFieldDef[i].mOname;
		tmp_name << ";";
		expr = AosParseExpr(tmp_name, errormsg, (rdata.getPtr())); 
		if (!expr)
		{
			OmnAlarm << errormsg << enderr;
			return false;
		}

		if (AosExpr::checkIsAgrFunc(expr))
		{
			tmp_name = AosJqlUtil::stripFunc(mFieldDef[i].mOname);
			mFieldDef[i].mOname = tmp_name;
			if (tmp_name == "*")
			{
				expr = 0;
			}
			else
			{
				tmp_name << ";";
				expr = AosParseExpr(tmp_name, errormsg, (rdata.getPtr())); 
				if (!expr)
				{
					OmnAlarm << errormsg << enderr;
					return false;
				}
			}
		}

		mFieldDef[i].mOExpr = expr;
		if (mFieldDef[i].mOExpr)
		{
			mFieldNameExprs[i] = mFieldDef[i].mOExpr->cloneExpr();
		}
		else
		{
			mFieldNameExprs[i] = 0;
		}
	}	

	AosXmlTagPtr fields = def->getFirstChild("fields");
	aos_assert_r(fields, false);

	AosXmlTagPtr field = fields->getFirstChild(true);
	while (field)
	{
		tmp_name = field->getNodeText();
		tmp_name << ";";
		expr = AosParseExpr(tmp_name, errormsg, rdata.getPtr()); 
		if (!expr)
		{
			OmnAlarm << errormsg << enderr;
			return false;
		}

	//	aos_assert_r(expr, false);
		mFieldNameExprs.push_back(expr);

		mGroupByField.push_back(make_pair(mFieldNameExprs.size()-1, false));
		field = fields->getNextChild();
	}

	mGroupByNeedSort = def->getAttrBool("need_sort", false);
	if (mGroupByField.empty())
	{
		aos_assert_r(!mGroupByNeedSort, false);
	}

	mGroupBy = true;
	mUsingFieldValues = true;
	mFieldValues.clear();

	AosXmlTagPtr aggregates = def->getFirstChild(AOSTAG_AGGREGATIONS);
	if (!aggregates) return true;

	OmnString oname, cname;
	bool distinct = false;
	bool found = false;
	AosXmlTagPtr aggregate = aggregates->getFirstChild(true);
	while (aggregate)
	{
		type = aggregate->getAttrStr("type");
		oname = aggregate->getNodeText("oname");
		cname = aggregate->getNodeText("cname");
		aos_assert_r(oname != "" && cname != "", false);
		distinct = aggregate->getAttrBool("distinct");

		found = false;
		for (u32 i=0; i<size; i++)
		{
			if (oname == mFieldDef[i].mOname &&
				cname == mFieldDef[i].mCname)
			{
				if (oname == "*" && type == "count")
				{
					type = "count_all";
				}

				found = true;
				groupByAggregate gba;
				gba.mType = type;
				gba.mIdx = i;
				gba.mDistinct = distinct;

				mGroupByAggregate.push_back(gba);
				break;
			}
		}

		if (!found)
		{
			OmnAlarm << "failed to find aggr field" << aggregate->toString() << enderr;
		}

		aggregate = aggregates->getNextChild();
	}

	mHavingCond = 0;
	AosXmlTagPtr having_def = def->getFirstChild("having");
	if (having_def)
	{
		AosCondType::E type = AosCondType::toEnum(having_def->getAttrStr(AOSTAG_ZKY_TYPE));
		if (AosCondType::isValid(type))
		{
			mHavingCond = AosConditionObj::getConditionStatic(having_def, rdata);
			aos_assert_r(mHavingCond, false);
		}
	}

	return true;
}


bool
AosDatasetByQuery::parseOrderBy(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	mOrderByField.clear();
	if (!def) return true;

	AosXmlTagPtr fields = def->getFirstChild("fields");
	if (!fields) return true;

	OmnString name, errormsg, tmp_name;
	AosExprObjPtr expr;
	bool reverse = false;
	AosXmlTagPtr field = fields->getFirstChild(true);
	while (field)
	{
		name = field->getNodeText();

		bool found = false;
		u32 idx = 0;
		for (u32 i=0; i<mFieldDef.size(); i++)
		{
			if (name == mFieldDef[i].mCname)
			{
				found = true;
				idx = i;
				break;
			}
		}

		reverse = field->getAttrBool("reverse");
		if (found)
		{
			mOrderByField.push_back(make_pair(idx, reverse));
		}
		else
		{
			tmp_name = name;
			tmp_name << ";";
			expr = AosParseExpr(tmp_name, errormsg, rdata.getPtr()); 
			if (!expr)
			{
				OmnAlarm << errormsg << enderr;
				return false;
			}

			//aos_assert_r(expr, false);
			mFieldNameExprs.push_back(expr);

			mOrderByField.push_back(make_pair(mFieldNameExprs.size() - 1, reverse));
		}

		field = fields->getNextChild();
	}

	if (mOrderByField.empty()) return true;

	mOrderBy = true;
	mUsingFieldValues = true;
	mFieldValues.clear();

	return true;
}


bool
AosDatasetByQuery::groupby(const AosRundataPtr &rdata)
{
	if (!mGroupBy) return true;

	if (mGroupByNeedSort)
	{
		aos_assert_r(!mGroupByField.empty(), false);

		comp cmp;
		cmp.mCrtSortField = mGroupByField;
		stable_sort(mFieldValues.begin(), mFieldValues.end(), cmp);
	}

	vector<AosValueRslt> vv;
	vector< vector<AosValueRslt> > vvs;

	if (mFieldValues.empty())
	{
		return true;
	}

	bool rslt = true;
	if (mGroupByField.empty())
	{
		rslt = groupbyAggregate(0, mFieldValues.size() - 1, vv, rdata);
		aos_assert_r(rslt, false);

		rslt = checkHaving(vv, rdata);
		if (rslt) vvs.push_back(vv);
	}
	else
	{
		comp cmp;
		cmp.mCrtSortField = mGroupByField;

		int rsltInt;
		u32 start_idx = 0, end_idx = 1;
		u32 size = mFieldValues.size();
		for (; end_idx<size; end_idx++)
		{
			rsltInt = cmp.compare(mFieldValues[start_idx], mFieldValues[end_idx]);
			aos_assert_r(rsltInt <= 0, false);

			if (rsltInt == 0) continue;

			rslt = groupbyAggregate(start_idx, end_idx - 1, vv, rdata);
			aos_assert_r(rslt, false);

			rslt = checkHaving(vv, rdata);
			if (rslt) vvs.push_back(vv);

			start_idx = end_idx;
		}

		rslt = groupbyAggregate(start_idx, end_idx - 1, vv, rdata);
		aos_assert_r(rslt, false);

		rslt = checkHaving(vv, rdata);
		if (rslt) vvs.push_back(vv);
	}

	mFieldValues.swap(vvs);

	return true;
}


bool
AosDatasetByQuery::groupbyAggregate(
		const u32 start_idx,
		const u32 end_idx,
		vector<AosValueRslt> &vv,
		const AosRundataPtr &rdata)
{
	vv.clear();
	aos_assert_r(start_idx <= end_idx && end_idx < mFieldValues.size(), false);

	OmnString type;
	u32 idx;
	bool distinct;
	AosValueRslt value_rslt;
	u32 size = mGroupByAggregate.size();

	int vv_idx = 0;
	bool firstValue = true;
	if (size == 1) firstValue = false;
	vv = mFieldValues[start_idx];

	for (u32 i=0; i<size; i++)
	{
		type = mGroupByAggregate[i].mType;
		idx = mGroupByAggregate[i].mIdx;
		distinct = mGroupByAggregate[i].mDistinct;

		vector< vector<AosValueRslt> > tmp;
		set<AosValueRslt> st;
		bool found_null = false;

		for (u32 j=start_idx; j<=end_idx; j++)
		{
			if (distinct)
			{
				if (mFieldValues[j][i].isNull())
				{
					if (!found_null)
					{
						tmp.push_back(mFieldValues[j]);
						found_null = true;
					}
				}
				else if (st.count(mFieldValues[j][i]) == 0)
				{
					tmp.push_back(mFieldValues[j]);
				}
			}
			else
			{
				tmp.push_back(mFieldValues[j]);
			}
		}

		value_rslt.setNull();
		u32 ss = tmp.size();
		if (type == "count_all")
		{
			int64_t num = ss;
			value_rslt.setI64(num);
		}
		else if (type == "count")
		{
			int64_t num = 0;
			for (u32 j=0; j<ss; j++)
			{
				if (tmp[j][idx].isNull()) continue;
				num++;
			}
			value_rslt.setI64(num);
		}
		else if (type == "sum")
		{
			bool found = false;
			for (u32 j=0; j<ss; j++)
			{
				if (AosDataType::isNumericType(tmp[j][idx].getType())) continue;

				if (!found)
				{
					value_rslt = tmp[j][idx];
					found = true;
				}
				else
				{
					//value_rslt = value_rslt + tmp[j][idx];
					AosDataType::E type = AosDataType::autoTypeConvert(value_rslt.getType(), tmp[j][idx].getType());
					value_rslt = AosValueRslt::doArith(ArithOpr::eAdd, type, value_rslt, tmp[j][idx]);
				}
			}
		}
		else if (type == "aggr_concat")
		{
			OmnString value = "";
			for (u32 j=0; j<ss; j++)
			{
				value << tmp[j][idx].getStr();
				if(value != "") if(j < ss -1) value << "/";
			}
			value_rslt.setStr(value);
		}
		else if (type == "max")
		{
			bool found = false;
			for (u32 j=0; j<ss; j++)
			{
				if (tmp[j][idx].isNull()) continue;

				if (!found)
				{
					value_rslt = tmp[j][idx];
					found = true;
				}
				else
				{
					// by andy zhang
					AosDataType::E type = AosDataType::autoTypeConvert(tmp[j][idx].getType(), value_rslt.getType());
					if (!tmp[j][idx].isNull() && !value_rslt.isNull())
					{
						if (AosValueRslt::doComparison(eAosOpr_gt, type, tmp[j][idx], value_rslt))  //if (tmp[j][idx] > value_rslt)
						{
							value_rslt = tmp[j][idx];
							if (!firstValue) vv_idx = j;
						}
					}
				}
			}
		}
		else if (type == "min")
		{
			bool found = false;
			for (u32 j=0; j<ss; j++)
			{
				if (tmp[j][idx].isNull()) continue;

				if (!found)
				{
					value_rslt = tmp[j][idx];
					found = true;
				}
				else
				{
					if (tmp[j][idx] < value_rslt)
					{
						value_rslt = tmp[j][idx];
						if (!firstValue) vv_idx = j;
					}
				}
			}
		}
		else if (type == "first")
		{
			value_rslt = tmp[0][idx];
		}
		else if (type == "last")
		{
			value_rslt = tmp[ss-1][idx];
			if (!firstValue) vv_idx = ss - 1;
		}
		else if (type == "avg")
		{
			bool found = false;
			int64_t num = 0;
			for (u32 j=0; j<ss; j++)
			{
				if (AosDataType::isNumericType(tmp[j][idx].getType())) continue;

				if (!found)
				{
					value_rslt = tmp[j][idx];
					found = true;
					num++;
				}
				else
				{
					AosDataType::E type = AosDataType::autoTypeConvert(value_rslt.getType(), tmp[j][idx].getType());
					value_rslt = AosValueRslt::doArith(ArithOpr::eAdd, type, value_rslt, tmp[j][idx]);
					num++;
				}
			}
			if (found)
			{
				AosValueRslt num_rslt(num);
				//value_rslt = value_rslt / num_rslt;
				AosDataType::E type = AosDataType::autoTypeConvert(value_rslt.getType(), num_rslt.getType());
				value_rslt = AosValueRslt::doArith(ArithOpr::eDiv, type, value_rslt, num_rslt);
			}
		}
		else
		{
			OmnAlarm << "error type:" << type << enderr;
			continue;
		}
		if (!firstValue) vv = tmp[vv_idx];
		vv[idx] = value_rslt;
	}
	return true;
}


bool
AosDatasetByQuery::checkHaving(
		vector<AosValueRslt> &vv,
		const AosRundataPtr &rdata)
{
	if (!mHavingCond) return true;

	bool rslt;
	bool outofmem;
	u32 size = mFieldDef.size();
	mDataRecord->clear();

	for (u32 i=0; i<size; i++)
	{
		rslt = mDataRecord->setFieldValue(mFieldDef[i].mCidx, vv[i], outofmem, rdata.getPtrNoLock());
		aos_assert_r(rslt, false);
		aos_assert_r(!outofmem, false);
	}

	rslt = mHavingCond->evalCond(mDataRecord, rdata);
	return rslt;
}


bool
AosDatasetByQuery::orderby(const AosRundataPtr &rdata)
{
	if (!mOrderBy) return true;

	aos_assert_r(!mOrderByField.empty(), false);

	comp cmp;
	cmp.mCrtSortField = mOrderByField;
	stable_sort(mFieldValues.begin(), mFieldValues.end(), cmp);

	return true;
}


bool
AosDatasetByQuery::comp::operator () (
		const vector<AosValueRslt> &l,
		const vector<AosValueRslt> &r)
{
	int rsltInt = compare(l, r);
	if (rsltInt < 0) return true;
	return false;
}


int
AosDatasetByQuery::comp::compare(
		const vector<AosValueRslt> &l,
		const vector<AosValueRslt> &r)
{
	u32 crt = 0;
	bool reverse = false;
	int reverse_int = 1;
	u32 size = mCrtSortField.size();
	for (u32 i=0; i<size; i++)
	{
		crt = mCrtSortField[i].first;
		reverse = mCrtSortField[i].second;
		reverse_int = (reverse) ? -1 : 1;

		if (l.size() <= crt || r.size() <= crt)
		{
			return -1 * reverse_int;
		}

		if (l[crt] < r[crt])
		{
			return -1 * reverse_int;
		}

		if (l[crt].isNull()) return -1 * reverse_int;
		if (r[crt].isNull()) return 1 * reverse_int;

		AosDataType::E type = AosDataType::autoTypeConvert(l[crt].getType(), r[crt].getType());
		if (AosValueRslt::doComparison(eAosOpr_gt, type, l[crt], r[crt]))   //if (l[crt] > r[crt])
		{
			return 1 * reverse_int;
		}
	}
	return 0;
}


bool
AosDatasetByQuery::getRecord(
		const OmnString &name,
		AosDataRecordObjPtr &record)
{
	if (name != mDataRecord->getRecordName())
	{
		return true;
	}

	record = mDataRecord;
	return true;
}


bool
AosDatasetByQuery::reset(const AosRundataPtr &rdata)
{
	return true;
}


bool
AosDatasetByQuery::gatherDocids(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool	
AosDatasetByQuery::getRecords(vector<AosDataRecordObjPtr> &records)
{
	aos_assert_r(mDataRecord, false);
	records.push_back(mDataRecord);
	return true;
}
