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
// 2014/05/07 Created By Andy Zhang 
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlSelectTable.h" 
#include "JQLStatement/JqlSelect.h"
#include "JQLStatement/JqlWhere.h"

#include "JQLExpr/ExprFieldName.h"
#include "JQLExpr/Ptrs.h"

#include "AosConf/DataRecordCtnr.h"           
#include "AosConf/DataFieldStr.h"           
#include "AosConf/DataRecord.h"           

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEUtil/JqlUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"

#include <set>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "DataRecord/DataRecord.h"
using namespace AosConf;

AosJqlSelectTable::AosJqlSelectTable()
:
mIsInited(false),
mIsCondFilter(false),
mCondsIsAlwaysFalse(false)
{
	 mInputDataset = NULL; 
	 mIsSubQuery = false;
}


AosJqlSelectTable::~AosJqlSelectTable()
{
}

bool
AosJqlSelectTable::init(
		const AosRundataPtr &rdata, 
		const AosJqlSelectPtr &select)
{
	if (mIsInited) return true;

	mInputFieldsMap.clear();
	mInputExprFields.clear();

	aos_assert_r(mTableName, false);
	mTableNameStr = mTableName->getValue(rdata.getPtrNoLock());

	bool rslt = initOutputFieldList(rdata, select);
	aos_assert_r(rslt, false);

	rslt = initWhereConds(rdata, select);
	aos_assert_r(rslt, false);

	rslt = initGroupByFields(rdata, select);
	aos_assert_r(rslt, false);

	rslt = initCubeFields(rdata, select);
	aos_assert_r(rslt, false);

	rslt = initRollupLists(rdata, select);
	aos_assert_r(rslt, false);

	rslt = initOrderByFields(rdata, select);
	aos_assert_r(rslt, false);

	rslt = initInputFields(rdata);
	aos_assert_r(rslt, false);

	rslt = checkOrderByFeildIsInConds(rdata);
	aos_assert_r(rslt, false);

	mIsInited = true;
	return true;
}


bool
AosJqlSelectTable::initOutputFieldList(
		const AosRundataPtr &rdata,
		const AosJqlSelectPtr &select)
{
	aos_assert_r(select, false);
	mOutputFields.clear();

	bool rslt = false;
	OmnString field_name;
	AosJqlSelectFieldPtr field = NULL; 
	AosJqlSelectFieldList* field_list = select->mFieldListPtr;
	aos_assert_r(select, false);

	for (u32 i = 0; i < field_list->size(); i++)
	{
		field = (*field_list)[i];
		aos_assert_r(field, false);
		rslt = handleField(rdata, select, field);
		if (!rslt)
		{
			AosSetErrorUser(rdata, __func__)
				<< "Field to handle field." << enderr;
			OmnAlarm << enderr;
			return false;
		}
		mOutputFields.push_back(field);
	}
	return true;
}


bool
AosJqlSelectTable::initWhereConds(
		const AosRundataPtr &rdata,
		const AosJqlSelectPtr &select)
{
	aos_assert_r(select, false);
	mConds.clear();

	bool rslt = false;
	AosExprObjPtr expr;
	map<OmnString, AosAliasName> aliasNameMap = select->mAliasNameMap;
	if(!select->mWherePtr) return true;

	expr = select->mWherePtr->mWhereExpr;
	mCondsIsAlwaysFalse = expr->isAlwaysFalse();
	vector<AosJqlSelectFieldPtr> where_fields;
	rslt = expr->getFields(rdata, where_fields);
	aos_assert_r(rslt, false);
	for (u32 i = 0; i < where_fields.size(); i++)
	{
		rslt = handleField(rdata, select, where_fields[i]);
		aos_assert_r(rslt, false);
	}

	mIsCondFilter = expr->isHasOr();
	if (!mIsCondFilter) mIsCondFilter = expr->isJoin();
	//if (!mIsCondFilter) mIsCondFilter = expr->isExprCond();

	mWhereCondsStr = expr->dump();
	aos_assert_r(expr, false);

	if (!mIsCondFilter)
	{
		rslt = expr->createConds(rdata, mConds, mTableNameStr);
		aos_assert_r(rslt, false);
	}

	for (u32 i = 0; i < mConds.size(); i++)
	{
		if (aliasNameMap.find(mConds[i]->mLHS) != aliasNameMap.end())
		{
			mConds[i]->mLHS = aliasNameMap[mConds[i]->mLHS].mOName;
		}
	}
	
	return true;
}

bool
AosJqlSelectTable::initGroupByFields(
		const AosRundataPtr &rdata,
		const AosJqlSelectPtr &select)
{
	bool rslt = false;
	mGroupByFields.clear();
	AosJqlGroupByPtr groupby = select->mGroupByPtr;
	if (!groupby) return true;

	OmnString field_name = "";
	AosJqlSelectFieldPtr field = NULL; 
	AosJqlSelectFieldList* field_list = groupby->getGroupFieldList(); 
	if (!field_list) return true;

	for (u32 i = 0; i < field_list->size(); i++)
	{
		field = (*field_list)[i];
		aos_assert_r(field, false);
		rslt = checkIsOrderFieldOrGroupByField(field, rdata);
		if (!rslt)
		{
			AosSetErrorUser(rdata, __func__)
				<< "The field is not group by field. Field is " << field->dump1() << enderr;
			OmnAlarm << enderr;
			return false;
		}

		rslt = handleField(rdata, select, field);
		if (!rslt)
		{
			AosSetErrorUser(rdata, __func__)
				<< "Field to handle field." << enderr;
			OmnAlarm << enderr;
			return false;
		}

		mGroupByFields.push_back(field);
	}
	return true;
}

bool
AosJqlSelectTable::initCubeFields(
		const AosRundataPtr &rdata,
		const AosJqlSelectPtr &select)
{
	bool rslt = false;
	mCubeFields.clear();
	AosJqlGroupByPtr groupby = select->mGroupByPtr;
	if (!groupby) return true;

	OmnString field_name = "";
	AosJqlSelectFieldPtr field = NULL; 
	AosJqlSelectFieldList* cube_lists = groupby->getCubeLists(); 
	if (!cube_lists) return true;

	for (u32 i = 0; i < cube_lists->size(); i++)
	{
		field = (*cube_lists)[i];
		aos_assert_r(field, false);
		rslt = checkIsOrderFieldOrGroupByField(field, rdata);
		if (!rslt)
		{
			AosSetErrorUser(rdata, __func__)
				<< "The field is not group by field. Field is " << field->dump1() << enderr;
			OmnAlarm << enderr;
			return false;
		}

		rslt = handleField(rdata, select, field);
		if (!rslt)
		{
			AosSetErrorUser(rdata, __func__)
				<< "Field to handle field." << enderr;
			OmnAlarm << enderr;
			return false;
		}

		mCubeFields.push_back(field);
	}
	return true;
}
bool

AosJqlSelectTable::initRollupLists(
		const AosRundataPtr &rdata,
		const AosJqlSelectPtr &select)
{
	bool rslt = false;
	mRollupLists.clear();

	AosJqlGroupByPtr groupby = select->mGroupByPtr;
	if (!groupby) return true;

	OmnString field_name = "";
	AosJqlSelectFieldPtr field = NULL;
	AosJqlSelectFieldList* field_list;
	AosJqlSelectFieldList checked_field_list;
	vector<AosJqlSelectFieldList *> *rollup_lists = groupby->getRollupLists();
	//aos_assert_r(rollup_lists, false);
	if (!rollup_lists) return true;

	for (u32 j = 0; j < rollup_lists->size(); j++)
	{
		field_list = (*rollup_lists)[j];
		checked_field_list.clear();
		for (u32 i = 0; i < field_list->size(); i++)
		{
			field = (*field_list)[i];
			aos_assert_r(field, false);
			rslt = checkIsOrderFieldOrGroupByField(field, rdata);
			if (!rslt)
			{
				AosSetErrorUser(rdata, __func__)
					<< "The field is not group by field. Field is " << field->dump1() << enderr;
				OmnAlarm << enderr;
				return false;
			}

			rslt = handleField(rdata, select, field);
			if (!rslt)
			{
				AosSetErrorUser(rdata, __func__)
					<< "Field to handle field." << enderr;
				OmnAlarm << enderr;
				return false;
			}

			checked_field_list.push_back(field);
		}
		mRollupLists.push_back(checked_field_list);
	}

	return true;
}


bool
AosJqlSelectTable::initOrderByFields(
		const AosRundataPtr &rdata,
		const AosJqlSelectPtr &select)
{
	bool rslt = false;
	mOrderByFields.clear();
	AosJqlOrderByPtr order = select->mOrderByPtr;
	if (!order) return true;

	OmnString field_name = "";
	AosJqlSelectFieldPtr field = NULL; 
	AosJqlOrderByFieldList* field_list = order->getOrderFieldList(); 
	aos_assert_r(field_list, false);

	for (u32 i = 0; i < field_list->size(); i++)
	{
		field = (*field_list)[i]->mField;
		aos_assert_r(field, false);
		//aos_assert_r(field->isName(), false);
		rslt = checkIsOrderFieldOrGroupByField(field, rdata);
		if (!rslt)
		{
			AosSetErrorUser(rdata, __func__)
				<< "The field is not group by field. Field is " << field->dump1() << enderr;
			OmnAlarm << enderr;
			return false;
		}

		rslt = handleField(rdata, select, field);
		if (!rslt)
		{
			AosSetErrorUser(rdata, __func__)
				<< "Field to handle field." << enderr;
			OmnAlarm << enderr;
			return false;
		}
		mOrderByFields.push_back((*field_list)[i]);
	}
	return true;
}


bool
AosJqlSelectTable::initInputFields(const AosRundataPtr &rdata)
{
	map<OmnString, AosJqlSelectFieldPtr>::iterator itr;
	OmnString table_name = mTableName->getValue(rdata.getPtrNoLock());
	OmnString field_name;
	for (itr = mInputFieldsMap.begin(); itr != mInputFieldsMap.end(); itr++) {
		mInputFields.push_back(itr->second);
	}
	if (!mInputExprFields.empty())
		mInputFields.insert(mInputFields.end(), mInputExprFields.begin(), mInputExprFields.end());

	return true;
}


bool
AosJqlSelectTable::checkOrderByFeildIsInConds(const AosRundataPtr &rdata)
{
	for (u32 i = 0; i < mOrderByFields.size(); i++)
	{
		for (u32 j = 0; j < mConds.size(); j++)
		{
			if (mOrderByFields[i]->mField->getFieldEscapeOName(rdata) == mConds[j]->mLHS)
			{
				mOrderByFields[i]->mIsInWhereCond = true;
				mConds[j]->mIsOrder = true;
				mConds[j]->mIsAsc = mOrderByFields[i]->mIsAsc;
			}
		}
	}
	return true;
}


OmnString
AosJqlSelectTable::generateGroupByConf(const AosRundataPtr &rdata)
{
	OmnString conf = "";
	OmnString oname, cname, fun_str; 
	conf << "<groupby type=\"norm\">";
	conf << "<fields>";

	for (u32 i = 0; i < mGroupByFields.size(); i++)
	{
		oname =  mGroupByFields[i]->getFieldEscapeOName(rdata);
		aos_assert_r(oname != "", "");
		conf <<  "<field>" << oname << "</field>";
	}

	conf << "</fields>";
	conf << "<aggregations>";

	for (u32 i = 0; i < mInputAggrFields.size(); i++)
	{
		OmnString distinct;
		fun_str = mInputAggrFields[i]->getAgrFuncTypeStr();
		//oname =  mInputAggrFields[i]->getFieldName(rdata);
		oname =  mInputAggrFields[i]->getFieldEscapeOName(rdata);
		cname =  mInputAggrFields[i]->getFieldEscapeCName(rdata);
		if (fun_str == "dist_count") distinct = "true";
		if (fun_str == "count")
		{
			AosExprObjPtr func =  mInputAggrFields[i]->getField();
			aos_assert_r(func, "");
			AosExprGenFuncPtr func_expr = dynamic_cast<AosExprGenFunc*>(func.getPtr());
			AosExprList *parms = func_expr->getParmList(); 
			//aos_assert_r(parms && parms->size() == 1, ""); 
		//	if ((*parms)[0]->isConstant())
		//	{
		//		oname = "*";
				oname = (*parms)[0]->dumpByNoEscape();	
				mInputAggrFields[i]->mOName = oname;
		//	}
		}
		conf << "<aggregation type=\"" << fun_str << "\" distinct=\"" << distinct << "\">"
			<< "<oname>" << oname << "</oname>"
			<< "<cname>" << cname << "</cname>"
			<< "</aggregation>";
	}

	conf << "</aggregations>";
	conf << "</groupby>";
	return conf;
}


OmnString
AosJqlSelectTable::generateOrderByConf(const AosRundataPtr &rdata)
{
	OmnString cond = "";
	OmnString field_name, iilname, conf;
	OmnString table_name = ""; 
	AosXmlTagPtr node = NULL;
	AosJqlQueryWhereCondPtr where_cond;
	table_name = mTableName->getValue(rdata.getPtrNoLock());
	aos_assert_r(table_name != "", "");

	AosXmlTagPtr conds = mTableDoc->getFirstChild("conds");
	aos_assert_r(conds, "");

	AosXmlTagPtr cond_tag = conds->getFirstChild("cond");
	aos_assert_r(cond_tag, "");

	set<OmnString> m_set;
	map<OmnString, bool> m_map; 
	AosXmlTagPtr term = cond_tag->getFirstChild(true);
	while (term)
	{
		term->setAttr("order", "false");
		term = cond_tag->getNextChild();
	}


	AosXmlTagPtr table_doc = AosJqlStmtTable::getTable(table_name, rdata);	
	for (u32 i = 0; i < mOrderByFields.size(); i++)
	{
		cond = "";
		AosJqlOrderByFieldPtr field = mOrderByFields[i];
		field_name = field->mField->getFieldEscapeOName(rdata);
		m_set.insert(field_name);
		m_map[field_name] = field->mIsAsc;
	}

	

	if (mInputDataset)
	{
		//do nothing for now.
		//There is no index for dataset query
	}
	else
	{
		iilname = AosJqlStatement::getIILName(rdata, table_doc, m_set); 

		bool flag = false;
		if (iilname != "")
		{
			term = cond_tag->getFirstChild(true);
			while (term)
			{
				OmnString iilname2 = term->getAttrStr("iilname");
				if (iilname == iilname2)
				{
					term->setAttr("order", "true");
					term->setAttr("reverse", !m_map[field_name]);
					flag = true;
					break;
				}
				term = cond_tag->getNextChild();
			}
			if (!flag)
			{
				where_cond = OmnNew AosJqlQueryWhereCond();
				where_cond->mIsOrder = true;
				where_cond->mIsAsc= m_map[field_name];
				where_cond->mOpr = "an";
				where_cond->mRHS = "*";
				where_cond->mIILName = iilname; 

				cond << AosJqlQueryWhereConds::generateTermConf(rdata, table_name, where_cond); 
				node = AosXmlParser::parse(cond AosMemoryCheckerArgs); 
				aos_assert_r(node, "");
				cond_tag->addNode(node);
			}
			return conf;
		}
	}


	conf  << "<orderby><fields>";
	for (u32 i = 0; i < mOrderByFields.size(); i++)
	{
		AosJqlOrderByFieldPtr field = mOrderByFields[i];
		field_name = field->mField->getFieldEscapeOName(rdata);
		conf << generateOrderByNormlConf(field, rdata);
	}
	conf << "</fields></orderby>";
	return conf;
}


OmnString
AosJqlSelectTable::generateOrderByNormlConf(
		const AosJqlOrderByFieldPtr &order_field,
		const AosRundataPtr &rdata)
{
	OmnString conf = "";
	OmnString table_name = mTableName->getValue(rdata.getPtrNoLock());
	aos_assert_r(table_name != "", "");
	aos_assert_r(order_field && order_field->mField, "");
	OmnString field_name = order_field->mField->getFieldEscapeCName(rdata);
	aos_assert_r(field_name != "", "");
	conf << "<field reverse=\"" << !(order_field->mIsAsc) << "\"><![CDATA[" << field_name << "]]></field>";
	return conf;
}


OmnString
AosJqlSelectTable::generateOutputDatasetConf(const AosRundataPtr &rdata)
{
	if (mIsSubQuery)
		return generateBuffOutputDatasetConf(rdata);

	OmnString name, field_name, tagname;
	OmnString field_value_type, precision, scale;
	OmnString table_name = mTableName->getValue(rdata.getPtrNoLock());
	aos_assert_r(table_name != "", 0);
	name << "output_datarecord" << "_" << table_name;

	boost::shared_ptr<DataRecord> drec = boost::make_shared<DataRecord>();     
	drec->setAttribute("zky_name", name);
	drec->setAttribute("type", "xml");

	AosJqlColumnPtr column = 0; 
	AosJqlSelectFieldPtr field;
	for (u32 i = 0; i < mOutputFields.size(); i++)
	{
		field_value_type = precision = scale = "";
		field = mOutputFields[i];
		field_name = field->getFieldEscapeOName(rdata);

		//jimodb-671, support subquery
		AosXmlTagPtr record_doc;
		if (mInputDataset)
		{
			//if inputdataset, we need to get columns from
			//dataset's data record
			column = getDatasetColumn(rdata, mInputDataset, field_name);
		}
		else
		{
			column = AosJqlStatement::getTableField(rdata, table_name, field_name);
		}

		if (column)
		{
			field_value_type = AosDataType::getTypeStr(column->type);
			field_value_type = AosJqlStatement::AosJType2DType(field_value_type);
			precision << column->precision;
			scale << column->scale;
		}

		field_name = field->getFieldEscapeCName(rdata);
		if (field_name == "") field_name =  field->getFieldEscapeOName(rdata);
		aos_assert_r(field_name != "",  "");

		boost::shared_ptr<DataFieldStr> data_field = boost::make_shared<DataFieldStr>();
		data_field->setAttribute("zky_name", field_name);
		tagname = field_name;                    
		tagname << "/_#text";                              
		data_field->setAttribute("zky_xpath", tagname);    
		data_field->setAttribute("type", "xpath");         
		data_field->setAttribute(AOSTAG_VALUE_TYPE, field_value_type);         
		data_field->setAttribute("zky_precision", precision);
		data_field->setAttribute("zky_scale", scale);
		drec->setField(data_field);
	}
	boost::shared_ptr<DataRecordCtnr> drecord = boost::make_shared<DataRecordCtnr>();                   
	drecord->setAttribute("type", "ctnr");
	drecord->setAttribute("zky_name", "doc");          
	drecord->setRecord(drec);
	drecord->setTagName("outputrecord");                                              
	return drecord->getConfig();
}


OmnString
AosJqlSelectTable::generateBuffOutputDatasetConf(const AosRundataPtr &rdata)
//const OmnString &stat_conf)
{
	//generate buff type output dataset which can be used
	//in where condition
	OmnString name, field_name, tagname;
	OmnString field_value_type, precision, scale;
	OmnString table_name = mTableName->getValue(rdata.getPtrNoLock());
	aos_assert_r(table_name != "", 0);
	name << "output_datarecord" << "_" << table_name;

	//AosJqlColumnPtr column = 0; 
	AosJqlSelectFieldPtr field;
	AosXmlTagPtr datafield_doc, datafields_doc, datarecord_doc, outputrecord_doc;

	OmnString datafields = "<datafields></datafields>";
	datafields_doc = AosXmlParser::parse(datafields AosMemoryCheckerArgs);

	OmnString datarecord = "<datarecord></datarecord>";
	datarecord_doc = AosXmlParser::parse(datarecord AosMemoryCheckerArgs);

	datarecord_doc->setAttr("zky_name", name);
	datarecord_doc->setAttr("type", "buff");

	OmnString outputrecord = "<outputrecord></outputrecord>";
	outputrecord_doc = AosXmlParser::parse(outputrecord AosMemoryCheckerArgs);

	outputrecord_doc->setAttr("zky_name", "doc");
	outputrecord_doc->setAttr("type", "ctnr");

	for (u32 i = 0; i < mOutputFields.size(); i++)
	{
		field_value_type = precision = scale = "";
		field = mOutputFields[i];
		field_name = field->getFieldEscapeOName(rdata);

		//jimodb-671, support subquery
		if (mInputDataset)
		{
			//if inputdataset, we need to get columns from
			//dataset's data record
			//jimodb-858
			datafield_doc = getDatasetField(rdata, mInputDataset, field);
			aos_assert_r(datafield_doc,"");
		}
		else
		{
			// jimodb-671 
			// if can not find field in stat doc, find it in table doc
			//datafield_doc = AosJqlStatement::getField(rdata, table_name, field_name, stat_conf);
			datafield_doc = AosJqlStatement::getTableDataField(rdata, table_name, field);
			 aos_assert_r(datafield_doc,""); 
		}
		datafields_doc->addNode(datafield_doc);
	}
	datarecord_doc->addNode(datafields_doc);
	outputrecord_doc->addNode(datarecord_doc);
	return outputrecord_doc->toString();
}
OmnString 
AosJqlSelectTable::generateHavingConf(const AosRundataPtr &rdata)
{
	return "";
}

bool
AosJqlSelectTable::handleField(
		const AosRundataPtr &rdata,
		const AosJqlSelectPtr &select,
		const AosJqlSelectFieldPtr &field)
{	
	bool rslt = false;
	OmnString field_name;

	rslt = handleAliasName(rdata, select, field);
	if (!rslt)
	{
		AosSetErrorUser(rdata, __func__)
			<< "Field to handle alias name." << enderr;
		OmnAlarm << enderr;
		return false;
	}

	if (field->isMemberField())
	{
		AosExprMemberOpt *mem_expr = dynamic_cast<AosExprMemberOpt*>(field->mExpr.getPtr());
		if (mem_expr->checkSequenceExist(rdata))
		{
			field->mExpr = mem_expr->getSequenceExpr(rdata);
			field->mOName = field->dumpByNoEscape();
			AosJqlUtil::escape(field->mOName);
			field->mCName = mem_expr->getSequenceName(rdata);
			AosJqlUtil::escape(field->mCName);
		}
	}

	if (field->isName() || field->checkIsAgrFunc())
	{
		field_name = field->getFieldName(rdata);
		aos_assert_r(field_name != "", false);
		if (field->checkIsAgrFunc())
			mInputAggrFields.push_back(field);

		mInputFieldsMap[field->mOName] = field;
	}
	else
	{
		vector<AosJqlSelectFieldPtr> fields;
		rslt = field->getFields(rdata, fields);
		if (!rslt)
		{
			AosSetErrorUser(rdata, __func__)
					<< "Field to get fields." << enderr;	
			OmnAlarm << enderr;
			return false;
		}

		for (u32 j = 0; j < fields.size(); j++)
		{
			rslt = handleAliasName(rdata, select, fields[j]);
			if (!rslt)
			{
				AosSetErrorUser(rdata, __func__)
					<< "Field to handle alias name." << enderr;
				OmnAlarm << enderr;
				return false;
			}

			mInputFieldsMap[fields[j]->mOName] = fields[j];
			if (fields[j]->checkIsAgrFunc())
			{
				mInputAggrFields.push_back(fields[j]);
			}
		}

		mInputExprFields.push_back(field);
	}
	return true;
}


bool
AosJqlSelectTable::handleAliasName(
		const AosRundataPtr &rdata,
		const AosJqlSelectPtr &select,
		const AosJqlSelectFieldPtr &field)
{
	aos_assert_r(select && field && mTableName, false);
	if (field->mIsInited) return true;

	map<OmnString, AosAliasName> aliasNameMap = select->mAliasNameMap;
	OmnString field_name;

	// Ketty ????
	//if (field->checkIsAgrFunc() || field->checkIsFunc())  
	if (field->isName())// || field->checkIsAgrFunc())
		field_name = field->getFieldName(rdata);
	else
		field_name = field->dumpByNoEscape();

	AosJqlUtil::escape(field_name);

	if (field->getAliasName(rdata) != "")
	{
		if (field->checkIsAgrFunc())
			field->mOName = field->getFieldName(rdata);
		else
			field->mOName = field_name;

		field->mCName = field->getAliasName(rdata);
	} 
	else if (aliasNameMap.find(field_name) != aliasNameMap.end())
	{
		field->mOName = aliasNameMap[field_name].mOName;
		field->mCName = aliasNameMap[field_name].mCName;
	}
	else
	{
		if (field->checkIsAgrFunc())
			field->mOName = field->getFieldName(rdata);
		else
			field->mOName = field_name;

		field->mCName = field_name;
	}
	field->mIsInited = true;
	field->mTableName = mTableName->getValue(rdata.getPtrNoLock());
	return true;
}


bool
AosJqlSelectTable::getAllField(
		const AosRundataPtr &rdata,
		const AosJqlSelectPtr &select,
		vector<AosJqlSelectFieldPtr> *&fields)
{
	aos_assert_r(NULL, false);
	return false;
};


vector<AosJqlSelectFieldPtr> 
AosJqlSelectTable::getTableFieldList(const OmnString &table_name, bool &rslt)
{
	vector<AosJqlSelectFieldPtr>  field_list;
	aos_assert_r(mTableName, field_list);
	OmnString name = mTableName->getValue(0);
	OmnString as_name;
	if (mAliasName)
		as_name = mAliasName->getValue(0);
	if (name == table_name || as_name == table_name)
	{
		rslt = true;
		return mInputFields;
	}
	rslt = false;
	return field_list;
}



AosJqlSelectTable*
AosJqlSelectTable::clone(const AosRundataPtr &rdata)const
{
	try
	{
		AosJqlSelectTable* v = new AosJqlSelectTable();
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


bool
AosJqlSelectTable::checkIsOrderFieldOrGroupByField(
		const AosJqlSelectFieldPtr &field,
		const AosRundataPtr &rdata)
{
	if (field->isName()) return true;
	if (field->checkIsAgrFunc() 
			|| field->checkIsFunc()) return true;
	return false;
}

AosJqlColumnPtr 
AosJqlSelectTable::getDatasetColumn(
		const AosRundataPtr &rdata, 
		const AosDatasetObjPtr &dataset,
		const OmnString &field_name)
{
	AosJqlColumnPtr tColumn = OmnNew AosJqlColumn;
	vector<AosDataRecordObjPtr> records;
	AosDataRecordObjPtr record;

	dataset->getRecords(records);
	record = records[0];
	aos_assert_r(record, NULL);

	AosXmlTagPtr record_doc = record->getRecordDoc();

	AosXmlTagPtr datarecord = record_doc->getFirstChild("datarecord");
	aos_assert_r(datarecord, NULL);

	AosXmlTagPtr fieldNodes = datarecord->getFirstChild("datafields");
	aos_assert_r(fieldNodes, NULL);

	AosXmlTagPtr fieldNode = fieldNodes->getFirstChild("datafield");
	aos_assert_r(fieldNode, NULL);
	while (fieldNode)
	{
		OmnString name = fieldNode->getAttrStr("zky_name");
		if (name != field_name)
		{
			fieldNode = record_doc->getNextChild("datafield");
			continue;
		}
		tColumn->name = field_name;
		OmnString type = fieldNode->getAttrStr("type");
		//OmnString type = fieldNode->getAttrStr("zky_value_type");
		if(type == "virtual")
			type = fieldNode->getAttrStr("data_type");
		tColumn->type = AosDataType::toEnum(type);
		tColumn->sdatatype = fieldNode->getAttrStr("sdatatype");
		tColumn->size = fieldNode->getAttrInt("zky_length", 0);
		tColumn->offset = fieldNode->getAttrU32("offset", 0);
		tColumn->precision = fieldNode->getAttrInt("zky_precision", -1);
		tColumn->scale = fieldNode->getAttrInt("zky_scale", -1);
		tColumn->defaultVal = fieldNode->getAttrStr("default", "");

		fieldNode = fieldNodes->getNextChild("datafield");
	}
	return tColumn;
}


// jimodb-671
// jimodb-858
AosXmlTagPtr
AosJqlSelectTable::getDatasetField(
		        const AosRundataPtr &rdata,
				const AosDatasetObjPtr &dataset,
				AosJqlSelectFieldPtr &field)
{
	vector<AosDataRecordObjPtr> records;
	AosDataRecordObjPtr record;

	OmnString datafield = "<datafield/>";
	AosXmlTagPtr datafield_doc = AosXmlParser::parse(datafield AosMemoryCheckerArgs);

	dataset->getRecords(records);
	record = records[0];
	aos_assert_r(record, NULL);
	OmnString field_name = field->getFieldEscapeOName(rdata);
	OmnString errmsg = "";
	OmnString fieldName;
	fieldName << field_name << ";";
	
	AosDataFieldType::E type;
	OmnString fname = field->mExpr->dumpByNoEscape();
	OmnString name;
	AosExprObjPtr expr;

	int startIdx = fname.indexOf(0, '(', false);
	if(startIdx != -1)
	{
		int endIdx = fname.indexOf(0,')',false);
		name = OmnString(fname.data()+startIdx+1,endIdx-startIdx-1);
	}
	if(name == "*" || strncmp(fname.data(), "count", startIdx) == 0)
	{
		type = AosDataFieldType::eBinU64;
	}

	/*
	if (name.subString("count("))
	{
		type = AosDataFieldType::eBinU64;
	}
	*/
	else
	{
		expr = AosParseExpr(fieldName, errmsg, rdata.getPtr()); 
		type = convertToDataFieldType(rdata.getPtr(), expr, record);
	}

	if (type == AosDataFieldType::eInvalid)
	{
		OmnString msg = "[ERR]: field ";
		msg << field_name << " does not exist";
		rdata->setJqlMsg(msg);
		return NULL;
	}
	OmnString str_type =  AosDataFieldType::toString(type);

	field_name = field->getFieldEscapeCName(rdata);
	if (field_name == "") field_name =  field->getFieldEscapeOName(rdata);
	aos_assert_r(field_name != "",  NULL);

	datafield_doc->setAttr("zky_name", field_name);
	datafield_doc->setAttr("type", str_type);
	
	return datafield_doc;
}

