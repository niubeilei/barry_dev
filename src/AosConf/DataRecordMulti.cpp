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

#include "AosConf/DataRecordMulti.h"

#include "AosConf/DataRecordFixbin.h"

#include <boost/make_shared.hpp>
using AosConf::DataRecordMulti;


DataRecordMulti::DataRecordMulti(const AosXmlTagPtr &def)
{
	aos_assert(def);
	vector<pair<OmnString ,OmnString> > v = def->getAttributes();
	for (size_t i=0; i<v.size(); i++)
	{
		setAttribute(v[i].first, v[i].second);
	}

	// 1. datafields 
	AosXmlTagPtr datafields = def->getFirstChild("datafields");
	aos_assert(datafields);
	AosXmlTagPtr fieldNode = datafields->getFirstChild("datafiled");
	while (fieldNode)
	{
		setDataField(fieldNode->toString());
		fieldNode = datafields->getNextChild("datafield");
	}

	// 2. datarecords
	AosXmlTagPtr datarecords = def->getFirstChild("datarecords");
	aos_assert(datarecords);
	AosXmlTagPtr recordPick = datarecords->getFirstChild("datarecord");
	while (recordPick)
	{
		appendRecordPick(recordPick->toString());
		recordPick = datarecords->getNextChild("datarecord");
	}


	// 3. picker 
	AosXmlTagPtr picker = def->getFirstChild("picker");
	aos_assert(picker);
	AosXmlTagPtr match_expr = picker->getFirstChild("match_expr");
	aos_assert(match_expr);
	string exprstr = match_expr->getNodeText();
	setSchemaPicker(exprstr);
}


string
DataRecordMulti::getConfig() 
{
	string config = "<datarecord ";
	for (map<string, string>::iterator itr=mAttrs.begin();
			itr!=mAttrs.end(); itr++)
	{
		config += " " + itr->first + "=\"" + itr->second + "\"";
	}
	config += ">";

	// 1. datafields
	config += "<datafields>";
	for (size_t i=0; i<mDataFields.size(); i++)
	{
		config += mDataFields[i];
	}
	config += "</datafields>";

	// 2. datarecords
	config += "<datarecords>";
	for (u32 i = 0; i < mRecordPicks.size(); i++)
	{
		config += mRecordPicks[i];
	}
	config += "</datarecords>";

	// 3. schema picker
	config += mSchemaPicker;

	config +="</datarecord>";
	return config;
}


