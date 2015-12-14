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

#include "AosConf/DataConnector.h"
#include <boost/make_shared.hpp>
using AosConf::DataConnector;

DataConnector::DataConnector(const AosXmlTagPtr &xml)
{
	aos_assert(xml);
	vector<pair<OmnString, OmnString> > v = xml->getAttributes();
	for(size_t i = 0; i < v.size(); i++)
	{
		setAttribute(v[i].first, v[i].second);
	}
			
	AosXmlTagPtr file = xml->getFirstChild("file");
	if (file)
	{
		mFile = file->toString();
	}

	AosXmlTagPtr dirs = xml->getFirstChild("dirs");
	if (dirs)
	{
		mDirs = dirs->toString();
	}

	AosXmlTagPtr split = xml->getFirstChild("split");
	if (split)
	{
		mSp = boost::make_shared<DataSplit>(split);
	}
}

void
DataConnector::setSplit(boost::shared_ptr<DataSplit> split) 
{
	mSp = split;	
}


void
DataConnector::setConds(string conds) {
		mConds = conds;	
	}

string
DataConnector::getConfig() 
{
	string config = "<dataconnector ";
	for (map<string, string>::iterator itr=mAttrs.begin();
			itr != mAttrs.end(); itr++)
	{
		config += " " + itr->first + "=\"" + itr->second + "\"";
	}
	config += ">";
	if (mFile != "") config += mFile;
	if (mDirs != "") config += mDirs;
	if (mSp) config += mSp->getConfig();
	config += "</dataconnector>";
	return config;
}


