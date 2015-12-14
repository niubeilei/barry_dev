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

#include "AosConf/DataSchema.h"

#include "AosConf/DataRecordFixbin.h"
#include "AosConf/DataRecordMulti.h"

#include <boost/make_shared.hpp>
using AosConf::DataSchema;

DataSchema::DataSchema(const AosXmlTagPtr &xml)
{
	aos_assert(xml);
	vector<pair<OmnString ,OmnString> > v = xml->getAttributes();
	for (size_t i=0; i<v.size(); i++) {
		setAttribute(v[i].first, v[i].second);
	}

	AosXmlTagPtr datarecord = xml->getFirstChild("datarecord");
	if (datarecord) 
	{
		if (datarecord->getAttrStr("type") == "fixbin")
			mRecord = boost::make_shared<DataRecordFixbin>(datarecord);
		else if (datarecord->getAttrStr("type") == "multi")
			mRecord = boost::make_shared<DataRecordMulti>(datarecord);
		else
			mRecord = boost::make_shared<DataRecord>(datarecord);
	}
}

	
string
DataSchema::getConfig() {
	string config = "<dataschema ";
	for (map<string, string>::iterator itr=mAttrs.begin();
			itr!=mAttrs.end(); itr++)
	{
		config += " " + itr->first + "=\"" + itr->second + "\"";
	}
	config += ">";

	if (mRecordConf != "")
		config += mRecordConf;
	if (mRecord)
		config += mRecord->getConfig();

	for (u32 i = 0; i < mRecNames.size(); i++){
		config += "<record zky_datarecord_name=\"" + mRecNames[i] + "\"/>";
	}
	
	config += "</dataschema>";
	return config;
}


