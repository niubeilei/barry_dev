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
// 2014/05/08	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlStatQuery.h"

#include "AosConf/DatasetStat.h" 
#include "AosConf/DataFieldExpr.h" 
#include "AosConf/DataConnectorStatQuery.h"         

#include "JQLStatement/JqlQueryWhereConds.h"

#include "API/AosApi.h"
#include "SEUtil/IILName.h"
#include "SEUtil/JqlUtil.h"
#include "StatUtil/Statistic.h"
#include "StatUtil/Jimos/StatModel.h"
#include "StatUtil/Jimos/StatTime.h"
#include "Util/DataTypes.h"

#include <stdint.h>
#include <algorithm>

//vector<AosStatisticPtr> AosJqlStatQuery::mAllStats;


AosJqlStatQuery::AosJqlStatQuery()
{
}


AosJqlStatQuery::~AosJqlStatQuery()
{
}


bool
AosJqlStatQuery::checkIsStatQuery(
		const AosRundataPtr &rdata,
		const AosJqlSelectTablePtr &base_table,
		vector<AosXmlTagPtr> &all_stat_docs)
{
	bool rslt;

	vector<AosStatisticPtr> all_stats;
	initStatistics(rdata, all_stat_docs, all_stats);
	vector<OmnString> mea_grpby_fields;
	vector<AosJqlSelectFieldPtr> norm_fields;
	vector<AosJqlSelectFieldPtr> measure_fields;
	rslt = analyzeQryFields(rdata, base_table->mOutputFields,
			base_table->mGroupByFields, base_table->mOrderByFields,
			norm_fields, measure_fields, false);
	if (!rslt)
	{
		return false;
	}
	//arvin 2015.11.03
	//JIMODB-1105
	//We want to short cut "select count(*) from t" to use query 
	//instead of statistics model.
	if(base_table->mOutputFields.size()== 1 && measure_fields.size() == 1
			&& !base_table->mGroupByFields.size() && !base_table->mOrderByFields.size()&& !norm_fields.size())
	{
		OmnString select_field = (base_table->mOutputFields[0])->dumpByNoEscape();
		if(select_field == "count(*)")
			return false;
	}                                                                                                            

	if(measure_fields.empty())
	{
		if (!base_table->mGroupByFields.empty() || 
				!base_table->mCubeFields.empty() ||
				!base_table->mRollupLists.empty())
		{
			//this might be a "select distinct without measures". Add a 
			//count(*) measure
			measure_fields.push_back(genCountField(rdata));
		}
		else
		{
			OmnScreen << "!!!!! check stat false. no meaure_field and group by" << endl; 
			return false;
		}
	}

	bool picked;
	AosStatTimeUnit::E time_unit;
	for(u32 idx = 0 ; idx < measure_fields.size(); idx++)
	{
		picked = false;
		for(u32 i=0; i<all_stats.size(); i++)
		{
			if(checkIsPickStat(rdata, all_stats[i], 
						measure_fields[idx], norm_fields, base_table->mConds))
			{
				picked = true;
				break;
			}
		}
		if(!picked)
		{
			OmnScreen << "!!!!! check stat. pick false." << endl; 
			return false;
		}
	}
	return true;
}


bool
AosJqlStatQuery::checkIsPickStat(
		       const AosRundataPtr &rdata,
               const AosStatisticPtr &stat,
		       const AosJqlSelectFieldPtr &measure_field,
		       vector<AosJqlSelectFieldPtr> &norm_fields,
		       vector<AosJqlQueryWhereCondPtr> &qry_conds)
{
	aos_assert_r(stat, false);
	OmnString fname;

	AosStatFieldPtr m_field = getStatField(rdata, measure_field);
	//check measure_field
	if(!stat->isContainMeasure(m_field->getFieldName()))
	{
		OmnScreen << "Stat Check False. "
			<< "stat_internal_key:" << stat->getIdentifyKey() << "; "
			<< "measure_field:" << measure_field->dumpByNoEscape()
			<< endl;
		return false;
	}
	// check norm fields.
	for(u32 i=0; i<norm_fields.size(); i++)
	{
		fname = norm_fields[i]->getFieldEscapeOName(rdata);
		if(stat->isContainKeyField(fname))	continue;
		if(stat->isContainTimeField(fname))	continue;
		
		// maybe date_day(xxx)
		OmnString time_unit_str = norm_fields[i]->getFuncName();
		AosStatTimeUnit::E time_unit =  AosStatTimeUnit::getTimeUnit(time_unit_str);
		if (time_unit != AosStatTimeUnit::eInvalid && time_unit != AosStatTimeUnit::eAllTime )
		{
			vector<OmnString> fnames = norm_fields[i]->getFieldNames(rdata);
			if (fnames.size() == 1)
			{
				OmnString time_fname = fnames[0];
				AosJqlUtil::escape(time_fname);
				if(stat->isContainTimeField(time_fname, time_unit))
				{
					continue;
				}
			}
		}

		//failed to recognize the normal field, report error
		OmnScreen << "Stat Check False. "
			<< "stat_internal_key:" << stat->getIdentifyKey() << "; "
			<< "field:" << norm_fields[i]->dumpByNoEscape()
			<< endl;
		return false;
	}

	// check qry conds.
	for(u32 i=0; i<qry_conds.size(); i++)
	{
		OmnString fname = qry_conds[i]->getLeftValue();
		OmnString field_name = fname;
		fname = getFieldName(field_name);

		if(stat->isContainKeyField(fname))	continue;
		if(stat->isContainTimeField(fname))	continue;

		OmnScreen << "Stat Check False."
			<< "stat_internal_key:" << stat->getIdentifyKey() << "; "
			<< "qry cond:" << qry_conds[i]->toString(rdata) << endl;
		return false;
	}
	return true;

}

bool
AosJqlStatQuery::analyzeMeasureFields(
		const AosRundataPtr &rdata,
		const vector<AosJqlSelectFieldPtr> &measure_fields,
		vector<OmnString> &mea_grpby_fields)
{
	for(size_t i = 0;i < measure_fields.size();i++)
	{
		if(measure_fields[i]->isConditionMeasure())
		{
			vector<AosJqlQueryWhereCondPtr> conds;
			AosExprObjPtr expr = measure_fields[i]->getField();
			vector<AosExprObjPtr> exprList = *(expr->getParmList());
			OmnString field = exprList[0]->dumpByNoEscape();
			mea_grpby_fields.push_back(field);
			exprList[1]->createConds(rdata,conds,"");
			for(size_t j = 0;j < conds.size();j++)
			{
				OmnString key = conds[j]->getLeftValue();
				OmnString fname = key;
				key = getFieldName(fname);
				mea_grpby_fields.push_back(key);
			}
		}
	}
	return true;
}

bool
AosJqlStatQuery::init(
		const AosRundataPtr &rdata,
		const OmnString &stat_objid,
		vector<AosJqlSelectFieldPtr> &select_fields,
		vector<AosJqlSelectFieldPtr>  &grpby_fields,
		vector<AosJqlSelectFieldPtr>  &cube_grpby_fields,
		vector< vector<AosJqlSelectFieldPtr> > &rollup_grpby_fields_grp,
		vector<AosJqlQueryWhereCondPtr> &qry_conds,
		vector<AosJqlOrderByFieldPtr> &orderby_fields,
		AosJqlHavingPtr &having_conds,
		AosJqlLimitPtr &limit,
		vector<AosXmlTagPtr> &all_stat_docs)
{
	bool rslt;
	
	vector<AosStatisticPtr> all_stats;
	initStatistics(rdata, all_stat_docs, all_stats);

	mSelectFields = select_fields;
	mGrpbyFields = grpby_fields;
	mCubeGrpbyFields = cube_grpby_fields;
	mRollupGrpbyFieldsGrp = rollup_grpby_fields_grp;
	mQryConds = qry_conds;
	mOrderbyFields = orderby_fields;
	mHavingPtr = having_conds;
	mLimitPtr = limit;
	mTimeUnit = AosStatTimeUnit::eInvalid;  
#if  0
	//arvin 2015.08.14
	//JIMODB-422
	if(!syntaxCheck(rdata))
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	mBasicStat = getBasicStat(all_stats);
	//yang
	bool* statmatch = new bool[all_stats.size()];
	for(u32 i = 0; i < all_stats.size(); i++)
	{
		statmatch[i] = true;
	}
	for(u32 i=0;i<all_stats.size();i++)
	{
		AosStatisticPtr stat = all_stats[i];
		for(u32 j=0; j<mQryConds.size(); j++)
		{
			OmnString fname = mQryConds[j]->getLeftValue();
			//vector<AosJqlSelectFieldPtr> fields;
			//mQryConds[j]->getFields(rdata,fields);
			//OmnString fname = qry_conds[i]->getFieldName(rdata);
			OmnString field_name = fname;
			fname = getFieldName(field_name);

			if(stat->isContainKeyField(fname) || stat->isContainTimeField(fname))
			{
				//match, do nothing
			}
			else
			{
				//don't match,exclude it
				statmatch[i] = false;
			}
		}
	}
	vector<AosStatisticPtr> matched_stats;
	for(u32 i=0;i<all_stats.size();i++)
		if(statmatch[i])
			matched_stats.push_back(all_stats[i]);
	//now got all stat models matching the query conditons,
	//so now just pick a perfect one
	mBasicStat = getBasicStat(matched_stats);

	delete []statmatch;


	if (!mBasicStat)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
	return false;
	}
#endif

	//if cube group by field is not empty, add it to groupby
	//field list
	if (!cube_grpby_fields.empty())
	{
		for (u32 i = 0; i < cube_grpby_fields.size(); i++)
			grpby_fields.push_back(cube_grpby_fields[i]);
	}

	rslt = pickPerfectStat(rdata, all_stats, select_fields, 
			grpby_fields, qry_conds, orderby_fields, all_stat_docs);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	
	for(u32 i=0; i<rollup_grpby_fields_grp.size(); i++)
	{
		rslt = pickPerfectStat(rdata, all_stats, select_fields, 
				rollup_grpby_fields_grp[i], qry_conds, orderby_fields, all_stat_docs);
		if (!rslt)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
	}

	return true;
}


AosStatisticPtr
AosJqlStatQuery::getBasicStat(vector<AosStatisticPtr> &all_stats)
{
	int max_idx = -1;
	u32 max_key_num = 0;
	for(u32 i=0; i<all_stats.size(); i++)
	{
		if(all_stats[i]->getStatKeyNum() >= max_key_num)
		{
			max_key_num = all_stats[i]->getStatKeyNum();
			max_idx = i;
		}
	}
	
	if(max_idx == -1)	return 0;
	return all_stats[max_idx];
}


bool
AosJqlStatQuery::pickPerfectStat(
		const AosRundataPtr &rdata,
		vector<AosStatisticPtr> &all_stats,
		vector<AosJqlSelectFieldPtr> &select_fields,
		vector<AosJqlSelectFieldPtr> &grpby_fields,
		vector<AosJqlQueryWhereCondPtr> &qry_conds,
		vector<AosJqlOrderByFieldPtr> &orderby_fields,
		vector<AosXmlTagPtr> &all_statDocs)

{
	bool rslt;
	
	vector<AosJqlSelectFieldPtr> norm_fields;
	vector<AosJqlSelectFieldPtr> measure_fields;
	rslt = analyzeQryFields(rdata, select_fields, grpby_fields,
			orderby_fields, norm_fields, measure_fields, true);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	//if no actural measure field, add a count(*)
	if (measure_fields.empty())
	{
		//this might be a "select distinct without measures". Add a 
		//count(*) measure
		measure_fields.push_back(genCountField(rdata));
	}
	//arvin 2015.09.11, get Measure Condition KeyFields
	vector<OmnString> mea_grpby_fields;
	rslt = analyzeMeasureFields(rdata,measure_fields,mea_grpby_fields);
	aos_assert_r(rslt,false);

	AosJqlSelectFieldPtr m_field;
	AosStatisticPtr picked_stat = 0;
	vector<AosStatisticPtr> match_stats;
	AosStatisticPtr stat;
	OmnString measureScreen = "\nmeasure:",time_unit_str;
	AosStatTimeUnit::E ptime_unit = AosStatTimeUnit::eInvalid;
	AosStatTimeUnit::E time_unit = ptime_unit;
	bool multiStatQuery =  false;
	for(u32 m_idx = 0; m_idx<measure_fields.size(); m_idx++)
	{
		m_field = measure_fields[m_idx];	
		if(!picked_stat && !multiStatQuery)
		{
			for(size_t i = 0;  i < all_stats.size(); i++)
			{
				stat = all_stats[i];
				if(!checkIsPickOneStat(rdata, stat,
							measure_fields, norm_fields, qry_conds,mTimeUnit))
				{
					continue;
				}
				match_stats.push_back(stat);
			}
			for(size_t i = 0;i < match_stats.size(); i++)
			{
				stat = match_stats[i];
				if(!picked_stat)
				{
					picked_stat = stat;
					time_unit_str = picked_stat->getTimeUnit();
					ptime_unit =  AosStatTimeUnit::getTimeUnit(time_unit_str);
					continue;
				}
				time_unit_str = stat->getTimeUnit();
				time_unit = AosStatTimeUnit::getTimeUnit(time_unit_str);
				int pkey_num = picked_stat->getStatKeyNum();
				int key_num = stat->getStatKeyNum();
				if((ptime_unit <time_unit && pkey_num == key_num)
						|| (ptime_unit == time_unit && pkey_num > key_num)
						|| pkey_num > key_num)
				{
					picked_stat = stat;
					ptime_unit = time_unit;	
				}
			}
			if(!picked_stat)
				multiStatQuery = true;
			measureScreen << m_field->dumpByNoEscape() << " ";
		}
		if(multiStatQuery)
		{
			for(size_t i = 0;  i < all_stats.size(); i++)
			{
				if(checkIsPickStat(rdata, all_stats[i],
							m_field, norm_fields, qry_conds))
				{
					picked_stat = all_stats[i];
					break;
				}
			}
		}
		addToMeasureGrps(picked_stat, m_field, grpby_fields);
	}

	mBasicStat = picked_stat;
	
	OmnScreen << "\n==================MatchStatModel========================="
		<< "\npick perfect stat. pick succ. the perfected stat is:"
		<< "\nstat_model:" << picked_stat->getStatName() << ";"
		<< "\nstat_internal_key:" << picked_stat->getIdentifyKey() << "; "
		<< measureScreen << endl;
	return true;


}


bool
AosJqlStatQuery::initStatistics(
		const AosRundataPtr &rdata,
		vector<AosXmlTagPtr> &all_stat_docs,
		vector<AosStatisticPtr> &all_stats)
{
	bool rslt;
	AosStatisticPtr crt_stat;
	for(u32 i=0; i<all_stat_docs.size(); i++)
	{
		crt_stat = OmnNew AosStatistic();
		rslt = crt_stat->config(rdata, all_stat_docs[i]);
		if (!rslt)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		all_stats.push_back(crt_stat);
	}
	return true;
}


bool
AosJqlStatQuery::analyzeQryFields(
		const AosRundataPtr &rdata,
		vector<AosJqlSelectFieldPtr> &raw_select_fields,
		vector<AosJqlSelectFieldPtr> &raw_grpby_fields,
		vector<AosJqlOrderByFieldPtr> &raw_orderby_fields,
		vector<AosJqlSelectFieldPtr> &norm_fields,
		vector<AosJqlSelectFieldPtr> &measure_fields,
		bool	checkFlag)
{
	bool rslt;
	vector<AosJqlSelectFieldPtr> tmp_complex_fields;
	
	// analyze select fields.
	for(u32 i=0; i<raw_select_fields.size(); i++)
	{
		analyzeField(rdata, raw_select_fields[i],
				norm_fields, measure_fields, tmp_complex_fields, true);

		//if select field has time unit ,it must be appear in group fields
		if(checkFlag)
		{
			rslt = isInGrpbyFields(rdata, raw_select_fields[i], raw_grpby_fields);
			aos_assert_r(rslt, false);
		}
	}
	
	// analyze grpby fields.
	vector<AosJqlSelectFieldPtr> tmp_measure_fields;
	tmp_complex_fields.clear();
	for(u32 i=0; i<raw_grpby_fields.size(); i++)
	{
		analyzeField(rdata, raw_grpby_fields[i],
				norm_fields, tmp_measure_fields, tmp_complex_fields, true);
		if(tmp_measure_fields.size() != 0 ||
				tmp_complex_fields.size() != 0)
		{
			OmnScreen << "Stat Check False. grpby field error."
				<< raw_grpby_fields[i]->dumpByNoEscape()
				<< endl;
			return false;
		}
	}

	// analyze order by fields.
	tmp_complex_fields.clear();
	for(u32 i=0; i<raw_orderby_fields.size(); i++)
	{
		analyzeField(rdata, raw_orderby_fields[i]->mField,
				norm_fields, measure_fields, tmp_complex_fields, true);
		if(tmp_complex_fields.size() != 0)
		{
			OmnScreen << "Stat Check False. orderby field not support."
				<< raw_orderby_fields[i]->mField->dumpByNoEscape()
				<< endl;
			return false;
		}
	}

	//Jackie 2015/07/30 JIMODB-94
	if(measure_fields.size() == 0 && raw_grpby_fields.size()>0)
	{
		OmnString msg = "[ERR] : unsupport this jql format";
		rdata->setJqlMsg(msg);
		return false;
	}
	
	return true;
}


bool
AosJqlStatQuery::analyzeField(
		const AosRundataPtr &rdata,
		const AosJqlSelectFieldPtr &raw_field,
		vector<AosJqlSelectFieldPtr> &norm_fields,
		vector<AosJqlSelectFieldPtr> &measure_fields,
		vector<AosJqlSelectFieldPtr> &complex_fields,
		const bool ignore_timeunit_field)
{
	//bool rslt;

	//Phil 12/30/2014 use original name to know
	//field types
	OmnString oname = raw_field->getFieldEscapeOName(rdata);
	aos_assert_r(oname != "", false);
	if ( oname.indexOf("0x28", 0) > -1 && 
			raw_field->mExpr->getType() == AosExprType::eFieldName)
	{
		AosExprObjPtr expr;
		oname << ";";
		AosJqlUtil::unescape(oname);
		// Chen Ding, 2015/01/31
		// expr = AosParseJQL(oname, false, rdata.getPtr());
		OmnString errmsg;
		expr = AosParseExpr(oname, errmsg, rdata.getPtr());
		aos_assert_r(expr, false);
		raw_field->mExpr = expr;
		//raw_field->mCName = raw_field->mOName;
	}

	if(raw_field->isName())
	{
		addToFieldVector(norm_fields, raw_field);
		return true;
	}
	
	if(raw_field->checkIsAgrFunc())
	{
		addToFieldVector(measure_fields, raw_field);
		return true;
	}
	
	if(ignore_timeunit_field && raw_field->checkIsFunc())
	{
		OmnString func_str = raw_field->getFuncName();
		if(AosStatTimeUnit::isTimeUnitStr(func_str))
		{
			// maybe date_day(xxx)
			addToFieldVector(norm_fields, raw_field);
			return true;
		}
	}

	// maybe round(sum(xxx)). 
	// maybe sum(xxx) / count(xxx).	
	vector<AosJqlSelectFieldPtr> sub_fields;
	raw_field->getFieldsByStat(rdata, sub_fields);
	for(u32 i=0; i<sub_fields.size(); i++)
	{
		analyzeField(rdata, sub_fields[i], norm_fields,
				measure_fields, complex_fields, ignore_timeunit_field);
	}
	addToFieldVector(complex_fields, raw_field);
	return true;
}


bool
AosJqlStatQuery::addToFieldVector(
		vector<AosJqlSelectFieldPtr> &vt_field,
		const AosJqlSelectFieldPtr &crt_field)
{
	OmnString fname = crt_field->dumpByNoEscape();
	for(u32 i = 0; i<vt_field.size(); i++)
	{
		if(vt_field[i]->dumpByNoEscape() == fname)	return true;	
	}
	
	// not exist.
	vt_field.push_back(crt_field);
	return true;
}

//arvin 2015.11.04
//JIMODB-1084
//if you want to support multi_stat query,
//you must pass "measure_field",otherwise please pass "measure_fields"
bool
AosJqlStatQuery::checkIsPickOneStat(
		const AosRundataPtr &rdata,
		const AosStatisticPtr &stat,
		const vector<AosJqlSelectFieldPtr> & measure_fields,
		const vector<AosJqlSelectFieldPtr> &norm_fields,
		const vector<AosJqlQueryWhereCondPtr> &qry_conds,
		AosStatTimeUnit::E &time_unit)
{
	aos_assert_r(stat, false);
	OmnString fname;

	// check measure_field
	for(size_t i = 0;i < measure_fields.size(); i++)
	{
		AosStatFieldPtr m_field = getStatField(rdata, measure_fields[i]);
		if(!stat->isContainMeasure(m_field->getFieldName()))
		{
			OmnScreen << "Stat Check False. "
				<< "stat_internal_key:" << stat->getIdentifyKey() << "; "
				<< "measure_field:" << measure_fields[i]->dumpByNoEscape()
				<< endl;
			return false;
		}
	}

	// check norm fields.
	for(u32 i=0; i<norm_fields.size(); i++)
	{
		fname = norm_fields[i]->getFieldEscapeOName(rdata);
		if(stat->isContainKeyField(fname))	continue;
		if(stat->isContainTimeField(fname))	continue;
		
		// maybe date_day(xxx)
		OmnString time_unit_str = norm_fields[i]->getFuncName();
		time_unit =  AosStatTimeUnit::getTimeUnit(time_unit_str);
		if (time_unit != AosStatTimeUnit::eInvalid && time_unit != AosStatTimeUnit::eAllTime )
		{
			vector<OmnString> fnames = norm_fields[i]->getFieldNames(rdata);
			if (fnames.size() == 1)
			{
				OmnString time_fname = fnames[0];
				AosJqlUtil::escape(time_fname);
				if(stat->isContainTimeField(time_fname, time_unit))
				{
					continue;
				}
			}
		}

		//failed to recognize the normal field, report error
		OmnScreen << "Stat Check False. "
			<< "stat_internal_key:" << stat->getIdentifyKey() << "; "
			<< "field:" << norm_fields[i]->dumpByNoEscape()
			<< endl;
		return false;
	}

	// check qry conds.
	for(u32 i=0; i<qry_conds.size(); i++)
	{
		OmnString fname = qry_conds[i]->getLeftValue();
		OmnString field_name = fname;
		fname = getFieldName(field_name);

		if(stat->isContainKeyField(fname))	continue;
		if(stat->isContainTimeField(fname))	continue;

		OmnScreen << "Stat Check False."
			<< "stat_internal_key:" << stat->getIdentifyKey() << "; "
			<< "qry cond:" << qry_conds[i]->toString(rdata) << endl;
		return false;
	}
	return true;
}


AosStatFieldPtr
AosJqlStatQuery::getStatField(
		const AosRundataPtr &rdata,
		const AosJqlSelectFieldPtr &field)
{
	if(field->isName())
	{
		OmnString fname = field->getFieldEscapeOName(rdata);
		OmnString tableName = field->getTableName();
		aos_assert_r(tableName != "", NULL);
		//AosJqlStatementPtr jqlStat = OmnNew AosJqlStatement();
		//AosXmlTagPtr dataFieldXml = jqlStat->getTableDataField(rdata, tableName, field);
		AosJqlColumnPtr column = AosJqlStatement::getTableField(rdata, tableName, fname);
		//aos_assert_r(dataFieldXml, NULL);
		aos_assert_r(column, NULL);
		//OmnString fieldType = dataFieldXml->getAttrStr("type", "");
		AosDataType::E type = column->type;
		//AosDataType::E type = AosDataType::toEnum(fieldType);
		AosStatFieldPtr key_field = OmnNew AosStatKeyField(fname, type);
		return key_field;
	}

	if(field->checkIsAgrFunc())
	{
		OmnString aggr_func_str = field->getAgrFuncTypeStr();
		if (aggr_func_str == "") return 0;


		OmnString fname, raw_fname;
		OmnString aggrStr = aggr_func_str.toLower();
		raw_fname = field->getFieldName(rdata);
		if(aggrStr == "accu_count")
		{
			//yang 2015-4-3,fix bug
			fname = aggrStr;
			fname << "(*)";
			raw_fname = "*";
			//fname = field->mCName;
			//raw_fname = field->mOName;
		}
		else
		{
			if(field->isConditionMeasure())
			{
				fname = field->getMeasureName();
			}
			else
			{
				OmnString func_name = field->mExpr->getFuctName().toLower();
				fname= field->dumpByStat();
				fname.replace(0,func_name.length(),func_name);
			}
			//raw_fname = field->getFieldEscapeOName(rdata);
			raw_fname = field->getFieldName(rdata);

		}
		AosJqlUtil::escape(fname);
		AosJqlUtil::escape(raw_fname);
		
		AosStatFieldPtr m_field = OmnNew AosStatMeasureField(fname,
			raw_fname, aggr_func_str);
		return m_field;
	}

	if(field->checkIsFunc())
	{
		OmnString func_str = field->getFuncName();
		if(AosStatTimeUnit::isTimeUnitStr(func_str))
		{
			AosStatTimeUnit::E unit = AosStatTimeUnit::getTimeUnit(func_str);
			
			vector<OmnString> vt_fnames = field->getFieldNames(rdata);
			aos_assert_r(vt_fnames.size() == 1, 0);
			OmnString fname = vt_fnames[0];
			AosJqlUtil::escape(fname);
		
			AosStatFieldPtr time_field = OmnNew AosStatTimeField(unit, fname);
			return time_field;
		}
	}

	return 0;
}


bool
AosJqlStatQuery::addToMeasureGrps(
		const AosStatisticPtr &stat,
		const AosJqlSelectFieldPtr &m_field,
		vector<AosJqlSelectFieldPtr> &grpby_fields)
{
	OmnString crt_stat_name = stat->getStatName();
	int find_idx = -1;
	for(u32 i=0; i<mQryMeasureGrps.size(); i++)
	{
		if(mQryMeasureGrps[i].mStat->getStatName() == crt_stat_name)
		{
			find_idx = i;
			break;
		}
	}
	
	if(find_idx == -1)
	{
		MeasureGroup mg;
		mg.mStat = stat; 
		mg.mMeasureFields.push_back(m_field);
		mg.mGrpbyFields = grpby_fields;
		mQryMeasureGrps.push_back(mg);
		return true;
	}
	
	MeasureGroup & m_grp = mQryMeasureGrps[find_idx];
	if(m_grp.mGrpbyFields.size() < grpby_fields.size())
	{
		// this must has roll up group by.
		m_grp.mGrpbyFields = grpby_fields;
	}

	OmnString m_fname = m_field->dumpByNoEscape();
	for(u32 i = 0; i<m_grp.mMeasureFields.size(); i++)
	{
		if(m_grp.mMeasureFields[i]->dumpByNoEscape() == m_fname)
		{
			// this must has roll up group by.
			return true;
		}
	}
	m_grp.mMeasureFields.push_back(m_field);
	return true;
}


OmnString
AosJqlStatQuery::generateTableConf(
		const AosRundataPtr &rdata,
		const OmnString &table_name,
		AosJqlSelectTablePtr &table)
{
	if (table_name == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return "";
	}

	OmnString table_str = "<table ";
	table_str << "zky_type=\"single\" "
			  << "zky_name=\"" << table_name<<"\" "
			  << "jimo_name=\"jimo_dataset\" "
			  << "jimo_type=\"jimo_dataset\" "
			  << "current_version=\"0\" "
			  << "zky_otype=\"zkyotp_jimo\" "
			  << "zky_objid = \"AosDatasetByStatQuerySingle\" "		// this is jimo objid.
			  << "zky_classname=\"AosDatasetByStatQuerySingle\" "
			  << ">";
	table_str << "<versions><ver_0>libDatasetJimos.so</ver_0></versions>";

	bool rslt = generateStatQryInfoConf(rdata, table_str);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return "";
	}

	OmnString scanner_str = generateDataScannerConf(rdata);
	if (scanner_str == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return "";
	}
	table_str << scanner_str;

	OmnString input_rcd_str = generateInputRecordConf(rdata,table_name);
	if (input_rcd_str == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return "";
	}
	
	// jimodb-671
	table->mInputRecord = AosXmlParser::parse(input_rcd_str AosMemoryCheckerArgs);


	table_str << input_rcd_str;

	// jimodb-671 2015/09/14
	OmnString output_rcd_str = table->generateOutputDatasetConf(rdata);
	if (output_rcd_str == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return "";
	}

	table_str << output_rcd_str;

	table_str << "</table>";
	return table_str;
}


bool
AosJqlStatQuery::generateStatQryInfoConf(const AosRundataPtr &rdata, OmnString &str)
{
	u32 page_size = 20;		// Temp page size.
	str << "<page_size size=\"" << page_size << "\"></page_size>";

	
	if(mQryConds.size())
	{
		//AosJqlQueryWhereConds qry_conds = OmnNew AosJqlQueryWhereConds();
		//qry_conds->initByStat(rdata, qry_conds, mBasicStat->getKeyFields());
		//AosXmlTagPtr conds_xml, where_filter;
		//qry_conds->getConfig(rdata, conds_xml, where_filter); 
		//if(conds_xml)	str << conds_xml->toString();
		//if(where_filter) str << where_filter->toString();

		str << "<qry_conds>";
		AosStatCond stat_cond;
		OmnString fname, opr_str;
		for(u32 i=0; i<mQryConds.size(); i++)
		{
			fname = mQryConds[i]->getLeftValue();
//			vector<AosJqlSelectFieldPtr> fields;
//			mQryConds[i]->getFields(rdata,fields);
			//OmnString fname = qry_conds[i]->getFieldName(rdata);
//			OmnString fname = fields[0]->dumpByNoEscape();
			OmnString field_name = fname;
			fname = getFieldName(field_name);

			if(mBasicStat->isContainKeyField(fname))
			{
				stat_cond.mStatField = OmnNew AosStatKeyField(fname);
			}
			else if(mBasicStat->isContainTimeField(fname))
			{
				//AosStatTimeUnit::E unit = AosStatTimeUnit::eInvalid;// Temp
				OmnString timeUnitStr = mBasicStat->getTimeUnit();
				AosStatTimeUnit::E time_unit = AosStatTimeUnit::getTimeUnit(timeUnitStr);
				stat_cond.mStatField = OmnNew AosStatTimeField(time_unit, fname);
		
			}
			else
			{
				OmnAlarm << "Cond Error!" << enderr;
				continue;
			}

			opr_str = mQryConds[i]->getOpr();	
			if (opr_str == "not in")
			{
				stat_cond.mCondOpr = AosOpr_toEnum("in");
				u32 len = mQryConds[i]->mValues.size(); 
				aos_assert_r(!mQryConds[i]->mValues.empty(), false);
				OmnString tmp_value = *(mQryConds[i]->mValues.begin());
				OmnString first_value = *(mQryConds[i]->mValues.begin()); 
				OmnString last_value = mQryConds[i]->mValues[len-1];

				std::stable_sort(mQryConds[i]->mValues.begin(), mQryConds[i]->mValues.end());
				vector<OmnString>::iterator iter = std::unique(mQryConds[i]->mValues.begin(), mQryConds[i]->mValues.end());
				mQryConds[i]->mValues.erase(iter, mQryConds[i]->mValues.end());
				stat_cond.mCondValue1 ="";
				stat_cond.mCondValue1 << "lt," << first_value << ";";
				if (len > 1)
				{
					for (u32 j = 1; j < mQryConds[i]->mValues.size(); j++)
					{
						stat_cond.mCondValue1 << "r4," << tmp_value << ","  << mQryConds[i]->mValues[j] << ";";
						tmp_value = mQryConds[i]->mValues[j];
					}
				}
				stat_cond.mCondValue1 << "gt," << last_value << ";";
				stat_cond.mCondValue2 = "";	// Temp.
				stat_cond.mFiltersConf = "";
				str << stat_cond.toXmlString();
			}
			else
			{
				stat_cond.mCondOpr = AosOpr_toEnum(opr_str);
				//arvin 2015.08.18
				//JIMODB-456
				OmnString value1;
				AosValueRslt v1;
				if(mQryConds[i]->mRHSExpr && mQryConds[i]->mRHSExpr->getDataType(rdata.getPtr(),0) == AosDataType::eDateTime)
				{
					mQryConds[i]->mRHSExpr->getValue(rdata.getPtr(),0,v1);
					//arvin 2015.09.15
					//JIMODB-764
					if(stat_cond.mStatField->mFieldType == AosStatField::eTimeField)
					{
						AosDateTime tt = v1.getDateTime();
						tt.setFormat("%Y-%m-%d %H:%M:%S");	
						i64 time = AosTimeUtil::str2EpochTime(tt.toString());
						value1 << time; 
					}
					else
					{
						value1 << v1.getI64();
					}

				}
				else
				{
					value1 << mQryConds[i]->getRightValue();
				}
				
				stat_cond.mCondValue1 = value1; 
				stat_cond.mCondValue2 = "";	// Temp.
				stat_cond.mFiltersConf = "";
				//arvin 2015.08.19
				//JIMODB-504
				statCondExprProc(stat_cond,opr_str);
				
				str << stat_cond.toXmlString();
			}
		}
		str << "</qry_conds>";
	}

	AosStatFieldPtr stat_field;
	if(mGrpbyFields.size())
	{
		str << "<groupby_fields>";
		for(u32 i=0; i<mGrpbyFields.size(); i++)
		{
			stat_field = getStatField(rdata, mGrpbyFields[i]);
			if(!stat_field)
			{
				OmnScreen << "error!" << mGrpbyFields[i]->dumpByNoEscape()
					<< endl;
				continue;
			}
			str << stat_field->toXmlString();
		}
		str << "</groupby_fields>";
	}
	
	if(!mCubeGrpbyFields.empty())
	{
		str << "<cube_groupby_fields>";
		for(u32 i=0; i<mCubeGrpbyFields.size(); i++)
		{
			stat_field = getStatField(rdata, mCubeGrpbyFields[i]);
			if(!stat_field)
			{
				OmnScreen << "error!" << mCubeGrpbyFields[i]->dumpByNoEscape()
					<< endl;
				continue;
			}

			str << stat_field->toXmlString();
		}
		str << "</cube_groupby_fields>";
	}

	if(mRollupGrpbyFieldsGrp.size())
	{
		for(u32 j = 0; j<mRollupGrpbyFieldsGrp.size(); j++)
		{
			str << "<rollup_groupby_fields>";
			for(u32 i=0; i<mRollupGrpbyFieldsGrp[j].size(); i++)
			{
				stat_field = getStatField(rdata, mRollupGrpbyFieldsGrp[j][i]);
				if(!stat_field)
				{
					OmnScreen << "error!" 
						<< mRollupGrpbyFieldsGrp[j][i]->dumpByNoEscape()
						<< endl;
					continue;
				}
				str << stat_field->toXmlString();
			}
			str << "</rollup_groupby_fields>";
		}
	}

	if(mOrderbyFields.size())
	{
		str << "<orderby_fields>";
		for(u32 i=0; i<mOrderbyFields.size(); i++)
		{
			stat_field = getStatField(rdata, mOrderbyFields[i]->mField);
			if(!stat_field)
			{
				OmnScreen << "error!" 
					<< mOrderbyFields[i]->mField->dumpByNoEscape()
					<< endl;
				continue;
			}
			str << "<orderby_field "
				<< "is_asc=\"" << (mOrderbyFields[i]->mIsAsc ? "true":"false") << "\" >" 
				<< stat_field->toXmlString()
				<< "</orderby_field>";
		}
		
		str << "</orderby_fields>";
	}

	if(mHavingPtr)
	{
		OmnString having_conds = mHavingPtr->getHavingConf();	
		having_conds.escape();
		OmnString meetShuffleFields = "false";
		if(hitShuffleFields(rdata))
		{
			meetShuffleFields = "true";	
		}
		//convert count0x28_rec_count0x29 to count0x280x2a0x29
		having_conds.replace("_rec_count","0x2a",true);
		having_conds << ";";
		str << "<having_conds zky_having_expr=\"" << having_conds << "\" " 
			<< "hit_shuffle_fields=\""<< meetShuffleFields <<"\">"
		    << "</having_conds>";
	}

	// Add by Gavin, 2014/12/19
	if (mLimitPtr)
	{
		u64 limitOffset = mLimitPtr->getOffset();
		u64 limitRawCount = mLimitPtr->getRowCount();
		str << "<limit limit_offset=\"" << limitOffset << "\" " 
				<< "limit_row_count=\"" << limitRawCount << "\">"
			<< "</limit>";
	}
	
	str << "<qry_internal_stats>";
	for(u32 i=0; i<mQryMeasureGrps.size(); i++)
	{
		str << "<stat stat_name=\"" << mQryMeasureGrps[i].mStat->getStatName() << "\" >";
		str << "<measure_fields>";
		for(u32 j=0; j<mQryMeasureGrps[i].mMeasureFields.size(); j++)
		{
			AosJqlSelectFieldPtr measure_field= mQryMeasureGrps[i].mMeasureFields[j];
			stat_field = getStatField(rdata, measure_field);
			OmnString original_name = stat_field->getFieldName();
			//arvin 
			if(measure_field->isConditionMeasure())
			{
				OmnString name = measure_field->getFuncName().toLower();
				AosExprObjPtr expr = measure_field->getField();
				vector<AosExprObjPtr> exprList = *(expr->getParmList());
				OmnString field_name = exprList[0]->dumpByNoEscape();
				name << "0x28" << field_name << "0x29";
				stat_field->setFieldName(name);
				stat_field->setCondExpr(exprList[1]);
			}

			AosDataType::E tp = mQryMeasureGrps[i].mStat->getMeasureDataType(original_name);
			stat_field->setDataType(tp);
			//arvin 2015.07.29
			//JIMODB-114:add actual_data_type to Query XMl
			AosDataType::E type = mQryMeasureGrps[i].mStat->getMeasureType(stat_field->getFieldName());
			stat_field->setType(type);

			str << stat_field->toXmlString();
		}
		str << "</measure_fields>";
		
		str << "<groupby_fields>";
		for(u32 j=0; j<mQryMeasureGrps[i].mGrpbyFields.size(); j++)
		{
			stat_field = getStatField(rdata, mQryMeasureGrps[i].mGrpbyFields[j]);
			str << stat_field->toXmlString();
		}
		str << "</groupby_fields>";
		str << "</stat>";
	}
	str << "</qry_internal_stats>";
	
	return true;
}



OmnString
AosJqlStatQuery::generateDataScannerConf(const AosRundataPtr &rdata)
{
	boost::shared_ptr<DataScanner> data_scanner = boost::make_shared<DataScanner>();
	data_scanner->setAttribute("jimo_objid", "datascanner_cube_jimodoc_v0");  
	
	OmnString name = "";
	name << "datascanner_stat";
	data_scanner->setAttribute("zky_name", name); 
	
	boost::shared_ptr<DataConnectorStatQuery> data_conn = boost::make_shared<DataConnectorStatQuery>();     
	data_conn->setAttribute("doc_type", "stat_doc");
	data_scanner->setConnector(data_conn);
	return data_scanner->getConfig();
}


OmnString
AosJqlStatQuery::generateInputRecordConf(const AosRundataPtr &rdata, const OmnString& table_name)
{
	boost::shared_ptr<DataRecord> drec = boost::make_shared<DataRecord>();
	drec->setTagName("inputrecord");
	drec->setAttribute("zky_name", "stat_rcd_buff");
	drec->setAttribute("type", "buff");
	//bool rslt;
	
	vector<AosJqlSelectFieldPtr> norm_fields;
	vector<AosJqlSelectFieldPtr> measure_fields;
	vector<AosJqlSelectFieldPtr> complex_fields;
	for(u32 i=0; i<mSelectFields.size(); i++)
	{
		analyzeField(rdata, mSelectFields[i],
				norm_fields, measure_fields, complex_fields, false);
	}
	for(u32 i=0; i<norm_fields.size(); i++)
	{
		appendNormFieldPriv(rdata, drec,table_name,norm_fields[i]);	
	}
	
	for(u32 i=0; i<measure_fields.size(); i++)
	{
		appendNormFieldPriv(rdata, drec, table_name,measure_fields[i]);	
	}
	
	return drec->getConfig();
}


bool
AosJqlStatQuery::appendNormFieldPriv(
		const AosRundataPtr &rdata,
		boost::shared_ptr<DataRecord> &drec,
		const OmnString&	tableName,
		const AosJqlSelectFieldPtr &field)
{
	boost::shared_ptr<DataFieldStr> data_field;
	data_field = boost::make_shared<DataFieldStr>();
	//fname like the following format: 
	//	1 : sum(key_field1) 
	//	2 : key_field1
	AosJqlColumnPtr column;
	OmnString fname;
	OmnString tableFieldName = field->getFieldName(rdata);
	AosDataType::E fieldType; 
	OmnString func_name = field->getFuncName();
	
	fname = field->dumpByNoEscape();
	if(field->checkIsAgrFunc())
	{
		fname.replace(0, func_name.length(), func_name.toLower());
	}

	if(func_name.toLower()=="count" || func_name == "dist_count" || func_name.toLower() == "accu_count")
	{
			fieldType = AosDataType::eU64;
	}
	else
	{
		column = AosJqlStatement::getTableField(rdata,tableName,tableFieldName);
		fieldType = column->type;
		if(!field->checkIsAgrFunc())
		{
			if(mBasicStat && mBasicStat->isContainTimeField(tableFieldName))
			{
				convertTimeFormat(rdata,column->format);
			}
		}
	}
	AosJqlUtil::escape(fname);
	switch(fieldType)
	{
		case AosDataType::eInt64:	
			{
				data_field->setAttribute("type", "bin_int64");
				data_field->setAttribute("zky_name", fname);
			}
			break;
		case AosDataType::eU64:	
			{
				data_field->setAttribute("type", "bin_u64");
				data_field->setAttribute("zky_name", fname);
			}
			break;
		case AosDataType::eOmnStr:	
		case AosDataType::eString:
			{
				data_field->setAttribute("type", "str");
				data_field->setAttribute("zky_name", fname);
			}
			break;
		case AosDataType::eNumber:	
			{
				data_field->setAttribute("type", "number");
				data_field->setAttribute("zky_name", fname);
				data_field->setAttribute("zky_precision", column->precision);
				data_field->setAttribute("zky_scale", column->scale);
			}
			break;
		case AosDataType::eDouble:	
			{
				data_field->setAttribute("type", "bin_double");
				data_field->setAttribute("zky_name", fname);
			}
			break;
		case AosDataType::eDateTime:	
			{
				data_field->setAttribute("type", "bin_datetime");
				data_field->setAttribute("zky_name", fname);
				data_field->setAttribute("format", column->format);
			}
			break;
		default:
			{
				data_field->setAttribute("type", "buff");
				data_field->setAttribute("zky_name", fname);
			}
			break;
	}
	drec->setField(data_field);
	return true;
}


bool
AosJqlStatQuery::appendExprFieldPriv(
		const AosRundataPtr &rdata,
		boost::shared_ptr<DataRecord> &drec,
		//const OmnString &field_name,
		const AosJqlSelectFieldPtr &field)
{
	boost::shared_ptr<DataFieldExpr> data_field;
	data_field = boost::make_shared<DataFieldExpr>();  
	data_field->setExpr(field->dumpByStat());

	OmnString fname = field->dumpByNoEscape();
	AosJqlUtil::escape(fname);
	
	data_field->setAttribute("type", "expr"); 
	data_field->setAttribute("zky_name", fname);
	drec->setField(data_field);
	return true;
}

//generate count(*) select field
AosJqlSelectFieldPtr
AosJqlStatQuery::genCountField(const AosRundataPtr &rdata)
{ 
	AosExprObjPtr expr;
	AosExprObjPtr alias;
	AosJqlSelectFieldPtr field;

	// Chen Ding, 2015/01/31
	// expr = AosParseJQL("count(*)", false, rdata.getPtr());
	OmnString errmsg;
	expr = AosParseExpr("count(*)", errmsg, rdata.getPtr());
	// expr = AosParseJQL("count(*);", false, rdata.getPtr());
	field = OmnNew AosJqlSelectField();
	field->setField(expr);

	alias = OmnNew AosExprString("_not_used_");
	field->setAliasName(alias);

	return field;
}


OmnString
AosJqlStatQuery::getStrTimeUnit(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &statDoc)
{
	AosXmlTagPtr vct2ds = statDoc->getFirstChild("vector2ds");
	aos_assert_r(vct2ds, "");
	AosXmlTagPtr vct2d = vct2ds->getFirstChild("vt2d");
	aos_assert_r(vct2d, "");
	AosXmlTagPtr vt2d_info = vct2d->getFirstChild("vt2d_info");
	aos_assert_r(vt2d_info, "");
	OmnString tt = vt2d_info->getAttrStr("grpby_time_unit");
	return tt;
}

//arvin 2015.08.16
//JIMODB-422
bool
AosJqlStatQuery::syntaxCheck(const AosRundataPtr &rdata)
{
	//if fieldType is eCase, skip this check
	for(size_t i = 0; i < mSelectFields.size(); i++)
	{
		if(mSelectFields[i].getPtr()->mExpr->getType() == AosExprType::eCase) return true;
	}

	//check group by fields;
	bool rslt = false;
	vector<AosJqlSelectFieldPtr> measureFields;
	OmnString errmsg = "[ERR] : ";
	rslt = groupBySyntaxCheck(rdata,errmsg,measureFields);
	aos_assert_rr(rslt,rdata,false);
	
	//check order_by_field
	rslt = orderBySyntaxCheck(rdata,errmsg);
	aos_assert_rr(rslt,rdata,false);
	
	//check having field
	rslt = havingConditionSyntaxCheck(rdata,errmsg,measureFields);
	aos_assert_rr(rslt,rdata,false);
	
	return true;
}

bool
AosJqlStatQuery::groupBySyntaxCheck(
		const AosRundataPtr rdata,
		OmnString &errmsg,
		vector<AosJqlSelectFieldPtr> &measureFields)
{
	for(size_t i = 0; i < mSelectFields.size();i++)
	{
		if(!mSelectFields[i]->checkIsAgrFunc())
		{
			if(mSelectFields[i]->mExpr->getType() == AosExprType::eArith) return true;
			if(mSelectFields[i]->mExpr->getType() == AosExprType::eGenFunc) return true;
			OmnString field_name = mSelectFields[i]->getFieldName(rdata);
			OmnString fname = field_name;		
			fname = getFieldName(field_name);

			if(!isInGrpbyFields(rdata,fname))
			{
				errmsg << field_name <<" must appear in the group by!";
				rdata->setJqlMsg(errmsg);
				return false;
			}
			continue;
		}
		measureFields.push_back(mSelectFields[i]);
	}
	for(size_t i = 0; i < mGrpbyFields.size();i++)
	{
		if(mGrpbyFields[i]->checkIsAgrFunc())
		{
			errmsg << mGrpbyFields[i]->dumpByNoEscape() << " can't appear in the group by!";
			rdata->setJqlMsg(errmsg);
			return false;
		}
	}
	return true;
}

bool
AosJqlStatQuery::orderBySyntaxCheck(
		const AosRundataPtr &rdata,
		OmnString &errmsg)
{
	bool flag;
	if(mGrpbyFields.size() > 0)
	{
		//check OrderByField in GroupByField
		for(size_t i = 0; i < mOrderbyFields.size(); i++)
		{
			AosJqlSelectFieldPtr orderbySelectField = mOrderbyFields[i]->mField; 
			AosExprObjPtr expr = orderbySelectField->getField();
			OmnString orderByField = expr->dumpByNoEscape();
#if 0
			if(orderbySelectField->checkIsAgrFunc())
			{
				errmsg << "meausre can't appear in the groupby field!";
				rdata->setJqlMsg(errmsg);
				return false;
			}
#endif

			//order by can be either groupby field or measures
			if(!isInGrpbyFields(rdata,orderByField) &&
			   !orderbySelectField->checkIsAgrFunc())
			{
				errmsg << "orderby field must appear in the groupby field or meaures!";
				rdata->setJqlMsg(errmsg);
				return false;
			}
		}
	}
	else
	{
		//check OrderByField in SelectField
		for(size_t i = 0; i < mOrderbyFields.size(); i++)
		{
			AosExprObjPtr expr = mOrderbyFields[i]->mField->getField();
			OmnString orderByField = expr->dumpByNoEscape();
			flag = false;
			for(size_t i = 0; i < mSelectFields.size(); i++)
			{
				AosExprObjPtr expr1 = mSelectFields[i]->getField();
				OmnString selectField = expr1->dumpByNoEscape();
				if(orderByField == selectField)
				{
					flag = true;
					break;
				}
			}
			if(!flag)
		 	{
				errmsg << orderByField << " must appear in the selected field!";
				rdata->setJqlMsg(errmsg);
				return false;
			}
		}
	}
	return true;
}


bool
AosJqlStatQuery::havingConditionSyntaxCheck(
		const AosRundataPtr &rdata,
		OmnString &errmsg,
		const vector<AosJqlSelectFieldPtr> &measureFields)
{
	bool flag = false;
	if(mHavingPtr)
	{
		OmnString havingField,havingFieldStr;
		havingFieldStr = mHavingPtr->mHavingConf;
		havingFieldStr << ";";
		OmnString err;
		AosExprObjPtr havingExpr =  AosParseExpr(havingFieldStr,err,rdata.getPtr());
		if(havingExpr->getType() == AosExprType::eComparison)
		{
			havingField = havingExpr->getLHS()->dumpByNoEscape();
		}
		else
		{
			havingField = havingExpr->dumpByNoEscape();
		}
		AosJqlUtil::escape(havingField);
		for(size_t i = 0; i < measureFields.size();i++)
		{
			AosExprObjPtr expr = measureFields[i]->getField();
			OmnString measure  = expr->dumpByNoEscape();
			AosJqlUtil::escape(measure);
			if(havingField == measure)
			{
				flag = true;
				break;
			}
		}
		if(!flag)
		{
			errmsg << " having condition must be measure field!";
			rdata->setJqlMsg(errmsg);
			return false;
		}
	}
	return true;
}

bool
AosJqlStatQuery::isInGrpbyFields(
		const AosRundataPtr &rdata,
		const OmnString &orderByField)
{
	for(size_t i = 0; i < mGrpbyFields.size(); i++)
	{
		OmnString grpbyField = mGrpbyFields[i]->getFieldName(rdata);
		OmnString field_name = grpbyField;
		OmnString grbField = grpbyField;
		grpbyField = getFieldName(field_name);
		
		//case : orderByField is "_day(time)" ,but grpbyField is "_day0x28time0x29"
		AosJqlUtil::unescape(grbField);
		if(grpbyField == orderByField || grbField == orderByField)
		{
			return true;
		}
	}
	return false;
}


bool
AosJqlStatQuery::isInGrpbyFields(
		const AosRundataPtr &rdata,
		const AosJqlSelectFieldPtr &selectField,
		const vector<AosJqlSelectFieldPtr> &groupByFields)
{
	if(selectField->checkIsAgrFunc())
	{
		//if(mSelectFields[i]->mExpr->getType() == AosExprType::eArith) return true;
		//if(mSelectFields[i]->mExpr->getType() == AosExprType::eGenFunc) return true;
		return true;
	}
	//sum(xxx)/count(xxx) is not support now
	if(selectField->mExpr->getType() == AosExprType::eArith) return true;

	OmnString sltField = selectField->getFieldName(rdata);
	sltField = getFieldName(sltField);

 	for(size_t i = 0; i < groupByFields.size(); i++)
	{
		OmnString grpbyField = groupByFields[i]->getFieldName(rdata);
		OmnString field_name = grpbyField;
		OmnString grbField = grpbyField;
		grpbyField = getFieldName(field_name);
		
		//case : orderByField is "_day(time)" ,but grpbyField is "_day0x28time0x29"
		AosJqlUtil::unescape(grbField);
		if(grpbyField == sltField)
		{
			return true;
		}
	}
	OmnString errmsg = "";
	errmsg << "[ERR] : select field " << sltField << " must be appear in group by list!";
	rdata->setJqlMsg(errmsg);
	return false;
}


bool
AosJqlStatQuery::statCondExprProc(
		AosStatCond &stat_cond,
		const OmnString &opr_str)
{
	OmnString opr,conf;
	switch(stat_cond.mCondOpr)
	{
		case eAosOpr_like:
			{
				opr = "like";
				stat_cond.mCondOpr = eAosOpr_an;
				break;
			}
		case eAosOpr_not_like:
			{
				opr = "notlike";
				stat_cond.mCondOpr = eAosOpr_an;
				break;
			}
		default:
			{
				//do nothing
				//arvin 2015.08.20
				//JIMODB-533
				return true;
			}
	}
	conf << "<query_filter type=\"keyfield\""
		<< " field_idx=\"0\""
		<< AOSTAG_SEPARATOR << "=\"0x01\"" 
		<< " opr=\"" << opr << "\">"
		<< "<cond zky_type=\"expr\"><![CDATA[`" 
		<< stat_cond.mStatField->getFieldName() << "` "  << opr_str  << " \"" <<  stat_cond.mCondValue1
		<< "\"]]></cond>"
		<< "</query_filter>";
	stat_cond.mCondValue1 = "*";
	stat_cond.mFiltersConf = conf;
	return true;
}

bool
AosJqlStatQuery::convertTimeFormat(const AosRundataPtr& rdata,string &format)
{
	OmnString timeUnit;
	for(size_t i = 0; i < mGrpbyFields.size();i++)
	{
		OmnString field = mGrpbyFields[i]->getFieldName(rdata);
		field = getFieldName(field);
		if(mBasicStat->isContainTimeField(field))
		{
			timeUnit = mGrpbyFields[i]->getFuncName();
			break;
		}
	}
	timeUnit.toLower();
	if(timeUnit == "_year")          
		format = "%Y";               
	else if(timeUnit == "_month")    
		format = "%Y-%m";            
	else if(timeUnit == "_week")     
		format = "%Y %W";            
	else if(timeUnit == "_day")      
		format = "%Y-%m-%d";         
	else if(timeUnit == "_hour")     
		format = "%Y-%m-%d %H";      
	else if(timeUnit == "_minute")   
		format = "%Y-%m-%d %H:%m";   
	else if(timeUnit == "_second")   
		format = "%Y-%m-%d %H:%m:%s";
	return true;
}

OmnString 
AosJqlStatQuery::getFieldName(const OmnString &field)
{
	OmnString field_name = field;
	int startIdx = field.indexOf(0, '(', false);
	if(startIdx != -1)
	{
		int endIdx = field.indexOf(0,')',false);
		field_name = OmnString(field.data()+startIdx+1,endIdx-startIdx-1);
	}
	return field_name;
}

bool
AosJqlStatQuery::hitShuffleFields(const AosRundataPtr &rdata)
{
	aos_assert_r(mBasicStat,false);
	
	vector<OmnString> shuffle_fields = mBasicStat->getShuffleFields();

	vector<OmnString> groupByFields;
	vector<OmnString>::iterator itr;
	for(size_t i = 0; i < mGrpbyFields.size(); i++)
	{
		OmnString groupByField = mGrpbyFields[i]->getFieldName(rdata.getPtrNoLock());
		groupByFields.push_back(groupByField);	
	}

	if(shuffle_fields.empty() && !groupByFields.empty())
		return false;
	if(mGrpbyFields.empty() && !(mBasicStat->getStatKeyNum()))
		return true;

	for(size_t i = 0;i < shuffle_fields.size(); i++)
	{
		itr = find(groupByFields.begin(),groupByFields.end(),shuffle_fields[i]);
		if(itr == groupByFields.end())
		{
			return false;
		}
	}
	return true;
}

