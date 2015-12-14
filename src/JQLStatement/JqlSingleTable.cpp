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
#include "JQLStatement/JqlSingleTable.h"

#include "JQLStatement/JqlQueryWhereConds.h"
#include "JQLStatement/JqlQueryInputDataset.h"
#include "JQLStatement/JqlSingleHbaseTable.h"
#include "JQLStatement/JqlSingleStatTable.h"
#include "JQLStatement/JqlSelect.h"

#include "JQLExpr/ExprFieldName.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"
#include "Util/OmnNew.h"
#include "Debug/Debug.h"
#include "API/AosApi.h"


OmnString
AosJqlSingleTable::generateTableConf(const AosRundataPtr &rdata)
{
	if (!mIsInited && mTableName)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return "";
	}

	OmnString conds;
	AosXmlTagPtr table = AosStr2Xml(rdata.getPtr(), "<table></table>" AosMemoryCheckerArgs);
	mTableDoc = table;

	OmnString table_name = mTableName->getValue(rdata.getPtr());

	// jimodb-671 phil
	if (!mInputDataset)
	{
		// normal table needs to this
		table_name = AosJqlStatement::getObjid(rdata, JQLTypes::eTableDoc, table_name);

		if (table_name == "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return "";
		}
	}
	if (mInputDataset)
	{
		//jimodb-671 Phil
		//this is subquery	
		table->setAttr("zky_type", "query_nest");
		table->setAttr("zky_name", table_name);
		table->setAttr("jimo_name", "jimo_dataset");
		table->setAttr("jimo_type", "jimo_dataset");
		table->setAttr("current_version", "0");
		table->setAttr("zky_otype", "zkyotp_jimo");
		table->setAttr("zky_objid", "AosDatasetByQueryNest");
		table->setAttr("zky_classname", "AosDatasetByQueryNest");
	}
	else
	{
		table->setAttr("zky_type", "single");
		table->setAttr("zky_name", table_name);
		table->setAttr("jimo_name", "jimo_dataset");
		table->setAttr("jimo_type", "jimo_dataset");
		table->setAttr("current_version", "0");
		table->setAttr("zky_otype", "zkyotp_jimo");
		table->setAttr("zky_objid", "AosDatasetByQuerySingle");
		table->setAttr("zky_classname", "AosDatasetByQuerySingle");
	}
	
	// JIMODB-991
	if (mIsSubQuery) table->setAttr("psize",AOSTAG_NOT_A_NUMBER);   
	if (mCondsIsAlwaysFalse) table->setAttr("psize", "-1");

	OmnString ver_str = "<versions><ver_0>libDatasetJimos.so</ver_0></versions>";
	AosXmlTagPtr ver = AosXmlParser::parse(ver_str AosMemoryCheckerArgs); 
	table->addNode(ver); 

	if (!mConds.empty() && ! mIsCondFilter)
	{
		conds = generateWhereConf(rdata);
		//if (conds == "") return "";
	} else {
		if (mIsCondFilter) 
		{
			OmnString where_filter_str = AosJqlQueryWhereConds::generateWhereFilterConf(rdata, mWhereCondsStr);
			AosXmlTagPtr where_filter = AosXmlParser::parse(where_filter_str AosMemoryCheckerArgs);
			mTableDoc->addNode(where_filter);
		}
		if (!mIndexHint) 
			conds = generateDefaultIndexConf(rdata);
		else
		{
			conds << "<conds>" << generateIndexHint(rdata) << "</conds>";
		}
	}

	AosXmlTagPtr node = AosXmlParser::parse(conds AosMemoryCheckerArgs);
	//if (!node)
	//{
	//	AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
	//	OmnAlarm << __func__ << enderr;
	//	return "";
	//}
aos_assert_use_err(node, rdata, "generateTableConf::generateTableConf", "Failed to generate conds config.", "");
	table->addNode(node);

	if (!mGroupByFields.empty() || !mInputAggrFields.empty())
	{
		OmnString groupby_conf = generateGroupByConf(rdata);
		AosXmlTagPtr conf = AosXmlParser::parse(groupby_conf AosMemoryCheckerArgs);
		if (conf)
		{
			if (!mGroupByFields.empty())
				conf->setAttr("need_sort", "true");
			table->addNode(conf);
		}
	}

	if (!mOrderByFields.empty())
	{
		OmnString order_conf = generateOrderByConf(rdata);
		if (order_conf != "")
		{
			AosXmlTagPtr conf = AosXmlParser::parse(order_conf AosMemoryCheckerArgs);
			table->addNode(conf);
		}
	}

	OmnString conf_str = "";
	AosXmlTagPtr conf;
	//jimodb-671 Phil
	//If has subquery, the input dataset is the result
	//of the subquery.
	if (!mInputDataset)
	{
		conf_str = generateInputDatasetConf(rdata);
		conf = AosXmlParser::parse(conf_str AosMemoryCheckerArgs);
		//if (!conf)
		//{
		//	AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		//	OmnAlarm << __func__ << enderr;
		//	return "";
		//}
		aos_assert_use_err(conf, rdata, "generateTableConf::generateTableConf", "Failed to generate input dataset config.", "");
		table->addNode(conf);
	}
	conf_str = generateOutputDatasetConf(rdata);
	conf = AosXmlParser::parse(conf_str AosMemoryCheckerArgs);
	//if (!conf)
	//{
	//	AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
	//	OmnAlarm << __func__ << enderr;
	//	return "";
	//}
aos_assert_use_err(conf, rdata, "generateTableConf::generateTableConf", "Failed to generate output dataset config.", "");
	table->addNode(conf);
	return table->toString();
}


OmnString
AosJqlSingleTable::generateWhereConf(const AosRundataPtr &rdata)
{
//	if (!mWherePtr) return "";
	OmnString table_name = mTableName->getValue(rdata.getPtrNoLock());
	if (table_name == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return "";
	}

	AosXmlTagPtr conds, where_filter;
	AosJqlQueryWhereConds query_where_conds;

	if (mWhereCondsStr == "") query_where_conds.setIsAllSingle(true);

	// jimodb-671
	// get record from inputdataset
	AosDataRecordObjPtr record;

	if (mInputDataset)
	{
		vector<AosDataRecordObjPtr> records;

		mInputDataset->getRecords(records);
		record = records[0];
		aos_assert_r(record, "");	
	}
	bool rslt = query_where_conds.init(rdata, mConds, table_name, record);
	if ( !rslt ) return "";
	query_where_conds.getConfig(rdata, conds, where_filter);
	if (!conds)
	{
		//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		//OmnAlarm << __func__ << enderr;
		return "";
	}

	if (mIndexHint != NULL) 
	{
		OmnString index_hint_str = generateIndexHint(rdata);
		aos_assert_r(index_hint_str != "", "");
		AosXmlTagPtr index_hint = AosXmlParser::parse(index_hint_str AosMemoryCheckerArgs);
		if (!index_hint)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return "";
		}

		AosXmlTagPtr child = index_hint->getFirstChild(true);
		while (child)
		{
			conds->addNode(child);
			child = index_hint->getNextChild();
		}
	}

	if (where_filter) mTableDoc->addNode(where_filter);        
	return conds->toString();
}


OmnString
AosJqlSingleTable::generateDefaultIndexConf(const AosRundataPtr &rdata)
{
	if (!mTableName)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return "";
	}
	OmnString iilname, value, opr;

	OmnString table_name = mTableName->getValue(rdata.getPtrNoLock());
	AosXmlTagPtr table_doc = AosJqlStmtTable::getTable(table_name, rdata);
	OmnString conds = "";
	conds << "<conds><cond type=\"AND\">"
		  <<  AosJqlQueryWhereConds::generateDefaultTermConf(rdata, table_doc, table_name)
		  << "</cond></conds>";
	return conds;
}

OmnString
AosJqlSingleTable::generateIndexHint(const AosRundataPtr &rdata)
{
	OmnString str = "<cond type=\"AND\">";
	AosExprObjPtr expr;
	AosJqlQueryWhereCondPtr cond = OmnNew AosJqlQueryWhereCond();
	cond->mOpr = "an";
	cond->mRHS = "*";
	OmnString table_name = mTableName->getValue(rdata.getPtrNoLock());

	for (u32 i = 0; i < mIndexHint->size(); i++)
	{
		expr = (*mIndexHint)[i];
		if (!expr &&  expr->getValue(rdata.getPtrNoLock()) != "")
		{
			AosSetErrorUser(rdata,  __func__) 
				<< "Index name is null." << enderr;
			OmnAlarm << __func__ << enderr;
			return "";
		}
		cond->mIILName << "_zt44_" << expr->getValue(rdata.getPtrNoLock());
		str <<  AosJqlQueryWhereConds::generateTermConf(rdata, table_name, cond);
	}

	str << "</cond>";
	return str;
}


OmnString
AosJqlSingleTable::generateInputDatasetConf(const AosRundataPtr &rdata)
{
	OmnString table_name = mTableName->getValue(rdata.getPtrNoLock());
	AosXmlTagPtr table_doc = AosJqlStmtTable::getTable(table_name, rdata);
	AosJqlQueryInputDataset input_dataset(table_doc, table_name, mInputFields);
	input_dataset.init(rdata);
	AosXmlTagPtr conf = input_dataset.getConfig(rdata);
	if (!conf)
	{
		AosSetErrorUser(rdata,  __func__) 
				<< "Failed to generate input dataset config." << enderr;
		OmnAlarm << __func__ << enderr;
		return "";
	}
	return conf->toString();
}


bool 
AosJqlSingleTable::getAllField(const AosRundataPtr &rdata, vector<AosJqlSelectFieldPtr> *&fields)
{
	if (!fields)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	//arvin 2015.07.22
	//JIMODB-50:return correct meassage
	bool rslt = false;
	for(size_t i = 0;i < fields->size();i++)
	{
		OmnString fieldName = (*fields)[i]->getFieldName(rdata);
		if(fieldName == "*")
		{
			rslt = true;
			break;
		}
	}

	//jimodb-671 Phil
	AosXmlTagPtr record_doc;
	if (mInputDataset)
	{
		vector<AosDataRecordObjPtr> records;
		AosDataRecordObjPtr record;

		//if has subquery, get the record doc from the input
		//dataset
		mInputDataset->getRecords(records);
		record = records[0];
		aos_assert_r(record, false);
		
		// jimodb-871
		record_doc = record->getRecordDoc()->getFirstChild("datarecord");
	}

	else
	{
		OmnString table_name;
		table_name = mTableName->getValue(rdata.getPtrNoLock());

		AosXmlTagPtr table_doc = AosJqlStmtTable::getTable(table_name, rdata);
		if (!table_doc && rslt)
		{
			//arvin 2015.07.22
			//JIMODB-50:return correct meassage
			OmnString msg = "[ERR] : Table \"";
			msg << table_name << "\" doesn't exist!";
			rdata->setJqlMsg(msg);
			return false;
		}

		OmnString schemaObjid = table_doc->getAttrStr("zky_use_schema");
		AosXmlTagPtr schema_doc = AosJqlStatement::getDocByObjid(rdata, schemaObjid);
		if (!schema_doc) return false;

		record_doc = schema_doc->getFirstChild("datarecord");
	}
	if (!record_doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	AosXmlTagPtr fieldsNode = record_doc->getFirstChild("datafields");
	if (!fieldsNode)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	OmnString field_name; 
	AosJqlSelectFieldPtr field;
	AosXmlTagPtr col = fieldsNode->getFirstChild("datafield");
	while (col)
	{
		field_name = col->getAttrStr("zky_name");
		if (field_name == "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		field = OmnNew AosJqlSelectField;
		field->setField(OmnNew AosExprFieldName(field_name.getBuffer()));
		fields->push_back(field);
		col = fieldsNode->getNextChild("datafield");
	}
	return true;
}


AosJqlSingleTablePtr
AosJqlSingleTable::createStatic(
		const AosRundataPtr &rdata,
		const AosJqlSelectPtr &select,
		const AosExprObjPtr &table_expr,
		const AosExprObjPtr &alias,
		AosExprList *index_hint)
{
	if (!select && table_expr)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return NULL;
	}
	AosJqlSingleTablePtr table;
	//mTableName = table;
	//mAliasName = alias;

	if (AosJqlSingleHbaseTable::checkIsHbaseTable(
			rdata, table_expr, select))
	{
		table = OmnNew AosJqlSingleHbaseTable();
	}
	else if(AosJqlSingleStatTable::checkIsStatQuery(rdata, select, table_expr, alias))
	{
		// Ketty 2014/09/24
		table = OmnNew AosJqlSingleStatTable();
	}
	else {
		table = OmnNew AosJqlSingleTable();
	}
	if (!table)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return table;
	}
	table->mTableName = table_expr;
	table->mAliasName = alias;
	table->mIndexHint = index_hint;
	return table;
}

AosJqlSingleTable* 
AosJqlSingleTable::clone(const AosRundataPtr &rdata) const
{
	try
	{
		AosJqlSingleTable* v = new AosJqlSingleTable();
		return v;
	}
	catch(const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}
	OmnShouldNeverComeHere;
	return 0;
}

