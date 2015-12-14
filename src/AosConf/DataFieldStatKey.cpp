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

#include "AosConf/DataFieldStatKey.h"
using AosConf::DataFieldStatKey;

DataFieldStatKey::DataFieldStatKey(const AosXmlTagPtr &xml)
:DataField(xml)
{
	mAttrs["type"] = "statkey";
}


DataFieldStatKey::DataFieldStatKey()
{
}


DataFieldStatKey::~DataFieldStatKey()
{
}


void 
DataFieldStatKey::setDataField(const boost::shared_ptr<DataField> &field)
{
	mDataFields.push_back(field);
}

string
DataFieldStatKey::getConfig() 
{
	string conf = "<datafield ";
	for (map<string, string>::iterator itr=mAttrs.begin();
			itr!=mAttrs.end(); itr++)
	{
		conf += " " + itr->first + "=\"" + itr->second + "\"";
	}
	conf += ">";
	for(size_t i=0; i<mDataFields.size(); i++)
	{
		conf += mDataFields[i]->getConfig();
	}
	conf += "</datafield>";
	return conf;
}


