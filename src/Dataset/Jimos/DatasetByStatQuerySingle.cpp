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
// This dataset uses one data scanner as its inputs. The data scanner
// can be any type. Most datasets can be implemented by this class.
//
// Modification History:
// 2014/05/05 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "Dataset/Jimos/DatasetByStatQuerySingle.h"

#include "API/AosApi.h"
#include "QueryUtil/QueryFilter.h"
#include "SEInterfaces/GroupQueryObj.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/GroupQueryObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/DataSplitterObj.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/RecordsetObj.h"
#include "StatEngine/StatQryEngine.h"
#include "StatUtil/Statistic.h"
#include "StatUtil/StatRecord.h"
#include "StatUtil/Jimos/StatModel.h"
#include "SEUtil/IILName.h"
#include "JQLStatement/Ptrs.h"
#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlLimit.h"
#include "JQLStatement/JqlHaving.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDatasetByStatQuerySingle_0(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDatasetByStatQuerySingle(version);
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


AosDatasetByStatQuerySingle::AosDatasetByStatQuerySingle(const int version)
:
AosDatasetByQuery(AOSDATASET_STAT_QUERY_SINGLE, version),
mDataReady(false)
{
}


AosDatasetByStatQuerySingle::~AosDatasetByStatQuerySingle()
{
}


AosJimoPtr
AosDatasetByStatQuerySingle::cloneJimo()  const
{
	try
	{
		return OmnNew AosDatasetByStatQuerySingle(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool
AosDatasetByStatQuerySingle::config(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &conf)
{
	aos_assert_r(conf, false);

	bool rslt = AosDatasetByQuery::config(rdata, conf);
	aos_assert_r(rslt, false);

	rslt = initStatQryProcers(rdata, conf);
	aos_assert_r(rslt, false);

	mStatQryEngine = OmnNew AosStatQryEngine(rdata.getPtr(), &mStatQryInfo);
	aos_assert_r(mStatQryEngine, false);
	rslt = mStatQryEngine->frontInit();
	aos_assert_r(rslt, false);
	
	rslt = initInputDataRecord(rdata, conf);
	aos_assert_r(rslt && mInputDataRecord, false);

	rslt = initInputStatRcdFieldDef(rdata);
	aos_assert_r(rslt, false);
	
	rslt = initTmpRecordset(rdata, conf);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosDatasetByStatQuerySingle::initStatQryProcers(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf)
{
	aos_assert_r(conf, false);
	bool rslt;
	bool hitShuffleFields = false;
	u32 page_size = 0;
	u64 page_offset = 0;
	vector<AosStatCond>	qry_conds;
	vector<AosStatFieldPtr> grpby_fields;
	vector<AosStatFieldPtr> cube_grpby_fields;
	vector<vector<AosStatFieldPtr> > rollup_grpby_fields_grp;
	vector<AosStatFieldPtr> orderby_fields;
	vector<bool> orderby_ascs;
	AosJqlHaving having_conds;
	
	mTableName = conf->getAttrStr("zky_name","");
	AosXmlTagPtr page_size_conf = conf->getFirstChild("page_size", true);
	aos_assert_r(page_size_conf, false);
	page_size = page_size_conf->getAttrU32("size", 0);

	// config qry_conds.
	AosXmlTagPtr qry_confs = conf->getFirstChild("qry_conds", true);
	if(qry_confs)
	{
		AosStatCond cond;
		AosXmlTagPtr cond_conf = qry_confs->getFirstChild(true);
		while(cond_conf)
		{
			rslt = cond.config(cond_conf);
			aos_assert_r(rslt, false); 

			qry_conds.push_back(cond);
			cond_conf = qry_confs->getNextChild();
		}
	}

	// config grpby fields.
	AosXmlTagPtr grpby_confs = conf->getFirstChild("groupby_fields", false);
	if(grpby_confs)
	{
		AosXmlTagPtr k_conf = grpby_confs->getFirstChild(true);
		while(k_conf)
		{
			AosStatFieldPtr field = AosStatField::configStatic(k_conf);
			aos_assert_r(field, false);
			if(field->mFieldType == AosStatField::eTimeField)
				mTimeUnit = field->getTimeUnit();
			grpby_fields.push_back(field);	//Temp
			k_conf = grpby_confs->getNextChild();
		}
	}
	// config rollup grpby fields.
	AosXmlTagPtr rollup_grpby_confs = conf->getFirstChild("rollup_groupby_fields", true);
	while(rollup_grpby_confs)
	{
		vector<AosStatFieldPtr> rollup_grpby_fields;
		AosXmlTagPtr r_conf = rollup_grpby_confs->getFirstChild(true);
		while(r_conf)
		{
			AosStatFieldPtr field = AosStatField::configStatic(r_conf);
			aos_assert_r(field, false);
			
			rollup_grpby_fields.push_back(field);
			r_conf = rollup_grpby_confs->getNextChild();
		}

		rollup_grpby_fields_grp.push_back(rollup_grpby_fields);
		rollup_grpby_confs = conf->getNextChild("rollup_groupby_fields");
	}

	// config cube grpby fields.
	AosXmlTagPtr cube_grpby_confs = conf->getFirstChild("cube_groupby_fields", true);
	if (cube_grpby_confs)
	{
		AosXmlTagPtr c_conf = cube_grpby_confs->getFirstChild(true);
		while(c_conf)
		{
			AosStatFieldPtr field = AosStatField::configStatic(c_conf);
			aos_assert_r(field, false);
			
			cube_grpby_fields.push_back(field);
			c_conf = cube_grpby_confs->getNextChild();
		}
	}

	// config order by field.
	AosXmlTagPtr orderby_field_confs = conf->getFirstChild("orderby_fields");
	if(orderby_field_confs)
	{
		bool is_asc;
		AosXmlTagPtr f_conf;
		AosXmlTagPtr each_conf = orderby_field_confs->getFirstChild(true);
		while(each_conf)
		{
			is_asc = each_conf->getAttrBool("is_asc", false);
				
			f_conf = each_conf->getFirstChild(true);
			aos_assert_r(f_conf, false);
		
			AosStatFieldPtr field = AosStatField::configStatic(f_conf);
			aos_assert_r(field, false);
			
			orderby_fields.push_back(field);
			orderby_ascs.push_back(is_asc);
			each_conf = orderby_field_confs->getNextChild();
		}
	}

	// Add by Gavin, 2015/1/4
	OmnString havingCond = "";
	AosXmlTagPtr havingNode = conf->getFirstChild("having_conds");	
	if (havingNode)
	{
		havingCond = havingNode->getAttrStr("zky_having_expr");
		OmnString meetShuffleFields = havingNode->getAttrStr("hit_shuffle_fields");
		if(meetShuffleFields == "true")
			hitShuffleFields = true;
	}
	// Add by Gavin, 2014/12/19
	AosJqlLimitPtr limitPtr = OmnNew AosJqlLimit();
	AosXmlTagPtr limitNode = conf->getFirstChild("limit");
	if (limitNode)
	{
		u64 limitOffset = limitNode->getAttrU64("limit_offset", 0);
		u64 limitRowCount = limitNode->getAttrU64("limit_row_count", 0);
		limitPtr->setOffset(limitOffset);
		limitPtr->setRowCount(limitRowCount);
	
		mStartIdx = limitOffset;
		mPageSize = limitRowCount;

		//bryant 2015/9/11
		page_size = limitRowCount;
		page_offset = limitOffset;
	}
	else
	{
		limitPtr = NULL;
	}


	rslt = mStatQryInfo.init(rdata, page_size, page_offset, qry_conds,
			grpby_fields,cube_grpby_fields, rollup_grpby_fields_grp, 
			orderby_fields, orderby_ascs, havingCond,hitShuffleFields);
	aos_assert_r(rslt, false);
	
	AosXmlTagPtr stats_def = conf->getFirstChild("qry_internal_stats");
	aos_assert_r(stats_def, false);

	StatQryProcer procer;
	OmnString stat_name;
	AosXmlTagPtr stat_conf = stats_def->getFirstChild();
	u32 cond_idx = 0;
	while(stat_conf)
	{
		stat_name = stat_conf->getAttrStr("stat_name", "");
		aos_assert_r(stat_name != "", false);
		
		procer.reset();
		procer.mStat = AosStatistic::retrieveStat(rdata, stat_name); 
		procer.mStatQryInfo.initStat(rdata, procer.mStat);
		procer.mStatQryInfo.mQryMeasures.clear();

		AosXmlTagPtr m_conf = stat_conf->getFirstChild("measure_fields");
		AosXmlTagPtr each_conf = m_conf->getFirstChild();
		while(each_conf)
		{
			OmnString isAggrCond = each_conf->getAttrStr("is_aggr_cond","");
			if(isAggrCond == "true")
			{
				OmnString cond = "cond";
				cond << cond_idx;
				each_conf->setAttr("measure_field_name",cond);
				cond_idx++;
			}
			AosStatFieldPtr field = AosStatField::configStatic(each_conf);
			procer.mStatQryInfo.addQryMeasure(field);	

			each_conf = m_conf->getNextChild();
		}
	
		vector<AosStatFieldPtr> sub_grpby_fields;
		AosXmlTagPtr g_conf = stat_conf->getFirstChild("groupby_fields");
		each_conf = g_conf->getFirstChild();
		while(each_conf)
		{

			AosStatFieldPtr field = AosStatField::configStatic(each_conf);

			sub_grpby_fields.push_back(field);	//Temp

			each_conf = g_conf->getNextChild();
		}
		
		vector<vector<AosStatFieldPtr> > sub_rollup_grpby_fields_grp;
		vector<AosStatFieldPtr>  sub_cube_grpby_fields;
		procer.mStatQryInfo.init(rdata, page_size, page_offset, qry_conds,		// Modify by Gavin, 2014/12/19
				sub_grpby_fields,sub_cube_grpby_fields, sub_rollup_grpby_fields_grp, orderby_fields, orderby_ascs, havingCond,hitShuffleFields);

		rslt = initIILQuery(rdata, procer.mStatQryInfo, 
				procer.mIILQuery, procer.mNeedIILQuery);
		aos_assert_r(rslt, false);
		
		rslt = configDataScanner(rdata, conf, procer.mStat, 
				procer.mStatQryInfo, procer.mDataScanner);
		
		mStatQryProcers.push_back(procer);
		stat_conf = stats_def->getNextChild();
	}
		
	aos_assert_r(mStatQryProcers.size() > 0, false);
	AosStatisticPtr crt_stat = mStatQryProcers[0].mStat;
	mStatQryInfo.initStat(rdata, crt_stat);
	
	set<OmnString> exist_names;
	set<OmnString>::iterator itr;
	for(u32 i=0; i<mStatQryProcers.size(); i++)
	{
		OmnString fname;
		for(u32 j=0; j<mStatQryProcers[i].mStatQryInfo.mQryMeasures.size(); j++)
		{
			fname = mStatQryProcers[i].mStatQryInfo.mQryMeasures[j]->getFieldName();
		//	itr = exist_names.find(fname);
		//	if(itr != exist_names.end())	continue;
			
			//if improved, we need to translate dist_count() to sum()

			mStatQryInfo.addQryMeasure(mStatQryProcers[i].mStatQryInfo.mQryMeasures[j]);
			exist_names.insert(fname);
		}
	}

	//
	//if no measures, it might be a distinct query, we need 
	//to add a count(*) measure
	//
	if (mStatQryInfo.mQryMeasures.size() <= 0)
	{
		OmnString fieldName = "count(*);";
		OmnString rawName = "*";
		fieldName.escape();
		rawName.escape();

		AosStatFieldPtr measureField = OmnNew AosStatMeasureField(
				fieldName, rawName, "count");
		mStatQryInfo.addQryMeasure(measureField);
	}

	if(crt_stat->hasDistCountMeasure())
	{
		//remove the last statkey
		//mStatQryInfo.mStatKeyFields.pop_back();
	}

	return true;
}


bool
AosDatasetByStatQuerySingle::initIILQuery(
		const AosRundataPtr &rdata,
		AosStatQueryInfo &stat_qry_info,
		AosGroupQueryObjPtr &iil_query,
		bool &need_iil_query)
{
	iil_query = AosGroupQueryObj::createQueryStatic(rdata);
	bool rslt;
	
	bool need_full_scan;
	u32 order_field_num = stat_qry_info.mOrderbyFields.size();
	if(order_field_num == 0)
	{
		rslt = setIILQryGroupFieldsP1(stat_qry_info, iil_query, need_full_scan);
	}
	else if(order_field_num == 1)
	{
		rslt = setIILQryGroupFieldsP2(stat_qry_info, iil_query, need_full_scan);
	}
	else
	{
		rslt = setIILQryGroupFieldsP3(stat_qry_info, iil_query, need_full_scan);
	}
	aos_assert_r(rslt, false);
	iil_query->setPageSize(1000);
	
#if 0
	//if has having condition, should need full scan
	if (stat_qry_info.mHavingCond != "")
		need_full_scan = true;
#endif

	need_iil_query = !need_full_scan;
	stat_qry_info.mNeedFullScan = need_full_scan;

	OmnString iil_name;
	vector<AosStatCond> &key_conds = stat_qry_info.mQryKeyConds;
	for(u32 i=0; i<key_conds.size(); i++)
	{
		iil_name = AosIILName::composeStatKeyIILName(
			stat_qry_info.mStatName, key_conds[i].mStatField->mName);

		AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
		query_context->setOpr(key_conds[i].mCondOpr);
		query_context->setStrValue(key_conds[i].mCondValue1);
		query_context->setStrValue2(key_conds[i].mCondValue2);

//		OmnString filters_str = "<filters>";
//		filters_str << key_conds[i].mFiltersConf << "</filters>";

//		AosXmlParser parser;
//		AosXmlTagPtr root = parser.parse(filters_str, "" AosMemoryCheckerArgs);
//		aos_assert_r(root,false);
//		AosXmlTagPtr query_filters = root->getFirstChild("query_filters");
		AosXmlTagPtr query_filters = key_conds[i].mFiltersXml;

		AosQueryFilterObjPtr filter;
		if (query_filters)
		{
			AosXmlTagPtr query_filter = query_filters->getFirstChild();
			while (query_filter)
			{
				filter = AosQueryFilter::create(query_filter, rdata); 
				aos_assert_r(filter,false);
				query_context->addFilter(filter);
				query_filter = query_filters->getNextChild();
			}
		}
		iil_query->addCond(iil_name, query_context);




		//iil_query->addCond(iil_name, key_conds[i].mCondOpr,
		//		key_conds[i].mCondValue1, key_conds[i].mCondValue2);
		need_iil_query = true;
	}
	
	// Temp
	if(!need_iil_query)
	{
		OmnString key_fname;
		if(stat_qry_info.mStatKeyFields.size() != 0)
		{
			key_fname = stat_qry_info.mStatKeyFields[0];
		}
		else
		{
			// means this stat has no key. no group by key. 
			key_fname = AOS_EMPTY_KEY;
		}
		OmnString order_iilname = AosIILName::composeStatKeyIILName(
				stat_qry_info.mStatName, key_fname);
		iil_query->setGroupFields(order_iilname, false);
		//do we need to set page size to be 0??? 
		//seems we can always handle 1000 docids 
		//in one round and run many rounds instead
		//of getting all the docids (e.g. a million)
		//at one time
		iil_query->setPageSize(0);

	}
	return true;
}


bool
AosDatasetByStatQuerySingle::setIILQryGroupFieldsP1(
		AosStatQueryInfo &stat_qry_info,
		AosGroupQueryObjPtr &iil_query,
		bool &need_full_scan)
{
	// this pattern is qry has no order field.
	aos_assert_r(iil_query, false);
	aos_assert_r(stat_qry_info.mOrderbyFields.size() == 0, false);

	// find first grpby key field and use it.
	vector<AosStatFieldPtr> & grpby_fields = stat_qry_info.mGrpbyFields;
	//aos_assert_r(grpby_fields.size() >=1, false);
	
	vector<OmnString> iilNameList;
	OmnString order_iilname;
	for(u32 i=0; i<grpby_fields.size(); i++)
	{
		if(grpby_fields[i]->getFieldType() != AosStatField::eKeyField)	continue;
		
		order_iilname = AosIILName::composeStatKeyIILName(
				stat_qry_info.mStatName, grpby_fields[i]->mName);
		
		iilNameList.push_back(order_iilname);
		if (iilNameList.size() == 2)
			break;

	}

	if (iilNameList.size() >= 2)
	{
		iil_query->setGroupFields(iilNameList[0], false, iilNameList[1], false);
		need_full_scan = false;
		return true;
	}
	else if (iilNameList.size() == 1)
	{
		iil_query->setGroupFields(iilNameList[0], false);
		need_full_scan = false;
		return true;
	}
	
	// means grpby time.
	need_full_scan = true;
	return true;
}


bool
AosDatasetByStatQuerySingle::setIILQryGroupFieldsP2(
		AosStatQueryInfo &stat_qry_info,
		AosGroupQueryObjPtr &iil_query,
		bool &need_full_scan)
{
	// this pattern is qry has one order field.
	aos_assert_r(iil_query, false);
	aos_assert_r(stat_qry_info.mOrderbyFields.size() == 1, false);
	
	AosStatFieldPtr order_field = stat_qry_info.mOrderbyFields[0];
	vector<AosStatFieldPtr> & grpby_fields = stat_qry_info.mGrpbyFields;
	AosStatField::Type tp = order_field->getFieldType();

	vector<OmnString> iilNameList;
	OmnString order_iilname;
	vector<bool> orderby_asc;
	if(tp == AosStatField::eKeyField)
	{
		order_iilname = AosIILName::composeStatKeyIILName(
				stat_qry_info.mStatName, order_field->mName);
		iilNameList.push_back(order_iilname);
		orderby_asc.push_back(!stat_qry_info.mOrderbyAscs[0]);
	}
	
	//next, check groupby fields iilnames
	for(u32 i=0; i<grpby_fields.size(); i++)
	{
		if(grpby_fields[i]->getFieldType() != AosStatField::eKeyField)	continue;
		
		order_iilname = AosIILName::composeStatKeyIILName(
				stat_qry_info.mStatName, grpby_fields[i]->mName);
		
		iilNameList.push_back(order_iilname);
		orderby_asc.push_back(false);
		if (iilNameList.size() == 2)
			break;

	}

	if (iilNameList.size() >= 2)
	{
		iil_query->setGroupFields(iilNameList[0], orderby_asc[0], iilNameList[1], orderby_asc[1]);
		need_full_scan = false;
		return true;
	}
	else if (iilNameList.size() == 1)
	{
		iil_query->setGroupFields(iilNameList[0], orderby_asc[0]);
		need_full_scan = false;
		return true;
	}

	// means is order by time or value.
	need_full_scan = true;
	return true;
}


bool
AosDatasetByStatQuerySingle::setIILQryGroupFieldsP3(
		AosStatQueryInfo &stat_qry_info,
		AosGroupQueryObjPtr &iil_query,
		bool &need_full_scan)
{
	// this pattern is qry has more than one order field.
	aos_assert_r(iil_query, false);
	aos_assert_r(stat_qry_info.mOrderbyFields.size() >= 2, false);

	vector<OmnString> valid_field_idxs;
	vector<AosStatFieldPtr> & grpby_fields = stat_qry_info.mGrpbyFields;
	vector<AosStatFieldPtr> & orderby_fields = stat_qry_info.mOrderbyFields;
	vector<OmnString> iilNameList;
	OmnString order_iilname;
	vector<bool> orderby_asc;
	for(u32 i=0; i<orderby_fields.size(); i++)
	{
		AosStatField::Type tp = orderby_fields[i]->getFieldType();
		
		if(tp == AosStatField::eKeyField)
		{
			order_iilname = AosIILName::composeStatKeyIILName(
					stat_qry_info.mStatName, orderby_fields[i]->mName);
			iilNameList.push_back(order_iilname);
			orderby_asc.push_back(!stat_qry_info.mOrderbyAscs[i]);
		}
		else
			break;

		if(iilNameList.size() == 2)	break;	
	}

	//next, check groupby fields iilnames
 	if ( iilNameList.size()< 2)
	{
		for(u32 i=0; i<grpby_fields.size(); i++)
		{
			if(grpby_fields[i]->getFieldType() != AosStatField::eKeyField)	continue;

			order_iilname = AosIILName::composeStatKeyIILName(
					stat_qry_info.mStatName, grpby_fields[i]->mName);

			iilNameList.push_back(order_iilname);
			orderby_asc.push_back(false);
			if (iilNameList.size() == 2)
				break;

		}
	}

	if (iilNameList.size() >= 2)
	{
		iil_query->setGroupFields(iilNameList[0], orderby_asc[0], iilNameList[1], orderby_asc[1]);
		need_full_scan = false;
		return true;
	}
	else if (iilNameList.size() == 1)
	{
		iil_query->setGroupFields(iilNameList[0], orderby_asc[0]);
		need_full_scan = false;
		return true;
	}

	aos_assert_r(iilNameList.size() == 0, false)
	need_full_scan = true;
	return true;
}


bool
AosDatasetByStatQuerySingle::configDataScanner(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf,
		const AosStatisticPtr &stat,
		AosStatQueryInfo &stat_qry_info,
		AosDataScannerObjPtr & data_scanner)
{
	// Create data_scanner. 
	AosXmlTagPtr ds_conf = conf->getFirstChild("datascanner");

	//don't overwrite the global conf
	AosXmlTagPtr scanner_conf = ds_conf->clone(AosMemoryCheckerArgsBegin);
	aos_assert_rr(scanner_conf, rdata, false);
	
	AosXmlTagPtr conntect_conf = scanner_conf->getFirstChild("dataconnector");
	aos_assert_r(conntect_conf, false);
	
	AosXmlTagPtr stat_cubes_conf = stat->getStatCubesConf();
	aos_assert_r(stat_cubes_conf, false);
	conntect_conf->addNode(stat_cubes_conf);

	OmnString stat_qry_str = stat_qry_info.toXmlString();

	AosXmlTagPtr stat_qry_conf = AosXmlParser::parse(stat_qry_str AosMemoryCheckerArgs);
	aos_assert_r(stat_qry_conf, false);
	
	
	//arvin 2015.09.02
	AosXmlTagPtr stat_key_fields = stat_qry_conf->getFirstChild("stat_key_fields");
	aos_assert_r(stat_key_fields, false);
	AosXmlTagPtr field_doc = stat_key_fields->getFirstChild("field");
	bool rslt;
	OmnString format,key,type_str;
	while(field_doc)
	{
		key = field_doc->getAttrStr("field_name","");
		AosJqlColumnPtr column = AosJqlStatement::getTableField(rdata,mTableName,key);
		type_str = AosDataType::getTypeStr(column->type);
		field_doc->setAttr("type",type_str.data());
		field_doc  = stat_key_fields->getNextChild();
	}
	
	//generate order by key type map before group by because when we generate stat groupby field order
	//  we must consider the order by feld first
	int idx = 0;
	AosDataType::E keyType;
	OmnString fieldName, fieldType;
	map<OmnString,int> orderByFields;
	AosXmlTagPtr order_by_fields = stat_qry_conf->getFirstChild("orderby_fields");
	if(order_by_fields)
	{
		field_doc = order_by_fields->getFirstChild("orderby_field");
		aos_assert_r(field_doc, false);
		while(field_doc)
		{
			AosXmlTagPtr field = field_doc->getFirstChild("field");
			aos_assert_r(field, false);
			fieldType = field->getAttrStr("field_type");
			if(fieldType == "key_field" || fieldType == "time_field")
			{
				fieldName = field->getAttrStr("zky_name");
				type_str = field->getAttrStr("data_type");
				keyType = AosDataType::toEnum(type_str);
				mGroupByKeyType[idx] = keyType;
				orderByFields[fieldName] = -12345;
				idx++;
			}
			field_doc = order_by_fields->getNextChild();
		}
	}

	//generate groupby key type map
	//for such case,this is no group by fields
	//	e.g select sum(num2,num2>10) from count_con;
	AosXmlTagPtr group_by_fields = stat_qry_conf->getFirstChild("group_by_fields");
	if(group_by_fields)
	{
		field_doc = group_by_fields->getFirstChild("field");
		while(field_doc)
		{
			fieldName = field_doc->getAttrStr("zky_name");
			if(orderByFields[fieldName] == -12345) 
			{
				field_doc = group_by_fields->getNextChild();
				continue;
			}
			type_str = field_doc->getAttrStr("data_type");
			keyType = AosDataType::toEnum(type_str);
			mGroupByKeyType[idx] = keyType;
			idx++;
			field_doc = group_by_fields->getNextChild();
		}
	}	
	u32 cond_idx = 0;
	AosXmlTagPtr qryMeasure_docs = stat_qry_conf->getFirstChild("qry_measures");
	aos_assert_r(qryMeasure_docs, false);
	field_doc = qryMeasure_docs->getFirstChild("field");
	while(field_doc)
	{
		OmnString isAggrCond = field_doc->getAttrStr("is_aggr_cond","");
		if(isAggrCond == "true")
		{
			OmnString cond = "cond";
			cond << cond_idx;
			field_doc->setAttr("measure_field_name",cond);
			cond_idx++;
		}
		field_doc  = qryMeasure_docs->getNextChild();
	}
	conntect_conf->addNode(stat_qry_conf);

	data_scanner = AosCreateDataScanner(rdata.getPtr(), scanner_conf);
	aos_assert_r(data_scanner, false);
	// need ??
	//data_scanner->setTask(mTask);
	rslt = data_scanner->config(rdata, scanner_conf);
	if (!rslt)
	{
		OmnAlarm << "may be config error" << enderr;
	}
	
	return true;
}


bool
AosDatasetByStatQuerySingle::initInputDataRecord(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf)
{
	AosXmlTagPtr rcd_def = conf->getFirstChild("inputrecord");
	aos_assert_r(rcd_def, false);

	// change tag_name. Temp.
	OmnString rcd_def_str = rcd_def->toString();
	rcd_def_str.replace("inputrecord", "datarecord", true);	

	OmnString rcd_ctnr_conf_str = "<record_ctnr type=\"ctnr\" zky_name=\"stat_rcd_ctnr\">"; 
	rcd_ctnr_conf_str << rcd_def_str
		<< "</record_ctnr>";
	
	AosXmlTagPtr rcd_ctnr_conf = AosXmlParser::parse(rcd_ctnr_conf_str AosMemoryCheckerArgs);
	aos_assert_r(rcd_ctnr_conf, false);	

	mInputDataRecord = AosDataRecordObj::createDataRecordStatic(rcd_ctnr_conf,
			0, rdata.getPtr() AosMemoryCheckerArgs);
	aos_assert_r(mInputDataRecord, false);
	return true;
}


bool
AosDatasetByStatQuerySingle::initInputStatRcdFieldDef(const AosRundataPtr &rdata)
{
	aos_assert_r(mInputDataRecord, false);
	
	InputStatFieldDef field_def;

	u32 field_num = mInputDataRecord->getNumFields();
	AosDataFieldObj *data_field;
	OmnString fname;
	int	stat_field_idx;
	for(u32 i=0; i<field_num; i++)
	{
		data_field = mInputDataRecord->getFieldByIdx1(i);
		aos_assert_r(data_field, false);

		// Ketty 2014/12/02
		if(data_field->getType() == AosDataFieldType::eExpr)	continue; 

		fname = data_field->getName();
		aos_assert_r(fname != "", false);	
	
		stat_field_idx = mStatQryEngine->getFieldIdx(fname);
		//arvin 2015-10-8
		AosJqlColumnPtr column = AosJqlStatement::getTableField(rdata,mTableName,fname);
		if(column->type != AosDataType::eInvalid) mKeyIdxTypeMap[stat_field_idx] = column->type;
		
		if(stat_field_idx == -1)
		{
			OmnScreen << "Stat Rcd Ignore input field:" << fname << endl;
			continue;
		}

		field_def.mStatFname = fname;
		field_def.mStatFieldIdx = stat_field_idx;
		field_def.mInputFieldIdx = i;
		mInputStatFieldDefs.push_back(field_def);
	}
	return true;
}


bool
AosDatasetByStatQuerySingle::initTmpRecordset(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf)
{
	// create Recordset
	AosXmlTagPtr recordset_conf = conf->getFirstChild("recordset");
	if (!recordset_conf)
	{
		OmnString str = "<recordset />";
		AosXmlParser xmlparser;
		recordset_conf = xmlparser.parse(str, "" AosMemoryCheckerArgs);
	}
	aos_assert_r(recordset_conf, false);

	mTmpRecordset = AosRecordsetObj::createRecordsetStatic(rdata.getPtrNoLock(), recordset_conf);
	aos_assert_r(mTmpRecordset, false);
	return true;
}


bool
AosDatasetByStatQuerySingle::sendStart(const AosRundataPtr &rdata)
{
	// Temp.
	//if(!mNeedIILQuery)	return directStatQuery(rdata);

	OmnScreen << "Ketty Stat Print. Query Start." << endl;
	mQueryStartTime = OmnGetTimestamp();
	
	bool rslt;
	bool finished = false;
	while(1)
	{
		rslt = queryIIL(rdata, finished);
		aos_assert_r(rslt, false);
		
		if(finished) break;	// no more stat_docids;
		
		rslt = readDataByDataScanner(rdata);
		aos_assert_r(rslt, false);

		//apply having and check page full
		//check if there have accumulate func,leave having operation last to do
		bool hasAccumulate = false;
		for(size_t i = 0; i < mStatQryInfo.mQryMeasures.size(); i++)
		{
			if(mStatQryInfo.mQryMeasures[i]->mName.hasPrefix("accu_")) hasAccumulate = true;
		}
		if(!hasAccumulate)
		{
			mStatQryEngine->applyHaving();
			if(mStatQryEngine->pageFull())	break;		// one page finish.
		}

	}

	int merged_num = mStatQryEngine->mergeStatRecord(); 
	aos_assert_r(merged_num != -1, false);              

	// read finish.
	rslt = mStatQryEngine->dataFinished(rdata);
	aos_assert_r(rslt, false);
	
	mDataReady = true;

	OmnScreen << "Ketty Stat Print. Query Finish. " << endl;
	for(u32 i=0; i<mStatQryProcers.size(); i++)
	{
		OmnScreen << "Time: "
			<< "qry_iil_time:" << mStatQryProcers[i].mQryIILTime << "; "
			<< "qry_stat_time:" << mStatQryProcers[i].mQryStatTime << "; "
			<< "stat_docid_num:" << mStatQryProcers[i].mTotalDocidNum << "; "
			<< endl;
	}
	return true;
}


/*
bool
AosDatasetByStatQuerySingle::directStatQuery(const AosRundataPtr &rdata)
{
	mQueryStartTime = OmnGetTimestamp();
	bool rslt;
	
	OmnScreen << "Ketty Stat Print. Query Start. no iil qry. " << endl;
	
	u64 t2 = OmnGetTimestamp();

	AosBuffPtr stat_docids_buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	rslt = readDataByDataScanner(rdata, stat_docids_buff);
	aos_assert_r(rslt, false);
	
	u64 qry_stat_time = (OmnGetTimestamp() - t2);

	// read finish.
	rslt = mStatQryEngine->dataFinished(rdata);
	aos_assert_r(rslt, false);
	
	mDataReady = true;
	mQueryFinishTime = OmnGetTimestamp();
		
	OmnScreen << "Ketty Stat Print. Query Finish. no iil qry. "
			<< "qry_stat_time:" << qry_stat_time << "; "
			<< endl;
	return true;
}
*/


bool
AosDatasetByStatQuerySingle::queryIIL(const AosRundataPtr &rdata, bool &finished)
{
	bool rslt;
	u64 t1 = 0, t2 = 0;
	finished = true;
	for(u32 i=0; i<mStatQryProcers.size(); i++)
	{
		if(!mStatQryProcers[i].mIILQuery)
		{
			// means this stat has no key. no group by key. 
			if(mStatQryProcers[i].mStatDocidsBuff->dataLen() == 0)
			{
				mStatQryProcers[i].mStatDocidsBuff->setU64(1);	
				mStatQryProcers[i].mStatDocidsBuff->reset();
				mStatQryProcers[i].mTotalDocidNum++;
				finished = false;
			}
			continue;
		}
	
		t1 = OmnGetTimestamp();
		rslt = queryEachIIL(rdata, mStatQryProcers[i].mIILQuery,
				mStatQryProcers[i].mStatDocidsBuff,
				mStatQryProcers[i].mTotalDocidNum);	
		aos_assert_r(rslt, false);
		t2 = OmnGetTimestamp();
		mStatQryProcers[i].mQryIILTime += (t2 - t1);
		
		if(mStatQryProcers[i].mStatDocidsBuff->dataLen() != 0)
		{
			finished = false;
		}
	}
	
	return true;
}


bool
AosDatasetByStatQuerySingle::queryEachIIL(
		const AosRundataPtr &rdata,
		const AosGroupQueryObjPtr &iil_query,
		const AosBuffPtr &rslt_buff,
		u32 &docid_num)
{
	rslt_buff->setDataLen(0);
	rslt_buff->reset();

	//if(!iil_query)
	//{
	//	// means this stat has no key. no group by key. 
	//	rslt_buff->setU64(1);	
	//	rslt_buff->reset();
	//	docid_num++;
	//	return true;
	//}
	
	AosBitmapObjPtr id_bitmap;
	bool rslt = iil_query->nextBlock(id_bitmap);
	aos_assert_r(rslt, false);
	if(!id_bitmap)
	{
		OmnScreen << "Ketty test no more docid!" << endl;
		return true;	// no more data.
	}
		
	OmnScreen << "Ketty test docid start! bitmap_size:" << id_bitmap->getNumBits() << endl;
	id_bitmap->reset();
	u64 docid;
	while(id_bitmap->nextDocid(docid))
	{
		//OmnScreen << "Ketty test docid:" << docid << endl;
		rslt_buff->setU64(docid);
		docid_num++;
	}
	rslt_buff->reset();
	return true;
}


bool
AosDatasetByStatQuerySingle::readDataByDataScanner(const AosRundataPtr &rdata)
{
	bool rslt;
	u64 t1 = 0, t2 = 0;
	for(u32 i=0; i<mStatQryProcers.size(); i++)
	{
		if(mStatQryProcers[i].mStatDocidsBuff->dataLen() == 0)
		{
			// no more stat_docids.
			continue;
		}

		t1 = OmnGetTimestamp();
		
		rslt = readDataByEachDataScanner(rdata, mStatQryProcers[i].mDataScanner,
				mStatQryProcers[i].mStatDocidsBuff);
		aos_assert_r(rslt, false);
			
		t2 = OmnGetTimestamp();
		mStatQryProcers[i].mQryStatTime += (t2 - t1);
	}
	
	return true;
	
}


bool
AosDatasetByStatQuerySingle::readDataByEachDataScanner(
		const AosRundataPtr &rdata,
		const AosDataScannerObjPtr &data_scanner,
		const AosBuffPtr &stat_docids_buff)
{
	u64 tStart, tEnd;
	static u64 mSerialFromCostTime;
	aos_assert_r(stat_docids_buff, false);

	bool rslt = data_scanner->reset(rdata);
	aos_assert_r(rslt, false);

	rslt = data_scanner->setValueBuff(stat_docids_buff, rdata);
	aos_assert_r(rslt, false);
	data_scanner->startReadData(rdata);

	AosStatQryEnginePtr crt_qry_engine;
	AosBuffDataPtr buffdata;
	vector<AosStatRecord *> stat_rcds;
	while(1)
	{
		rslt = data_scanner->getNextBlock(buffdata, rdata);
		aos_assert_r(rslt, false);
		if (!buffdata) 
		{
			break;
		}
		while (1)
		{
			// Ken Lee, I think this place
			// need some code for shuffered buff.
			// maybe a while(1) is needed.
			break;
		}

		AosBuffPtr stat_data = buffdata->buffData();
		aos_assert_r(stat_data, false);
		crt_qry_engine = OmnNew AosStatQryEngine(rdata.getPtr(), &mStatQryInfo);
		crt_qry_engine->frontInit();
		if(mGroupByKeyType.size() > 0)
			crt_qry_engine->setGroupByKeyType(mGroupByKeyType);
		tStart = OmnGetTimestamp();
		crt_qry_engine->serializeFrom(rdata, stat_data.getPtr());
		tEnd = OmnGetTimestamp();
		mSerialFromCostTime += tEnd - tStart;
		OmnScreen << "(Statistics counters : Engine) serializeFrom cost --- Time : "
			<< mSerialFromCostTime << endl;

		
		//arvin 2015-10-8
		mStatQryEngine->setKeyIdxTypeMap(mKeyIdxTypeMap);
		mStatQryEngine->collectRecord(crt_qry_engine.getPtr(),true);
		//int merged_num = mStatQryEngine->mergeStatRecord(crt_qry_engine.getPtr(), true);
		//aos_assert_r(merged_num != -1, false);

		rslt = data_scanner->setRemainingBuff(buffdata, false,
				buffdata->dataLen(), rdata);
		aos_assert_r(rslt, false);
	}
	
	for(u32 i=0; i<stat_rcds.size(); i++)
	{
		delete stat_rcds[i];
	}

	return true;
}



void 
AosDatasetByStatQuerySingle::setTaskDocid(const u64 task_docid)
{
	mTaskDocid = task_docid;
}


bool
AosDatasetByStatQuerySingle::sendFinish(const AosRundataPtr &rdata)
{
	return true;
}


bool 
AosDatasetByStatQuerySingle::reset(const AosRundataPtr &rdata)
{
	//aos_assert_rr(mDataScanner, rdata, false);
	
	for(u32 i=0; i<mStatQryProcers.size(); i++)
	{
		mStatQryProcers[i].mDataScanner->reset(rdata);
	}
	return true;
}

bool
AosDatasetByStatQuerySingle::nextRecordset(
		const AosRundataPtr &rdata,
		AosRecordsetObjPtr &recordset)
{
	OmnScreen << "Ketty Stat Print. Get Recordset Start." << endl;
	u64 t1 = OmnGetTimestamp();
	
	aos_assert_r(mDataReady, false);
	bool rslt;
	
	recordset = cloneRecordset(rdata.getPtr());
	aos_assert_r(recordset, false);

	mDataRecord->clear();	// This is output data record.
	AosValueRslt value_rslt;
	bool outofmem;
	mTotalValues = mStatQryEngine->getRcdNum();
	mNumValues = mTotalValues - mStartIdx;
	if(mNumValues > mPageSize)	
		mNumValues = mPageSize;

	for (int k = 0; k < mNumValues; k++)
	{
		rslt = getNextInputRecord(rdata, mStartIdx + k, mInputDataRecord);
		aos_assert_r(rslt, false);
	
		for (u32 i=0; i<mFieldDef.size(); i++)
		{

			rslt = mFieldDef[i].mOExpr->getValue(rdata.getPtrNoLock(), mInputDataRecord.getPtr(), value_rslt);
			aos_assert_r(rslt, false);

			rslt = mDataRecord->setFieldValue(mFieldDef[i].mCidx, value_rslt, outofmem, rdata.getPtrNoLock());
			aos_assert_r(rslt, false);
			aos_assert_r(!outofmem, false);
		}
		mDataRecord->flush(true);
	}
	//move to next record for next query
	mStartIdx += mNumValues;             

	recordset->swap(rdata.getPtrNoLock(), mDataRecord);
	OmnScreen << "Ketty Stat Print. Get Recordset Finish. "
		<< "time:" << OmnGetTimestamp() - t1 << "; "
		<< "total_time:" << OmnGetTimestamp() - mQueryStartTime << "; "
		<< endl;

	mQueryFinishTime = OmnGetTimestamp();
	return true;
}


bool
AosDatasetByStatQuerySingle::getNextInputRecord(
		const AosRundataPtr &rdata,
		const u32 rcd_idx,
		AosDataRecordObjPtr &record)
{
	record->clear();
	AosValueRslt value_rslt;
	bool rslt, outofmem = false;
	for(u32 idx=0; idx<mInputStatFieldDefs.size(); idx++)
	{
		rslt = mStatQryEngine->getFieldValue(rcd_idx,
			mInputStatFieldDefs[idx].mStatFieldIdx, value_rslt);
		aos_assert_r(rslt, false);

		rslt = record->setFieldValue(mInputStatFieldDefs[idx].mInputFieldIdx,
				value_rslt, outofmem, rdata.getPtr());
		aos_assert_r(rslt, false);
	}
	record->flush(false);
	
	return true;
}
		
#if 0	
bool  
AosDatasetByStatQuerySingle::checkIsTimeField(
		const AosRundataPtr &rdata,
		const OmnString &field_name)
{
	
	OmnString field,fname;
	field = field_name;
	field.unescape();
	fname = field;
	int startIdx = fname.indexOf(0, '(', false);
	if(startIdx != -1)
	{
		int endIdx = fname.indexOf(0,')',false);
		field = OmnString(fname.data()+startIdx+1,endIdx-startIdx-1);
	}
	
	if (mStatQryProcers[0].mStat->isContainMeasure(field_name))
	{
		return false;	
	}
	if (mStatQryProcers[0].mStat->isContainTimeField(field))
	{
		return true;
	}
	return false;
}
#endif
/*
bool
AosDatasetByStatQuerySingle::getNextRecord(
		const AosRundataPtr &rdata,
		const u32 rcd_idx,
		AosDataRecordObjPtr &record)
{
	AosValueRslt value_rslt;
	bool rslt, outofmem = false;
	for(u32 idx=0; idx<mStatFieldDefs.size(); idx++)
	{
		rslt = mStatQryEngine->getFieldValue(rcd_idx,
			mStatFieldDefs[idx].mInputFieldIdx, value_rslt);
		aos_assert_r(rslt, false);
		
		outofmem = false;
		rslt = record->setFieldValue(mStatFieldDefs[idx].mOutputFieldIdx,
				value_rslt, outofmem, rdata.getPtr());
		aos_assert_r(rslt, false);
		// aos_assert_r(!outofmem, false);
	}
	record->flush(true);
		
	return true;
}
*/


bool
AosDatasetByStatQuerySingle::getRsltInfor(
		AosXmlTagPtr &infor,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mStatQryEngine, false);
		
	OmnString str = "<Contents";
		str << " total=\"" << mTotalValues << "\""
			<< " start_idx=\"" << mStatQryInfo.mPageOffset << "\""
			<< " num=\"" << mNumValues << "\""
			<< "/>";

	infor = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(infor, false);
	
	aos_assert_r(infor, false);

	OmnString time_str;
	time_str << (mQueryFinishTime - mQueryStartTime) / 1000.0 << "ms";
	infor->setAttr("time", time_str);

	return true;
}


