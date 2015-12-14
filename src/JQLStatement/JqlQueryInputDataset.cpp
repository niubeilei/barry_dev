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
#include "JQLStatement/JqlQueryInputDataset.h"
#include "JQLStatement/JqlStmtDataRecord.h"
#include "JQLStatement/JqlStmtTable.h"
#include "JQLStatement/JqlStmtIndex.h"
#include "JQLStatement/JqlStmtSchema.h"
#include "JQLStatement/JqlSelectField.h"

#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SEUtil/JqlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/OmnNew.h"

using namespace AosConf;


AosJqlQueryInputDataset::AosJqlQueryInputDataset()
{
	mType = eInvalid;
}

AosJqlQueryInputDataset::AosJqlQueryInputDataset(
		const AosXmlTagPtr &table_doc,
		const OmnString table_name,
		const vector<AosJqlSelectFieldPtr> &inputFields)
{
	mType = eInvalid;
	mTableDoc = table_doc;
	mTableName = table_name;
	mInputFields = inputFields;
	mType = eInvalid;
	aos_assert((mTableDoc || mTableName != "") && !mInputFields.empty());
}

AosJqlQueryInputDataset::~AosJqlQueryInputDataset()
{
}

bool
AosJqlQueryInputDataset::init(const AosRundataPtr &rdata)
{
	mRundata = rdata;
	if (mTableName == "") 
	{
		if (!mTableDoc)
		{
			AosSetErrorUser(rdata, __func__)
				<< "This is BUG. mTableDoc is NUll." << enderr;
			OmnAlarm << enderr;
			return false;
		}
		mTableName = mTableDoc->getAttrStr("zky_tb_r_name");
	}

	getType(); 
	/*
	if (!rslt)
	{
		AosSetErrorUser(rdata, __func__)
			<< "This is BUG." << enderr;
		OmnAlarm << enderr;
		return false;
	}


	switch(mType)
	{
		case eXML:
			mDocType = "norm_doc";
			mRecordTyoe = "xml";
			break;
		case eFixBin:
			mDocType = "fixed_doc";
			mDsche = "dataschema_record_bylenid_jimodoc_v0";
			mRecordTyoe = "fixbin";
			break;
		case eCSV:
			mDocType = "csv_doc";
			mDsche = "dataschema_record_bylenid_jimodoc_v0";
			mRecordTyoe = "csv";
			break;
		case eVirtual:
			mDocType = "multi_fixed_doc";
			mDsche = "dataschema_record_bylenid_jimodoc_v0";
			mRecordTyoe = "fixbin";
			break;
		default:
			AosSetErrorUser(rdata, __func__)
				<< "Table Type Is Undefied." << enderr;
			OmnAlarm << enderr;
			return false;
	}
	*/
	return true;
}

AosXmlTagPtr 
AosJqlQueryInputDataset::getConfig(const AosRundataPtr &rdata)
{
	OmnString conf = generateInputDatasetConf(rdata);
	if (conf == "")
	{
		AosSetErrorUser(mRundata, __func__)
			<< "Failed to generate input dataset config." << enderr;
		OmnAlarm << enderr;
		return NULL;
	}
	return AosXmlParser::parse(conf AosMemoryCheckerArgs);
}

OmnString
AosJqlQueryInputDataset::generateInputDatasetConf(const AosRundataPtr &rdata)
{
	OmnString name = "";
	name << "dataset_" <<  mTableName;

	boost::shared_ptr<DataRecord> drec;
	
	if (mType == eXML) 
	{
		drec = generateInputRecord(rdata);
	}
	//aos_assert_r(drec, "");

	boost::shared_ptr<DataConnector> dcon = generateConnector(rdata);
	boost::shared_ptr<DataScanner> dscan = generateScanner(rdata);
	dscan->setConnector(dcon);

	boost::shared_ptr<DataSchema> dsche = generateSchema(rdata);
	boost::shared_ptr<DataSet> dset = boost::make_shared<DataSet>();                   
	dset->setAttribute("jimo_objid", "dataset_bydatascanner_jimodoc_v0");
	dset->setAttribute("zky_name", name);          
	dset->setScanner(dscan);                                             

	if (mType == eXML) 
	{
		dsche->setRecord(drec);
	}
	else
	{
		OmnString schemaObjid = mTableDoc->getAttrStr("zky_use_schema");
		AosXmlTagPtr schema_doc = AosJqlStatement::getDocByObjid(rdata, schemaObjid);
		aos_assert_r(schema_doc, "");
		u64 schema_docid = schema_doc->getAttrU64(AOSTAG_DOCID, 0);
		aos_assert_r(schema_docid, "");
		dsche->setAttribute(AOSTAG_DOCID, schema_docid);
	}
	dset->setSchema(dsche);
	dset->setTagName("inputdataset");
	return dset->getConfig();
}


boost::shared_ptr<DataScanner> 
AosJqlQueryInputDataset::generateScanner(const AosRundataPtr &rdata)
{
	OmnString name; 
	name << "dataset_datacanner" <<  mTableName;
	boost::shared_ptr<DataScanner> dscan = boost::make_shared<DataScanner>();
	dscan->setAttribute("jimo_objid", "datascanner_cube_jimodoc_v0");  
	dscan->setAttribute("zky_name", name);  
	return dscan;
}

boost::shared_ptr<DataConnector>
AosJqlQueryInputDataset::generateConnector(const AosRundataPtr &rdata)
{
	boost::shared_ptr<DataConnector> dcon = boost::make_shared<DataConnector>(); 
	dcon->setAttribute("jimo_objid","dataconnector_readdoc_bysort_jimodoc_v0");
	dcon->setAttribute("doc_type", mDocType);
	return dcon;
}

boost::shared_ptr<DataSchema> 
AosJqlQueryInputDataset::generateSchema(const AosRundataPtr &rdata)
{
//	if (mType == eVirtual) return generateMultiRecordSchema(rdata);

	boost::shared_ptr<DataSchema> dsche = boost::make_shared<DataSchema>(); 
	OmnString name = "";
	name << "schema_" <<  mTableName;
	dsche->setAttribute("zky_name", name);
	dsche->setAttribute("zky_dataschema_type", "static");
	dsche->setAttribute("jimo_objid", mDsche); 
	return dsche;

	/*
	AosXmlTagPtr schema_docs = mTableDoc->getFirstChild("schema_docs"); 
	aos_assert_r(schema_docs, dsche);
	AosXmlTagPtr schema_doc = schema_docs->getFirstChild(true);
	aos_assert_r(schema_doc, dsche);
	schema_doc->setAttr("jimo_objid", mDsche); 
	*/


	/*
	boost::shared_ptr<DataSchema> dsche = boost::make_shared<DataSchema>(); 
	OmnString name = "";
	name << "schema_" <<  mTableName;
	dsche->setAttribute("zky_name", name);
	dsche->setAttribute("zky_dataschema_type", "static");
	dsche->setAttribute("jimo_objid", mDsche); 
	return dsche;
	*/
}


boost::shared_ptr<DataSchema> 
AosJqlQueryInputDataset::generateMultiRecordSchema(const AosRundataPtr &rdata)
{
	boost::shared_ptr<DataSchemaMultiRecord> dsche;

	AosXmlTagPtr schema_docs = mTableDoc->getFirstChild("schema_docs"); 
	aos_assert_r(schema_docs, dsche);
	AosXmlTagPtr schema_doc = schema_docs->getFirstChild(true);
	aos_assert_r(schema_doc, dsche);
	schema_doc->setAttr("jimo_objid", mDsche); 

	dsche = boost::make_shared<DataSchemaMultiRecord>(schema_doc); 
	return dsche;
}


boost::shared_ptr<DataRecord>
AosJqlQueryInputDataset::generateInputRecord(const AosRundataPtr &rdata)
{
	boost::shared_ptr<DataRecord> drec;
	switch(mType)
	{
		case eXML:
			return generateInputRecordByXML(rdata);
		case eFixBin:
			return generateInputRecordByFixBin(rdata);
		case eCSV:
			return generateInputRecordByCSV(rdata);
		default:
			return drec;
	}
	return drec;
}


boost::shared_ptr<DataRecord>
AosJqlQueryInputDataset::generateInputRecordByFixBin(const AosRundataPtr &rdata)
{
	boost::shared_ptr<DataRecord> drec = boost::make_shared<DataRecord>();     
	OmnString name, tmpname;
	name << "datarecord" << "_" << mTableName;
	drec->setAttribute("zky_name", name);
	drec->setAttribute("type", mRecordTyoe);

	u32 field_size = 0;
	i64 size = 0, offset = 0;
	AosJqlColumnPtr column = 0; 
	AosJqlStatement::getTableColumns(mRundata, mTableName, field_size);
	drec->setAttribute("zky_length", field_size);
	AosJqlSelectFieldPtr field = NULL;
	OmnString field_name, field_type;
	vector<OmnString> field_names;
	vector<boost::shared_ptr<DataField> > func_fields;

	for (u32 i = 0; i < mInputFields.size(); i++)
	{
		field = mInputFields[i];
		field_name = field->getFieldEscapeOName(mRundata);
		tmpname = field_name;

		if (!(field->checkIsAgrFunc()) && !(field->isName()))
			//field->isMultiFieldExpr())
		{
			boost::shared_ptr<DataFieldExpr> data_field = boost::make_shared<DataFieldExpr>();  
			data_field->setExpr(field_name);
			AosJqlUtil::escape(field_name);
			data_field->setAttribute("zky_name", field_name);
			data_field->setAttribute("type", "expr"); 
			func_fields.push_back(data_field);
			continue;
		}

		column = AosJqlStatement::getTableField(mRundata, mTableName, field_name);
		if (!column && tmpname.toLower() != "zky_docid")
		{
			AosSetErrorUser(mRundata, __func__)
				<< "Field is undefined : " << field_name << "." << enderr; 
			OmnAlarm << enderr;
			return drec;
		}

		if (column)
		{
			field_type = AosDataType::getTypeStr(column->type);
			field_type = AosJqlStatement::AosJType2DType(field_type);
		}
		else
		{
			field_name = tmpname;
			field_type = "metadata";
			boost::shared_ptr<DataFieldStr> data_field = boost::make_shared<DataFieldStr>();
			AosJqlUtil::escape(field_name);
			data_field->setAttribute("zky_name", field_name);
			data_field->setAttribute("type", field_type);
			data_field->setAttribute("zky_metadata_type", field_name);
			drec->setField(data_field);
			continue;
		}

		aos_assert_r(field_type != "", drec);

		size = column->size;
		boost::shared_ptr<DataFieldStr> data_field = boost::make_shared<DataFieldStr>();
		AosJqlUtil::escape(field_name);
		data_field->setAttribute("zky_name", field_name);
		data_field->setAttribute("zky_length", size);
		data_field->setAttribute("zky_offset", column->offset);
		data_field->setAttribute("zky_precision", column->precision);
		data_field->setAttribute("zky_scale", column->scale);
		data_field->setAttribute("type", field_type);
		data_field->setAttribute(AOSTAG_VALUE_DFT_VALUE, column->defaultVal);
		drec->setField(data_field);
		offset += size;
	}
	drec->setFields(func_fields);
	return drec;
}

boost::shared_ptr<DataRecord>
AosJqlQueryInputDataset::generateInputRecordByCSV(const AosRundataPtr &rdata)
{
	boost::shared_ptr<DataRecord> drec = boost::make_shared<DataRecord>();     
	OmnString name, tmpname, defaultVal;
	name << "datarecord" << "_" << mTableName;
	drec->setAttribute("zky_name", name);
	drec->setAttribute("type", mRecordTyoe);

	OmnString field_name, func_str, field_type, precision, scale;
	AosJqlSelectFieldPtr field; 
	map<OmnString, OmnString> field_map;
	vector<boost::shared_ptr<DataField> > func_fields;
	for (u32 i = 0; i < mInputFields.size(); i++)
	{
		field = mInputFields[i];
		field_name = field->getFieldEscapeOName(mRundata);
		/*
		if (field->checkIsAgrFunc())
		{
			func_str = field->dumpByNoEscape();
			field_map[field_name] = func_str;
		}
		*/
		if (field->isName() || field->checkIsAgrFunc())
		{
			AosJqlUtil::escape(field_name);
			field_map[field_name] = field_name;
			tmpname = field_name;
			if (tmpname.toLower() == "zky_docid")
			{
				boost::shared_ptr<DataFieldStr> data_field = boost::make_shared<DataFieldStr>();
				data_field->setAttribute("type", "metadata");
				data_field->setAttribute("zky_name", tmpname);
				data_field->setAttribute("zky_metadata_type", tmpname);
				drec->setField(data_field);
			}
		}
		else //if (field->isMultiFieldExpr())
		{
			boost::shared_ptr<DataFieldExpr> data_field = boost::make_shared<DataFieldExpr>();  
			field_name = field->dumpByNoEscape();
			data_field->setExpr(field_name);
			AosJqlUtil::escape(field_name);
			data_field->setAttribute("zky_name", field_name);
			data_field->setAttribute("type", "expr"); 
			func_fields.push_back(data_field);
		}
	}

	AosXmlTagPtr columns = mTableDoc->getFirstChild("columns");
	AosXmlTagPtr column = columns->getFirstChild("column");
	while (column)
	{
		field_name = column->getAttrStr("name");
		field_type = column->getAttrStr("datatype");
		defaultVal = column->getAttrStr("default");
		precision = column->getAttrStr("zky_precision");
		scale = column->getAttrStr("zky_scale");
		aos_assert_r(field_type != "", drec);

		field_type = AosJqlStatement::AosJType2DType(field_type);

		if (field_type == "expr")
		{
			boost::shared_ptr<DataFieldExpr> data_field = boost::make_shared<DataFieldExpr>(column);  
			func_fields.push_back(data_field);
			column = columns->getNextChild();
			continue;
		}
		if (field_map.find(field_name) != field_map.end())
			field_name = field_map[field_name];

		AosJqlUtil::escape(field_name);
		boost::shared_ptr<DataFieldStr> data_field = boost::make_shared<DataFieldStr>();
		data_field->setAttribute("type", field_type);
		data_field->setAttribute("zky_name", field_name);
		data_field->setAttribute("zky_precision", precision);
		data_field->setAttribute("zky_scale", scale);
		data_field->setAttribute(AOSTAG_VALUE_DFT_VALUE, defaultVal);
		drec->setField(data_field);
		column = columns->getNextChild();
	}
	drec->setFields(func_fields);

	OmnString schema_name = mTableDoc->getAttrStr("zky_use_schema");
	AosXmlTagPtr doc = AosJqlStatement::getDocByObjid(rdata, schema_name);
	OmnString v1, v2, v3;
	if (doc)
	{
		AosXmlTagPtr child = doc->getFirstChild("datarecord");
		OmnString record_name = child->getAttrStr("zky_name");
		record_name = AosJqlStatement::getObjid(
				rdata, JQLTypes::eDataRecordDoc, record_name);
		AosXmlTagPtr record = AosJqlStatement::getDocByObjid(rdata, record_name);
		if (record)
		{
			v1 = record->getAttrStr("zky_field_delimiter", ",");
			v2 = record->getAttrStr("zky_row_delimiter", "CRLF");
			v3 = record->getAttrStr("zky_text_qualifier", "DQM");
		}
	}

	drec->setAttribute("type", "csv");
	drec->setAttribute("zky_field_delimiter", v1);                                       
	drec->setAttribute("zky_row_delimiter", v2);
	drec->setAttribute("zky_text_qualifier", v3);
	return drec;
}

boost::shared_ptr<DataRecord>
AosJqlQueryInputDataset::generateInputRecordByXML(const AosRundataPtr &rdata)
{
	boost::shared_ptr<DataRecord> drec = boost::make_shared<DataRecord>();     
	OmnString name;
	name << "datarecord" << "_" << mTableName;
	drec->setAttribute("zky_name", name);
	//drec->setAttribute("type", mRecordTyoe);
	drec->setAttribute("type", "xml");

	OmnString field_name;
	AosJqlColumnPtr column = 0; 
	AosJqlSelectFieldPtr field;
	for (u32 i = 0; i < mInputFields.size(); i++)
	{
		field = mInputFields[i];
		field_name = field->getFieldEscapeOName(mRundata);

		if (!(field->checkIsAgrFunc()) && !(field->isName()))
		//if (field->isMultiFieldExpr())
		{
			field_name = field->dumpByNoEscape();
			boost::shared_ptr<DataFieldExpr> data_field = boost::make_shared<DataFieldExpr>();  
			data_field->setExpr(field->dump1());    // temp.       
			AosJqlUtil::escape(field_name);
			data_field->setAttribute("zky_name", field_name);
			data_field->setAttribute("type", "expr");    // temp.
			drec->setField(data_field);
			continue;
		}

		AosJqlUtil::escape(field_name);
		boost::shared_ptr<DataFieldStr> data_field = boost::make_shared<DataFieldStr>();
		data_field->setAttribute("zky_name", field_name);
		OmnString tagname = field->getXpathName();                    
		aos_assert_r(tagname != "", drec);
		data_field->setAttribute("zky_xpath", tagname);
		data_field->setAttribute("type", "xpath");

		data_field->setAttribute("zky_value_type", "xpath");

		// Add by Young, 2015/02/11
		OmnString field_value_type, precision, scale;
		field_value_type = precision = scale = "";
		column = AosJqlStatement::getTableField(rdata, mTableName, field_name);
		if (column)
		{
			field_value_type = AosDataType::getTypeStr(column->type);
			field_value_type = AosJqlStatement::AosJType2DType(field_value_type);
			precision << column->precision;
			scale << column->scale;
		}
		data_field->setAttribute(AOSTAG_VALUE_TYPE, field_value_type);         
		data_field->setAttribute("zky_precision", precision);
		data_field->setAttribute("zky_scale", scale);
		// young, end

		drec->setField(data_field);
	}
	return drec;
}

bool
AosJqlQueryInputDataset::getType()
{
	mDsche = "dataschema_record_bylenid_jimodoc_v0";
	if (!mTableDoc) {
		mType = eXML;
		mDocType = "norm_doc";
		mRecordTyoe = "xml";
		return true;
	}

	OmnString data_type = mTableDoc->getAttrStr("zky_data_format");
	if (data_type == "xml")
	{
		mType = eXML;
		mDocType = "norm_doc";
		mRecordTyoe = "xml";
	}
	else
	{
		mDocType = "common_doc";
	}
	return true;
}


