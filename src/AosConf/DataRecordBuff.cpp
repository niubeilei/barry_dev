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

#include "AosConf/DataRecordBuff.h"

#include "AosConf/DataFieldStr.h"
#include <boost/make_shared.hpp> 
using AosConf::DataRecordBuff;

AosConf::DataRecordBuff::DataRecordBuff()
{
	mAttrs["type"] = "buff";
}


AosConf::DataRecordBuff::DataRecordBuff(const AosXmlTagPtr &xml)
:DataRecord(xml)
{
	mAttrs["type"] = "buff";
}


string
AosConf::DataRecordBuff::getConfig() 
{
	string config = "<datarecord ";
	for (map<string, string>::iterator itr=mAttrs.begin();
			itr!=mAttrs.end(); itr++)
	{
		config += " " + itr->first + "=\"" + itr->second + "\"";
	}
	config += "><datafields>";

	for (size_t i=0; i<mFields.size(); i++)
	{
		config += mFields[i]->getConfig();
	}
	config += "</datafields></datarecord>";
	return config;
}


vector<boost::shared_ptr<AosConf::DataField> >& 
AosConf::DataRecordBuff::getFields()
{
	return mFields;
}


void 
AosConf::DataRecordBuff::setField(boost::shared_ptr<AosConf::DataField> &df)
{
	mFields.push_back(df);	
}


void 
AosConf::DataRecordBuff::setField(
		const string &name, 
		const string &type, 
		const int length)
{
	boost::shared_ptr<DataField> df = boost::make_shared<DataField>();
	df->setAttribute("zky_name", name);
	df->setAttribute("type", type);
	mFields.push_back(df);
}
