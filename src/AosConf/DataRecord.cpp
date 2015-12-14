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

#include "AosConf/DataRecord.h"

#include "AosConf/DataFieldStr.h"
#include "AosConf/DataFieldMath.h"
#include "AosConf/DataFieldConst.h"
#include "AosConf/DataFieldExpr.h"
#include "AosConf/DataField.h"
#include <boost/make_shared.hpp> 
using AosConf::DataRecord;

AosConf::DataRecord::DataRecord(const AosXmlTagPtr &xml)
{
	setConfig(xml);
}

bool
AosConf::DataRecord::setConfig(const AosXmlTagPtr &xml)
{
	aos_assert_r(xml, false);
	vector<pair<OmnString ,OmnString> > v = xml->getAttributes();
	for (size_t i=0; i<v.size(); i++) {
		setAttribute(v[i].first, v[i].second);
	}

	AosXmlTagPtr datafields = xml->getFirstChild("datafields");
	AosXmlTagPtr datafield = datafields->getFirstChild();
	while(datafield) 
	{
		string field_type = datafield->getAttrStr("type");
		if (field_type == "str")
		{
			mFields.push_back(boost::make_shared<DataFieldStr>(datafield));
		}
		else if (field_type == "math")
		{
			mFields.push_back(boost::make_shared<DataFieldMath>(datafield));
		}
		else if (field_type == "const")
		{
			mFields.push_back(boost::make_shared<DataFieldConst>(datafield));
		}
		else if (field_type == "expr")
		{
			mFields.push_back(boost::make_shared<DataFieldExpr>(datafield));
		}
		else
		{
			mFields.push_back(boost::make_shared<DataField>(datafield));
		}
		datafield = datafields->getNextChild();
	}
	return true;
}

void
AosConf::DataRecord::setFields(vector<boost::shared_ptr<DataField> > &fields)
{
	mFields.insert(mFields.end(), fields.begin(), fields.end());
}

void
AosConf::DataRecord::setField(const boost::shared_ptr<DataField> &field) 
{
	mFields.push_back(field);
}

void
AosConf::DataRecord::setField(const string &fieldname)
{
	boost::shared_ptr<DataField> field = boost::make_shared<DataField>();
	field->setAttribute("zky_name", fieldname);
	setField(field);
}

string
AosConf::DataRecord::getConfig() 
{
	//string config = "<datarecord ";
	if (mTagName == "")
		mTagName = "datarecord";
	// Ketty 2014/10/30
	string config = "<" + mTagName + " ";
	for (map<string, string>::iterator itr=mAttrs.begin();
			itr!=mAttrs.end(); itr++)
	{
		config += " " + itr->first + "=\"" + itr->second + "\"";
	}
	config += "><datafields>";

	// Young, 2014/09/03 
	string docid_str = "";
	for (size_t i=0; i<mFields.size(); i++)
	{
		string type = mFields[i]->getAttribute("type");
		if (type == "docid") 
		{
			docid_str += mFields[i]->getConfig();
		}
		else
		{
			config += mFields[i]->getConfig();
		}
	}
	config += docid_str;

	// Ketty 2014/10/30
	//config += "</datafields></datarecord>";
	config += "</datafields></" + mTagName + ">";
	return config;
}
