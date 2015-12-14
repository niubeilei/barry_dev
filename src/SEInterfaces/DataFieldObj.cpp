////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 07/17/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/DataFieldObj.h"

#include "SEInterfaces/BuffData.h"


AosDataFieldObjPtr AosDataFieldObj::smCreator;


AosDataFieldObj::AosDataFieldObj()
:
AosJimo(AosJimoType::eDataField, 0),
mIsNull(false)
{
}

AosDataFieldObj::AosDataFieldObj(const int version)
:
AosJimo(AosJimoType::eDataField, version),
mIsNull(false)
{
}


AosDataFieldObjPtr 
AosDataFieldObj::createStrFieldStatic(AosRundata *rdata)
{
	aos_assert_rr(smCreator, rdata, NULL);
	return smCreator->createStrField(rdata);
}
	

AosDataFieldObjPtr 
AosDataFieldObj::createDataFieldStatic(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	aos_assert_rr(smCreator, rdata, NULL);
	return smCreator->createDataField(def, record, rdata);
}

	
bool
AosDataFieldObj::isVirtualField(const AosXmlTagPtr &def)
{
	if (!def) return false;
	OmnString type = def->getAttrStr(AOSTAG_TYPE);
	AosDataFieldType::E t = AosDataFieldType::toEnum(type);
	if (t != AosDataFieldType::eStr)
	{
		return true;
	}
	return false;
}


vector<AosXmlTagPtr>
AosDataFieldObj::getVirtualField(
				const AosXmlTagPtr &tableDoc,
				const AosRundataPtr &rdata)
{
	vector<AosXmlTagPtr> fieldConfV;
	aos_assert_r(tableDoc, fieldConfV);
	// 1. create map case (virtual field)
	AosXmlTagPtr mapsNode = tableDoc->getFirstChild("maps");
	if (!mapsNode)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return fieldConfV;
	} 
	AosXmlTagPtr mapNode = mapsNode->getFirstChild("map");
	while (mapNode) 
	{
		vector<AosXmlTagPtr> confV = getMapDatafieldConf(rdata, tableDoc, mapNode);
		fieldConfV.insert(fieldConfV.begin(), confV.begin(), confV.end());
		mapNode = mapsNode->getNextChild("map");
	}

	// 2. create index case (when use virtual field)
	AosXmlTagPtr columnsNode = tableDoc->getFirstChild("columns");
	if (!columnsNode)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return fieldConfV;
	}
	AosXmlTagPtr columnNode = columnsNode->getFirstChild("column");
	while (columnNode)
	{
		OmnString type = columnNode->getAttrStr("type");
		if (type == "virtual")
		{
			vector<AosXmlTagPtr> confV = getVirtualFieldConf(rdata, tableDoc, columnNode);
			fieldConfV.insert(fieldConfV.begin(), confV.begin(), confV.end());
		}
		columnNode = columnsNode->getNextChild("column");
	}
	return fieldConfV;
}


vector<AosXmlTagPtr> 
AosDataFieldObj::getVirtualFieldConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &tableDoc,
		const AosXmlTagPtr &def)
{
	// def's format just like this:
	// <column>
	// 	<keys>
	// 		<key>...</key>
	// 		<key>...</key>
	// 		...
	// 	</keys>
	// 	<map>
	// 		<keys>
	// 	 		<key>...</key>
	// 			<key>...</key>
	// 			...
	// 		</keys>
	// 		<values>
	// 			<value></value>
	// 		</values>
	// 	</map>
	// </column>
	
	vector<AosXmlTagPtr> fieldConfV;
	AosXmlTagPtr keysNode = def->getFirstChild("keys");
	AosXmlTagPtr mapNode = def->getFirstChild("map");
	if (!mapNode)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return fieldConfV;
	}
	AosXmlTagPtr valuesNode = mapNode->getFirstChild("values");
	if (!keysNode && valuesNode)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return fieldConfV;
	}

	// 1. constants key datafield config
	//    get key's length, key's name
	u32 keyLen = 0;
	OmnString keyName = "cps";
	OmnString keyfieldsConf = "";
	AosXmlTagPtr keyNode = keysNode->getFirstChild("key");
	while (keyNode)
	{
		// tmpname nust be a fieldname
		OmnString fieldname = keyNode->getNodeText();
		keyName << fieldname;
		keyfieldsConf << "<field>" << fieldname << "</field>";

		//Jozhi
		//AosXmlTagPtr tmpconf= getDoc(JQLTypes::eDataFieldDoc, fieldname);
		//if (!tmpconf)
		//{
		//	AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		//	OmnAlarm << __func__ << enderr;
		//	return fieldConfV;
		//}
		//keyLen += tmpconf->getAttrU32("zky_length", 0);
		AosXmlTagPtr col_tags = tableDoc->getFirstChild("columns");
		aos_assert_r(col_tags, fieldConfV);
		AosXmlTagPtr col_tag = col_tags->getFirstChild(true);
		bool found = false;
		while(col_tag)
		{
			if (fieldname == col_tag->getAttrStr("name"))
			{
				found = true;
				break;
			}
			col_tag = col_tags->getNextChild();
		}
		aos_assert_r(col_tag && found, fieldConfV);
		keyLen += col_tag->getAttrU32("size", 0);

		keyNode = keysNode->getNextChild("key");
	}


	// 2. constants value datafield config
	// 	  get value's name(iilname), value's length(max length)
	OmnString valueName = mapNode->getAttrStr("zky_iilname");
	OmnString need_split = "true";
	u32 valueLen = 0;
	AosXmlTagPtr valueNode = valuesNode->getFirstChild("value");
	if  (valueNode)
	{
		valueLen = valueNode->getAttrU32("max_len", 0);
		if (!valueLen > 0)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return fieldConfV;
		}
		OmnString agr_type = valueNode->getAttrStr("agrtype");	
		if (agr_type != "")
		{
			need_split = "false";
		}
		else
		{
			agr_type = "invalid";
		}
		//Jozhi
		//mVirtualFieldOprType.push_back(agr_type);
	}


	// 3. push key datafield and value datafield into fieldConfV
	OmnString keyFieldStr, valueFieldStr;
	keyFieldStr << "<datafield type=\"math\" zky_sep=\"0x01\" zky_opr=\"concat\" zky_name=\"" << keyName << "\">" << keyfieldsConf << "</datafield>";

	OmnString virtualfield_name = def->getAttrStr("name");
	valueFieldStr << "<datafield type=\"iilmap\" zky_name=\"" << virtualfield_name << "\" zky_iilname=\"" << valueName << "\" zky_value_from_field=\"" << keyName << "\" zky_ignore_serialize=\"true\" zky_needswap=\"false\" zky_needsplit=\"" << need_split << "\" zky_sep=\"0x02\"></datafield>";
	OmnString tmpstr = "";
	tmpstr << keyName << "_" << valueName;
	OmnString mapName = mapNode->getAttrStr("zky_name");
	//Jozhi
	//mVirtualFieldNames.push_back(make_pair(tmpstr, mapName));
	//mVirtualFieldIONames.push_back(make_pair(keyName, valueName));
	//mVirtualFieldIOLens.push_back(make_pair(keyLen, valueLen));

	AosXmlTagPtr keyFieldConf = AosXmlParser::parse(keyFieldStr AosMemoryCheckerArgs);
	AosXmlTagPtr valueFieldConf = AosXmlParser::parse(valueFieldStr AosMemoryCheckerArgs);
	if (!keyFieldConf && valueFieldConf)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return fieldConfV;
	}
	fieldConfV.push_back(keyFieldConf);
	fieldConfV.push_back(valueFieldConf);

	return fieldConfV;
}


vector<AosXmlTagPtr> 
AosDataFieldObj::getMapDatafieldConf(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &tableDoc,
		const AosXmlTagPtr &mapNode)
{
	// This function will constans two datafield config with map config, 
	// one is key datafieldConf, and another is value datafieldConf.
	// The following processes one map tag. It assumes the following:
	// 	<map zky_map_name="xxx">
	// 		<keys .../>
	// 			<key ...>field_name</key>
	// 			...
	// 			<key .../>
	// 		</keys>
	// 		<values .../>
	// 			<value .../>		There should be only one value subtag
	// 		</values> 
	// 	</map>
	
	int field_seqno = 0;
	vector<AosXmlTagPtr> fieldConfV;

	AosXmlTagPtr keysNode = mapNode->getFirstChild("keys");
	AosXmlTagPtr valuesNode = mapNode->getFirstChild("values");
	if (!keysNode && valuesNode)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return fieldConfV;
	}
	//OmnString map_name = mapNode->getAttrStr("zky_map_name");

	u32 keylen, valuelen;
	keylen = valuelen = 0;
	OmnString keyFieldsConf, valueFieldsConf, keyName;
	keyName = "cps";
	AosXmlTagPtr keyNode = keysNode->getFirstChild("key");

	// Chen Ding, 2014/07/31
	bool isEmptyKey = false;	
	if (!keyNode)
	{
		// This is the case:
		// CREATE MAP map-name ON tablename
		// KEYS ()
		// ...
		isEmptyKey = true;
		keyName = "__zkyatvf_";
		//Jozhi
		//keyName << mFieldSeqno++;
		keyName << field_seqno++;
		keylen = 15; 
	}
	else
	{
		while (keyNode)
		{
			// 1. For each key field, it will create the following config:
			// 	  <field>field_name</field>
			// 2. Retrieve the field's length, and add it to keylen
			// 3. Construct 'keyName' by concatenating all key field names
			//    together. This will be used as the name of the virtual field.
			OmnString field_name = keyNode->getNodeText();
			if (field_name == "")
			{
				AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
				OmnAlarm << __func__ << enderr;
				return fieldConfV;
			}

			keyFieldsConf << "<field>" << field_name << "</field>";
			keyName << "_" << field_name;

			//Jozhi
			//AosXmlTagPtr tmpFieldConf = getDoc(JQLTypes::eDataFieldDoc, field_name);
			//if (!tmpFieldConf)
			//{
			//	AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			//	OmnAlarm << __func__ << enderr;
			//	return fieldConfV;
			//}
			//keylen += tmpFieldConf->getAttrU32("zky_length", 0);
			AosXmlTagPtr col_tags = tableDoc->getFirstChild("columns");
			aos_assert_r(col_tags, fieldConfV);
			AosXmlTagPtr col_tag = col_tags->getFirstChild(true);
			bool found = false;
			while(col_tag)
			{
				if (field_name == col_tag->getAttrStr("name"))
				{
					found = true;
					break;
				}
				col_tag = col_tags->getNextChild();
			}
			aos_assert_r(col_tag && found, fieldConfV);
			keylen += col_tag->getAttrU32("size", 0);


			keyNode = keysNode->getNextChild("key");
		}
		//Jozhi
		//keyName << "_" << mFieldSeqno++;
		keyName << "_" << field_seqno++;
	}

	OmnString valueType;
	AosXmlTagPtr valueNode = valuesNode->getFirstChild("value");
	if (valueNode)
	{
		// It assumes 'valuesNode' in the following format:
		// 	<values ...>
		// 		<value agrtype="max" type="expr" ...>
		// 			<expr>xxx</expr>
		// 		</value>
		// 	</values>
		// Young: 2014/07/29
		OmnString type = valueNode->getAttrStr("agrtype");	

		// Chen Ding, 2014/07/31
		if (isEmptyKey)
		{
			if (type != "sum" || type != "max" || type != "min" || type != "count")
			{
				OmnAlarm << "Empty key must use aggregation functions: "
					<< type << enderr;
				return fieldConfV;
			}
		}

		if (type == "") type = "invalid";
		//Jozhi
		//mVirtualFieldOprType.push_back(type);
		valueType = valueNode->getAttrStr("type");

		OmnString node_text = valueNode->getNodeText();
		OmnString data_type = valueNode->getAttrStr("datatype");
		if (data_type == "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return fieldConfV;
		}
		if (valueType == "expr")
		{
			// This is an expression. It needs to:
			// 1. The expression should go to the corresponding data proc.
			// 2. Need to determine field length.
			valueFieldsConf << "<expr datatype=\"" << data_type << "\">" << node_text << "</expr>";
		}
		else
		{
			// This is a single field. It retrieves the field's definition.
			// From the field definition, it can retrieve the field length.
			valueFieldsConf << "<field>" << node_text << "</field>";
		}
	}

	OmnString data_type = valueNode->getAttrStr("datatype");
	//Jozhi
	//valuelen = getValueLength(data_type);
	AosDataType::E dt = AosDataType::toEnum(data_type);
	valuelen = AosDataType::getValueSize(dt);

	if (valuelen <= 0) valuelen = 20;

	// Construct the key field config
	OmnString keyFieldStr, valueFieldStr;

	// Chen Ding, 2014/07/31
	if (isEmptyKey)
	{
		keyFieldStr << "<datafield type=\"str\" "
			<< AOSTAG_ISCONST << "=\"true\" "
			<< AOSTAG_VALUE << "=\"__null_entry__\" "
			<< "zky_name=\"" << keyName << "\"/>";
	}
	else
	{
		keyFieldStr << "<datafield type=\"math\" zky_sep=\"0x01\" "
			<< "zky_opr=\"concat\" "
			<< "zky_name=\"" << keyName << "\">" 
			<< keyFieldsConf << "</datafield>";
	}

	//Jozhi
	//OmnString valueFieldName = getNextValueFieldName();
	OmnString valueFieldName = "__zkyatvf_";
	valueFieldName << field_seqno++;

	if (valueType == "expr")
	{
		valueFieldStr << "<datafield "
			<< "type=\"expr\" "
			<< "zky_otype=\"datafield\" "
			<< "zky_name=\"" << valueFieldName << "\">" 
			<< valueFieldsConf << "</datafield>";
	}
	else
	{
		// It is just a single field, no expressions.
		valueFieldStr << "<datafield type=\"math\" zky_sep=\"0x01\" "
			<< "zky_opr=\"concat\" "
			<< "zky_name=\"" << valueFieldName << "\">" 
			<< valueFieldsConf << "</datafield>";
	}

	OmnString tmpstr = "";
	tmpstr << keyName << "_" << valueFieldName;
	OmnString mapname = mapNode->getAttrStr("zky_name");
	//mVirtualFieldNames.push_back(make_pair(tmpstr, mapname));
	//mVirtualFieldIONames.push_back(make_pair(keyName, valueFieldName));
	//mVirtualFieldIOLens.push_back(make_pair(keylen, valuelen));

	AosXmlTagPtr keyFieldConf = AosXmlParser::parse(keyFieldStr AosMemoryCheckerArgs);
	AosXmlTagPtr valueFieldConf = AosXmlParser::parse(valueFieldStr AosMemoryCheckerArgs);
	if (!keyFieldConf && valueFieldConf)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return fieldConfV;
	}
	fieldConfV.push_back(keyFieldConf);
	fieldConfV.push_back(valueFieldConf);

	return fieldConfV;
}


