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
// 2013/10/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlQueryWhereConds.h"

#include "JQLStatement/JqlQueryWhereCond.h"
#include "JQLStatement/JqlSelectField.h"
#include "JQLStatement/JqlStmtTable.h"
#include "JQLStatement/JqlStmtIndex.h"
#include "JQLStatement/JQLCommon.h"

#include "Debug/Debug.h"
#include "WordParser/WordNorm.h"
#include "WordParser/WordParser.h"
#include "DataRecord/DataRecord.h"
#include "SEInterfaces/QueryType.h"
#include "SEUtil/IILName.h"
#include "SEUtil/JqlUtil.h"


AosJqlQueryWhereConds::AosJqlQueryWhereConds()
{
	mConds.clear();
	mInited = false;
	mSep = '\1';
	mFieldNum = 0;
	mIsAllSingle = false;
	mKeysExpr = NULL;
	mIsBad = false;
}


AosJqlQueryWhereConds::~AosJqlQueryWhereConds()
{
	mConds.clear();
}


bool
AosJqlQueryWhereConds::init(
		const AosRundataPtr &rdata,
		vector<AosJqlQueryWhereCondPtr> &conds,
		const OmnString &table_name,
		const AosDataRecordObjPtr &record)
{
	if (mIsAllSingle) return handleAllSingleConds(rdata, conds);

	bool rslt = false;
	if (table_name == "")
	{
		AosSetErrorUser(rdata, __func__) << "Table name is null." << enderr;
		OmnAlarm << enderr;
		return false;
	}

	mTableName = table_name;

	// jimo-671
	// if has record, check conds datatype by
	// record for subquery
	if(record)
	{	
		for (size_t i=0; i<conds.size(); i++)
		{
			// AosXmlTagPtr record_doc ,datarecord, datafields,datafield;
			//while(datafield)
			//{
			aos_assert_r(conds[i]->mExpr, false);
			//AosDataType::E type = conds[i]->mExpr->getDataType(rdata.getPtr(), record.getPtr());
			//AosXmlTagPtr record_doc = record->getRecordDoc();
			//AosXmlTagPtr datarecord = record_doc->getFirstChild("datarecord");
			//AosXmlTagPtr datafields = datarecord->getFirstChild("datafields");
			//AosXmlTagPtr datafield = datafields->getFirstChild("datafield");
			//AosDataType::E type = AosDataType::toEnum(datafield->getAttrStr("zky_value_type"));

			AosDataType::E type = conds[i]->mExpr->getDataType(rdata.getPtr(), record.getPtr());
			if (type == AosDataType::eInvalid) return false;
			//datafield = datafields->getNextChild("datafield");
			//}
		}
	}
	else
	{
		mTableDoc = AosJqlStmtTable::getTable(mTableName, rdata);
		if(mTableDoc)
		{
			rslt = checkCondsDataType(rdata, mTableDoc, conds);
			aos_assert_r(rslt, false);
		}
	}

	mConds = conds;
	mRundata = rdata;
	if (mConds.empty())
	{
		AosSetErrorUser(rdata, __func__)
			<< "mConds is empty." << enderr;
		OmnAlarm << enderr;
		return false;
	}

	rslt  = initFieldCondsMap();
	aos_assert_r(rslt, false);
	
	// jimodb-671
	if (!record)
	{
		if (mTableDoc && mTableDoc->getAttrStr("zky_data_format") != "xml")
		{
			rslt = initCmpIILMap();
			aos_assert_r(rslt, false);

			rslt = initSingleIILMap();
			aos_assert_r(rslt, false);
		}
	}
	rslt = mergeRedundantConds();
	aos_assert_r(rslt, false);
	rslt = splitConds();
	aos_assert_r(rslt, false);

	mFieldNum = mCmpIILConds.size();
	mInited = true;
	return true;
}


bool
AosJqlQueryWhereConds::init(
		const AosRundataPtr &rdata,
		vector<AosJqlQueryWhereCondPtr> &conds,
		const AosXmlTagPtr &index_doc)
{
	if (mIsAllSingle) return handleAllSingleConds(rdata, conds);

	bool rslt = false;

	if (!index_doc)
	{
		AosSetErrorUser(rdata, __func__)
			<< "Index doc is null ." << enderr;
		OmnAlarm << enderr;
		return false;
	}
	mRundata = rdata;
	mConds = conds;

	if (mConds.empty())
	{
		AosSetErrorUser(rdata, __func__)
			<< "mConds is empty." << enderr;
		OmnAlarm << enderr;
		return false;
	}

	FieldStr field_name;
	OmnString iil_name = index_doc->getAttrStr("zky_iilname");
	if (iil_name == "")
	{
		AosSetErrorUser(rdata, __func__)
			    << "IIL name is null." << enderr;
		OmnAlarm << enderr;
		return false;
	}

	rslt  = initFieldCondsMap();
	aos_assert_r(rslt, false);

	AosXmlTagPtr cols = index_doc->getFirstChild("columns");
	u32 cols_num = cols->getNumChilds();
	if (cols_num < 1)
	{
		AosSetErrorUser(rdata, __func__)
			<< "Index doc childs number is NULL." << enderr;
		OmnAlarm << enderr;
		return false;
	}

	if (cols_num > 1)
	{
		AosXmlTagPtr col = cols->getFirstChild(true);
		while (col)
		{
			field_name = col->getAttrStr("zky_name");
			if (field_name == "")
			{
				AosSetErrorUser(rdata, __func__)
					<< "Field name is null." << enderr;
				OmnAlarm << enderr;
				return false;
			}

			addEntry<OmnString, OmnString>(mCmpIILMap, iil_name, field_name);
			col = cols->getNextChild();
		}
	}
	else
	{
		AosXmlTagPtr col = cols->getFirstChild(true);
		field_name = col->getAttrStr("zky_name");
		if (field_name == "")
		{
			AosSetErrorUser(rdata, __func__)
				<< "Field name is null." << enderr;
			OmnAlarm << enderr;
			return false;
		}

		mSingleIILMap[field_name] = iil_name;
	}

	rslt = mergeRedundantConds();
	aos_assert_r(rslt, false);
	mFieldNum = mFieldCondsMap.size();
	if (cols_num < mFieldNum)
	{
		AosSetErrorUser(rdata, __func__)
			<< "Failed don`t match." << enderr;
		OmnAlarm << enderr;
		return false;
	}

	rslt = splitConds();
	aos_assert_r(rslt, false);

	mInited = true;
	return true;

}


bool
AosJqlQueryWhereConds::init(
		const AosRundataPtr &rdata,
		vector<AosJqlQueryWhereCondPtr> &conds,
		const OmnString &iilname,
		const AosExprList *fields)
{
	if (mIsAllSingle) return handleAllSingleConds(rdata, conds);

	bool rslt = false;
	if (!fields)
	{
		AosSetErrorUser(rdata, __func__)
			<< "FieldList is null." << enderr;
		OmnAlarm << enderr;
		return false;
	}
	mRundata = rdata;
	mConds = conds;
	mKeysExpr = fields;//new vector<AosExprObjPtr>();
	//mKeysExpr->insert(mKeysExpr->end(), fields->begin(), fields->end());

	if (mConds.empty())
	{
		AosSetErrorUser(rdata, __func__)
			<< "mConds is empty." << enderr;
		OmnAlarm << enderr;
		return false;
	}

	rslt  = initFieldCondsMap();
	aos_assert_r(rslt, false);

	OmnString field_name = "";
	for (u32 i = 0; i < fields->size(); i++)
	{
		field_name = (*fields)[i]->getValue(mRundata.getPtrNoLock());
		addEntry<OmnString, OmnString>(mCmpIILMap, iilname, field_name);
	}

	rslt = mergeRedundantConds();
	aos_assert_r(rslt, false);
	mFieldNum = mFieldCondsMap.size();

	rslt = splitConds();
	aos_assert_r(rslt, false);

	mInited = true;
	return true;
}


bool
AosJqlQueryWhereConds::initByStat(
		const AosRundataPtr &rdata,
		vector<AosJqlQueryWhereCondPtr> &qry_conds,
		vector<OmnString> &stat_keys)
{
	bool rslt = false;
	mConds = qry_conds;

	rslt  = initFieldCondsMap();
	aos_assert_r(rslt, false);

	for (u32 i = 0; i < stat_keys.size(); i++)
	{
		mSingleIILMap[stat_keys[i]] = stat_keys[i];
	}

	rslt = mergeRedundantConds();
	aos_assert_r(rslt, false);

	rslt = splitConds();
	aos_assert_r(rslt, false);

	mInited = true;
	return true;
}


bool
AosJqlQueryWhereConds::getConfig(
		const AosRundataPtr &rdata,
		AosXmlTagPtr &conds,
		AosXmlTagPtr &where_filter)
{
	aos_assert_r(mInited, false);
	bool rslt = optimizeByCompIIL();
	aos_assert_r(rslt, false);

	OmnString conds_str = generateCondsConf();
	OmnString filter_str = generateWhereFilterConf();

	conds = AosXmlParser::parse(conds_str AosMemoryCheckerArgs);
	where_filter = AosXmlParser::parse(filter_str AosMemoryCheckerArgs);
	if (!(conds || where_filter))
	{
		AosSetErrorUser(rdata, __func__)
			<< "Failed to generate config." << enderr;
		OmnAlarm << enderr;
		return false;
	}
	return true;
}


bool
AosJqlQueryWhereConds::initFieldCondsMap()
{
	FieldStr field = "";
	for (u32 i = 0; i < mConds.size(); i++)
	{
		field = mConds[i]->mLHS;
		mConds[i]->mTableName = mTableName;
		if (field == "")
		{
			AosSetErrorUser(mRundata, __func__)
				<< "This is BUG. Field is null." << enderr;
			OmnAlarm << enderr;
			return false;
		}
		addEntry<FieldStr, AosJqlQueryWhereCondPtr>(mFieldCondsMap, field, mConds[i]);
	}
	return true;
}

bool
AosJqlQueryWhereConds::initCmpIILMap()
{
	// <table>
	// 	<columns>
	// 	......
	// 	</columns>
	// 	<cmp_indexs>
	// 		<cmp_index zky_iilname="xxxx">
	// 			<field zky_name="xxxx">
	// 			..................
	// 		</cmp_index>
	// 	</cmp_indexs>
	// <table>

	CmpIILStr cmpiil_name = "";
	FieldStr field_name = "";
	AosXmlTagPtr conf = mTableDoc->getFirstChild("cmp_indexes");
	if (!conf) return true;

	AosXmlTagPtr node = conf->getFirstChild(true);
	while (node)
	{
		cmpiil_name = node->getAttrStr("zky_iilname");
		if (cmpiil_name == "")
		{
			AosSetErrorUser(mRundata, __func__)
				<< "This is BUG. IIL name is null." << enderr;
			OmnAlarm << enderr;
			return false;
		}

		AosXmlTagPtr child = node->getFirstChild(true);
		while (child)
		{
			field_name = child->getAttrStr(AOSTAG_NAME);
			if (field_name == "")
			{
				AosSetErrorUser(mRundata, __func__)
				<< "This is BUG. IIL name is null." << enderr;
				OmnAlarm << enderr;
				return false;
			}
	//		if (mFieldCondsMap.find(field_name) != mFieldCondsMap.end())
	//		{
				addEntry<OmnString, OmnString>(mCmpIILMap, cmpiil_name, field_name);
				addEntry<OmnString, OmnString>(mFieldCmpIILMap, field_name, cmpiil_name);
	//		}
			child = node->getNextChild();
		}
		node = conf->getNextChild();
	}

	return true;
}

bool
AosJqlQueryWhereConds::initSingleIILMap()
{
	// <table>
	// 	<columns>
	// 	......
	// 	</columns>
	// 	<cmp_indexs>
	// 		..................
	// 	</cmp_indexs>
	// 	<indexes>
	//		<index zky_name="xxx" zky_ilname="xxxx" index_objid= "xxxx">
	//		.........................
	// 	</indexes>
	//
	// <table>

	SingleIILStr iil_name = "";
	FieldStr field_name = "";
	AosXmlTagPtr conf = mTableDoc->getFirstChild("indexes");
	if (!conf) return true;
	AosXmlTagPtr node = conf->getFirstChild(true);
	while (node)
	{
		if (node->getAttrStr("zky_type") != "cmp")
		{
			iil_name = node->getAttrStr("zky_iilname");
			//aos_assert_r(iil_name != "" && field_name != "", false);
			field_name = node->getAttrStr("zky_name");
			//aos_assert_r(iil_name != "" && field_name != "", false);
			if (iil_name == "" || field_name == "")
			{
				node = conf->getNextChild();
				continue;
				AosSetErrorUser(mRundata, __func__)
					<< "This is BUG." << enderr;
				OmnAlarm << enderr;
				return false;
			}
	//		if (mFieldCondsMap.find(field_name) != mFieldCondsMap.end())
			mSingleIILMap[field_name] = iil_name;
		}
		node = conf->getNextChild();
	}
	return true;
}

bool
AosJqlQueryWhereConds::optimizeByCompIIL()
{
	bool rslt = sortCmpIILByPriorityLevel();
	aos_assert_r(rslt, false);
	bool flag = false;

	u32 pick_field_num = 0;
	CmpIILStr cmpiil_name;
	AosJqlQueryWhereCondPtr cond;

	vector<FieldStr> vec;
	vector<CmpIILStr> vec_cmpiils;

	mCmpIILMapByPriorityLevelItr itr = mCmpIILMapByPriorityLevel.begin();
	while (itr != mCmpIILMapByPriorityLevel.end() && pick_field_num < mFieldNum)
	{
		for (u32 j = 0; j < itr->second.size(); j++)
		{
			flag = false;
			if (pick_field_num >= mFieldNum) break;

			cmpiil_name = itr->second[j];
			aos_assert_r(cmpiil_name != "" && mCmpIILMap.find(cmpiil_name) != mCmpIILMap.end(), false);
			vec = mCmpIILMap[cmpiil_name];
			for (u32 i = 0; i < vec.size(); i++)
			{
				if (mFieldCondsMap.find(vec[i]) == mFieldCondsMap.end()) continue;
				aos_assert_r(mFieldCondsMap[vec[i]].size() == 1, false);

				cond = mFieldCondsMap[vec[i]][0];
				aos_assert_r(cond, false);
				if (!cond->mIsUsed)
				{
					flag = true;
					pick_field_num++;
				}
				cond->mIsUsed = true;
			}
			if (flag) vec_cmpiils.push_back(cmpiil_name);
		}
		itr++;
	}

	//aos_assert_r(mFieldNum == pick_field_num, false);
	if (mFieldNum != pick_field_num)
	{
		AosSetErrorUser(mRundata, __func__)
			<< "This is BUG. mFieldNum != pick_field_num." << enderr;
		OmnAlarm << enderr;
		return false;
	}
	rslt = handleCmpIIL(mRundata, vec_cmpiils);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosJqlQueryWhereConds::handleCmpIIL(
		const AosRundataPtr &rdata,
		vector<CmpIILStr> &vec_cmpiils)
{
	bool rslt = false;
	bool flag = true;
	bool is_using_single = false;
	int field_num = 0;
	mCmpIILConds.clear();
	CmpIILStr cmpiil_name;
	vector<FieldStr> vec;
	AosJqlQueryWhereCondPtr tmp_cond;
	for (u32 j = 0; j < vec_cmpiils.size(); j++)
	{
		flag = true;
		field_num = 0;
		cmpiil_name = vec_cmpiils[j];
		if (cmpiil_name == "")
		{
			AosSetErrorUser(mRundata, __func__)
				<< "CMPIIL name is null." << enderr;
			OmnAlarm << enderr;
			return false;
		}
		vec = mCmpIILMap[cmpiil_name];

		aos_assert_r(mCmpIILMap.find(vec[0]) == mCmpIILMap.end(), false);
		AosJqlQueryWhereCondPtr cond = OmnNew AosJqlQueryWhereCond;
		cond->mIILName = cmpiil_name;
	//	cond->setData(tmp_cond);

		bool is_need_append_sep = true;
		for (u32 i = 0; i < vec.size(); i++)
		{
			if (!(cond->mQueryFilters.empty()))
			{
				if (flag) flag = false;
			}

			if (mFieldCondsMap.find(vec[i]) == mFieldCondsMap.end()) {
				if (flag) flag = false;
				continue;
			}
			aos_assert_r(mFieldCondsMap[vec[i]].size() == 1, false);
			tmp_cond = mFieldCondsMap[vec[i]][0]->clone();
			aos_assert_r(tmp_cond, false);
			if (!flag)
			{
				cond->addQueryFilter(mRundata, tmp_cond, i);
				continue;
			}
			field_num++;
			if (i == 0)
			{
				cond->setData(tmp_cond);
				cond->mIILName = cmpiil_name;
			}
			else
			{
				is_need_append_sep = false;
				rslt = longitudinalMergeCond(cond, tmp_cond);
				if (!rslt)
				{
					AosSetErrorUser(mRundata, __func__)
						<< "This is BUG." << enderr;
					OmnAlarm << enderr;
					return false;
				}
			}
		}

		if (field_num <= 1)
		{
			flag = true;
			for (u32 i = 1; i < vec.size(); i++)
			{
				if (mFieldCondsMap.find(vec[i]) != mFieldCondsMap.end()) {
					if (mSingleIILMap.find(vec[i]) == mSingleIILMap.end()) {
						flag = false;
						break;
					}
				}
			}
			if (flag)
			{
				for (u32 i = 0; i < vec.size(); i++)
				{
					if (mSingleIILMap.find(vec[i]) != mSingleIILMap.end())
					{
						if (mFieldCondsMap.count(vec[i]) != 0)
						{
							aos_assert_r(mFieldCondsMap[vec[i]].size() > 0, false);
							mFieldCondsMap[vec[i]][0]->mIILName = mSingleIILMap[vec[i]];
							mSingleIILConds.push_back(mFieldCondsMap[vec[i]][0]);
							is_using_single = true;
							break;
						}
					}
				}
			}
			if (cond->mOpr == "eq") cond->mOpr = "pf";
		}
		if (is_need_append_sep) cond->mRHS << mSep;
		if (!is_using_single) mCmpIILConds.push_back(cond);
		is_using_single = false;
	}
	return true;
}

bool
AosJqlQueryWhereConds::sortCmpIILByPriorityLevel()
{
	// 1. Sort compound IILs based on the number of range based conditions.
	FieldStr field_name = "";
	CmpIILStr cmpiil_name = "";
	u32 num_range = 0;
	bool flag = false;
	for (CmpIILMapItr itr = mCmpIILMap.begin(); itr != mCmpIILMap.end(); itr++)
	{
		flag = false;
		num_range = 0;
		cmpiil_name = itr->first;
		//aos_assert_r(cmpiil_name != "" && !(itr->second.empty()), false);
		if (!(cmpiil_name != "" && !(itr->second.empty())))
		{
			AosSetErrorUser(mRundata, __func__)
				<< "CMPIIL name is null." << enderr;
			OmnAlarm << enderr;
			return false;
		}

		for (u32 i = 0; i < itr->second.size(); i++)
		{
			field_name = itr->second[i];
			if (mFieldCondsMap.find(field_name) == mFieldCondsMap.end() || flag)
			{
				// The field 'field_name' in the compound IIL is not used
				// in any conditions.
				if (i == 0)
				{
					// This is the first field. This compound IIL
					// can be used to scan IIL, which is better
					// than scanning docs.
					num_range = itr->second.size();
					break;
				}
				else
				{
					// This is not the first field. This means the
					// first field must be a conditioned field.
					if(!flag) flag = true;
				}
				num_range++;
			}
			else
			{
				// The field is used by a condition.
				aos_assert_r(mFieldCondsMap[field_name].size() == 1, false);
				if (mFieldCondsMap[field_name][0]->isRange() || !flag)
				{
					flag = true;
					if (mFieldCondsMap[field_name][0]->isNotAllowedUseIIL())
						num_range++;
				}
			}
		}
		addEntry<u32, OmnString>(mCmpIILMapByPriorityLevel, num_range, cmpiil_name);
	}
	return true;
}


bool
AosJqlQueryWhereConds::splitConds()
{
	// 1. For each condition, check whether its field name is in
	//    a compound IIL. If yes, put that condition to 'mCmpIILConds'.
	//
	//aos_assert_r(mInited, false);
	FieldStr field_name = "";
	for (FieldCondsMapItr itr = mFieldCondsMap.begin(); itr != mFieldCondsMap.end(); itr++)
	{
		aos_assert_r(itr->second.size() == 1, false);
		aos_assert_r(itr->second[0], false);
		field_name = itr->second[0]->mLHS;
		if (itr->second[0]->mOpr == "contain") {
			mKeyWordConds.push_back(itr->second[0]);
		}
		else if (mFieldCmpIILMap.find(field_name) != mFieldCmpIILMap.end()) {
			mCmpIILConds.push_back(itr->second[0]);
		}
		else if (mSingleIILMap.find(field_name) != mSingleIILMap.end() &&
					!itr->second[0]->isNotAllowedUseIIL()) {
			itr->second[0]->mIILName = mSingleIILMap[field_name];
			mSingleIILConds.push_back(itr->second[0]);
		}
		else
			mNotAllowedUseIndexConds.push_back(itr->second[0]);
	}
	return true;
}


bool
AosJqlQueryWhereConds::mergeRedundantConds()
{
	SingleIILStr iil_name;
	FieldStr field_name;
	FieldCondsMapItr itr;
	AosJqlQueryWhereCond::QueryFilter query_filter;
	for (itr = mFieldCondsMap.begin(); itr != mFieldCondsMap.end(); itr++)
	{
		//aos_assert_r(!itr->second.empty(), false);
		if (itr->second.empty())
		{
			AosSetErrorUser(mRundata, __func__)
				<< "Conditions is empty." << enderr;
			OmnAlarm << enderr;
			return false;
		}

		AosJqlQueryWhereCondPtr cond = OmnNew AosJqlQueryWhereCond;
		cond->setData(itr->second[0]);
		if (itr->second.size() == 1)
		{
			if (itr->second[0]->mOpr == "in")
			{
				OmnString str = "";
				for (u32 j = 0; j < itr->second[0]->mValues.size(); j++)
				{
					str <<"pf,";
					str << itr->second[0]->mValues[j] << mSep;
					str << ";";
				}
				cond->mRHS = str;
			}
			else if (itr->second[0]->mOpr == "not in")
			{
				u32 len = cond->mValues.size();
				aos_assert_r(cond->mValues.empty(), false);
				OmnString tmp_value = *(cond->mValues.begin());
				OmnString first_value = *(cond->mValues.begin());
				OmnString last_value = cond->mValues[len-1];

				std::stable_sort(cond->mValues.begin(), cond->mValues.end());
				vector<OmnString>::iterator iter = std::unique(cond->mValues.begin(), cond->mValues.end());
				cond->mValues.erase(iter, cond->mValues.end());

				cond->mRHS ="";

				cond->mRHS << "lt," << first_value << ";";
				if (len > 1)
				{
					for (u32 j = 1; j < cond->mValues.size(); j++)
					{
						cond->mRHS << "r4," << tmp_value << ","  << cond->mValues[j] << ";";
						tmp_value = cond->mValues[j];
					}
				}

				cond->mRHS << "gt," << last_value << ";";
				cond->mOpr = "in";
				cond->mIILName = iil_name;
			}
			else if (itr->second[0]->isNeedCastOpr())
			{
				iil_name = AosJqlStatement::getIILName(mRundata, mTableDoc, cond->mLHS);
				if (iil_name != "" &&
						mTableDoc &&
						mTableDoc->getAttrStr("zky_data_format") != "xml")
				{
					cond->mOpr = "an";
					cond->mRHS = "*";
					cond->mIILName = iil_name;
					query_filter.mOpr = "an";
					query_filter.mFieldName = cond->mLHS;
					query_filter.mValue = "*";
					query_filter.mCondStr = "";
					query_filter.mCondStr << itr->second[0]->toString(mRundata);
					cond->mQueryFilters.push_back(query_filter);
				}
			}
			else
			{
				if (itr->second[0]->mCMPIILName != "")
				{
					cond->mRHS << mSep;
					cond->mValue2 << mSep;
				}
			}

		} else {
			for (u32 i = 1; i < itr->second.size(); i++)
			{
				bool rslt = transverseMergeCond(cond, itr->second[i]);
				if (itr->second[i]->mIsMegre == true)
				{
					cond = itr->second[i];
				}
				aos_assert_r(rslt, false);
				if (itr->second.empty())
				{
					AosSetErrorUser(mRundata, __func__)
						<< "Failed to transverse merge condis." << enderr;
					OmnAlarm << enderr;
					return false;
				}
			}
		}

		itr->second.clear();
		itr->second.push_back(cond);
	}

	for (itr = mFieldCondsMap.begin(); itr != mFieldCondsMap.end(); itr++)
	{
		itr->second = itr->second;
	}

	return true;
}


OmnString
AosJqlQueryWhereConds::generateCondsConf()
{
	OmnString conds = "";
	conds << "<conds><cond type=\"AND\">";
	if (!mCmpIILConds.empty() || !mSingleIILConds.empty() || !mKeyWordConds.empty())
	{
		for (u32 i = 0; i < mCmpIILConds.size(); i++){
			conds << generateTermConf(mRundata, mTableName, mCmpIILConds[i]);
		}

		for (u32 i = 0; i < mKeyWordConds.size(); i++){
			conds << generateKeyWordTermCond(mKeyWordConds[i]);
		}

		for (u32 i = 0; i < mSingleIILConds.size(); i++){
			conds << generateTermConf(mRundata, mTableName, mSingleIILConds[i], true);
		}
	} else {
		conds << generateDefaultTermConf(mRundata, mTableDoc, mTableName);
	}
	conds << "</cond></conds>";
	return conds;
}

AosJqlQueryWhereCondPtr
AosJqlQueryWhereConds::getCond()
{
	if (!mCmpIILConds.empty())
		return mCmpIILConds[0];
	if (!mSingleIILConds.empty())
		return mSingleIILConds[0];
	return NULL;
}

OmnString
AosJqlQueryWhereConds::generateDefaultTermConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &table_doc,
		const OmnString &table_name)
{
	OmnString iilname, value, opr;

	if (table_doc && table_doc->getAttrStr("zky_data_format") != "xml")
	{
		AosXmlTagPtr index_nodes = table_doc->getFirstChild("indexes");
		aos_assert_r(index_nodes, "");
		AosXmlTagPtr index_node = index_nodes->getFirstChild(true);
		//aos_assert_r(index_node, "");
		//arvin 2015.07.23
		//JIMODB-79
		/*
		if(!index_node)
		{
			OmnString msg = "[ERR] : Table\"";
			msg << table_name << "\" have no indexes!";
			rdata->setJqlMsg(msg);
			return "";
		}
		OmnString objid = index_node->getAttrStr("idx_name");
		index_node = AosJqlStatement::getDocByObjid(rdata, objid);
		iilname = index_node->getAttrStr("zky_iilname");
		aos_assert_r(iilname != "", 0);
		value = "*"; opr="an";
		*/
        OmnString dbname = table_doc->getAttrStr("zky_database");
        iilname << "_zt44_" << dbname << "_" << table_name;
        value = "*";
        opr = "an";
	} else {
		iilname = AosIILName::composeCtnrMemberListing("", AOSTAG_PARENTC);
		value = table_name; opr="eq";
	}

	AosJqlQueryWhereCondPtr cond = OmnNew AosJqlQueryWhereCond();
	cond->mOpr = opr;
	cond->mIILName = iilname;
	cond->mRHS = value;
	OmnString term;
	term << generateTermConf(rdata, table_name, cond);
	return term;
}


bool
AosJqlQueryWhereConds::generateDefaultCond(
		const AosRundataPtr &rdata,
		AosXmlTagPtr &table_doc,
		const OmnString &table_name,
		vector<AosJqlQueryWhereCondPtr> &conds)
{
	OmnString iilname, value, opr, name;
	if (!table_doc)
		table_doc = AosJqlStmtTable::getTable(table_name, 0);

	if (table_doc && table_doc->getAttrStr("zky_data_format") != "xml") {
		AosXmlTagPtr index_nodes = table_doc->getFirstChild("indexes");
		aos_assert_r(index_nodes, "");
		AosXmlTagPtr index_node = index_nodes->getFirstChild(true);
		aos_assert_r(index_node, "");
		OmnString objid = index_node->getAttrStr("idx_name");
		index_node = AosJqlStatement::getDocByObjid(rdata, objid);
		iilname = index_node->getAttrStr("zky_iilname");
		AosXmlTagPtr cols = index_node->getFirstChild(true);
		aos_assert_r(cols, false);
		AosXmlTagPtr col = cols->getFirstChild(true);
		aos_assert_r(col, false);
		name = col->getAttrStr("zky_name");
		aos_assert_r(iilname != "", 0);
		value = "*"; opr="an";
	} else {
		iilname = AosIILName::composeCtnrMemberListing("", AOSTAG_PARENTC);
		value = table_name; opr="eq";
	}

	AosJqlQueryWhereCondPtr cond = OmnNew AosJqlQueryWhereCond();
	cond->mOpr = opr;
	cond->mIILName = iilname;
	cond->mRHS = value;
	cond->mLHS = name;
	conds.push_back(cond);
	return true;
}


OmnString
AosJqlQueryWhereConds::generateWhereFilterConf(
		const AosRundataPtr &rdata,
		const OmnString &expr_str)
{
	OmnString str = expr_str;
	AosJqlUtil::unescape(str);
	OmnString filter;
	filter << "<where zky_type=\"and\">";
	filter 	<< "<filter zky_type=\"expr\" >"
				<< 	"<![CDATA[" << str << 	"]]>"
				<< "</filter>";
	filter << "</where>";
	return filter;
}

OmnString
AosJqlQueryWhereConds::generateWhereFilterConf()
{
	if (mNotAllowedUseIndexConds.empty()) return "";
	OmnString filter, str;
	filter << "<where zky_type=\"and\">";

	for (u32 i = 0; i < mNotAllowedUseIndexConds.size(); i++){
		str =  mNotAllowedUseIndexConds[i]->toString(mRundata);
		AosJqlUtil::unescape(str);
		filter 	<< "<filter zky_type=\"expr\" >"
				<< 	"<![CDATA["
				<< 		str
				<< 	"]]>"
				<< "</filter>";
	}

	filter << "</where>";
	return filter;
}

OmnString
AosJqlQueryWhereConds::generateTermConf(
		const AosRundataPtr &rdata,
		const OmnString &table_name,
		const AosJqlQueryWhereCondPtr &cond,
		const bool is_single)
{
	OmnString conf = "";
	aos_assert_r(cond, conf);

	if (is_single)
	{
		if (cond->mOpr == "in")
		{
			OmnString str = "";
			for (u32 j = 0; j < cond->mValues.size(); j++)
			{
				str <<"eq,";
				str << cond->mValues[j];
				str << ";";
			}
			cond->mRHS = str;
		}
	}

	cond->mRHS = trimSep(cond->mRHS);
	cond->mValue2 = trimSep(cond->mValue2);
	aos_assert_r(cond->mIILName != "", "");

	AosValueRslt v1;
	OmnString value1, value2;
		///if(cond->mLHSExpr->getDataType(rdata.getPtr(), 0) == AosDataType::eDateTime
	if (cond->mRHSExpr && cond->mRHSExpr->getDataType(rdata.getPtr(), 0) == AosDataType::eDateTime)
	{
		AosRundata * rr = (AosRundata*) rdata.getPtr();
		//cond->mLHSExpr->getValue(rr, 0, v1);
		cond->mRHSExpr->getValue(rr, 0, v1);
		value1 << v1.getI64();
		//value2 << cond->mValue2;
		if (!cond->mValue2Rslt.isNull())
			value2 << cond->mValue2Rslt.getI64();
	}
	else
	{
		value1 << cond->mRHS;
		value2 << cond->mValue2;
	}

	conf 	<< "<term type=\"" << AOSTERMTYPE_JIMO << "\" "
			<< " classname=\"AosTermArithNew\""
			<< " order=\"" << cond->mIsOrder << "\" reverse=\"" << !cond->mIsAsc << "\" "
			<< " zky_opr=\"" << cond->mOpr  << "\" "
			<< " iilname=\"" << cond->mIILName << "\">"
			<< 		"<value1><![CDATA[" << value1 << "]]></value1>"
			<< 		"<value2><![CDATA[" << value2 << "]]></value2>"
			<< "<query_filters>"
			<< generateQuerFilterConf(rdata, table_name, cond)
			<< "</query_filters>"
			<< "</term>";

	return conf;
}


OmnString
AosJqlQueryWhereConds::generateKeyWordTermCond(const AosJqlQueryWhereCondPtr &cond)
{
	bool rslt = false;
	OmnString term;
	OmnString value;
	OmnString keyword = cond->mRHS;
	OmnString tablename = mTableName;
	OmnString iilname;

	AosWordParserPtr wordpaeser = OmnNew AosWordParser();
	aos_assert_r(wordpaeser, "");
	wordpaeser->setSrc(keyword.data(), 0, keyword.length());
	char word_str[AosXmlTag::eMaxWordLen+1];
	int str_len;
	AosJqlQueryWhereCondPtr new_cond;

	while (1)
	{
		rslt = wordpaeser->nextWord(word_str, str_len);
		if (!rslt) break;
		value = OmnString(word_str, str_len);
		new_cond = OmnNew AosJqlQueryWhereCond();
		iilname = AosIILName::composeTableAttrWordIILName(tablename, cond->mLHS, value);
		new_cond->mOpr = "an";
		new_cond->mIILName = iilname;
		new_cond->mRHS = "*";
		term << generateTermConf(mRundata, tablename, new_cond);
	}
	return term;
}

OmnString
AosJqlQueryWhereConds::generateQuerFilterConf(
		const AosRundataPtr &rdata,
		const OmnString &table_name,
		const AosJqlQueryWhereCondPtr &cond)
{
	OmnString conf = "";
	aos_assert_r(cond, conf);

	int field_idx;
	AosJqlQueryWhereCond::QueryFilter query_filter;

	OmnString sep = "0x01";
	for (u32 i = 0; i < cond->mQueryFilters.size(); i++)
	{
		query_filter = cond->mQueryFilters[i];
		AosJqlColumnPtr col = AosJqlStatement::getTableField(rdata, table_name, query_filter.mFieldName);
		field_idx = query_filter.mFieldIdx;
		if (field_idx == -1 ) field_idx = AosJqlQueryWhereConds::getFieldIdx(rdata, NULL, cond->mIILName, query_filter.mFieldName);
		aos_assert_r(field_idx > -1, "");
		conf << "<query_filter type=\"" << query_filter.mType << "\""
			 << " field_idx=\"" << field_idx <<"\" "
			 << AOSTAG_SEPARATOR << "=\"" << sep << "\"";
		if (col)
		{
			OmnString field_type = AosDataType::getTypeStr(col->type);
			field_type = AosJqlStatement::AosJType2DType(field_type);

			conf << " zky_value_type=\"" << field_type << "\" "
				 << " zky_precision=\"" << col->precision << "\" "
				 << " zky_scale=\"" << col->scale << "\" ";
		}

		conf << " opr=\"" << query_filter.mOpr << "\">"
			 <<		"<value1><![CDATA[" << query_filter.mValue << "]]></value1>";
		if (query_filter.mValue2 != "") conf <<	"<value2><![CDATA[" << query_filter.mValue2 << "]]></value2>";
		if (query_filter.mCondStr != "") conf << "<cond zky_type=\"expr\"><![CDATA[" << query_filter.mCondStr << "]]></cond>";
		conf << "</query_filter>";
	}

	return conf;
}

int
AosJqlQueryWhereConds::getFieldIdx(
		const AosRundataPtr &rdata,
		const OmnString &cmpiil_name,
		const OmnString &field_name)
{
	int idx = -1;
	aos_assert_r(cmpiil_name != "" && field_name != "", -1);
	OmnString tmp = cmpiil_name;
	OmnString iil_name = OmnString(&tmp[6]);
	OmnString objid = AosJqlStatement::getObjid(rdata, JQLTypes::eIndexDoc, iil_name);
	AosXmlTagPtr doc = AosJqlStatement::getDocByObjid(rdata, objid);
	if (doc)
	{
		idx = AosJqlQueryWhereConds::getFieldIdx(rdata, doc, cmpiil_name, field_name);
		if (idx < 0)
		{
			AosSetErrorUser(rdata, __func__)
				<< " Field Undefined. Field Index :" << idx << enderr;
			OmnAlarm << __func__ << enderr;
		}
		return idx;
	}
	else
	{
		if (mKeysExpr== NULL)
		{
			AosSetErrorUser(rdata, __func__)
				<< "mKeysExpr Is NULL. Index Doc Is NULL." << enderr;
			OmnAlarm << __func__ << enderr;
			return idx;
		}

		for (u32 i = 0; i < mKeysExpr->size(); i++)
		{
			if (field_name == (*mKeysExpr)[i]->getValue(rdata.getPtrNoLock()))
			{
				return i;
			}
		}
	}
	return idx;
}

int
AosJqlQueryWhereConds::getFieldIdx(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &index_doc,
		const OmnString &cmpiil_name,
		const OmnString &field_name)
{
	int idx = 0;
	aos_assert_r(cmpiil_name != "" && field_name != "", -1);
	OmnString tmp = cmpiil_name;
	OmnString iil_name = OmnString(&tmp[6]);
	AosXmlTagPtr doc = index_doc;
	if (!doc)
	{
		OmnString objid = AosJqlStatement::getObjid(rdata, JQLTypes::eIndexDoc, iil_name);
		doc = AosJqlStatement::getDocByObjid(rdata, objid);
		aos_assert_r(doc, -1);
	}

	AosXmlTagPtr columns = doc->getFirstChild("columns");
	aos_assert_r(columns, -1);
	AosXmlTagPtr column = columns->getFirstChild(true);
	while (column)
	{
		if (column->getAttrStr("zky_name") == field_name) return idx;
		idx++;
		column = columns->getNextChild();
	}
	aos_assert_r(-1, -1);
	return -1;
}

bool
AosJqlQueryWhereConds::checkIsLastField(
		const AosRundataPtr &rdata,
		const OmnString &cmpiil_name,
		const OmnString &field_name)
{
	aos_assert_r(cmpiil_name != "" && field_name != "", -1);
	OmnString tmp = cmpiil_name;
	OmnString iil_name = OmnString(&tmp[6]);
	OmnString objid = AosJqlStatement::getObjid(rdata, JQLTypes::eIndexDoc, iil_name);
	AosXmlTagPtr doc = AosJqlStatement::getDocByObjid(rdata, objid);
	aos_assert_r(doc, -1);

	AosXmlTagPtr columns = doc->getFirstChild("columns");
	aos_assert_r(columns, -1);
	AosXmlTagPtr column = columns->getLastChild();
	if (column)
	{
		if (column->getAttrStr("zky_name") == field_name)
			return true;
	}
	return false;
}



bool
AosJqlQueryWhereConds::handleAllSingleConds(
		const AosRundataPtr &rdata,
		vector<AosJqlQueryWhereCondPtr> &conds)
{
	mInited = true;
	mSingleIILConds.insert(mSingleIILConds.end(), conds.begin(), conds.end());
	return true;
}


bool
AosJqlQueryWhereConds::transverseMergeCond(
		AosJqlQueryWhereCondPtr &to_cond,
		AosJqlQueryWhereCondPtr &cond)
{
	AosJqlQueryWhereCond::QueryFilter query_filter;
	aos_assert_r(to_cond->mLHS == cond->mLHS, false);
	to_cond->setOrder(cond);
	AosOpr opr1 = AosOpr_toEnum(to_cond->mOpr);
	AosOpr opr2 = AosOpr_toEnum(cond->mOpr);
	OmnString opr, value, value2, cond_str, tmp_str;
	switch (opr1)
	{
	case eAosOpr_gt:
		switch (opr2)
		{
		case eAosOpr_Invalid:
			aos_assert_r(false, false);
		default:
			gt_all(to_cond, cond);
			break;
		}
		break;
	case eAosOpr_ge:
		switch (opr2)
		{
		case eAosOpr_Invalid:
			aos_assert_r(false, false);
		case eAosOpr_gt:
			return transverseMergeCond(cond, to_cond);
		default:
			ge_all(to_cond, cond);
			break;
		}
		break;
	case eAosOpr_eq:
		switch (opr2)
		{
		case eAosOpr_Invalid:
			aos_assert_r(false, false);
		case eAosOpr_gt:
		case eAosOpr_ge:
			return transverseMergeCond(cond, to_cond);
		default:
			eq_all(to_cond, cond);
			break;
		}
		break;
	case eAosOpr_lt:
		switch (opr2)
		{
		case eAosOpr_Invalid:
			aos_assert_r(false, false);
		case eAosOpr_gt:
		case eAosOpr_ge:
		case eAosOpr_eq:
			return transverseMergeCond(cond, to_cond);
		default:
			lt_all(to_cond, cond);
			break;
		}
		break;

	case eAosOpr_le:
		switch (opr2)
		{
		case eAosOpr_Invalid:
			aos_assert_r(false, false);
		case eAosOpr_gt:
		case eAosOpr_ge:
		case eAosOpr_eq:
		case eAosOpr_lt:
			return transverseMergeCond(cond, to_cond);
		default:
			le_all(to_cond, cond);
			break;
		}
		break;

	case eAosOpr_ne:
		switch (opr2)
		{
		case eAosOpr_Invalid:
			aos_assert_r(false, false);
		case eAosOpr_gt:
		case eAosOpr_ge:
		case eAosOpr_eq:
		case eAosOpr_lt:
		case eAosOpr_le:
			return transverseMergeCond(cond, to_cond);
		default:
			ne_all(to_cond, cond);
			break;
		}

		break;
	case eAosOpr_like:
	case eAosOpr_not_like:
		switch (opr2)
		{
		case eAosOpr_Invalid:
			aos_assert_r(false, false);
		case eAosOpr_gt:
		case eAosOpr_ge:
		case eAosOpr_eq:
		case eAosOpr_lt:
		case eAosOpr_le:
		case eAosOpr_ne:
			return transverseMergeCond(cond, to_cond);
		default:
			like_all(to_cond, cond);
			break;
		}
		break;
	default:
		switch (opr2)
		{
			case eAosOpr_Invalid:
				aos_assert_r(false, false);
			case eAosOpr_gt:
			case eAosOpr_ge:
			case eAosOpr_eq:
			case eAosOpr_lt:
			case eAosOpr_le:
			case eAosOpr_ne:
				return transverseMergeCond(cond, to_cond);
			default:
				other_all(to_cond, cond);
				break;
		}
		break;
	}

	to_cond->mCondStr.removeWhiteSpaces();
	if (to_cond->mCondStr != "") {
		query_filter.setData(-1, "an",to_cond->mLHS, "*", "", to_cond->mCondStr);
		to_cond->mQueryFilters.push_back(query_filter);
	}
	return true;
}


bool
AosJqlQueryWhereConds::longitudinalMergeCond(
		AosJqlQueryWhereCondPtr &to_cond,
		AosJqlQueryWhereCondPtr &from_cond)
{
	CmpIILStr cmpiil_name = to_cond->mIILName;
	FieldStr field_name = to_cond->mLHS;
	to_cond->setOrder(from_cond);
	int idx = getFieldIdx(mRundata, cmpiil_name, from_cond->mLHS);
	aos_assert_r(idx > -1, false);

	AosJqlQueryWhereCond::QueryFilter query_filter;
	AosOpr opr1 = AosOpr_toEnum(to_cond->mOpr);
	AosOpr opr2 = AosOpr_toEnum(from_cond->mOpr);
	OmnString opr, value, value2, cond_str;

	switch (opr1)
	{
	case eAosOpr_Invalid:
		switch (opr2)
		{
			case eAosOpr_Invalid:
				aos_assert_r(NULL, false);
				break;
			case eAosOpr_in:
				{
					for (u32 i = 0; i < from_cond->mValues.size(); i++){
						value << "pf," << from_cond->mValues[i] << ";";
					}
					to_cond->mValues.insert(to_cond->mValues.end(), from_cond->mValues.begin(), from_cond->mValues.end());
					opr = "in";
				}
				break;
			default:
				to_cond->setData(from_cond);
				return true;
		}
		break;

	case eAosOpr_gt:
	case eAosOpr_lt:
	case eAosOpr_ge:
	case eAosOpr_le:
	case eAosOpr_like:
	case eAosOpr_not_like:
	case eAosOpr_null:
	case eAosOpr_not_null:
	case eAosOpr_not_in:
		switch (opr2)
		{
		case eAosOpr_Invalid:
			aos_assert_r(NULL, false);
			break;
		default:
			return to_cond->addQueryFilter(mRundata, from_cond, idx);
		}
		break;
	case eAosOpr_ne:
		other_all(to_cond, from_cond);
		break;
	case eAosOpr_prefix:
	case eAosOpr_eq:
		switch (opr2)
		{
		case eAosOpr_Invalid:
			aos_assert_r(NULL, false);
			break;
		default:
			eq_all(to_cond, from_cond);
			break;
		}
		break;
	case eAosOpr_in:
		switch (opr2)
		{
		case eAosOpr_Invalid:
			aos_assert_r(NULL, false);
			break;
		default:
			in_all(to_cond, from_cond);
			break;
		}
		break;
	case eAosOpr_an:
	case eAosOpr_range_ge_le:
	case eAosOpr_range_ge_lt:
	case eAosOpr_range_gt_le:
	case eAosOpr_range_gt_lt:
		return to_cond->addQueryFilter(mRundata, from_cond, idx);
	default:
		aos_assert_r(NULL, false);
		break;
	}
	return true;
}

bool
AosJqlQueryWhereConds::gt_all(AosJqlQueryWhereCondPtr &cond, AosJqlQueryWhereCondPtr &cond2)
{
	aos_assert_r(cond && cond2, false);
	AosValueRslt  vv_rslt;
	OmnString opr, value, value2, cond_str, str, str1;
	opr = value = value2 = cond_str = "";
	OmnString vv = "";

	AosOpr opr1 = AosOpr_toEnum(cond->mOpr);
	aos_assert_r(opr1 == eAosOpr_gt, false);

	AosOpr opr2 = AosOpr_toEnum(cond2->mOpr);
	if (cond->mLHS == cond2->mLHS)
	{
		switch (opr2)
		{
		case eAosOpr_gt:
			opr = "gt";
			if (cond->mRHS >= cond2->mRHS) value = cond->mRHS;
			else value = cond2->mRHS;
			break;

		case eAosOpr_ge:
			if (cond->mRHS >= cond2->mRHS) {
				opr = "gt";
				value = cond->mRHS;
			}
			else {
				opr = "ge";
				value = cond2->mRHS;
			}
			break;

		case eAosOpr_eq:
			aos_assert_r(cond->mRHS < cond2->mRHS, false);
			opr = "eq";
			value = cond2->mRHS;
			break;

		case eAosOpr_lt:
			opr = "r4";
		case eAosOpr_le:
			if (opr == "") opr = "r3";
			aos_assert_r(cond->mRHS < cond2->mRHS, false);
			value = cond->mRHS;
			value2 = cond2->mRHS;
			cond2->mRHSExpr->getValue(mRundata.getPtr(), 0, vv_rslt);
			break;

		case eAosOpr_in:
			opr = "in";
			cond->mValues.insert(cond->mValues.end(), cond2->mValues.begin(), cond2->mValues.end());
			break;

		case eAosOpr_ne:
		case eAosOpr_like:
		case eAosOpr_not_like:
		case eAosOpr_null:
		case eAosOpr_not_null:
		case eAosOpr_not_in:
			opr = "gt";
			value = cond->mRHS;
			cond->addQueryFilter(mRundata, cond2, -1);
			break;
		default:
			aos_assert_r(NULL, false);
			break;
		}
		cond->mOpr = opr;
		cond->mRHS = value;
		cond->mValue2 = value2;
		cond->mValue2Rslt = vv_rslt;
		cond->mIsMegre = true;
		return true;
	}
	return false;
}


bool
AosJqlQueryWhereConds::ge_all(AosJqlQueryWhereCondPtr &cond, AosJqlQueryWhereCondPtr &cond2)
{
	aos_assert_r(cond && cond2, false);
	AosValueRslt vv_rslt;
	OmnString opr, value, value2, cond_str, str, str1;
	opr = value = value2 = cond_str = "";

	AosOpr opr1 = AosOpr_toEnum(cond->mOpr);
	aos_assert_r(opr1 == eAosOpr_ge, false);

	AosOpr opr2 = AosOpr_toEnum(cond2->mOpr);
	if (cond->mLHS == cond2->mLHS)
	{
		switch (opr2)
		{
		case eAosOpr_gt:
			return gt_all(cond2, cond);
		case eAosOpr_ge:
			opr = "ge";
			if (cond->mRHS >= cond2->mRHS) {
				value = cond->mRHS;
			} else {
				value = cond2->mRHS;
			}
			break;

		case eAosOpr_eq:
			aos_assert_r(cond->mRHS < cond2->mRHS, false);
			opr = "eq";
			value = cond2->mRHS;
			break;

		case eAosOpr_lt:
			aos_assert_r(cond->mRHS < cond2->mRHS, false);
			opr = "r2";
		case eAosOpr_le:
			aos_assert_r(cond->mRHS < cond2->mRHS, false);
			if (opr == "") opr = "r1";
			value = cond->mRHS;
			value2 = cond2->mRHS;
			cond2->mRHSExpr->getValue(mRundata.getPtr(), 0, vv_rslt);
			break;

		case eAosOpr_in:
			opr = "in";
			cond->mValues.insert(cond->mValues.end(), cond2->mValues.begin(), cond2->mValues.end());
			break;

		case eAosOpr_ne:
		case eAosOpr_like:
		case eAosOpr_not_like:
		case eAosOpr_null:
		case eAosOpr_not_null:
		case eAosOpr_not_in:
			opr = "ge";
			value = cond->mRHS;
			cond->addQueryFilter(mRundata, cond2, -1);
			break;
		default:
			aos_assert_r(NULL, false);
			break;
		}
		cond->mOpr = opr;
		cond->mRHS = value;
		cond->mValue2 = value2;
		cond->mValue2Rslt = vv_rslt;
		cond->mCondStr = cond_str;
		cond->mIsMegre = true;
		return true;
	}
	return false;

}


bool
AosJqlQueryWhereConds::lt_all(AosJqlQueryWhereCondPtr &cond, AosJqlQueryWhereCondPtr &cond2)
{
	aos_assert_r(cond && cond2, false);
	OmnString opr, value, value2, cond_str, str, str1;
	opr = value = value2 = cond_str = "";

	AosOpr opr1 = AosOpr_toEnum(cond->mOpr);
	aos_assert_r(opr1 == eAosOpr_lt, false);

	AosOpr opr2 = AosOpr_toEnum(cond2->mOpr);
	if (cond->mLHS == cond2->mLHS)
	{
		switch (opr2)
		{
		case eAosOpr_gt:
			return gt_all(cond2, cond);
		case eAosOpr_ge:
			return ge_all(cond2, cond);
		case eAosOpr_eq:
			return eq_all(cond2, cond);
		case eAosOpr_lt:
			opr = "lt";
			if (cond->mRHS > cond2->mRHS) {
				value = cond2->mRHS;
			}
			else{
				value = cond->mRHS;
			}
			break;

		case eAosOpr_le:
			if (cond->mRHS >= cond2->mRHS) {
				opr = "le";
				value = cond->mRHS;
			}
			else{
				opr = "lt";
				value = cond->mRHS;
			}
			break;

		case eAosOpr_in:
			opr = "in";
			cond->mValues.insert(cond->mValues.end(), cond2->mValues.begin(), cond2->mValues.end());
			break;

		case eAosOpr_ne:
		case eAosOpr_like:
		case eAosOpr_not_like:
		case eAosOpr_null:
		case eAosOpr_not_null:
		case eAosOpr_not_in:
			opr = "lt";
			value = cond->mRHS;
			cond->addQueryFilter(mRundata, cond2, -1);
			break;
		default:
			aos_assert_r(NULL, false);
			break;
		}
		cond->mOpr = opr;
		cond->mRHS = value;
		cond->mValue2 = value2;
		cond->mIsMegre = true;
		return true;
	}
	return false;
}

bool
AosJqlQueryWhereConds::le_all(AosJqlQueryWhereCondPtr &cond, AosJqlQueryWhereCondPtr &cond2)
{
	aos_assert_r(cond && cond2, false);
	OmnString opr, value, value2, cond_str, str, str1;
	opr = value = value2 = cond_str = "";

	AosOpr opr1 = AosOpr_toEnum(cond->mOpr);
	aos_assert_r(opr1 == eAosOpr_le, false);

	AosOpr opr2 = AosOpr_toEnum(cond2->mOpr);
	if (cond->mLHS == cond2->mLHS)
	{
		switch (opr2)
		{
		case eAosOpr_gt:
			return gt_all(cond2, cond);
		case eAosOpr_ge:
			return ge_all(cond2, cond);
		case eAosOpr_eq:
			return eq_all(cond2, cond);
		case eAosOpr_lt:
			return lt_all(cond2, cond);
		case eAosOpr_le:
			opr = "le";
			if (cond->mRHS >= cond2->mRHS) {
				value = cond2->mRHS;
			}
			else{
				value = cond->mRHS;
			}
			break;

		case eAosOpr_in:
			opr = "in";
			cond->mValues.insert(cond->mValues.end(), cond2->mValues.begin(), cond2->mValues.end());
			break;

		case eAosOpr_ne:
		case eAosOpr_like:
		case eAosOpr_not_like:
		case eAosOpr_null:
		case eAosOpr_not_null:
		case eAosOpr_not_in:
			opr = "le";
			value = cond->mRHS;
			cond->addQueryFilter(mRundata, cond2, -1);
			break;
		default:
			aos_assert_r(NULL, false);
			break;
		}
		cond->mOpr = opr;
		cond->mRHS = value;
		cond->mValue2 = value2;
		cond->mIsMegre = true;
		return true;
	}
	return false;

}


bool
AosJqlQueryWhereConds::eq_all(AosJqlQueryWhereCondPtr &cond, AosJqlQueryWhereCondPtr &cond2)
{
	int idx = -1;
	bool is_last = checkIsLastField(mRundata, cond->mIILName, cond2->mLHS);
	aos_assert_r(cond && cond2, false);
	OmnString opr, value, value2, cond_str, str, str1, vv;
	opr = value = value2 = cond_str = "";

	AosOpr opr1 = AosOpr_toEnum(cond->mOpr);
	aos_assert_r(opr1 == eAosOpr_eq || opr1 == eAosOpr_prefix, false);

	AosOpr opr2 = AosOpr_toEnum(cond2->mOpr);
	if (cond->mLHS == cond2->mLHS)
	{
		switch (opr2)
		{
		case eAosOpr_in:
			opr = "in";
			cond->mValues.push_back(cond->mRHS);
			cond->mValues.insert(cond->mValues.end(), cond2->mValues.begin(), cond2->mValues.end());
			break;

		case eAosOpr_gt:
		case eAosOpr_ge:
		case eAosOpr_eq:
		case eAosOpr_lt:
		case eAosOpr_le:
		case eAosOpr_ne:
		case eAosOpr_like:
		case eAosOpr_not_like:
		case eAosOpr_null:
		case eAosOpr_not_null:
		case eAosOpr_not_in:
		case eAosOpr_range_ge_le:
		case eAosOpr_range_ge_lt:
		case eAosOpr_range_gt_le:
		case eAosOpr_range_gt_lt:
			opr = "gt";
			value = cond->mRHS;
			cond->addQueryFilter(mRundata, cond2, -1);
			break;
		default:
			aos_assert_r(NULL, false);
			break;
		}
	}
	else
	{
		switch (opr2)
		{
			case eAosOpr_gt:
				opr = "r3";
			case eAosOpr_ge:
				vv = cond->mRHS;
				value2 << vv << mSep << "\255";
				vv << mSep << cond2->mRHS;
				if (vv == "") opr = "r1";
				value = vv;
				break;
			case eAosOpr_eq:
				opr = "pf";
				vv = cond->mRHS;
				vv << mSep << cond2->mRHS;
				value = vv;
				break;

			case eAosOpr_lt:
				opr = "r1";
			case eAosOpr_le:
				if (opr == "") opr = "r3";
				aos_assert_r(cond->mRHS < cond2->mRHS, false);
				value << cond->mRHS << mSep << "\000";
				value2 << cond->mRHS << mSep << cond2->mRHS;
				break;

			case eAosOpr_in:
				opr = "in";
				for (u32 i = 0; i < cond2->mValues.size(); i++)
				{
					value << "pf," << cond->mRHS << mSep << cond2->mValues[i];
					if (!is_last) value << mSep;
					value << ";";
				}
				break;

			case eAosOpr_an:
			case eAosOpr_ne:
			case eAosOpr_like:
			case eAosOpr_null:
			case eAosOpr_not_null:
			case eAosOpr_not_like:
				opr = "pf";
				value <<  cond->mRHS << mSep;
				idx = AosJqlQueryWhereConds::getFieldIdx(mRundata, cond->mIILName, cond2->mLHS);
				cond->addQueryFilter(mRundata, cond2, idx);
				break;
			case eAosOpr_range_ge_le:
				opr = "r1";
				value <<  cond->mRHS << mSep << cond2->mRHS;
				value2 <<  cond->mRHS << mSep << cond2->mValue2;
				break;
			case eAosOpr_range_ge_lt:
				opr = "r2";
				value <<  cond->mRHS << mSep << cond2->mRHS;
				value2 <<  cond->mRHS << mSep << cond2->mValue2;
				break;
			case eAosOpr_range_gt_le:
				opr = "r3";
				value <<  cond->mRHS << mSep << cond2->mRHS;
				value2 <<  cond->mRHS << mSep << cond2->mValue2;
				break;
			case eAosOpr_range_gt_lt:
				opr = "r4";
				value <<  cond->mRHS << mSep << cond2->mRHS;
				value2 <<  cond->mRHS << mSep << cond2->mValue2;
				break;
			default:
				aos_assert_r(NULL, false);
				break;
		}
		if (!is_last && opr != "in")
		{
			if (value != "") value << mSep;
			if (value2 != "") value2 << mSep;
		}
		if (is_last && opr == "pf" && (cond->mQueryFilters.empty()))
		{
			opr = "eq";
		}
	}

	cond->mOpr = opr;
	cond->mRHS = value;
	cond->mValue2 = value2;
	cond->mCondStr = cond_str;
	cond->mIsMegre = true;
	return true;
}

bool
AosJqlQueryWhereConds::in_all(AosJqlQueryWhereCondPtr &cond, AosJqlQueryWhereCondPtr &cond2)
{
	int idx = -1;
	bool is_last = checkIsLastField(mRundata, cond->mIILName, cond2->mLHS);
	aos_assert_r(cond && cond2, false);
	OmnString opr, value, value2, cond_str, str, str1;
	opr = value = value2 = cond_str = "";

	AosOpr opr1 = AosOpr_toEnum(cond->mOpr);
	aos_assert_r(opr1 == eAosOpr_in, false);

	AosOpr opr2 = AosOpr_toEnum(cond2->mOpr);
	if (cond->mLHS == cond2->mLHS)
	{
		switch (opr2)
		{
		case eAosOpr_gt:
			return gt_all(cond2, cond);
		case eAosOpr_ge:
			return ge_all(cond2, cond);
		case eAosOpr_eq:
			return eq_all(cond2, cond);
		case eAosOpr_lt:
			return lt_all(cond2, cond);
		case eAosOpr_le:
			return le_all(cond2, cond);
		case eAosOpr_ne:
			return ne_all(cond2, cond);
		case eAosOpr_like:
		case eAosOpr_not_like:
			return like_all(cond2, cond);
		case eAosOpr_in:
			opr = "in";
			cond->mValues.insert(cond->mValues.end(), cond2->mValues.begin(), cond2->mValues.end());
			break;
		case eAosOpr_an:
		case eAosOpr_not_in:
		case eAosOpr_null:
		case eAosOpr_not_null:
		case eAosOpr_range_ge_le:
		case eAosOpr_range_ge_lt:
		case eAosOpr_range_gt_le:
		case eAosOpr_range_gt_lt:
			opr = "in";
			cond->addQueryFilter(mRundata, cond2, -1);
			break;
		default:
			aos_assert_r(NULL, false);
			break;
		}
	}
	else
	{
		switch (opr2)
		{
			case eAosOpr_gt:
				str = "gt";
			case eAosOpr_ge:
				if (str == "") str = "ge";
			case eAosOpr_lt:
				if (str == "") str = "lt";
			case eAosOpr_le:
				if (str == "") str = "le";
				opr = "in";
				for (u32 i = 0; i < cond->mValues.size(); i++)
				{
					if (value != "") value << ";";
					value << str << ","
						<< cond->mValues[i] << mSep << cond2->mRHS ;
					if (!is_last) value << mSep;
				}
				break;
			case eAosOpr_eq:
			{
				opr = "in";
				vector<OmnString> values;
				for (u32 i = 0; i < cond->mValues.size(); i++) {
					OmnString value1 ;
					value1 << cond->mValues[i] << mSep << cond2->mRHS;
					values.push_back(value1);
					value << "pf," << cond->mValues[i] << mSep << cond2->mRHS ;
					if (!is_last) value << mSep;
					value << ";";
				}
				cond->mValues = values;
				break;
			}

			case eAosOpr_in:
				opr = "in";
				//cond->mValues.insert(cond->mValues.end(), cond2->mValues.begin(), cond2->mValues.end());
				for (u32 i = 0; i < cond->mValues.size(); i++) {
					for (u32 j = 0; j < cond2->mValues.size(); j++)
					{
						value << "pf," << cond->mValues[i] << mSep << cond2->mValues[j];
						if (!is_last) value << mSep;
						value << ";";
					}
				}
				break;
			case eAosOpr_an:
			case eAosOpr_like:
			case eAosOpr_not_like:
			case eAosOpr_null:
			case eAosOpr_not_null:
			case eAosOpr_not_in:
				opr = "in";
				for (u32 i = 0; i < cond->mValues.size(); i++) {
					value << "pf," << cond->mValues[i];
					if (!is_last) value << mSep;
					value << ";";
				}
				idx = AosJqlQueryWhereConds::getFieldIdx(mRundata, cond->mIILName, cond2->mLHS);
				cond->addQueryFilter(mRundata, cond2, idx);
				break;
			case eAosOpr_range_ge_le:
				opr = "r1";
				for (u32 i = 0; i < cond->mValues.size(); i++) {
					value << opr <<","
						<< cond->mValues[i] << mSep << cond2->mRHS  << ","
						<< cond->mValues[i] << mSep << cond2->mValue2 ;
					if (!is_last) value << mSep;
					value << ";";
				}
				opr = "in";
				break;
			case eAosOpr_range_ge_lt:
				opr = "r2";
				for (u32 i = 0; i < cond->mValues.size(); i++) {
					value << opr <<","
						<< cond->mValues[i] << mSep << cond2->mRHS  << ","
						<< cond->mValues[i] << mSep << cond2->mValue2 ;
					if (!is_last) value << mSep;
					value << ";";
				}
				opr = "in";
				break;
			case eAosOpr_range_gt_le:
				opr = "r3";
				for (u32 i = 0; i < cond->mValues.size(); i++) {
					value << opr <<","
						<< cond->mValues[i] << mSep << cond2->mRHS  << ","
						<< cond->mValues[i] << mSep << cond2->mValue2 ;
					if (!is_last) value << mSep;
					value << ";";
				}
				opr = "in";
				break;
			case eAosOpr_range_gt_lt:
				opr = "r4";
				for (u32 i = 0; i < cond->mValues.size(); i++) {
					value << opr <<","
						<< cond->mValues[i] << mSep << cond2->mRHS  << ","
						<< cond->mValues[i] << mSep << cond2->mValue2;
					if (!is_last) value << mSep;
					value << ";";
				}
				opr = "in";
				break;
			default:
				aos_assert_r(NULL, false);
				break;
		}

		if (!is_last && opr != "in")
		{
			if (value != "") value << mSep;
			if (value2 != "") value2 << mSep;
		}

		if (is_last && opr == "pf" && (cond->mQueryFilters.empty()))
		{
			opr = "eq";
		}
	}
	cond->mOpr = opr;
	cond->mRHS = value;
	cond->mValue2 = value2;
	cond->mCondStr = cond_str;
	cond->mIsMegre = true;
	return true;
}

bool
AosJqlQueryWhereConds::like_all(AosJqlQueryWhereCondPtr &cond, AosJqlQueryWhereCondPtr &cond2)
{
	aos_assert_r(cond && cond2, false);
	OmnString opr, value, value2, cond_str, str, str1;
	opr = value = value2 = cond_str = "";

	AosOpr opr1 = AosOpr_toEnum(cond->mOpr);
	aos_assert_r(opr1 == eAosOpr_like  || opr1 == eAosOpr_not_like, false);

	AosOpr opr2 = AosOpr_toEnum(cond2->mOpr);
	if (cond->mLHS == cond2->mLHS)
	{
		switch (opr2)
		{
		case eAosOpr_gt:
			return gt_all(cond2, cond);
		case eAosOpr_ge:
			return ge_all(cond2, cond);
		case eAosOpr_eq:
			return eq_all(cond2, cond);
		case eAosOpr_lt:
			return lt_all(cond2, cond);
		case eAosOpr_le:
			return le_all(cond2, cond);
		case eAosOpr_ne:
			return ne_all(cond2, cond);
		case eAosOpr_in:
			opr = "in";
			cond->mValues.insert(cond->mValues.end(), cond2->mValues.begin(), cond2->mValues.end());
			break;
		case eAosOpr_like:
		case eAosOpr_not_like:
		case eAosOpr_null:
		case eAosOpr_not_null:
		case eAosOpr_not_in:
			opr = "an";
			value = "*";
			cond_str << cond->toString(mRundata);
			cond_str << " and ";
			cond_str << cond2->toString(mRundata);
			break;
		default:
			aos_assert_r(NULL, false);
			break;
		}
		cond->mOpr = opr;
		cond->mRHS = value;
		cond->mValue2 = value2;
		cond->mCondStr = cond_str;
		cond->mIsMegre = true;
		return true;
	}
	return false;


}

bool
AosJqlQueryWhereConds::ne_all(AosJqlQueryWhereCondPtr &cond, AosJqlQueryWhereCondPtr &cond2)
{
	aos_assert_r(cond && cond2, false);
	OmnString opr, value, value2, cond_str, str, str1;
	opr = value = value2 = cond_str = "";

	AosOpr opr1 = AosOpr_toEnum(cond->mOpr);
	aos_assert_r(opr1 == eAosOpr_ne, false);

	AosOpr opr2 = AosOpr_toEnum(cond2->mOpr);
	if (cond->mLHS == cond2->mLHS)
	{
		switch (opr2)
		{
		case eAosOpr_gt:
			return gt_all(cond2, cond);
		case eAosOpr_ge:
			return ge_all(cond2, cond);
		case eAosOpr_eq:
			return eq_all(cond2, cond);
		case eAosOpr_lt:
			return lt_all(cond2, cond);
		case eAosOpr_le:
			return le_all(cond2, cond);
		case eAosOpr_in:
			opr = "in";
			cond->mValues.insert(cond->mValues.end(), cond2->mValues.begin(), cond2->mValues.end());
			break;
		case eAosOpr_like:
		case eAosOpr_not_like:
		case eAosOpr_not_in:
		case eAosOpr_null:
		case eAosOpr_not_null:
		case eAosOpr_ne:
			opr = "an";
			value = "*";
			cond_str << cond->toString(mRundata);
			cond_str << " and ";
			cond_str << cond2->toString(mRundata);
			break;
		default:
			aos_assert_r(NULL, false);
			break;
		}
		cond->mOpr = opr;
		cond->mRHS = value;
		cond->mValue2 = value2;
		cond->mCondStr = cond_str;
		cond->mIsMegre = true;
		return true;
	}
	return false;
}


bool
AosJqlQueryWhereConds::other_all(AosJqlQueryWhereCondPtr &cond, AosJqlQueryWhereCondPtr &cond2)
{
	aos_assert_r(cond && cond2, false);
	OmnString opr, value, value2, cond_str, str, str1;
	opr = value = value2 = cond_str = "";

	opr = "an";
	value = "*";
	if (cond->mLHS == cond2->mLHS)
	{
		cond_str << cond->toString(mRundata);
		cond_str << " and ";
		cond_str << cond2->toString(mRundata);
		cond->mCondStr = cond_str;
	}
	else
	{
		if (cond2->mCMPIILName== "")
		{
			cond2->mCMPIILName = cond->mIILName;
		}
		int idx = AosJqlQueryWhereConds::getFieldIdx(mRundata, cond2->mCMPIILName, cond->mLHS);
		cond->addQueryFilter(mRundata, cond, idx);
		idx = AosJqlQueryWhereConds::getFieldIdx(mRundata, cond2->mCMPIILName, cond2->mLHS);
		cond->addQueryFilter(mRundata, cond2, idx);
	}

	cond->mOpr = opr;
	cond->mRHS = value;
	cond->mIsMegre = true;
	return true;
}


bool
AosJqlQueryWhereConds::checkCondsDataType(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &tabledoc,
		vector<AosJqlQueryWhereCondPtr> &conds)

{
	aos_assert_r(tabledoc, false);
	OmnString schema_objid = tabledoc->getAttrStr("zky_use_schema");
	AosXmlTagPtr schemaDoc = AosGetDocByObjid(schema_objid, rdata);
	aos_assert_r(schemaDoc, false);
	AosXmlTagPtr recordNode = schemaDoc->getFirstChild("datarecord");
	aos_assert_r(schemaDoc, false);

	//Jackie 2015-09-07 JIMODB-681
	if (mTableDoc && mTableDoc->getAttrStr("zky_data_format") == "xml") return true;

	AosDataRecordObjPtr record = AosDataRecordObj::createDataRecordStatic(recordNode, 0, rdata.getPtr() AosMemoryCheckerArgs);
	aos_assert_r(record, false);
	for (size_t i=0; i<conds.size(); i++)
	{
		aos_assert_r(conds[i]->mExpr, false);
		AosDataType::E type = conds[i]->mExpr->getDataType(rdata.getPtr(), record.getPtr());
		if (type == AosDataType::eInvalid) return false;
	}
	return true;
}
