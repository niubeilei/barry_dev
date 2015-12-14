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

#include "AosConf/DataSplit.h"
using AosConf::DataSplit;

DataSplit::DataSplit(const AosXmlTagPtr &xml)
{
	aos_assert(xml);
	vector<pair<OmnString, OmnString> > v = xml->getAttributes();
	for (size_t i = 0; i < v.size(); i++){
		setAttribute(v[i].first, v[i].second);
	}
}

string
DataSplit::getConfig() {
	string config = "<split ";
	for (map<string, string>::iterator itr=mAttrs.begin();
			itr!=mAttrs.end(); itr++)
	{
		config += " " + itr->first + "=\"" + itr->second + "\"";
	}
	config += "/>";
	return config;
}


