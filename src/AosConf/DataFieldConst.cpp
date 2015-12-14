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

#include "AosConf/DataFieldConst.h"
using AosConf::DataFieldConst;

DataFieldConst::DataFieldConst(const AosXmlTagPtr &xml)
:DataField(xml)
{
	aos_assert(xml);
	AosXmlTagPtr value_node = xml->getFirstChild("zky_const");
	if (value_node)
	{
		mConstStr = value_node->getNodeText(); 
	}
	mAttrs["type"] = "const";
}


DataFieldConst::DataFieldConst()
{
	mAttrs["type"] = "const";
}


DataFieldConst::~DataFieldConst()
{
}


void
DataFieldConst::setConst(const string &str)
{
	mConstStr = str;
}
		

string
DataFieldConst::getConfig() 
{
	string conf = "<datafield ";
	for (map<string, string>::iterator itr=mAttrs.begin();
			itr!=mAttrs.end(); itr++)
	{
		conf += " " + itr->first + "=\"" + itr->second + "\"";
	}
	conf += ">";
	conf += "<zky_const>"+ mConstStr +"</zky_const>";
	conf += "</datafield>";
	return conf;
}


