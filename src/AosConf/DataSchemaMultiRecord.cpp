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

#include "AosConf/DataSchemaMultiRecord.h"

#include "AosConf/DataRecordFixbin.h"
#include "AosConf/DataRecordMulti.h"

#include <boost/make_shared.hpp>
using AosConf::DataSchemaMultiRecord;

DataSchemaMultiRecord::DataSchemaMultiRecord(const AosXmlTagPtr &xml)
{
	aos_assert(xml);
	vector<pair<OmnString ,OmnString> > v = xml->getAttributes();
	for (size_t i=0; i<v.size(); i++) {
		setAttribute(v[i].first, v[i].second);
	}

	AosXmlTagPtr data_records = xml->getFirstChild(true);
	aos_assert(data_records);
	// by young
	mRecordNode = data_records->toString();
}


string
DataSchemaMultiRecord::getConfig() {
	string config = "<dataschema ";
	for (map<string, string>::iterator itr=mAttrs.begin();
			itr!=mAttrs.end(); itr++)
	{
		config += " " + itr->first + "=\"" + itr->second + "\"";
	}
	config += " >";
	config += mRecordNode;
	config +="</dataschema>";
	return config;
}


