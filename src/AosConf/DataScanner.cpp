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

#include "AosConf/DataScanner.h"
#include "AosConf/DataConnectorStatQuery.h"

#include <boost/make_shared.hpp>
using AosConf::DataScanner;

DataScanner::DataScanner(const AosXmlTagPtr &xml)
{
	aos_assert(xml);
	vector<pair<OmnString, OmnString> > v = xml->getAttributes();
	for (size_t i = 0; i < v.size(); i++)
	{
		setAttribute(v[i].first, v[i].second);
	}

	AosXmlTagPtr dataconnector = xml->getFirstChild("dataconnector");
	aos_assert(dataconnector);

	OmnString jimo_objid = dataconnector->getAttrStr("jimo_objid", "");
	aos_assert(jimo_objid != "");

	if (jimo_objid == "dataconnector_readdoc_bysort_jimodoc_v0")
	{
		mConnector = boost::make_shared<DataConnectorStatQuery>(dataconnector);
	}
	else
	{
		mConnector = boost::make_shared<DataConnector>(dataconnector);
	}
}

void
DataScanner::setConnector(boost::shared_ptr<DataConnector> con) {
		mConnector = con;	
}

void
DataScanner::setConnectorName(const string &name) {
	mConnNames.push_back(name);
}

string
DataScanner::getConfig() {
	string config = "<datascanner ";
	for (map<string, string>::iterator itr = mAttrs.begin();
			itr != mAttrs.end(); itr++)
	{
		config += " " + itr->first + "=\"" + itr->second + "\"";
	}
	config += ">";

	if (mConnector)
		config += mConnector->getConfig();

	for (u32 i = 0; i < mConnNames.size(); i++) {
		config += "<dataconnector zky_dataconnector_name=\"" + mConnNames[i] + "\"/>";
	}

	config += "</datascanner>";
	return config;
}



