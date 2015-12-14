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

#include "AosConf/DataFieldExpr.h"
using AosConf::DataFieldExpr;

DataFieldExpr::DataFieldExpr(const AosXmlTagPtr &xml)
:DataField(xml)
{
	aos_assert(xml);
	AosXmlTagPtr expr = xml->getFirstChild("expr");
	if (expr)
	{
		//mExpr = expr->toString();
		mExpr = expr->getNodeText();
	}
	//mAttrs["type"] = "expr";
}


DataFieldExpr::DataFieldExpr()
{
	mAttrs["type"] = "expr";
	mAttrs["zky_otype"] = "datafield";
}


DataFieldExpr::~DataFieldExpr()
{
}


void
DataFieldExpr::setExpr(const string &str)
{
	mExpr = str;
}


string
DataFieldExpr::getConfig() 
{
	string conf = "<datafield ";
	for (map<string, string>::iterator itr=mAttrs.begin();
			itr!=mAttrs.end(); itr++)
	{
		conf += " " + itr->first + "=\"" + itr->second + "\"";
	}
	conf += ">";
	
	if (mExpr != "") 
	{
		conf += "<expr><![CDATA[" + mExpr + "]]></expr>";
	}

	conf += "</datafield>";
	return conf;
}


