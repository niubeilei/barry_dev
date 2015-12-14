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

#include "AosConf/DataFieldStr.h"
using AosConf::DataFieldStr;

DataFieldStr::DataFieldStr(const AosXmlTagPtr &xml)
{
	constFlag = false;
	aos_assert(xml);
	vector<pair<OmnString ,OmnString> > v = xml->getAttributes();
	for (size_t i=0; i<v.size(); i++) {
		setAttribute(v[i].first, v[i].second);
	}

	AosXmlTagPtr expr = xml->getFirstChild("expr");
	mExpr = "";
	if (expr)
	{
		mExpr = expr->toString();
	}
}


DataFieldStr::DataFieldStr()
{
	constFlag = false;
}


DataFieldStr::~DataFieldStr()
{
}


bool 
DataFieldStr::setConst(const string &str)
{
	constFlag = true;
	mConstStr = str;
	return constFlag;
}
		
void 
DataFieldStr::setDataField(boost::shared_ptr<DataFieldStr> field)
{
	mDataFields.push_back(field);
}

string
DataFieldStr::getConfig() 
{
	string conf = "<datafield ";
	for (map<string, string>::iterator itr=mAttrs.begin();
			itr!=mAttrs.end(); itr++)
	{
		conf += " " + itr->first + "=\"" + itr->second + "\"";
	}
	conf += ">";
	if (constFlag) {
		conf += "<zky_const>"+ mConstStr +"</zky_const>";
	}
	conf += mExpr;
	for(size_t i=0; i<mDataFields.size(); i++)
	{
		conf += mDataFields[i]->getConfig();
	}
	conf += "</datafield>";
	return conf;
}


