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
// 09/11/2015 Created by Barry
////////////////////////////////////////////////////////////////////////////

#include "AosConf/DataRecordCSV.h"

#include "AosConf/DataFieldStr.h"
#include <boost/make_shared.hpp> 
using AosConf::DataRecordCSV;


string
AosConf::DataRecordCSV::getConfig() 
{
	// cal the offset

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


void 
AosConf::DataRecordCSV::setField(const string &name, const string &type)
{
	boost::shared_ptr<DataField> df = boost::make_shared<DataField>();
	df->setAttribute("zky_name", name);
	df->setAttribute("type", type);
	df->setAttribute("zky_datatooshortplc", "cstr");
	//df->setAttribute("zky_length", length);
	mFields.push_back(df);
}

