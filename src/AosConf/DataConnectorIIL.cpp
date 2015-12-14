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

#include "AosConf/DataConnectorIIL.h"
#include <boost/make_shared.hpp>
using AosConf::DataConnectorIIL;

DataConnectorIIL::DataConnectorIIL(const AosXmlTagPtr &xml)
:
mNeedQueryCond(false)
{
	aos_assert(xml);
	vector<pair<OmnString, OmnString> > v = xml->getAttributes();
	for(size_t i = 0; i < v.size(); i++)
	{
		setAttribute(v[i].first, v[i].second);
	}

	AosXmlTagPtr split = xml->getFirstChild("split");
	if (split) mSp = boost::make_shared<DataSplit>(split);
}

void
DataConnectorIIL::setSplit(boost::shared_ptr<DataSplit> split) 
{
	mSp = split;	
}
	
void 
DataConnectorIIL::setQueryCond(const string &opr, const string &value)
{
	mNeedQueryCond = true;
	mOpr = opr;
	mValue = value;
}

string
DataConnectorIIL::getConfig() 
{
	string config = "<dataconnector ";
	for (map<string, string>::iterator itr=mAttrs.begin();
			itr != mAttrs.end(); itr++)
	{
		config += " " + itr->first + "=\"" + itr->second + "\"";
	}
	config += ">";
	if (mSp) config += mSp->getConfig();
	if (mNeedQueryCond)
	{
		config += "<query_cond zky_opr=\"" + mOpr + "\"><zky_value><![CDATA[" + mValue + "]]></zky_value></query_cond>";
	}
	config += "</dataconnector>";
	return config;
}


