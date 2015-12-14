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
// 04/25/2014 Created by Young
////////////////////////////////////////////////////////////////////////////

#include "AosConf/DataField.h"
#include "AosConf/DataSchema.h"
#include "AosConf/DataFieldExpr.h"
#include "AosConf/DataFieldMath.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/DataTypes.h"
#include <boost/make_shared.hpp>

using AosConf::DataField;
using AosConf::DataFieldExpr;
using AosConf::DataFieldMath;
using AosConf::DataSchema;
using boost::make_shared;

DataField::DataField(const AosXmlTagPtr &xml)
{
	aos_assert(xml);
	vector<pair<OmnString ,OmnString> > v = xml->getAttributes();
	for (size_t i=0; i<v.size(); i++) 
	{
		setAttribute(v[i].first, v[i].second);
	}
}


DataField::DataField()
{
}


DataField::~DataField()
{
}

		
string
DataField::getConfig() 
{
	string conf = "<datafield ";
	for (map<string, string>::iterator itr=mAttrs.begin();
			itr!=mAttrs.end(); itr++)
	{
		conf += " " + itr->first + "=\"" + itr->second + "\"";
	}
	conf += ">";
	conf += "</datafield>";
	return conf;
}


bool
DataField::getVirtualField(
				const AosXmlTagPtr &tableDoc,
				const AosXmlTagPtr &schema_doc,
				vector<AosXmlTagPtr> &inputs,
				vector<AosXmlTagPtr> &outputs)
{
	vector<boost::shared_ptr<DataField> > inputFields;
	vector<boost::shared_ptr<DataField> > outputFields;
	boost::shared_ptr<AosConf::DataSchema> schema = boost::make_shared<AosConf::DataSchema>(schema_doc);
	boost::shared_ptr<DataRecord> datarecord = schema->getRecord();
	vector<boost::shared_ptr<DataField> > &v = datarecord->getFields();
	AosXmlTagPtr datafield_tag;
	OmnString type;
	AosXmlTagPtr columns = tableDoc->getFirstChild("columns");
	aos_assert_r(columns, false);
	AosXmlTagPtr column = columns->getFirstChild();
	while(column)
	{
		type = column->getAttrStr("type");
		if (type == "virtual")
		{
			convertVirtualField(column, v, inputFields, outputFields);
		}
		column = columns->getNextChild();
	}

	for (u32 i=0; i<inputFields.size(); i++)
	{
		AosXmlTagPtr field_tag = AosXmlParser::parse(inputFields[i]->getConfig() AosMemoryCheckerArgs);
		aos_assert_r(field_tag, false);
		inputs.push_back(field_tag);
	}

	for (u32 i=0; i<outputFields.size(); i++)
	{
		AosXmlTagPtr field_tag = AosXmlParser::parse(outputFields[i]->getConfig() AosMemoryCheckerArgs);
		aos_assert_r(field_tag, false);
		outputs.push_back(field_tag);
	}

	return true;
}

bool
DataField::convertVirtualField(
				const AosXmlTagPtr &vfconf,
				vector<boost::shared_ptr<DataField> > &v,
				vector<boost::shared_ptr<DataField> > &inputFields,
				vector<boost::shared_ptr<DataField> > &outputFields)
{
	aos_assert_r(vfconf, false);
	OmnString column_type = vfconf->getAttrStr("type", "");
	aos_assert_r(column_type != "", false);
	if (column_type == "expr")
	{
		boost::shared_ptr<AosConf::DataFieldExpr> fieldObj = boost::make_shared<AosConf::DataFieldExpr>(vfconf);
		inputFields.push_back(fieldObj);
	}
	AosXmlTagPtr keys = vfconf->getFirstChild("keys");		
	aos_assert_r(keys, false);
	AosXmlTagPtr key = keys->getNextChild();
	OmnString filedname = "cps";
	while (key)
	{
		OmnString str = key->getNodeText();
		aos_assert_r(str != "", false);
		filedname << "_" << str;
		key = keys->getNextChild();
	};

	// This program will  construct two datafieldObj
	// one is keyField, and the other one is valueField
	OmnString vfname = vfconf->getAttrStr("name");
	AosXmlTagPtr mapNode = vfconf->getFirstChild("map");
	aos_assert_r(mapNode, false);
	OmnString datatype = mapNode->getAttrStr("datatype");
	//aos_assert(datatype != "");
	AosXmlTagPtr keysNode = vfconf->getFirstChild("keys");
	AosXmlTagPtr valuesNode = mapNode->getFirstChild("values");
	aos_assert_r(keysNode && valuesNode, false);

	// 1. key field
	u32 keyLen = 0;
	OmnString keyName, valueName;
	keyName << vfname << "_key";
	valueName = vfname;

	boost::shared_ptr<DataFieldMath> keyDataFieldObj = boost::make_shared<DataFieldMath>();
	keyDataFieldObj->setAttribute("zky_opr", "concat");
	keyDataFieldObj->setAttribute("zky_name", keyName);
	AosXmlTagPtr keyNode = keysNode->getFirstChild("key");
	int times = 0;
	while (keyNode)
	{
		times++;
		OmnString fieldname = keyNode->getNodeText();
		aos_assert_r(fieldname != "", false);

		boost::shared_ptr<DataFieldExpr> fieldExprObj = boost::make_shared<DataFieldExpr>();
		OmnString tmpExprName = "";
		tmpExprName << keyName << "_f" << times;
		fieldExprObj->setAttribute("zky_name", tmpExprName);
		fieldExprObj->setExpr(fieldname);
		inputFields.push_back(fieldExprObj);
		keyDataFieldObj->setFieldName(tmpExprName);

		for (size_t i=0; i<v.size(); i++)
		{
			if (fieldname == v[i]->getAttribute("zky_name"))
			{
				int len = atoi(v[i]->getAttribute("zky_length").data());	
				if (len <= 0) len = 50;
				keyLen += len;
				break;
			}
		}	
		if (keyLen <= 0) keyLen = 50;
		keyNode = keysNode->getNextChild("key");
	}

	// 2. value field
	OmnString mapIILName = mapNode->getAttrStr("zky_iilname");
	OmnString isNeedSplit = "true";
	u32 valueLen = 0;
	AosXmlTagPtr valueNode = valuesNode->getFirstChild("value");
	if  (valueNode)
	{
		valueLen = valueNode->getAttrU32("max_len", 0);
		aos_assert_r(valueLen > 0, false);
		OmnString agr_type = valueNode->getAttrStr("agrtype");  
		if (agr_type != "")
			isNeedSplit = "false";
		else
			agr_type = "invalid";
	}

	OmnString mapValueNodeText = valueNode->getNodeText();

	if (mapValueNodeText == "zky_docid" || mapValueNodeText == "`zky_docid`")
	{
		isNeedSplit = "false";
	}

	boost::shared_ptr<DataField> valueDataFieldObj = boost::make_shared<DataField>();
	//valueDataFieldObj->setAttribute("type", "iilmap");
	valueDataFieldObj->setAttribute("type", "iilentry");
	valueDataFieldObj->setAttribute("datatype", datatype);

	OmnString dftvalue = vfconf->getAttrStr(AOSTAG_VALUE_DFT_VALUE, "");
	if(dftvalue != "")
	{
		valueDataFieldObj->setAttribute(AOSTAG_VALUE_DFT_VALUE, dftvalue);
	}

	valueDataFieldObj->setAttribute("zky_name", vfname);
	valueDataFieldObj->setAttribute("zky_iilname", mapIILName);
	valueDataFieldObj->setAttribute("zky_value_from_field", keyName);
	valueDataFieldObj->setAttribute("zky_ignore_serialize", "true");
	valueDataFieldObj->setAttribute("zky_needswap", "false");
	valueDataFieldObj->setAttribute("zky_needsplit", isNeedSplit);

	keyDataFieldObj->setAttribute("zky_sep", "0x01");
	valueDataFieldObj->setAttribute("zky_sep", "0x02");

	inputFields.push_back(keyDataFieldObj);
	inputFields.push_back(valueDataFieldObj);


	// ice, 2014/09/05
	OmnString type = vfconf->getAttrStr("datatype", "");
	OmnString nfname = AosDataType::getFieldName(vfname, type);                                    
	if (type == "u64")
	{
		boost::shared_ptr<DataField> docidFieldObj = boost::make_shared<DataField>();
		docidFieldObj->setAttribute("zky_name", nfname);
		docidFieldObj->setAttribute("type", "u64");
		docidFieldObj->setAttribute("zky_value_from_field", vfname);
		inputFields.push_back(docidFieldObj);
	}

	//Young........................
	// virtual field 
	boost::shared_ptr<DataField> dfObj = boost::make_shared<DataFieldStr>();
	dfObj->setAttribute("type", "str");
	dfObj->setAttribute("zky_length", keyLen);
	dfObj->setAttribute("zky_name", vfname);
	dfObj->setAttribute("zky_otype", "datafield");
	outputFields.push_back(dfObj);
	return true;
}


