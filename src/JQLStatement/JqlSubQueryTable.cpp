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
#include "JQLStatement/JqlSubQueryTable.h"
#include "JQLStatement/JqlWhere.h"

#include "JQLExpr/ExprFieldName.h"
#include "JQLExpr/Expr.h"

#include "AosConf/DataConnector.h" 
#include "AosConf/DataRecordCtnr.h"
#include "AosConf/DataScanner.h"   
#include "AosConf/DataSet.h"  

#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
#include "Util/OmnNew.h"
#include "Debug/Debug.h"
#include "API/AosApi.h"

#include <boost/shared_ptr.hpp> 
#include <boost/make_shared.hpp>

using boost::shared_ptr; 
using namespace AosConf; 


AosJqlSubQueryTable::AosJqlSubQueryTable()
{
}

AosJqlSubQueryTable::~AosJqlSubQueryTable()
{
}


bool 
AosJqlSubQueryTable::init(
		const AosRundataPtr &rdata, 
		const AosJqlSelectPtr &select)
{
	if (mIsInited) return true;
	bool rslt = false;
	rslt = AosJqlSelectTable::init(rdata, select);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	rslt = mVirtualTable->init(rdata);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	mIsInited = true;
	return true;
}


OmnString
AosJqlSubQueryTable::generateTableConf(const AosRundataPtr &rdata)
{
	bool rslt = false;
	AosXmlTagPtr conf;
	mTableDoc = AosStr2Xml(rdata.getPtr(), "<table></table>" AosMemoryCheckerArgs);
	mTableDoc->setAttr("zky_type", "query_nest");
	mTableDoc->setAttr("zky_name", mVirtualName);
	mTableDoc->setAttr("jimo_name", "jimo_dataset"); 
	mTableDoc->setAttr("jimo_type", "jimo_dataset");
	mTableDoc->setAttr("current_version", "0");
	mTableDoc->setAttr("zky_otype", "zkyotp_jimo");
	mTableDoc->setAttr("zky_objid", "AosDatasetByQueryNest");
	mTableDoc->setAttr("zky_classname", "AosDatasetByQueryNest");
	                                                                              
	OmnString ver_str = "<versions><ver_0>libDatasetJimos.so</ver_0></versions>"; 
	AosXmlTagPtr ver = AosXmlParser::parse(ver_str AosMemoryCheckerArgs);         
	mTableDoc->addNode(ver);                                                          

	if (!mConds.empty())
	{                                              
		OmnString conds =  generateWhereConf(rdata);
		if (conds == "") return "";
		AosXmlTagPtr node = AosXmlParser::parse(conds AosMemoryCheckerArgs);
		if (!node)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return "";
		}
		mTableDoc->addNode(node);
	}

	if (!mGroupByFields.empty() || !mInputAggrFields.empty())
	{
		OmnString groupby_conf = generateGroupByConf(rdata);
		conf = AosXmlParser::parse(groupby_conf AosMemoryCheckerArgs);
		if (conf)
		{
			if (!mGroupByFields.empty())
				conf->setAttr("need_sort", "true");
			mTableDoc->addNode(conf);
		}
	}

	if (!mOrderByFields.empty())
	{
		OmnString order_conf = generateOrderByConf(rdata);
		conf = AosXmlParser::parse(order_conf AosMemoryCheckerArgs);
		if (!conf)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return "";
		}
		mTableDoc->addNode(conf);
	}

	rslt = generateInputDatasetConf(rdata);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return "";
	}

	OmnString conf_str = generateOutputDatasetConf(rdata);
	conf = AosXmlParser::parse(conf_str AosMemoryCheckerArgs);
	if (!conf)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return "";
	}
	mTableDoc->addNode(conf);
	return mTableDoc->toString();
}

/*
OmnString
AosJqlSubQueryTable::generateOutputDatasetConf(const AosRundataPtr &rdata)
{
	OmnString name, field_name, tagname;
	OmnString table_name = mTableName->getValue(rdata);
	if (table_name == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}
	name << "output_datarecord" << "_" << table_name;

	boost::shared_ptr<DataRecord> drec = boost::make_shared<DataRecord>();     
	drec->setAttribute("zky_name", name);
	drec->setAttribute("type", "xml");

	AosJqlSelectFieldPtr field;
	for (u32 i = 0; i < mOutputFields.size(); i++)
	{
		field = mOutputFields[i];
		field_name = field->getFieldEscapeCName(rdata);
		if (field_name == "") field_name =  field->getFieldEscapeOName(rdata);
		if (field_name == "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return  "";
		}

		boost::shared_ptr<DataFieldStr> data_field = boost::make_shared<DataFieldStr>();
		data_field->setAttribute("zky_name", field_name);
		tagname = field_name;                    
		tagname << "/_#text";                              
		data_field->setAttribute("zky_xpath", tagname);    
		data_field->setAttribute("type", "xpath");         
		drec->setField(data_field);
	}
	boost::shared_ptr<DataRecordCtnr> drecord = boost::make_shared<DataRecordCtnr>();                   
	drecord->setAttribute("type", "ctnr");
	drecord->setAttribute("zky_name", "doc");          
	drecord->setRecord(drec);
	drecord->setTagName("outputrecord");                                              
	return drecord->getConfig();
}
*/


OmnString
AosJqlSubQueryTable::generateWhereConf(const AosRundataPtr &rdata)
{
	OmnString table_name = mTableName->getValue(rdata.getPtrNoLock());
	if (table_name == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return "";
	}

	AosXmlTagPtr conds, where_filter;
	AosJqlQueryWhereConds query_where_conds;

	// jimodb-671
	AosDataRecordObjPtr record;
	query_where_conds.init(rdata, mConds, table_name, record);
	query_where_conds.getConfig(rdata, conds, where_filter);
	if (!conds)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return "";
	}
	if (where_filter) mTableDoc->addNode(where_filter);        
	return conds->toString();
}


bool
AosJqlSubQueryTable::generateInputDatasetConf(const AosRundataPtr &rdata)
{
	bool rslt = mVirtualTable->generateQueryConf(rdata, mTableDoc);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	return true;
}


bool
AosJqlSubQueryTable::getAllField(
		const AosRundataPtr &rdata,
		const AosJqlSelectPtr &select,
		vector<AosJqlSelectFieldPtr> *&fields)
{
	bool rslt = false;
	if (!mVirtualTable->mFieldListPtr)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	if (!!(mVirtualTable->mFieldListPtr->empty()))
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	OmnString v = (*(mVirtualTable->mFieldListPtr))[0]->getFieldName(rdata);
	if (v == "*")
	{
		if (!mVirtualTable->mTableReferencesPtr)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		for (u32 i = 0; i < mVirtualTable->mTableReferencesPtr->size(); i++)
		{
			AosJqlTableReferencePtr table_reference = (*(mVirtualTable->mTableReferencesPtr))[i];
			rslt = table_reference->getAllField(rdata, mVirtualTable, mVirtualTable->mFieldListPtr);
			if (!rslt)
			{
				AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
				OmnAlarm << __func__ << enderr;
				return false;
			} 
		}
	} 
	for (u32 i = 0; i < mVirtualTable->mFieldListPtr->size(); i++)
	{
		AosJqlSelectFieldPtr field = (*(mVirtualTable->mFieldListPtr))[i]->clone();
		if (field->mAlias) 
			field->mExpr = field->mAlias->cloneExpr();
		field->mAlias = NULL;
		fields->push_back(field);
	}
	return true;
}

//AosJqlSubQueryTablePtr 
//AosJqlSubQueryTable::clone(const AosRundataPtr &rdata)const 
//{
//	if (!0)
//	{
//		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
//		OmnAlarm << __func__ << enderr;
//		return 0;
//	}
//	return 0;
//}

void
AosJqlSubQueryTable::setVirtualTable(AosJqlSelect *virtual_table)
{
	mVirtualTable = virtual_table;
}

void 
AosJqlSubQueryTable::setName(char *name)
{
	mVirtualName = name;
	mTableName = OmnNew AosExprFieldName(name);
}

OmnString
AosJqlSubQueryTable::getName()
{
	return mVirtualName;
}


AosJqlSelectPtr 
AosJqlSubQueryTable::getVirtualTbale()
{
	return mVirtualTable;
}

