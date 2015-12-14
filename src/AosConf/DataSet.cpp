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

#include "AosConf/DataSet.h"

#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
//#include "AosConf/DataSchemaInternalStat.h"

#include <boost/make_shared.hpp> 
using AosConf::DataSet;
using AosConf::DataSchema;

DataSet::DataSet(const AosXmlTagPtr &xml)
{
	aos_assert(xml);
	vector<pair<OmnString ,OmnString> > v = xml->getAttributes();
	for (size_t i=0; i<v.size(); i++) {
		setAttribute(v[i].first, v[i].second);
	}

	AosXmlTagPtr datascanner = xml->getFirstChild("datascanner");
	if (datascanner) mScannerConf = datascanner->toString();

	AosXmlTagPtr dataschema = xml->getFirstChild("dataschema");
	if (dataschema) mSchemaConf = dataschema->toString();

}


boost::shared_ptr<DataSchema> 
DataSet::getSchema()
{
	AosXmlTagPtr schema = AosXmlParser::parse(mSchemaConf AosMemoryCheckerArgs);
	aos_assert_r(schema, boost::make_shared<DataSchema>());

	return boost::make_shared<DataSchema>(schema);
}


void
DataSet::setScanner(boost::shared_ptr<DataScanner> scanner) {
	mScannerConf = scanner->getConfig();
}

void
DataSet::setSchema(boost::shared_ptr<DataSchema> schema) {
	mSchemaConf = schema->getConfig();
}

void
DataSet::setScanName(const string &name){
	mScanNames.push_back(name);
}
void
DataSet::setSchemaName(const string &name){
	mSchemaNames.push_back(name);
}

string
DataSet::getConfig() {
	if (mTagName == "" ) mTagName = "dataset";
	string conf = "<" + mTagName ;
	for (map<string, string>::iterator itr = mAttrs.begin();
			itr != mAttrs.end(); itr++)
	{
		conf += " " + itr->first + "=\"" + itr->second + "\"";
	}
	conf += ">";
	conf += mScannerConf;
	conf += mSchemaConf;

	for (u32 i = 0; i < mScanNames.size(); i++) {
		conf += "<datascanner zky_datascanner_name=\"" + mScanNames[i] + "\"/>";
	}

	for (u32 i = 0; i < mSchemaNames.size(); i++) {
		conf += "<dataschema zky_dataschema_name=\"" + mSchemaNames[i] + "\"/>";
	}

	conf += "</" + mTagName + ">";
	return conf;
}

boost::shared_ptr<DataSet> 
DataSet::clone(){
	OmnString s = getConfig();
	AosXmlParser parser;
	AosXmlTagPtr config = parser.parse(s , "" AosMemoryCheckerArgs);
	return boost::make_shared<DataSet>(config);
}
