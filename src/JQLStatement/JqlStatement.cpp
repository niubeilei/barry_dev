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
#include "JQLStatement/JqlStatement.h"

#include "JQLStatement/JQLCommon.h"
#include "JQLStatement/JqlStmtTable.h"
#include "JQLStatement/JqlSelectField.h"

#include "JQLExpr/Expr.h"
#include "JQLExpr/ExprNameValue.h"

#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "IILTrans/RebuildBitmapTrans.h"
#include "QueryUtil/QueryFilter.h"
#include "QueryClient/QueryClient.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/BitmapObj.h"
#include "SearchEngine/DocServerCb.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/JobMgrObj.h"
#include "SEInterfaces/LoginMgrObj.h"
#include "SEInterfaces/ObjMgrObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEUtil/JqlTypes.h"
#include "SEUtil/JqlUtil.h"
#include "StatUtil/Statistic.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"

#include "DataRecord/DataRecord.h"

map<OmnString, OmnString>	AosJqlStatement::mHoldConfs;

AosJqlStatement::AosJqlStatement()
{
	mOp = JQLTypes::eOpInvalid;
	mContentFormat = "xml";
	mConfParms = NULL;
	mIsService = false;
}


AosJqlStatement::AosJqlStatement(const OmnString &contentType)
:
mOp(JQLTypes::eOpInvalid),
mContentFormat(contentType),
mIsService(false)
{
}


AosJqlStatement::~AosJqlStatement()
{
}

//
//get table columns based on table name
//
vector<AosJqlColumnPtr> *
AosJqlStatement::getTableColumns(
			const AosRundataPtr &rdata,
			const OmnString &tablename,
			u32 &recordLen)
{
	AosXmlTagPtr doc = 0;
	AosXmlTagPtr colDoc = 0;
	vector<AosJqlColumnPtr> *tColumns = NULL;
	AosJqlColumnPtr tColumn;

	//get the table definition firstly
	doc = AosJqlStmtTable::getTable(tablename, rdata);
	if (!doc)
	{
		//OmnScreen << "Failed to find table: " << tablename << endl;
		return 0;
	}

	recordLen = doc->getAttrU32("zky_record_len", 0);
	doc = doc->getFirstChild("columns");
	aos_assert_r(doc, tColumns);

	colDoc = doc->getFirstChild("column");
	aos_assert_r(colDoc, tColumns);
	tColumns = new vector<AosJqlColumnPtr>;
	while (colDoc)
	{
		//save column data to allColumns structure
		tColumn = OmnNew AosJqlColumn;
		tColumn->name = colDoc->getAttrStr("name");
		tColumn->type = AosDataType::toEnum(colDoc->getAttrStr("datatype"));
		tColumn->size = colDoc->getAttrU32("size", 0);
		tColumn->offset = colDoc->getAttrU32("offset", 0);
		tColumn->defaultVal = colDoc->getAttrStr("default", "");
		tColumn->precision = colDoc->getAttrInt("zky_precision", -1);
		tColumn->scale = colDoc->getAttrInt("zky_scale", -1);
		tColumns->push_back(tColumn);

	//	recordLen += tColumn->size;
		colDoc = doc->getNextChild();
	}

	return tColumns;
}


//
//get table column based on table name
//
/*
AosJqlColumnPtr
AosJqlStatement::getTableColumn(
			const AosRundataPtr &rdata,
			const OmnString &tablename,
			const OmnString &fieldname)
{
	AosXmlTagPtr doc = 0;
	AosXmlTagPtr colDoc = 0;
	OmnString type;

	//get the table definition firstly
	doc = AosJqlStmtTable::getTable(tablename, rdata);
	if (!doc) return 0;

	doc = doc->getFirstChild("columns");
	aos_assert_r(doc, NULL);

	colDoc = doc->getFirstChild("column");
	aos_assert_r(colDoc, NULL);
	while (colDoc)
	{
		if (fieldname == colDoc->getAttrStr("name"))
		{
			AosJqlColumnPtr tColumn = OmnNew AosJqlColumn;
			tColumn->name = colDoc->getAttrStr("name");
			type = colDoc->getAttrStr("datatype");
			if (type == "str") type = "string";
			tColumn->type = AosDataType::toEnum(type);
			tColumn->sdatatype = colDoc->getAttrStr("sdatatype");
			//tColumn->size = 4; //hardcode it for now
			tColumn->size = colDoc->getAttrInt("size", 0);// Young, 2014/04/21
			tColumn->offset = colDoc->getAttrU32("offset", 0);
			tColumn->precision = colDoc->getAttrInt("zky_precision", -1);
			tColumn->scale = colDoc->getAttrInt("zky_scale", -1);
			tColumn->defaultVal = colDoc->getAttrStr("default", "");
		//	aos_assert_r(tColumn->size > 0, NULL);
			return tColumn;
		}
		colDoc = doc->getNextChild();
	}
	return 0;
}
*/


AosJqlColumnPtr 
AosJqlStatement::getTableField(
		const AosRundataPtr &rdata, 
		const OmnString &tablename, 
		const OmnString &fieldname)
{
	AosJqlColumnPtr tColumn = OmnNew AosJqlColumn;

	AosXmlTagPtr tabledoc = AosJqlStmtTable::getTable(tablename, rdata);
	if ( !tabledoc ) return 0;

	OmnString schemaObjid = tabledoc->getAttrStr("zky_use_schema");
	AosXmlTagPtr schemaDoc = getDocByObjid(rdata, schemaObjid);
	aos_assert_r(schemaDoc, NULL);
	AosXmlTagPtr recordNode = schemaDoc->getFirstChild("datarecord");
	aos_assert_r(recordNode, NULL);
	
	AosXmlTagPtr fieldsNode = recordNode->getFirstChild("datafields");
	aos_assert_r(fieldsNode, NULL);

	AosXmlTagPtr fieldNode = fieldsNode->getFirstChild("datafield");
	while (fieldNode)
	{
		OmnString name = fieldNode->getAttrStr("zky_name");
		if (name != fieldname)
		{
			fieldNode = fieldsNode->getNextChild("datafield");
			continue;
		}
		tColumn->name = fieldname;
		OmnString type = fieldNode->getAttrStr("type");
		if(type == "virtual" || type == "expr")
			type = fieldNode->getAttrStr("data_type");
		tColumn->type = AosDataType::toEnum(type);
		tColumn->sdatatype = fieldNode->getAttrStr("sdatatype");
		tColumn->size = fieldNode->getAttrInt("zky_length", 0);
		tColumn->offset = fieldNode->getAttrU32("offset", 0);
		tColumn->precision = fieldNode->getAttrInt("zky_precision", -1);
		tColumn->scale = fieldNode->getAttrInt("zky_scale", -1);
		tColumn->defaultVal = fieldNode->getAttrStr("default", "");
		tColumn->format =  fieldNode->getAttrStr("format","");
		fieldNode = fieldsNode->getNextChild("datafield");
	}

	return tColumn;
}


//
//Get table name which contains a filed name
//from a list of table names
//
bool
AosJqlStatement::pickTableByField(
			const AosRundataPtr &rdata,
			const vector<OmnString> &tablenames,
			const OmnString &fname,
			OmnString &tablename)
{
	tablename = "";
	for (u32 i=0; i<tablenames.size(); i++)
	{
		OmnString tname = tablenames[i];
		if (isFieldInTable(rdata, tname.data(), fname.data()))
		{
			if (tablename != "")
			{
				OmnCout << "Field exists in more than one tables." << endl;
				return false;
			}
			tablename = tname;
		}
	}

	if (tablename == "")
	{
		OmnCout << "No table contains the field." << endl;
		return false;
	}

	return true;
}

//
//check if a field exists in a table
//
bool
AosJqlStatement::isFieldInTable(
		const AosRundataPtr &rdata,
		const char *table_name,
		const char *field_name)
{
	AosXmlTagPtr doc;
	AosXmlTagPtr colDoc;

	if (!field_name)
	{
		OmnAlarm << "Find field in talbe error: empty field name" << enderr;
		return false;
	}

	doc = getTableDoc(rdata, table_name);
	if (!doc) return false;

	doc = doc->getFirstChild("columns");
	aos_assert_r(doc, false);

	colDoc = doc->getFirstChild("column");
	aos_assert_r(colDoc, false);
	while (colDoc)
	{
		if (colDoc->getAttrStr("name") == field_name) return true;

		colDoc = doc->getNextChild();
	}

	return false;
}

//
//Create table from XML
//
bool
AosJqlStatement::createTable(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &xml)
{
	if (!xml)
	{
		OmnString errmsg = "Empty xml!";
		return false;
	}
	xml->setAttr(AOSTAG_PARENTC, AOSCTNR_TABLE_CONTAINER);

	return createTable(rdata, xml->toString());
}

//
//Create JQL Table
//
bool
AosJqlStatement::createTable(
		const AosRundataPtr &rdata,
		const OmnString &table_def)
{
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	return docclient->createJQLTable(rdata, table_def);
}


//
//Drop a JQL Table
//
bool
AosJqlStatement::dropTable(
		const AosRundataPtr &rdata,
		const OmnString &table_name)
{
	if (table_name == "") return false;

	// It does not work
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	return docclient->deleteDocByObjid(rdata, table_name);
}


//
//List all the table name
//
bool
AosJqlStatement::listDocs(
		const AosRundataPtr &rdata,
		const JQLTypes::DocType type,
		vector<AosXmlTagPtr> &values)
{
	vector<string> tmpValues;
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	bool rslt = docclient->getJQLDocs(rdata, type, tmpValues);
	if (!rslt) return false;

	for (u32 i=0; i<tmpValues.size(); i++)
	{
		AosXmlTagPtr xmlDoc = AosXmlParser::parse(tmpValues[i] AosMemoryCheckerArgs);
		if ( !xmlDoc )
		{
			OmnScreen << "\nERROR: string parse to xml, string:\n" << tmpValues[i] << endl;
			return false;
		}
		values.push_back(xmlDoc);
	}

	return true;
}


OmnString 
AosJqlStatement::getObjid(
		const AosRundataPtr &rdata,
		const JQLTypes::DocType type,
		const OmnString &name)
{
	OmnString newname = name;
	if (type == JQLTypes::eTableDoc)
	{
		newname = getTableFullName(rdata, name);
	}

	return AosObjid::getObjidByJQLDocName(type, newname);
}


bool
AosJqlStatement::getDocs(
		const AosRundataPtr &rdata,
		const JQLTypes::DocType type,
		vector<AosXmlTagPtr> &values)
{
	if ( !isNeedSave(type) )
	{
		return getHoldConfs(rdata, type, values);
	}

	return listDocs(rdata, type, values);
}


//
//Get table by its name
//
AosXmlTagPtr
AosJqlStatement::getTableDoc(
		const AosRundataPtr &rdata,
		const OmnString &tablename)
{
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	string str_doc = "";
	bool rslt = docclient->getDocByJQL(rdata, tablename, str_doc);
	if (!rslt) return NULL;

	return AosStr2Xml(rdata.getPtr(), str_doc AosMemoryCheckerArgs);
}


//
// print tables embedded in a xml file
//
OmnString
AosJqlStatement::printTableInXml(
		const vector<OmnString> &fields,
		map<OmnString, OmnString> &aliasFields,
		const AosXmlTagPtr &data)
{
	OmnString resultStr = "";
	if (fields.size() == 0) return "";
	vector<AosXmlTagPtr> records;
	AosXmlTagPtr child_doc = data->getFirstChild(true);            
	while(child_doc)                                               
	{                                                              
		records.push_back(child_doc);                                 
		child_doc = data->getNextChild();                          
	}                                                              
	OmnString value, field_name;
	OmnString escape_field_name;
	int len, tmp;
	tmp = 2;
	len = 0;
	map<int, int> field_value_len;
	map<OmnString, vector<OmnString> > fields_value;
	for(u32 i = 0; i < records.size(); i++)
	{
		value =  field_name = "";
		for (u32 j = 0; j < fields.size(); j++)
		{
			field_name = fields[j];
			
			// Ketty 2014/06/05
			escape_field_name = field_name;
			AosJqlUtil::escape(escape_field_name);
			//value = records[i]->getAttrStr(field_name);
			value = records[i]->getAttrStr(escape_field_name);
			if (value == "")
			{
				AosXmlTagPtr tmp_doc = records[i]->getFirstChild(field_name);
				if (tmp_doc)
				{
					value = tmp_doc->getNodeText();
				}
				else 
				{
					tmp_doc = records[i]->getFirstChild(true);
					if (tmp_doc)
					{
						value = tmp_doc->getAttrStr(field_name);
					}
				}
				if (value == "") 
				{
					if (aliasFields.find(field_name) != aliasFields.end())
						field_name = aliasFields[field_name];
					value = records[i]->getAttrStr(field_name);
				}
			}
		//	len = value.length() + tmp - getNonEnglishCharCount(value) / 3;
			len = value.visibleCharLen() + tmp - getNonEnglishCharCount(value) / 3;
			if (field_value_len.find(j) == field_value_len.end())
			{
				vector<OmnString> vec;
				vec.push_back(value);
				field_value_len[j] = len;
				fields_value[field_name] = vec;
			}
			else
			{
				if (field_value_len[j] < len)
				{
					 field_value_len[j] = len;
				}

				fields_value[field_name].push_back(value);
			}
		}
	}

	OmnString tmp_str;
	u32 tmp_len; 
	for (u32 i = 0; i < fields.size(); i++)
	{
		field_name = "";
		field_name = fields[i];
		if(aliasFields.find(field_name) != aliasFields.end())
		{
			field_name = aliasFields[field_name];
		}
/*		if (field_name.length() > field_value_len[i])
		{
			tmp_len = field_name.length();
			tmp_str = field_name;
			AosJqlUtil::unescape(tmp_str); 
			tmp_len = tmp_str.length();
			field_value_len[i] = tmp_len + tmp - getNonEnglishCharCount(tmp_str) / 3;
		}
*/
		tmp_str = field_name;
		AosJqlUtil::unescape(tmp_str);
		tmp_len = tmp_str.visibleCharLen();
		if(tmp_len + tmp > field_value_len[i])
		{
			field_value_len[i] = tmp_len + tmp - getNonEnglishCharCount(tmp_str) / 3;
		}

	}

	resultStr << printDividingLine(fields, field_value_len);
	for (u32 i = 0; i < fields.size(); i++)
	{
		field_name = "";
		field_name = fields[i];
		if(aliasFields.find(field_name) != aliasFields.end())
		{
			field_name = aliasFields[field_name];
		}

		aos_assert_r(field_value_len.find(i) != field_value_len.end(), "");
	/*	tmp_len = field_name.length();
		tmp_str = field_name;
		AosJqlUtil::unescape(tmp_str); 
		tmp_len = tmp_str.length();

		if (tmp_len + tmp >= i64(field_value_len[i]))
		{
			field_value_len[i] = tmp_len + tmp;
			len = tmp;
		}
		else if (field_name.length() > field_value_len[i]) 
		{
			field_value_len[i] = field_name.length() + tmp;
			len = tmp;
		}
		else
		{
			len = field_value_len[i] - field_name.length();
		}
*/
		AosJqlUtil::unescape(field_name);
		resultStr << printField(field_name, field_value_len[i]);	//for Chinese characters alignment issue, by white 2014-11-6 16:27:00
	}
	resultStr << "|\n";
	resultStr << printDividingLine(fields, field_value_len);

	field_name = fields[0];
	vector<OmnString> field_values = fields_value[field_name];
	if (field_values.size() == 0)
	{
	//	field_name = "";
		if (aliasFields.find(field_name) != aliasFields.end())
			field_name = aliasFields[field_name];
		field_values = fields_value[field_name];
	}
	for (u32 i = 0; i < field_values.size(); i++)
	{
		value = "";
		for(u32 j = 0; j < fields.size(); j++)
		{
			value = "";
			field_name = fields[j];
			len = field_value_len[j];
			if ((fields_value[field_name]).size() > i)
				value = (fields_value[field_name])[i]; 
			if (value == "")
			{
				if (aliasFields.find(field_name) != aliasFields.end())
					field_name = aliasFields[field_name];
				if ((fields_value[field_name]).size() > i)	
					value = (fields_value[field_name])[i]; 
			}
//			if (len > value.length())
//				resultStr << printField(value, len - value.length());
//			else
//				resultStr << printField(value, value.length()- len );
			resultStr << printField(value, field_value_len[j] );	//for Chinese characters alignment issue, by white 2014-11-6 16:27:00
		}
		//OmnCout << "|" << endl;
		resultStr << "|\n";
	}
	resultStr << printDividingLine(fields, field_value_len);
	return resultStr;
}


OmnString
AosJqlStatement::printDividingLine(
		const vector<OmnString> &fields,
		map<int, int> &field_value_len)
{
	OmnString tmp, field_name;
	tmp << "+";
	for(u32 i = 0; i < fields.size(); i++)
	{
		u32 len = field_value_len[i];
		aos_assert_r(len > 0, "");
		while (len-- > 0)
			tmp << "-";
		tmp << "+";
	}
	tmp << "\n";
	//OmnCout << tmp << endl;
	//return true;
	return tmp;
}


OmnString
AosJqlStatement::printField(
		const OmnString &value,
		const u32 desired_len)	//change the meaning of parameter
		//len to the desired length of the printed line, by white 2014-11-6 16:26:46
{
//	int pad_len = desired_len - value.length() + getNonEnglishCharCount(value) / 3 - 1;
	int pad_len = desired_len - value.visibleCharLen() + getNonEnglishCharCount(value) / 3 - 1;
		//we only manage Chinese characters that encoded to 3 bytes each in utf8
		//for Chinese characters alignment issue, by white 2014-11-6 16:27:00
	//if(pad_len <=0) pad_len = value.length() + 2;
	if(pad_len <=0) pad_len = 2;
	OmnString tmp;
	tmp << "| " << value;
	while (pad_len-- > 0)
		tmp << " ";
	//OmnCout << tmp;
	//return true;
	return tmp;
}


bool
AosJqlStatement::fieldExist(
		const AosRundataPtr &rdata,
		const AosJqlColumnPtr &column,
		const OmnString &name,
		const OmnString &value)
{
	OmnString error;
	if (column->type == AosDataType::eInvalid) return false;
	if (column->type == AosDataType::eString
			|| column->type == AosDataType::eChar
			|| column->type == AosDataType::eCharStr)
	{
		if ((int64_t)column->size < value.length())
		{
			error << "insert's value is too big! field name is : " << name << " !";
			AosSetEntityError(rdata, "JQLStatement_JqlStmtInsert",
					"insert's value is too big!", error) << enderr;
			return false;
		}
	}
	else if (column->type == AosDataType::eInt32)
	{
		int32_t tmp = value.parseInt64(0);
		OmnString vv;
		vv << tmp;
		if (vv != value)
		{
			return false;
		}
	}
	else if (column->type == AosDataType::eInt64)
	{
		int64_t tmp = value.parseInt64(0);
		OmnString vv;
		vv << tmp;
		if (vv != value)
		{
			return false;
		}
	}
	else if (column->type == AosDataType::eU32)
	{
		u32 tmp = value.toU64();
		aos_assert_r(tmp > 0, false);
		OmnString vv;
		vv << tmp;
		if (vv != value)
		{
			return false;
		}
	}
	else if (column->type == AosDataType::eU64)
	{
		u64 tmp = value.toU64(0);
		aos_assert_r(tmp > 0, false);
		OmnString vv;
		vv << tmp;
		if (vv != value)
		{
			return false;
		}
	}

	return true;
}

OmnString
AosJqlStatement::getSingleIILName(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &doc,
		const OmnString &fieldname)
{
	if (!doc) return "";
	OmnString iilname;
	aos_assert_r(doc->getAttrStr(AOSTAG_OTYPE, "") == AOSTAG_TABLE, "");
	AosXmlTagPtr index_nodes = doc->getFirstChild("indexes");
	aos_assert_r(index_nodes, "");
	AosXmlTagPtr index_node = index_nodes->getFirstChild(true);
	while (index_node)
	{
		if (index_node->getAttrStr("zky_type") == "cmp")
		{
			index_node = index_nodes->getNextChild();
			continue;
		}

		if (fieldname == index_node->getAttrStr("zky_name"))
		{
			iilname = index_node->getAttrStr("zky_iilname");
			aos_assert_r(iilname != "", 0);
			return iilname;
		}
		index_node = index_nodes->getNextChild();
	}
	return "";
}


OmnString
AosJqlStatement::getIILName(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &doc,
		const OmnString &fieldname)
{
	if (!doc) return "";
	OmnString iilname;
	aos_assert_r(doc->getAttrStr(AOSTAG_OTYPE, "") == AOSTAG_TABLE, "");
	AosXmlTagPtr index_nodes = doc->getFirstChild("indexes");
	aos_assert_r(index_nodes, "");
	AosXmlTagPtr index_node = index_nodes->getFirstChild(true);
	while (index_node)
	{
		if (fieldname == index_node->getAttrStr("zky_name"))
		{
			iilname = index_node->getAttrStr("zky_iilname");
			aos_assert_r(iilname != "", 0);
			return iilname;
		}
		index_node = index_nodes->getNextChild();
	}

	OmnString cmpiil_name, field_name;
	AosXmlTagPtr conf = doc->getFirstChild("cmp_indexes");
	if (!conf) return "";
	AosXmlTagPtr node = conf->getFirstChild(true);
	while (node)
	{
		cmpiil_name = node->getAttrStr("zky_iilname");
		if (cmpiil_name == "")
		{
			AosSetErrorUser(rdata, __func__)
				<< "This is BUG. IIL name is null." << enderr;
			OmnAlarm << enderr;
			return "";
		}

		AosXmlTagPtr child = node->getFirstChild(true);
		while (child)
		{
			field_name = child->getAttrStr(AOSTAG_NAME);
			if (field_name == fieldname) return cmpiil_name;
			child = node->getNextChild();
		}
		node = conf->getNextChild();
	}

	return "";
}

OmnString
AosJqlStatement::getIILName(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &doc,
		set<OmnString> &fields)
{
	if (fields.size() == 1)
	{
		return getSingleIILName(rdata, doc, *(fields.begin()));
	}
	if (!doc) return "";
	OmnString iilname;
	aos_assert_r(doc->getAttrStr(AOSTAG_OTYPE, "") == AOSTAG_TABLE, "");
	AosXmlTagPtr index_nodes = doc->getFirstChild("indexes");
	aos_assert_r(index_nodes, "");

	OmnString cmpiil_name, field_name;
	AosXmlTagPtr conf = doc->getFirstChild("cmp_indexes");
	if (!conf) return "";
	AosXmlTagPtr node = conf->getFirstChild(true);
	while (node)
	{
		if (node->getNumChilds() != (int64_t)fields.size())
		{
			node = conf->getNextChild();
			continue;
		}

		cmpiil_name = node->getAttrStr("zky_iilname");
		if (cmpiil_name == "")
		{
			AosSetErrorUser(rdata, __func__)
				<< "This is BUG. IIL name is null." << enderr;
			OmnAlarm << enderr;
			return "";
		}

		bool flag = true;
		AosXmlTagPtr child = node->getFirstChild(true);
		while (child)
		{
			field_name = child->getAttrStr(AOSTAG_NAME);
			if (fields.count(field_name) == 0)
			{
				flag = false;
				break;
			}
			child = node->getNextChild();
		}
		if (flag)
			return cmpiil_name;
		node = conf->getNextChild();
	}

	return "";
}


bool
AosJqlStatement::setPrintData(
		const AosRundataPtr &rdata,
		const OmnString &data)
{
	OmnString tmp = "<content><![CDATA[";
	
	// Modify by Young, when multiple CDATA in XML, we replace it.
	OmnString new_data = data;
	new_data.replace("<![CDATA[", "0x0333333", true);
	new_data.replace("]]>", "0x0444", true);

	tmp << new_data << "]]></content>";
	rdata->setResults(tmp);

	return true;
}


bool
AosJqlStatement::removeQueryRsltAttr(AosXmlTagPtr &data)
{
	aos_assert_r(data, false);
	AosXmlTagPtr child_doc = data->getFirstChild(true);
	while (child_doc)
	{
		child_doc->removeMetadata();
		child_doc = data->getNextChild();
	}
	return true;
}


AosXmlTagPtr
AosJqlStatement::getDocByKey(
		const AosRundataPtr &rdata,
		const OmnString &container,
		const OmnString &key_name,
		const OmnString &key_value)
{
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	return docclient->getDocByKey(rdata, container, key_name, key_value);
}

AosXmlTagPtr
AosJqlStatement::getDocByObjid(
		const AosRundataPtr &rdata,
		const OmnString &objid)
{
	// 1. If objid stored in map, return config
	if ( mHoldConfs.count(objid) )
	{
		OmnString confStr = mHoldConfs[objid];
		return AosXmlParser::parse(confStr AosMemoryCheckerArgs);
	}

	// 2. not in map, we will get config from database
	aos_assert_r(objid != "", NULL);
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	return docclient->getDocByObjid(objid, rdata);
}


AosXmlTagPtr
AosJqlStatement::getDocByDocid(
		const AosRundataPtr &rdata,
		const u64 &docid)
{
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	return docclient->getDocByDocid(docid, rdata);
}


bool
AosJqlStatement::deleteDocByObjid(
		const AosRundataPtr &rdata,
		const OmnString &objid)
{
	aos_assert_r(objid != "", false);
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	return docclient->deleteDocByObjid(rdata, objid);
}


bool
AosJqlStatement::deleteDocByDocid(
		const AosRundataPtr &rdata,
		const u64 &docid)
{
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	return docclient->deleteDocByDocid(rdata, docid);
}


bool
AosJqlStatement::modifyDoc(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &doc)
{
	aos_assert_r(doc, false);
	return AosModifyDoc(doc, rdata);
}


bool
AosJqlStatement::createDocByObjid(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf,
		const OmnString &objid)
{
	conf->setAttr(AOSTAG_OBJID,	objid);
	conf->setAttr(AOSTAG_PUBLIC_DOC, true);
	conf->setAttr(AOSTAG_CTNR_PUBLIC, true);

	// Now we only create XML type, we can create more types if we need
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	//rdata->setReceivedDoc(conf, true);
	return docclient->createDoc1(rdata, 0, rdata->getCid(), conf, false, 0, 0, false);
}


bool
AosJqlStatement::createDoc(
		const AosRundataPtr &rdata,
		const JQLTypes::DocType type,
		const OmnString &name,
		const AosXmlTagPtr &doc)
{
	aos_assert_r(doc, false);
	// 1. if doc not need save, we will put it into mHoldMap
	if (!isNeedSave(type))
	{
		setHoldConf(rdata, type, name, doc->toString());
		return true;
	}

	// 2. if doc nedd save, we will save it into database
	OmnString newname = name;
	if (type == JQLTypes::eTableDoc) newname = getTableFullName(rdata, name);
	OmnString objid = AosObjid::getObjidByJQLDocName(type, newname);
	doc->setAttr(AOSTAG_PUBLIC_DOC, "true");
	doc->setAttr(AOSTAG_CTNR_PUBLIC, "true");

OmnScreen << "\n####################################################################" << endl;
OmnCout << "#createDoc (objid::" << objid << ")\n" << doc->toString() << "\n" << endl;

	return createDocByObjid(rdata, doc, objid);
}


AosXmlTagPtr
AosJqlStatement::getDoc(
		const AosRundataPtr &rdata,
		const JQLTypes::DocType type,
		const OmnString &name)
{
	if ( isNeedSave(type) )
	{
		return getDocByName(rdata, type, name);
	}

	return AosXmlParser::parse(getHoldConf(rdata, type, name) AosMemoryCheckerArgs);
}


bool
AosJqlStatement::deleteDoc(
		const AosRundataPtr &rdata,
		const JQLTypes::DocType type,
		const OmnString &name)
{
	if ( isNeedSave(type) )
	{
		OmnString newname = name;
		if (type == JQLTypes::eTableDoc) newname = getTableFullName(rdata, name);
		OmnString objid = AosObjid::getObjidByJQLDocName(type, newname);
		return deleteDocByObjid(rdata, objid);
	}
	else
	{
		return dropHoldConf(rdata, type, name);
	}
	return false;
}


AosXmlTagPtr
AosJqlStatement::getDocByName(
		const AosRundataPtr &rdata,
		const JQLTypes::DocType type,
		const OmnString &name)
{
	OmnString newname = name;
	if (type == JQLTypes::eTableDoc) newname = getTableFullName(rdata, name);
	OmnString objid = AosObjid::getObjidByJQLDocName(type, newname);
	aos_assert_r(objid != "", 0);
	return getDocByObjid(rdata, objid);
}


OmnString
AosJqlStatement::getNameByExprObj(AosExprObjPtr &expr)
{
	aos_assert_r(expr, "");
	OmnString name = "";
	AosExprType::E type = expr->getType();
	if (AosExprType::eMemberOpt == type)
	{
		name = expr->dump();
	}
	else
	{
		AosValueRslt vv;
		bool rslt = expr->getValue(0, 0, vv);
		aos_assert_r(rslt, "");
		name = vv.getStr();
	}
	aos_assert_r(name != "", "");

	return name;
}

// Ken Lee, 2015/08/03 move to SEUtil/JqlUtil.h
/*
bool
AosJqlStatement::escape(OmnString &data)
{
	data.replace(".", "0x2e", true);
	data.replace("<", "0x3c", true);
	data.replace(">", "0x3e", true);
	data.replace("(", "0x28", true);
	data.replace(")", "0x29", true);
	data.replace("/", "0x2f", true);
	data.replace("*", "0x2a", true);
	data.replace(",", "0x2c", true);
	data.replace("\"", "0x22", true);
	data.replace("'", "0x27", true);
	data.replace("-", "0x2d", true);
	data.replace(":", "0x3a", true);
	data.replace(" ", "0x20", true);
	return true;
}


bool
AosJqlStatement::unescape(OmnString &data)
{
	data.replace("0x2e", ".", true);
	data.replace("0x3c", "<", true);
	data.replace("0x3e", ">", true);
	data.replace("0x27", "'", true);
	data.replace("0x28", "(", true);
	data.replace("0x29", ")", true);
	data.replace("0x2f", "/", true);
	data.replace("0x2a", "*", true);
	data.replace("0x2c", ",", true);
	data.replace("0x22", "\"", true);
	data.replace("0x20", " ", true);
	data.replace("0x2d", "-", true);
	data.replace("0x3a", ":", true);
	return true;
}
*/

OmnString
AosJqlStatement::getTableFullName(const AosRundataPtr &rdata, const OmnString &name)
{
	OmnString dbname = getCurDatabase(rdata);
	aos_assert_r(name != "", "");
	if (name.find('.', false) != -1) return name;
	OmnString fullname = "";
	//fullname << dbname.toLower() << "." << name;
	//Young : 2014/07/31
	fullname << dbname << "." << name;

	return fullname;
}


OmnString
AosJqlStatement::getCurDatabase(const AosRundataPtr &rdata)
{
	OmnString msg;
	AosSessionObjPtr session = rdata->getSession();
	if (!session)
	{
		msg << "Please login.";
		rdata->setJqlMsg(msg);
		return "";
	}
	return session->getDbName();
}


OmnString
AosJqlStatement::getHoldConf(
		const AosRundataPtr &rdata,
		const JQLTypes::DocType type,
		const OmnString &name)
{
	OmnString newname = name;
	if (type == JQLTypes::eTableDoc) newname = getTableFullName(rdata, name);
	OmnString objid = AosObjid::getObjidByJQLDocName(type, newname);
	if (mHoldConfs.count(objid) > 0)
	{
		return mHoldConfs[objid];
	}

	return "";
}


bool
AosJqlStatement::setHoldConf(
		const AosRundataPtr &rdata,
		const JQLTypes::DocType type,
		const OmnString &name,
		const OmnString &conf)
{
	OmnString newname = name;
	if (type == JQLTypes::eTableDoc) newname = getTableFullName(rdata, name);
	OmnString objid = AosObjid::getObjidByJQLDocName(type, newname);
	mHoldConfs[objid] = conf;
OmnScreen << "\n####################################################################\n" <<
	"#setHoldConf: \n" << conf << "\n" << endl;
	return true;
}


bool
AosJqlStatement::isConfExist(
		const AosRundataPtr &rdata,
		const JQLTypes::DocType type,
		const OmnString &name)
{
	if( getDoc(rdata, type, name) )
	{
		return true;
	}

	return false;
}


bool
AosJqlStatement::dropHoldConf(
		const AosRundataPtr &rdata,
		const JQLTypes::DocType type,
		const OmnString &name)
{
	OmnString newname = name;
	if (type == JQLTypes::eTableDoc) newname = getTableFullName(rdata, name);
	OmnString objid = AosObjid::getObjidByJQLDocName(type, newname);
	if (mHoldConfs.count(objid) > 0) mHoldConfs.erase(objid);
	return true;
}


bool
AosJqlStatement::isNeedSave(const JQLTypes::DocType type)
{
	bool flag = true;
	switch (type)
	{
	case JQLTypes::eTaskDoc:
	case JQLTypes::eDataProcDoc:
	case JQLTypes::eDataScannerDoc:
	case JQLTypes::eDataConnectorDoc:
	case JQLTypes::eDataFieldDoc:
		flag = false;
		break;
	default:
		break;
	}

	return flag;
}


bool
AosJqlStatement::getHoldConfs(
		const AosRundataPtr &rdata,
		const JQLTypes::DocType type,
		vector<AosXmlTagPtr> &values)
{
	OmnString tag = AosObjid::getObjidPrefixByJQLDocType(type);
	int cmplen = tag.length();
	for (map<OmnString, OmnString>::iterator itr = mHoldConfs.begin();
			itr != mHoldConfs.end(); itr++)
	{
		OmnString key = itr->first;
		if (strncmp(key.data(), tag.data(), cmplen) == 0)
		{
			OmnString confStr = itr->second;
			AosXmlTagPtr conf = AosXmlParser::parse(confStr AosMemoryCheckerArgs);
			aos_assert_r(conf, false);
			values.push_back(conf);
		}
	}

	return true;
}


AosXmlTagPtr
AosJqlStatement::getConf(
		const AosRundataPtr &rdata,
		boost::shared_ptr<AosConfBase> confObj,
		AosExprNameValues* parms)
{
	if ( !parms ) return 0;

	for (size_t i=0; i<(*parms).size(); i++)
	{
		OmnString key = (*parms)[i]->getName();
		OmnString value = (*parms)[i]->getValue()->getValue(rdata.getPtrNoLock());
		confObj->setAttribute(key, value);
	}

	OmnString confstr = confObj->getConfig();
	return AosXmlParser::parse(confstr AosMemoryCheckerArgs);
}


// This function just for shawn
bool
AosJqlStatement::rebuildBitmap(
		const AosRundataPtr &rdata,
		const OmnString &iilname)
{
	AosTransPtr trans = OmnNew AosIILTransRebuildBitmap(iilname, false, true, 0);
	bool timeout = false;
	AosBuffPtr resp;
	return AosSendTrans(rdata, trans, timeout, resp);
}


bool
AosJqlStatement::setCurDatabase(
		const AosRundataPtr &rdata,
		const OmnString &dbname)
{
	OmnString msg;
	AosSessionObjPtr session = rdata->getSession();
	if (!session)
	{
		msg << "Please login.";
		rdata->setJqlMsg(msg);
		return true;
	}
	session->setDbName(dbname);
	return true;
}


AosBitmapObjPtr
AosJqlStatement::querySafe(
		const AosRundataPtr &rdata,
		vector<QueryCondtion> &conds)
{
    AosBitmapObjPtr bitmap = AosBitmapObj::createBitmapStatic();
    for (size_t i=0; i<conds.size(); i++)
    {
        AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
        query_context->setBlockSize(0);
        query_context->setOpr(conds[i].mOpr);
        query_context->setStrValue(conds[i].mKey);
        bool rslt = AosQueryColumn(conds[i].mIILName, 0, bitmap, query_context, rdata);
        aos_assert_r(rslt, 0);

        if (bitmap->isEmpty()) break;
    }

    return bitmap;
}


AosBitmapObjPtr
AosJqlStatement::querySafe(
        const AosRundataPtr &rdata,
        QueryCondtion &cond)
{

    AosBitmapObjPtr bitmap = AosBitmapObj::createBitmapStatic();
    AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
    query_context->setBlockSize(0);
    query_context->setOpr(cond.mOpr);
    query_context->setStrValue(cond.mKey);
    bool rslt = AosQueryColumn(cond.mIILName, 0, bitmap, query_context, rdata);
    aos_assert_r(rslt, 0);

    return bitmap;
}


AosXmlTagPtr
AosJqlStatement::createDoc1(
		const AosRundataPtr &rdata,
		const OmnString &conf)
{
	AosXmlTagPtr doc = AosXmlParser::parse(conf AosMemoryCheckerArgs);
	if (!doc) return NULL;
	return AosCreateDoc(doc, rdata);
}


bool
AosJqlStatement::deleteIIL(
		const AosRundataPtr &rdata,
		const OmnString &iilname)
{
 	return AosIILClientObj::getIILClient()->deleteIIL(
			iilname, true, rdata);
}


bool
AosJqlStatement::createUser(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &userdoc)
{
	if (userdoc)
	{
		userdoc->setAttr(AOSTAG_PUBLIC_DOC, "true");
		userdoc->setAttr(AOSTAG_CTNR_PUBLIC, "true");

		OmnString resp;
		AosLoginMgrObjPtr login_mgr = AosLoginMgrObj::getLoginMgr();
		return login_mgr->createUserAccount(rdata, userdoc, resp);
	}

	return false;
}


bool
AosJqlStatement::modifyDocAttr(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &doc,
		const OmnString &name,
		const OmnString &value)
{
	return AosModifyDocAttrStr(rdata, doc, name, value);
}

bool
AosJqlStatement::restartJob(
		const AosRundataPtr &rdata,
		const OmnString &objid)
{
	AosJobMgrObjPtr jobmgr = AosJobMgrObj::getJobMgr();
	//AosXmlTagPtr doc = AosGetDocByObjid(objid, rdata);
	u64 userid = rdata->setUserid(AosObjMgrObj::getSuperUserDocid());
	rdata->setUserid(userid);

	OmnString args = "";
	args << "objid=" << objid << ","<< "cmdid=" << AOSCOMMAND_RESTARTJOB;

	return jobmgr->reStartJob(args, objid, rdata);
}


bool
AosJqlStatement::stopJob(
		const AosRundataPtr &rdata,
		const OmnString &objid)
{
	AosJobMgrObjPtr jobmgr = AosJobMgrObj::getJobMgr();
	//AosXmlTagPtr doc = AosGetDocByObjid(objid, rdata);
	u64 userid = rdata->setUserid(AosObjMgrObj::getSuperUserDocid());
	rdata->setUserid(userid);

	OmnString args = "";
	args << "objid=" << objid << ","<< "cmdid=" << AOSCOMMAND_STOPJOB;

	return jobmgr->stopJob(args, objid, rdata);
}


bool
AosJqlStatement::startJob(
		const AosRundataPtr &rdata,
		const OmnString &objid)
{
	AosJobMgrObjPtr jobmgr = AosJobMgrObj::getJobMgr();
	AosXmlTagPtr doc = AosGetDocByObjid(objid, rdata);
	u64 userid = rdata->setUserid(AosObjMgrObj::getSuperUserDocid());
	rdata->setUserid(userid);

	OmnString args = "";
	args << "objid=" << objid << ","<< "cmdid=" << AOSCOMMAND_STARTJOB;

	//rslt = jobmgr->startJob(args, service_name, job_name, doc, rdata);
	return jobmgr->startJob(args, objid, doc, rdata);
}


bool
AosJqlStatement::stopJob(const OmnString &objid, OmnString &args)
{
	return false;
}


AosXmlTagPtr
AosJqlStatement::executeQuery(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf)
{
	if (conf)
	{
		OmnString contents;
		AosQueryClient *query_client = AosQueryClient::getSelf();
		bool rslt = query_client->executeQuery(conf, contents, rdata);
		if (!rslt) return NULL;
		return AosXmlParser::parse(contents AosMemoryCheckerArgs);
	}

	OmnAlarm << __func__ << enderr;
	return NULL;
}


bool
AosJqlStatement::runSmartDoc(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf)
{
	if (conf)
	{
		return AosRunSmartDoc(rdata, conf);
	}

	return false;
}


bool
AosJqlStatement::refactorInternalStatConf(
		const AosRundataPtr &rdata,
		const OmnString &stat_doc_objid,
		const AosXmlTagPtr &stat_def_doc)
		//const OmnString &key)
{
	if (!stat_def_doc) return false;
	AosXmlTagPtr new_doc = AosStatistic::refactorInternalStatConf(rdata,
			stat_doc_objid, stat_def_doc);
	if (new_doc) return true;

	return false;
}


bool
AosJqlStatement::querySafeOnIIL(
		const AosRundataPtr &rdata,
		const i64 &page_size,
		const i64 block_size,
		const OmnString &term_str,
		vector<i64>	&docids,
		vector<string> &values,
		i64 &numDocs)
{
	AosQueryRsltObjPtr query_rslt = AosQueryRsltObj::getQueryRsltStatic();
	query_rslt->setWithValues(true);
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	AosBitmapObjPtr bitmap = AosBitmapObj::createBitmapStatic();
	AosXmlTagPtr term = AosStr2Xml(rdata.getPtr(), term_str AosMemoryCheckerArgs);
	aos_assert_r(term, false);

	OmnString opr = term->getNodeText("opr");
	OmnString v1 = term->getNodeText("value1");
	OmnString v2 = term->getNodeText("value2");
	OmnString iilname = term->getNodeText("iilname");
	AosXmlTagPtr query_filters = term->getFirstChild("query_filters");

	AosQueryFilterObjPtr filter;
	if (query_filters)
	{
		AosXmlTagPtr query_filter = query_filters->getFirstChild();
		while (query_filter)
		{
			filter = AosQueryFilter::create(query_filter, rdata);
			aos_assert_r(filter, false);
			query_context->addFilter(filter);
			query_filter = query_filters->getNextChild();
		}
	}


	query_context->setOpr(AosOpr_toEnum(opr));
	query_context->setStrValue(v1);
	if (v2 != "")
	{
		query_context->setStrValue2(v2);
	}

	query_context->setPageSize(block_size);
	query_context->setBlockSize(page_size + block_size);
	bool rslt = AosQueryColumn(iilname, query_rslt, 0, query_context, rdata);
	aos_assert_r(rslt, false);

	u64 docid;
	bool finished = false;
	numDocs = query_context->getTotalDocInRslt();
	AosIILType type = query_context->getIILType();
	int tmp = 1;
	OmnString vv;
	u64 u64_vv = 0;
	d64 d64_vv= 0;
	i64 i64_vv= 0;
	if (type == eAosIILType_BigD64)
	{
		while (query_rslt->nextDocidValue(docid, d64_vv, finished, rdata))
		{
			if (finished) break;
			if (tmp >= page_size)
			{
				docids.push_back((i64)docid);
				vv << d64_vv;
				values.push_back(vv.getBuffer());
			}
			tmp ++;
			vv = "";
		}
	}
	else if (type == eAosIILType_BigI64)
	{
		while (query_rslt->nextDocidValue(docid, i64_vv, finished, rdata))
		{
			if (finished) break;
			if (tmp >= page_size)
			{
				docids.push_back((i64)docid);
				vv << i64_vv;
				values.push_back(vv.getBuffer());
			}
			tmp ++;
			vv = "";
		}
	}

	else if (type == eAosIILType_U64 || type == eAosIILType_BigU64)
	{
		while (query_rslt->nextDocidValue(docid, u64_vv, finished, rdata))
		{
			if (finished) break;
			if (tmp >= page_size)
			{
				docids.push_back((i64)docid);
				vv << u64_vv;
				values.push_back(vv.getBuffer());
			}
			tmp ++;
			vv = "";
		}
	}

	else
	{
		while (query_rslt->nextDocidValue(docid, vv, finished, rdata))
		{
			if (finished) break;
			if (tmp >= page_size)
			{
				docids.push_back((i64)docid);
				values.push_back(vv.getBuffer());
			}
			tmp ++;
			vv = "";
		}
	}

	return true;
}



u32
AosJqlStatement::getNonEnglishCharCount(const OmnString & oString)
{
	u32 illChineseCharCount = 0;
	for (int i = 0; i < oString.length(); i++)
	{
		if((unsigned short)oString.getChar(i) > 127)
		{
			illChineseCharCount++;
		}
	}
	return illChineseCharCount;
}


OmnString
AosJqlStatement::AosJType2DType(const OmnString &type)
{
	if (type == "string"
		|| type == "date"
		//|| type == "datetime"
		|| type == "char"
		|| type == "varchar"
		|| type == "varchar2"
		|| type == "text")
	{
		return "str";
	}

	if (//type == "number" ||
			type == "float")
	{
		return "double";
	}

	if (type == "int")
	{
		return "u64";
	}
	return type;
}


OmnString
AosJqlStatement::AosDType2JType(const OmnString &type)
{
	if (type == "str")
	{
		return "string";
	}

//	if (type == "double")
//	{
//		return "number";
///	}

	return type;
}

bool
AosJqlStatement::doInsertInto(
		const AosRundataPtr &rdata,
		const OmnString &dataconf)
{
	AosXmlTagPtr data = AosXmlParser::parse(dataconf AosMemoryCheckerArgs);
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	return docclient->insertInto(data, rdata);
}

void 
AosJqlStatement::setOrigStatement(const OmnString &stmt)
{
	mOrigStmt = stmt;
}

OmnString 
AosJqlStatement::getOrigStatement() const
{
	return mOrigStmt;
}


bool 
AosJqlStatement::run(
		const AosRundataPtr &rdata,
		const AosJimoProgObjPtr &prog)
{
	OmnNotImplementedYet;
	return false;
}


void 
AosJqlStatement::setIsService(bool &isService)
{
	mIsService = isService;
}

bool
AosJqlStatement::isService() const
{
	return mIsService;
}




bool 
AosJqlStatement::run(
		AosRundata *rdata, 
		AosJimoProgObj *prog, 
		OmnString &statements_str,
		bool isparser)
{
	bool rslt = AosCheckIsLogin(rdata);
	if (!rslt) 
	{	
		OmnString msg = "";
		msg << "You are not logged in. Please login (if someone "
			"else is using your login, it is possible that you are "
			"kicked out by that person!)";
		rdata->setJqlMsg(msg);
		return false;
	}

	statements_str << "\n(AtomicStatement): " << mOrigStmt;
	return run(rdata, prog);
}
	
bool
AosJqlStatement::getStatements(vector<AosJqlStatementPtr> &statemets)
{
	OmnShouldNeverComeHere;
	return false;
}

// JIMODB-77
//  2015/7/28
bool 
AosJqlStatement::checkDatabase(const AosRundataPtr &rdata)
{
	return setDatabase(rdata);
}


bool
AosJqlStatement::setDatabase(const AosRundataPtr &rdata)
{
	OmnString msg;
	AosSessionObjPtr session = rdata->getSession();
	if (!session)
	{
		msg << "You are not logged in. Please login (if someone "
			"else is using your login, it is possible that you are "
			"kicked out by that person!)";
		rdata->setJqlMsg(msg);
		return false;
	}

	mCurDatabase = session->getDbName();
	if (mCurDatabase == "") 
	{
		rdata->setJqlMsg("ERROR: No database selected. Please select a database first!");
		return false;
	}

	return true;
}


AosXmlTagPtr
AosJqlStatement::getTableDataField(
		const AosRundataPtr &rdata,
		const OmnString &table_name,
		const AosJqlSelectFieldPtr &field)
{
	OmnString field_name = field->getFieldEscapeOName(rdata); 
	OmnString datafield = "<datafield/>";
	AosXmlTagPtr datafield_doc = AosXmlParser::parse(datafield AosMemoryCheckerArgs);

	AosDataRecordObjPtr record;
	AosXmlTagPtr recordNode;

	if (!mInputRecord)
	{
		AosXmlTagPtr tabledoc = AosJqlStmtTable::getTable(table_name, rdata);
		if ( !tabledoc ) return 0;

		OmnString schemaObjid = tabledoc->getAttrStr("zky_use_schema");
		AosXmlTagPtr schemaDoc = getDocByObjid(rdata, schemaObjid);
		aos_assert_r(schemaDoc, NULL);
		recordNode = schemaDoc->getFirstChild("datarecord");
		aos_assert_r(recordNode, NULL);
	}
	else
	{
		recordNode = mInputRecord;
	}

	record = AosDataRecordObj::createDataRecordStatic(recordNode, 0, rdata.getPtr() AosMemoryCheckerArgs);
	aos_assert_r(record, NULL);

	OmnString errmsg = "";
	OmnString fieldName;
	//when field like _week0x28deal_time0x29, wo should convert it into original field deal_time
	if(!field->checkIsAgrFunc() && field->mExpr->getType() == AosExprType::eGenFunc)
	{
		field_name = field->getFieldName(rdata);
	}

	fieldName << field_name << ";";
	AosExprObjPtr expr = AosParseExpr(fieldName, errmsg, rdata.getPtr()); 

	AosDataFieldType::E type = convertToDataFieldType(rdata.getPtr(), expr, record);

	field_name = field->getFieldEscapeCName(rdata);
	if (field_name == "") field_name =  field->getFieldEscapeOName(rdata);
	aos_assert_r(field_name != "",  NULL);

	if(field_name.hasPrefix("_week")) type = AosDataFieldType::eStr; 
	OmnString str_type =  AosDataFieldType::toString(type);
	
	datafield_doc->setAttr("zky_name", field_name);
	datafield_doc->setAttr("type", str_type);

	return datafield_doc;

}


AosDataFieldType::E
AosJqlStatement::convertToDataFieldType(
		AosRundata* rdata,
		const AosExprObjPtr &expr,
		const AosDataRecordObjPtr &record)
{
	AosDataType::E datatype = expr->getDataType(rdata, record.getPtr());
	OmnString str_type = AosDataType::getTypeStr(datatype);

	AosDataFieldType::E type = AosDataFieldType::toEnum(str_type);
	//aos_assert_r(type != AosDataFieldType::eInvalid, AosDataFieldType::eInvalid);
	switch(type)
	{
		case AosDataFieldType::eStr:
			break;
		case AosDataFieldType::eBinU64:
			break;
		case AosDataFieldType::eBinDouble:
			break;
		case AosDataFieldType::eU64:
			type = AosDataFieldType::eBinU64;
			break;
		case AosDataFieldType::eDouble:
		case AosDataFieldType::eNumber:
			type = AosDataFieldType::eBinDouble;
			break;
		case AosDataFieldType::eDateTime:
			type = AosDataFieldType::eBinDateTime;
			break;
		case AosDataFieldType::eInt64:
			type = AosDataFieldType::eBinInt64;
			break;
		case AosDataFieldType::eInvalid:
			break;

		default:
			OmnAlarm << "not handle this data field type: " << str_type << enderr;
			break;
	}

	return type;
}


OmnString	
AosJqlStatement::getFieldDataType(
		const AosRundataPtr &rdata, 
		const OmnString &field_name)
{
	AosXmlTagPtr fielddoc = getDoc(rdata, JQLTypes::eDataFieldDoc, field_name);
	aos_assert_r(fielddoc, "");
	return fielddoc->getAttrStr("type");
}
