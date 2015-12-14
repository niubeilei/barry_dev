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

#include "AosConf/DataFieldMath.h"
using AosConf::DataFieldMath;

DataFieldMath::DataFieldMath(const AosXmlTagPtr &xml)
:DataField(xml)
{
	aos_assert(xml);

	AosXmlTagPtr fieldNode = xml->getFirstChild("field");
	while (fieldNode)
	{
		string fieldStr = fieldNode->getNodeText();
		mFieldsStr.push_back(fieldStr);
		fieldNode = xml->getNextChild("field");
	}
	mAttrs["type"] = "math";
}


DataFieldMath::DataFieldMath()
{
	mAttrs["type"] = "math";
	mAttrs["zky_otype"] = "datafield";
}


DataFieldMath::~DataFieldMath()
{
}


void
DataFieldMath::setFieldName(const string &str)
{
	mFieldsStr.push_back(str);
}

string
DataFieldMath::getConfig() 
{
	string conf = "<datafield ";
	for (map<string, string>::iterator itr=mAttrs.begin();
			itr!=mAttrs.end(); itr++)
	{
		conf += " " + itr->first + "=\"" + itr->second + "\"";
	}
	conf += ">";

	for (size_t i=0; i<mFieldsStr.size(); i++)
	{
		conf += "<field><![CDATA["+ mFieldsStr[i] +"]]></field>";
	}

	conf += "</datafield>";
	return conf;
}


