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
#include "JQLStatement/JqlSingleHbaseTable.h"
#include "JQLStatement/JqlStmtTable.h"
#include "JQLStatement/JqlSingleStatTable.h"
#include "JQLStatement/JqlStmtDataRecord.h"

#include "AosConf/DataConnector.h"         
#include "AosConf/DataRecordCtnr.h"           
#include "AosConf/DataScanner.h"           
#include "AosConf/DataSet.h"        
#include "AosConf/HbaseDataSet.h"        

#include "SEInterfaces/QueryType.h"
#include "SEInterfaces/AggregationType.h"

#include "JQLExpr/ExprMemberOpt.h"
#include "JQLExpr/ExprFieldName.h"
#include "JQLExpr/ExprString.h"
#include "JQLExpr/Expr.h"

#include "Rundata/Rundata.h"
#include "SEUtil/IILName.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"
#include "Util/OmnNew.h"
#include "Debug/Debug.h"
#include "API/AosApi.h"

using namespace AosConf;

OmnString
AosJqlSingleHbaseTable::generateInputDatasetConf(const AosRundataPtr &rdata)
{
	mRundata = rdata;
	boost::shared_ptr<DataRecord> drec = createRecordByTag(rdata, "column", "columns");
	boost::shared_ptr<DataRecord> raw_key = createRecordByTag(rdata, "field", "raw_keys");
	boost::shared_ptr<DataConnector> dcon = createConnector();
	boost::shared_ptr<DataConnector> dcon2 = createConnector();
	dcon2->setAttribute("jimo_objid", "dataconnector_hbase_jimodoc_v0");
	OmnString db_name, db_addr, db_port, t_name;
	bool rslt = getHbaseConnMsg(db_name, db_addr, db_port, t_name);
	aos_assert_r(rslt, "");

	dcon2->setAttribute(AOSTAG_HBASE_DBNAME, db_name);
	dcon2->setAttribute(AOSTAG_HBASE_ADDR, db_addr);
	dcon2->setAttribute(AOSTAG_HBASE_PORT, db_port);
	dcon2->setAttribute(AOSTAG_HBASE_TABLE_NAME, t_name);
	boost::shared_ptr<DataScanner> dscan = createScanner();
	boost::shared_ptr<DataScanner> dscan2 = createScanner();
	dscan->setConnector(dcon);
	dscan2->setConnector(dcon2);

	boost::shared_ptr<DataSchema> dsche = createSchema();
	boost::shared_ptr<DataSchema> h_dsche = createSchema();
	dsche->setRecord(raw_key);
	h_dsche->setAttribute("jimo_objid", "dataschema_univariable_jimodoc_v0");
	h_dsche->setRecord(drec);

	boost::shared_ptr<HbaseDataSet> hbase_dset = createHbaseDataSet();
	boost::shared_ptr<DataSet> dset = createDataSet();
	dset->setAttribute("external_field", mRawKey);                                             
	dset->setScanner(dscan);                                             
	dset->setSchema(dsche);                                              
	hbase_dset->setDataSet(dset);
	hbase_dset->setSchema(h_dsche);
	hbase_dset->setScanner(dscan2);                                             
	return hbase_dset->getConfig();
}

boost::shared_ptr<HbaseDataSet>
AosJqlSingleHbaseTable::createHbaseDataSet()
{
	OmnString name; 
	boost::shared_ptr<HbaseDataSet> dset = boost::make_shared<HbaseDataSet>(); 
	OmnString jimo_objid = "dataset_byexternaldoc_jimodoc_v0";
	name << "dataset_hbase_" <<  mTableNameStr;
	dset->setAttribute("jimo_objid", jimo_objid);
	dset->setAttribute("zky_name", name);
	dset->setTagName("inputdataset");
	return dset;
}


boost::shared_ptr<DataSet>
AosJqlSingleHbaseTable::createDataSet()
{
	OmnString name = "";
	boost::shared_ptr<DataSet> dset = boost::make_shared<DataSet>(); 
	OmnString jimo_objid = "dataset_bydatascanner_jimodoc_v0";
	name << "dataset_" <<  mTableNameStr;
	dset->setAttribute("jimo_objid", jimo_objid);
	dset->setAttribute("zky_name", name);
	dset->setTagName("inputdataset");
	return dset;
}

boost::shared_ptr<DataScanner>
AosJqlSingleHbaseTable::createScanner()
{	
	OmnString name; 
	name << "dataset_datacanner" <<  mTableNameStr;
	boost::shared_ptr<DataScanner> dscan = boost::make_shared<DataScanner>();        
	dscan->setAttribute("jimo_objid", "datascanner_cube_jimodoc_v0");  
	dscan->setAttribute("zky_name", name);  
	return dscan;
}

boost::shared_ptr<DataConnector>
AosJqlSingleHbaseTable::createConnector()
{
	boost::shared_ptr<DataConnector> dcon = boost::make_shared<DataConnector>();     
	dcon->setAttribute("jimo_objid", "dataconnector_readdoc_bysort_jimodoc_v0"); 
	dcon->setAttribute("doc_type", "csv_doc"); 
	return dcon;
}


boost::shared_ptr<DataSchema> 
AosJqlSingleHbaseTable::createSchema()
{
	boost::shared_ptr<DataSchema> dsche = boost::make_shared<DataSchema>(); 
	OmnString name = "";
	name << "schema_" <<  mTableNameStr;
	dsche->setAttribute("zky_name", name);  
	dsche->setAttribute("zky_dataschema_type", "static");  
	dsche->setAttribute("jimo_objid", "dataschema_record_bylenid_jimodoc_v0");  
	return dsche;
}

boost::shared_ptr<DataRecord>
AosJqlSingleHbaseTable::createRecordByTag(
		const AosRundataPtr &rdata,
		OmnString tag_name,
		OmnString per_tagname)
{
	OmnString name; 
	name << "dataset_record" <<  mTableNameStr;
	OmnString v1, v2, v3;
	boost::shared_ptr<DataRecord> drec = boost::make_shared<DataRecord>();     
	OmnString table_name = mTableNameStr;
	aos_assert_r(table_name != "", drec);

	AosXmlTagPtr table_doc = AosJqlStmtTable::getTable(table_name, 0);
	aos_assert_r(table_doc, drec);

	OmnString schema_name = table_doc->getAttrStr("zky_use_schema");
	AosXmlTagPtr doc = getDocByObjid(rdata, schema_name);
	if (doc)
	{
		AosXmlTagPtr record = doc->getFirstChild("datarecord");
		aos_assert_r(record, drec);

		v1 = record->getAttrStr("zky_field_delimiter", ",");
		v2 = record->getAttrStr("zky_row_delimiter", "CRLF");
		v3 = record->getAttrStr("zky_text_qualifier", "DQM");
	}
	drec->setAttribute(AOSTAG_NAME, name);
	drec->setAttribute("type", "csv");
	drec->setAttribute("zky_field_delimiter", v1);
	drec->setAttribute("zky_row_delimiter", v2);
	drec->setAttribute("zky_text_qualifier", v3);

	AosXmlTagPtr per_tag = table_doc->getFirstChild(per_tagname);
	aos_assert_r(per_tag, drec);
	AosXmlTagPtr tag = per_tag->getFirstChild(tag_name);
	OmnString field;
	while (tag)
	{
		field = tag->getAttrStr("name");
		boost::shared_ptr<DataFieldStr> data_field = boost::make_shared<DataFieldStr>();
		data_field->setAttribute("type", "str");
		data_field->setAttribute("zky_name", field);
		if (mRawKey == "" && tag_name == "field")
			mRawKey = field;
		drec->setField(data_field);
		tag = per_tag->getNextChild();
	}
	return drec;
}


bool 
AosJqlSingleHbaseTable::checkIsHbaseTable(
		const AosRundataPtr &rdata,
		AosExprObjPtr table,
		AosJqlSelectPtr select)
{
	AosValueRslt vv;
	bool rslt = table->getValue(0,0,vv);
	aos_assert_r(rslt, false);

	OmnString table_name = vv.getStr(); 
	AosXmlTagPtr doc = getDoc(rdata, JQLTypes::eTableDoc, table_name);
	if (!doc) return false;

	OmnString vvv = doc->getAttrStr(AOSTAG_DATASOURCE, "");
	if (vvv != "")
		return true;
	return false;
}


bool
AosJqlSingleHbaseTable::getHbaseConnMsg(
		OmnString &db_name,
		OmnString &db_addr,
		OmnString &db_port,
		OmnString &t_name)
{
	OmnString table_name = mTableNameStr;
	AosXmlTagPtr table_doc = AosJqlStmtTable::getTable(table_name, 0);
	aos_assert_r(table_doc, false);

	AosXmlTagPtr hbase_tag = table_doc->getFirstChild(AOSTAG_HBASE);
	aos_assert_r(hbase_tag, false);

	db_name = hbase_tag->getAttrStr(AOSTAG_HBASE_DBNAME, "");
	db_addr = hbase_tag->getAttrStr(AOSTAG_HBASE_ADDR, ""); 
	aos_assert_r(db_addr != "", false);

	db_port = hbase_tag->getAttrStr(AOSTAG_HBASE_PORT, ""); 
	aos_assert_r(db_port != "", false);

	t_name = hbase_tag->getAttrStr(AOSTAG_HBASE_TABLE_NAME, ""); 
	aos_assert_r(t_name != "", false);
	return true;
}
