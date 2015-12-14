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
#include "JQLStatement/JqlStmtTable.h"

#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApi.h"

#include "SEUtil/JqlTypes.h"
#include "JQLExpr/Expr.h"
#include "JQLExpr/ExprFieldName.h"
#include "AosConf/DataSchema.h"
#include "AosConf/DataRecord.h"
#include "AosConf/DataField.h"

#include "boost/make_shared.hpp"
#include "boost/shared_ptr.hpp"
using AosConf::DataSchema;
using AosConf::DataRecord;
using AosConf::DataField;

AosJqlStmtTable::AosJqlStmtTable(const OmnString &errmsg)
:
mErrmsg(errmsg)
{
}

AosJqlStmtTable::AosJqlStmtTable()
{
	mDoc = 0;
	mName = "";
	mFirstFieldName = "";
	mLikeTable = "";
	mLike = "";
	mCurDatabase = "";
	mChecksum = 0;
	mAutoIncrement = 0;
	mColumns = 0;
	mOffset = 0;
	mIsVirtual = false;
	mRawKeys = NULL;
	mIsExists = false;
}

AosJqlStmtTable::~AosJqlStmtTable()
{
}


//
//member functions
//run: run the statement. Will be called by CLI/Webui binary
//
bool
AosJqlStmtTable::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	if (mErrmsg != "")
	{
		AosSetErrorUser(rdata, __func__) << mErrmsg << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	bool rslt = setDatabase(rdata);
	if (!rslt) return false;
	OmnString name = "";

	if (mOp != JQLTypes::eShow)
	{
		if (mName == "") return false;
	}

	if (mName.find('.', false) == -1)
	{
		//need to prepend database name
		if (mCurDatabase == "")
		{
			//can do nothing if database unknown
			rdata->setJqlMsg("ERROR: No database selected");
			return false;
		}
		
		name << mCurDatabase << "." << mName;
		mName = name; 
	}

	if (mOp == JQLTypes::eCreate) return createTables(rdata);       
	if (mOp == JQLTypes::eShow) return showTables(rdata);       
	if (mOp == JQLTypes::eDrop) return dropTable(rdata);       
	if (mOp == JQLTypes::eDescribe) return describeTable(rdata);

	AosSetEntityError(rdata, "JQL_table_run_err", "JQL Table", "")      
		<< "opr is undefined!" << enderr;                               
	return false;                                                           
}


bool
AosJqlStmtTable::createTables(const AosRundataPtr &rdata)
{
	OmnString msg;
	AosXmlTagPtr doc = getDoc(rdata, JQLTypes::eTableDoc, mTableName);
	if (doc)
	{
		OmnString error;
		error << "Table '" << mTableName << "' already exists";
		rdata->setJqlMsg(error);
		return true;
	}

	doc = convertToXml(rdata);
	msg = rdata->getJqlMsg();
	//  arvin 2015.08.01
	//  JIMODB-194,doc must be exist,but may be have an errmsg
	//	if (!doc)
	if (msg !="")
	{
		if (mIsExists) 
		{
			return true;
		}

		rdata->setJqlMsg(msg);
		return false;
	}

	if (mLikeTable != "")
	{
		if (mLikeTable.find('.', false) == -1)
		{
			OmnString name = "";

			//need to prepend database name
			if (mCurDatabase == 0)
			{
				//can do nothing if database unknown
				rdata->setJqlMsg("ERROR: No database selected for table to be cloned");
				return false;
			}

			name << mCurDatabase << "." << mLikeTable;
			mLikeTable = name; 
		}

		//we need to create a new table based on and old one
		return cloneTable(rdata); 
	}

	if ( !createDoc(rdata, JQLTypes::eTableDoc, mName, doc) )
	{
		msg << " [ERR] : Table '" << mName << "'created failed.";
		rdata->setJqlMsg(msg);
		return false;
	}

	msg << "Table '" << mTableName << "'created successfully.";
	//if (!mIsVirtual) createDefaultIndex(rdata);

	rdata->setJqlMsg(msg);
	rdata->setJqlUpdateCount(1);
	return true;
}


bool
AosJqlStmtTable::cloneTable(const AosRundataPtr &rdata)
{
	AosXmlTagPtr oldTable;

	//get the old table. If not exist return false
	oldTable = getTableDoc(rdata, mLikeTable);	
	if (!oldTable)
	{
		OmnString error;
		error << "WARNING: The base table doesn't exist: " << mLikeTable;
		rdata->setJqlMsg(error);
		return false;
	}
	
	//remove some meta data
	oldTable->removeAttr(AOSTAG_DOCID, 1, true);
	oldTable->removeAttr(AOSTAG_OBJID, 1, true);

	//clone doc from the old table
	if (!oldTable->replaceAttrValue(AOSTAG_TABLENAME, false, mLikeTable, mName))
	{
		OmnString error;
		error << "Failed to replace old table name in table doc.";
		rdata->setJqlMsg(error);
		return false;
	}
	
	//create the new table
	if ( !createDoc(rdata, JQLTypes::eTableDoc, mName, oldTable) )
	{
		OmnString msg = "Create table failed";
		rdata->setJqlMsg(msg);
		return false;
	}

	return true;
}


bool
AosJqlStmtTable::showTables(const AosRundataPtr &rdata)
{
	vector<AosXmlTagPtr> tables;
	if ( !getDocs(rdata, JQLTypes::eTableDoc, tables) )
	{
		OmnString error;
		error << "No table found";
		rdata->setJqlMsg(error);
		return true;
	}

	//int total = tables.size() ;
	//OmnString content;
	//content << "<content total=\"" << total << "\">"; 
	OmnString content = "<content>";
	int totalNum = 0;
	if(mLike != "")
	{
		mLike.remove(1, mLike.length()-1); 
		for (u32 i=0; i<tables.size(); i++)
		{
			string value = tables[i]->getAttrStr("zky_tb_r_name", "");
			size_t found = value.find(mLike);
			if (found != string::npos)
			{
				OmnString database = tables[i]->getAttrStr("zky_database", "");
				if (database == mCurDatabase)
				{
					totalNum++;
					content << "<record tablename=\"" << value << "\"/>";
				}
			}
		}
	}
	else
	{
		for(u32 i=0; i<tables.size(); i++)
		{
			string value = tables[i]->getAttrStr("zky_tb_r_name", "");
			OmnString database = tables[i]->getAttrStr("zky_database", "");
			if (database == mCurDatabase)
			{
				totalNum++;
				content << "<record tablename=\"" << value << "\"/>";
			}
		}
	}
	content << "</content>";
	vector<OmnString> fields;
	fields.push_back("tablename");

	map<OmnString, OmnString> alias_name;
	alias_name["tablename"] = "TableName";

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(content.data(), content.length(), "" AosMemoryCheckerArgs);
	rdata->setJqlUpdateCount(totalNum);
	OmnString msg = printTableInXml(fields, alias_name, doc);
	if (mContentFormat == "xml")
	{
		rdata->setResults(doc->toString());
		return true;
	}
	setPrintData(rdata, msg);
	
	return true;
}


bool
AosJqlStmtTable::dropTable(const AosRundataPtr &rdata)
{
	OmnString msg;
	OmnString name = getObjid(rdata, JQLTypes::eTableDoc, mName);
	bool rslt = deleteDocByObjid(rdata, name);
	if(!rslt)
	{
		rdata->setJqlMsg("Drop table failed!");
		return false;
	}
	rdata->setJqlMsg("Drop table scessful!");
	return true;
}


bool 
AosJqlStmtTable::runDropTable(
			const AosRundataPtr &rdata,
			const OmnString &tbl_name,
			const AosXmlTagPtr &defs)
{
	if (!rdata && defs)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	if ( !runSmartDoc(rdata, defs) )
	{                                                  
		OmnString error;
		error << "Failed to drop table doc"; 
		rdata->setJqlMsg(error);
		return false;                                  
	}                                                  
	
	bool rslt = AosJqlStatement::dropTable(rdata, tbl_name);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	return true;
}


bool
AosJqlStmtTable::describeTable(const AosRundataPtr &rdata)
{
	OmnString msg;
	OmnString r_name = getObjid(rdata, JQLTypes::eTableDoc, mName);
	AosXmlTagPtr doc = getTableDoc(rdata, r_name); 
	if (!doc)
	{
		msg << "ERROR: Table '" << mName << "' doesn't exist";
		rdata->setJqlMsg(msg);
		return true;
	}

	OmnString schemaObjid = doc->getAttrStr("zky_use_schema");
	AosXmlTagPtr schemaDoc = getDocByObjid(rdata, schemaObjid);
	aos_assert_r(schemaDoc, false);
	AosXmlTagPtr recordNode = schemaDoc->getFirstChild("datarecord");
	aos_assert_r(recordNode, false);
	AosXmlTagPtr fieldsNode = recordNode->getFirstChild("datafields");
	aos_assert_r(fieldsNode, false);

	OmnString content = "<content>";
	content << fieldsNode->toString() << "</content>";
	rdata->setResults(content);

	vector<OmnString> fields;
	fields.push_back("zky_name");
	fields.push_back("type");
	fields.push_back("zky_length");
	fields.push_back("nullable");
	fields.push_back("default");

	map<OmnString, OmnString> alias_name;
	alias_name["zky_name"] = "Name";
	alias_name["type"] = "Type";
	alias_name["zky_length"] = "Size";
	alias_name["nullable"] = "Nullable";
	alias_name["default"] = "DefaultValue";

	int totalNum = fieldsNode->getNumSubtags();
	rdata->setJqlUpdateCount(totalNum);
	msg = printTableInXml(fields, alias_name, fieldsNode);
	if (mContentFormat == "xml")
	{
		fieldsNode->setNodeName("content");
		rdata->setResults(fieldsNode->toString());
		return true;
	}
	setPrintData(rdata, msg);

	return true;
}


AosJqlStatement *
AosJqlStmtTable::clone()
{
	return OmnNew AosJqlStmtTable(*this);
}


void 
AosJqlStmtTable::dump()
{
}


/***************************
 * Getter/setter
 * **************************/
void 
AosJqlStmtTable::setName(OmnString name)
{
	mName = name;
	mTableName = name;
}

void 
AosJqlStmtTable::setLikeTable(OmnString likeTable)
{
	mLikeTable = likeTable;
}

void 
AosJqlStmtTable::setOption(AosJqlTableOption *opt)
{
	if (opt) 
	{
		mComment = opt->comment;
		mAutoIncrement = opt->autoInc;
		mChecksum = opt->checksum;
		mInputDataFormat = opt->inputDataFormat;
		mDataSource = opt->data_source;
		mDbName = opt->db_name;
		mDbAddr = opt->db_addr;
		mDbPort = "";
		mDbPort << opt->db_port;
		mDbTableName = opt->db_t_name;
		mRawKeys = opt->raw_keys;
		if (mInputDataFormat == "") mInputDataFormat = "xml";
	}
}

void 
AosJqlStmtTable::setColumns(vector<AosJqlColumnPtr> *columns)
{
	mColumns = columns;
}


void 
AosJqlStmtTable::setFullOption(u32 opt)
{
	mFull = opt;
}

void 
AosJqlStmtTable::setFromOption(OmnString opt)
{
	mFrom = opt;
}


void 
AosJqlStmtTable::setLikeOption(OmnString opt)
{
	mLike = opt;
}


bool
AosJqlStmtTable::setDatabase(const AosRundataPtr &rdata)
{
	mCurDatabase = getCurDatabase(rdata);
	if (mCurDatabase == "") 
	{
		rdata->setJqlMsg("ERROR: No database selected");
		return false;
	}

	return true;
}

void 
AosJqlStmtTable::setTbNameOption(const AosExprList* tblname_list)
{
	u32 times = tblname_list->size();
	for (u32 i=0; i<times; i++)
	{
		AosValueRslt value_rslt;                           
		bool rslt = (*tblname_list)[i]->getValue(0, 0, value_rslt);
		aos_assert(rslt);
		OmnString str = value_rslt.getStr();
		aos_assert(str != "");
		mTableNames.push_back(str);
	}
	delete tblname_list;
}


AosXmlTagPtr 
AosJqlStmtTable::getTable(
		const OmnString &table, 
		const AosRundataPtr &rdata)
{
	OmnString name = "";
	OmnString database = "";

	if (table.find('.', false) == -1)
	{
		database = getCurDatabase(rdata);
		if (database == "")
		{
			rdata->setJqlMsg("ERROR: No database selected");
			return 0;
		}
	}

	name = getTableName(table, database);
	name = getObjid(rdata, JQLTypes::eTableDoc, name);
	return getDocByObjid(rdata, name);
}

OmnString
AosJqlStmtTable::getTableName(
		const OmnString &name,
		const OmnString &database_name)
{
	if (name.find('.', false) != -1)
		return name;

	if (name == "" || database_name == "")
	{
		return "";
	}

	OmnString new_name = "";
	OmnString tmp = database_name;
//	tmp.toLower();
	if (tmp != "jimodb")
	{
		new_name << database_name << "." << name;
		return new_name;
	}
	return name;
}

OmnString
AosJqlStmtTable::getDatabaseFromFullName(
		const OmnString name)
{
	int pos = name.find('.', false);

	if (pos == -1 || pos == 0 || pos == (name.length() - 1))
		return NULL;

	return name.substr(0, pos - 1);
}

OmnString
AosJqlStmtTable::getTableFromFullName(
		const OmnString name)
{
	int pos = name.find('.', false);

	if (pos == -1 || pos == 0 || pos == (name.length() - 1))
		return NULL;

	return name.substr(pos + 1, 0);
}

AosXmlTagPtr 
AosJqlStmtTable::convertToXml(const AosRundataPtr &rdata)
{
	OmnString doc;
	OmnString map_str = "";
	doc << "<sqltable><columns>";

	//Gavin 2015/08/03 JIMODB-227
//	if (!isConfExist(rdata, JQLTypes::eSchemaDoc, mSchemaName))
//	{
//		OmnString errmsg;                                         
//		errmsg << " [ERR] : can not find the schema '" << mSchemaName << "'!" ;    
//		rdata->setJqlMsg(errmsg);                                 
//		return NULL;                                              
//	}
	if (mSchemaName != "" && mIsVirtual)
	{
		doc << generateVirtualColumnConf(); 
	}
	else if (mSchemaName != "") 
	{
		createColumnsMap();
		OmnString conf = generateTableConfBySchema(rdata, map_str);
		doc << conf;
	}
	else
	{ 
		if (!generateDftSchema(rdata)) return NULL;
	}

	doc << "</columns>";

	if (mRawKeys) doc << generateRawKeyConf(rdata);

	doc << "<indexes></indexes>";
	doc << "<cmp_indexes></cmp_indexes>";
	doc << "<maps>" << map_str << "</maps>";
	//AosXmlTagPtr sdoc = getDoc(rdata, JQLTypes::eSchemaDoc, mSchemaName); 
	//if (sdoc) doc << "<schema_docs>" << sdoc->toString() << "</schema_docs>";
	doc << "</sqltable>";

	AosXmlTagPtr table = AosXmlParser::parse(doc AosMemoryCheckerArgs); 
	aos_assert_r(table, NULL);

	if (mSchemaName == "") 
	{
		mSchemaName = getDefaultSchemaName();
		table->setAttr("zky_data_format", "xml");
	}
	string schemaObjid = getObjid(rdata,JQLTypes::eSchemaDoc, mSchemaName);
	table->setAttr("zky_use_schema", schemaObjid);
	table->setAttr("zky_record_len", mRecordLen);
	table->setAttr("zky_public_doc", "true");
	table->setAttr("zky_public_ctnr", "true");
	table->setAttr("zky_def_index", "true");
	table->setAttr("zky_database", getDatabaseFromFullName(mName));
	table->setAttr("zky_otype", AOSTAG_TABLE);
	table->setAttr(AOSTAG_DATASOURCE, mDataSource);
	table->setAttr("zky_stype", AOSTAG_TABLE);
	table->setAttr("zky_tb_r_name", getTableFromFullName(mName));
	table->setAttr("zky_pctrs", AOSCTNR_TABLE_CONTAINER);
	table->setAttr(AOSTAG_TABLENAME, getTableFromFullName(mName));
	table->setAttr(AOSTAG_PUB_CONTAINER, AOSCTNR_TABLE_CONTAINER);
	table->setAttr("zky_data_format", mInputDataFormat);
	table->setAttr("zky_virtual", mIsVirtual);

	return table;
}


bool
AosJqlStmtTable::generateDftSchema(const AosRundataPtr &rdata)
{
	OmnString conf;
	AosJqlColumnPtr column = NULL;
	if (!mColumns)
	{
		OmnAlarm << __func__ << enderr;
		return false;
	}
	mSchemaName = getDefaultSchemaName();
	boost::shared_ptr<DataSchema> ds = boost::make_shared<DataSchema>();
	boost::shared_ptr<DataRecord> dr = boost::make_shared<DataRecord>();
	for (u32 i = 0; i < mColumns->size(); i++ )
	{
		OmnString nullable = "";
		column = (*mColumns)[i];
		if (column->nullable == false) nullable = "not null";
		if (mFirstFieldName == "") mFirstFieldName = column->name;
		boost::shared_ptr<DataField> df = boost::make_shared<DataField>();
		df->setAttribute("zky_name", column->name);
		df->setAttribute("type", AosDataType::getTypeStr(column->type));
		df->setAttribute("zky_length", column->size);
		//df->setAttribute("zky_offset", mOffset);
		df->setAttribute("zky_nullable", nullable);
		df->setAttribute("zky_default_value", column->defaultVal);
		df->setAttribute("zky_name", column->name);
		//mOffset += column->size;
		dr->setField(df);
	}
	OmnString dft_record_name = "";
	dft_record_name << mSchemaName << "_dft_record";
	dr->setAttribute("zky_name", dft_record_name);
	AosXmlTagPtr recordDoc = AosXmlParser::parse(dr->getConfig() AosMemoryCheckerArgs); 
	aos_assert_r(recordDoc, false);
	bool rslt = createDoc(rdata, JQLTypes::eDataRecordDoc, dft_record_name, recordDoc);
	aos_assert_r(rslt, false);
	recordDoc = getDoc(rdata, JQLTypes::eDataRecordDoc, dft_record_name);
	aos_assert_r(recordDoc, false);
	u64 record_docid = recordDoc->getAttrU64(AOSTAG_DOCID, 0);
	dr->setAttribute(AOSTAG_DOCID, record_docid);

	ds->setRecord(dr);
	ds->setAttribute("zky_name", mSchemaName);

	AosXmlTagPtr schemaDoc = AosXmlParser::parse(ds->getConfig() AosMemoryCheckerArgs); 
	aos_assert_r(schemaDoc, false);
	return createDoc(rdata, JQLTypes::eSchemaDoc, mSchemaName, schemaDoc);
}


OmnString 
AosJqlStmtTable::getDefaultSchemaName()
{
	OmnString dft_schema_name = "";
	dft_schema_name << mName << "_dft_schema";
	return dft_schema_name;
}


OmnString
AosJqlStmtTable::generateVirtualColumnConf()
{
	OmnString conf;
	if (!mVirtualColumns)
	{
		//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}
	for (u32 i = 0; i < mVirtualColumns->size(); i++ )
	{
		conf << (*mVirtualColumns)[i]->toString();
	}
	return conf;
}


OmnString 
AosJqlStmtTable::generateRawKeyConf(const AosRundataPtr &rdata)
{
	OmnString conf;
	OmnString vv;
	AosExprObjPtr expr;
	conf << "<" << AOSTAG_HBASE <<  " " 
		 << AOSTAG_HBASE_DBNAME << "=\"" << mDbName << "\" "
		 << AOSTAG_HBASE_ADDR << "=\"" << mDbAddr << "\" "
		 << AOSTAG_HBASE_PORT << "=\"" << mDbPort << "\" "
		 << AOSTAG_HBASE_TABLE_NAME << "=\"" << mDbTableName << "\" >"
		 << "></" << AOSTAG_HBASE << ">"; 
	conf << "<raw_keys>";
	for (u32 i = 0; i < mRawKeys->size(); i++)
	{
		expr = (*mRawKeys)[i];
		if (!expr)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return NULL;
		}
		vv = expr->getValue(rdata.getPtrNoLock());
		if (vv == "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return NULL;
		}
		conf << "<field name=\""<< vv << "\"></field>";
	}
	conf << "</raw_keys>";
	return conf;
}

OmnString 
AosJqlStmtTable::generateTableConfBySchema(
		const AosRundataPtr &rdata,
		OmnString &map_str)
{
	OmnString doc;
	int offset = 0;
	AosJqlColumnPtr column = NULL;
	OmnString field_name = "";
	OmnString format = "";
	AosXmlTagPtr conf = getDoc(rdata, JQLTypes::eSchemaDoc, mSchemaName); 
	if (!conf)
	{
		//arvin 2015.08.01
		//JIMODB-194,check parser exist
		OmnAlarm << __func__ << enderr;
		OmnString msg = "[ERR] : Parser \"";
		msg << mSchemaName << "\" doesn't exist!";
		rdata->setJqlMsg(msg);
		return doc;
	}
	OmnString conf_str;
	if (mIsVirtual)
		conf_str = generateMultiRecordSchema(mRundata, conf);

	//AosXmlTagPtr child = conf->getFirstChild("datarecord"); 
	//OmnString record_name = child->getAttrStr("zky_name");
	//AosXmlTagPtr record = getDoc(JQLTypes::eDataRecordDoc, record_name);
	AosXmlTagPtr record = conf->getFirstChild("datarecord"); 
	if (!record)
	{
		//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return doc;
	}

	mInputDataFormat = record->getAttrStr("type");
	mRecordLen = record->getAttrU32("zky_length", 0);
	AosXmlTagPtr data_fields = record->getFirstChild(true);
	if (!data_fields)
	{
		//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}
	AosXmlTagPtr data_field = data_fields->getFirstChild(true);
	OmnString type;
	while (data_field)
	{
		OmnString data_field_name = data_field->getAttrStr("zky_name");
		bool is_virtual_field = data_field->getAttrBool("virtual_field", false);

		if ( !is_virtual_field ) 
		{
			data_field = data_fields->getNextChild();
			continue; // Add by Young, 2015/07/10
		}

		if (is_virtual_field)
		{
			OmnString map_obj = data_field->getAttrStr("zky_map_name");
			if (map_obj == "")
			{
				//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
				OmnAlarm << __func__ << enderr;
				return 0;
			}
			AosXmlTagPtr map_conf = getDocByObjid(rdata, map_obj);
			if (!map_conf)
			{
				//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
				OmnAlarm << __func__ << enderr;
				return 0;
			}
			map_conf->setAttr("datatype", data_field->getAttrStr("type"));
			map_str	<< map_conf->toString();
			continue;
		}
		if (!data_field)
		{
			//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}

		type = data_field->getAttrStr("type");
		if (type == "iilmap") {
			data_field = data_fields->getNextChild();
			continue;
		}
		
		type = AosJqlStatement::AosDType2JType(type);

		if (mFirstFieldName == "") mFirstFieldName = data_field->getAttrStr("zky_name");

		if (data_field->getAttrStr("zky_offset") != "") {
			offset = data_field->getAttrStr("zky_offset").toInt();
		}

		field_name = data_field->getAttrStr("zky_name");
		if (mColumnsMap.find(field_name) != mColumnsMap.end())
		{
			column = mColumnsMap[field_name];
			type = AosDataType::getTypeStr(column->type);
			format = column->format;
		}
		aos_assert_r(type != "", 0);

		column = 0;
		doc << "<column name=\"" << data_field->getAttrStr("zky_name") 
			<< "\" datatype=\"" << type 
			<< "\" format=\"" << format 
			<< "\" size=\"" << data_field->getAttrStr("zky_length") 
			<< "\" default=\"" << data_field->getAttrStr(AOSTAG_VALUE_DFT_VALUE, "") 
			<< "\" zky_precision=\"" << data_field->getAttrStr("zky_precision", "") 
			<< "\" zky_scale=\"" << data_field->getAttrStr("zky_scale", "") 
			<< "\" offset=\"" << offset << "\" ";
		if (is_virtual_field) doc << " virtual_field=\"true\" zky_type=\"iilmap\"";
		doc	<< " >";
		format = "";

		if (is_virtual_field)
		{
			AosXmlTagPtr keys = data_field->getFirstChild("keys");
			if (!keys)
			{
				//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
				OmnAlarm << __func__ << enderr;
				return 0;
			}
			doc << keys->toString();
		}
		doc << "</column>";
		offset += data_field->getAttrU32("zky_length", 0);//.toInt();
		data_field = data_fields->getNextChild();
	}
	doc << conf_str;
	return doc;
}

OmnString
AosJqlStmtTable::generateMultiRecordSchema(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &schema_doc)
{
	aos_assert_r(schema_doc, "");

	OmnString field_name;
	AosExprObjPtr expr;
	AosExprMemberOptPtr member;
	AosXmlTagPtr field;

	AosXmlTagPtr doc = schema_doc->clone(AosMemoryCheckerArgsBegin);
	AosXmlTagPtr data_records = doc->getFirstChild(true);
	AosXmlTagPtr record = data_records->getFirstChild(true);
	while (record)
	{
		AosXmlTagPtr t_fields = record->getFirstChild(true);
		t_fields = t_fields->getFirstChild(true);

		for (u32 i = 0; i < mVirtualColumns->size(); i++)
		{
			AosExprList* values = (*mVirtualColumns)[i]->mValues;
			for (u32 j = 0; j < values->size(); j++)
			{
				expr = (*values)[i];
				aos_assert_r(expr->getType() == AosExprType::eMemberOpt, "");
				member = dynamic_cast<AosExprMemberOpt*>(expr.getPtr());
				field_name = member->getMember3();
				field = t_fields->getChildByAttr("zky_name", field_name);
				if (field)
					field->setAttr("zky_name", (*mVirtualColumns)[i]->mName);
			}
		}

		record = data_records->getNextChild();
	}
	return doc->toString();
}

void
AosJqlStmtTable::createColumnsMap()
{
	if (!mColumns) return;
	OmnString field_name = "";
	AosJqlColumnPtr column = NULL;
	for (u32 i = 0; i < mColumns->size(); i++ )
	{
		column = (*mColumns)[i];
		field_name = column->name;
		mColumnsMap[field_name] = (*mColumns)[i];
	}
	return;
}

void
AosJqlStmtTable::setSchemaName(OmnString name)
{
	mSchemaName = name;
}
/*
bool
AosJqlStmtTable::createDefaultIndex(const AosRundataPtr &rdata)
{
	if (mFirstFieldName == "") return true;
	if (mInputDataFormat == "xml") return true;

	AosJqlStmtIndex::AosFieldIdxCols *cols = new AosJqlStmtIndex::AosFieldIdxCols();
	AosExprList *expr_list = new AosExprList();
	AosExprObj *expr = new AosExprFieldName(mFirstFieldName.data()); 
	if (expr->getValue(rdata.getPtrNoLock()) == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	expr_list->push_back(expr);
	cols->mFieldList = expr_list;
	//vector<AosJqlStmtIndex::AosFieldIdxCols*>    fieldColumns = new vector<AosJqlStmtIndex::AosFieldIdxCols*>;
	//fieldColumns.push_back(cols);

	AosJqlStmtIndex* statement = new AosJqlStmtIndex;
	//statement->setTableName(getTableFromFullName(mName));
	//statement->setFieldIndexColumns(fieldColumns);
	statement->setFieldList(cols);
	statement->setIsDefault();
	statement->setOp(JQLTypes::eCreate);
	return statement->run(rdata);
}
*/

bool
AosJqlStmtTable::removeDefaultIndex(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &doc)
{
	OmnString index_name;
	bool rslt = false;
	bool flag = doc->getAttrBool("zky_def_index", false);
	if (!flag) return true;

	doc->setAttr("zky_def_index", "false");
	AosXmlTagPtr indexes = doc->getFirstChild("indexes");
	if (!indexes)
	{
		//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	int num = indexes->getNumChilds();
	if (num < 1) return true;
	AosXmlTagPtr index = indexes->getFirstChild(true);
	if (!index)
	{
		//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	index_name = index->getAttrStr("idx_name");
	if (index_name == "")
	{
		//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	rslt = deleteDocByObjid(rdata, index_name);
	if (!rslt)
	{
		//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	rslt = indexes->removeAllChildren();
	if (!rslt)
	{
		//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	rslt = modifyDoc(rdata, doc);
	if (!rslt)
	{
		//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	return true;
}

void 
AosJqlStmtTable::setExists(bool flag)
{
	mIsExists = flag;
}


bool
AosJqlStmtTable::checkTable(
		const AosRundataPtr &rdata,
		const OmnString &database_name,
		const OmnString &table_name)
{
	if (database_name == "") 
		return false;
	if (table_name == "") 
		return false;

	OmnString name = getTableName(table_name, database_name);
	name = getObjid(rdata, JQLTypes::eTableDoc, name);
	if ( getDocByObjid(rdata, name) )
	{
		return true;
	}

	return false;
}

