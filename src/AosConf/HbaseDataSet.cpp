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

#include "AosConf/HbaseDataSet.h"
//#include "AosConf/DataSchemaInternalStat.h"

#include <boost/make_shared.hpp> 
using AosConf::HbaseDataSet;

HbaseDataSet::HbaseDataSet(const AosXmlTagPtr &xml)
{
	aos_assert(xml);
	vector<pair<OmnString ,OmnString> > v = xml->getAttributes();
	for (size_t i=0; i<v.size(); i++) {
		setAttribute(v[i].first, v[i].second);
	}

	AosXmlTagPtr datascanner = xml->getFirstChild("datascanner");
	mScanner = 	boost::make_shared<DataScanner>(datascanner);
	AosXmlTagPtr dataschema = xml->getFirstChild("dataschema");
	aos_assert(dataschema);
	OmnString jimo_objid = dataschema->getAttrStr("jimo_objid", "");
	aos_assert(jimo_objid != "");
	if (jimo_objid == "dataschema_internal_stat_jimodoc_v0")
	{
		// Ketty 2014/09/25
		OmnNotImplementedYet;
		//mSchema = boost::make_shared<DataSchemaInternalStat>(dataschema);
	}
	else
	{
		mSchema = boost::make_shared<DataSchema>(dataschema);
	}
}

void
HbaseDataSet::setScanner(boost::shared_ptr<DataScanner> scanner) {
	mScanner = scanner;
}

void
HbaseDataSet::setSchema(boost::shared_ptr<DataSchema> schema) {
	mSchema = schema;
}

void 
HbaseDataSet::setDataSet(boost::shared_ptr<DataSet> dataset)
{
	mDataSet = dataset;
}

string
HbaseDataSet::getConfig() {
	if (mTagName == "" ) mTagName = "dataset";
	string conf = "<" + mTagName ;
	for (map<string, string>::iterator itr = mAttrs.begin();
			itr != mAttrs.end(); itr++)
	{
		conf += " " + itr->first + "=\"" + itr->second + "\"";
	}
	conf += ">";
	if (mScanner) conf += mScanner->getConfig();
	if (mSchema) conf += mSchema->getConfig();
	if (mDataSet) conf += mDataSet->getConfig();
	conf += "</" + mTagName + ">";
	return conf;
}

