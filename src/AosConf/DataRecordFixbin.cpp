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

#include "AosConf/DataRecordFixbin.h"

#include "AosConf/DataFieldStr.h"
#include <boost/make_shared.hpp> 
using AosConf::DataRecordFixbin;

AosConf::DataRecordFixbin::DataRecordFixbin(const AosXmlTagPtr &xml)
:DataRecord(xml)
{
	mAttrs["type"] = "fixbin";
}


string
AosConf::DataRecordFixbin::getConfig() 
{
	// cal the offset
	arrange();

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
AosConf::DataRecordFixbin::getFields()
{
	arrange();
	return mFields;
}

void
AosConf::DataRecordFixbin::arrange() 
{
	if (!mArrangeInit)
	{
		mArrangeInit = true;
	}
	else
	{
		return;
	}
	for (size_t i=0; i<mFields.size(); i++)
	{
		 OmnString name = mFields[i]->getAttribute("zky_name");
		 boost::shared_ptr<DataField> df;
		 if (name.toUpper() == "LF") {
			df = mFields[i];
			mFields.erase(mFields.begin()+i);
			mFields.push_back(df);
			break;
		 }
	}
	
	int totalLen = 0;
	int totalOffset = 0;
	for (size_t i=0; i<mFields.size(); i++)
	{
		string type = mFields[i]->getAttribute("type");
		if (type == "docid") continue;
		if (type == "expr") continue;

		int length = atoi(mFields[i]->getAttribute("zky_length").data());
		int offset = 0;
		if (mFields[i]->getAttribute("zky_offset") == "") 
		{
			mFields[i]->setAttribute("zky_offset", totalOffset);
			totalOffset += length;
			totalLen += length;
		}
		else
		{
			offset = atoi(mFields[i]->getAttribute("zky_offset").data());
			if (offset > totalOffset) totalOffset = offset + length;
		}

		if (totalLen < (length + offset)) totalLen = length + offset;
	}

	int rcd_len = atoi(mAttrs["zky_length"].data());
	if (rcd_len < totalLen)
		setAttribute("zky_length", totalLen);
}

void 
AosConf::DataRecordFixbin::setField(const string &name, const string &type, const int length)
{
	boost::shared_ptr<DataField> df = boost::make_shared<DataField>();
	df->setAttribute("zky_name", name);
	df->setAttribute("type", type);
	df->setAttribute("zky_datatooshortplc", "cstr");
	df->setAttribute("zky_length", length);
	mFields.push_back(df);
}

void
AosConf::DataRecordFixbin::setField(const boost::shared_ptr<DataField> &field) 
{
	DataRecord::setField(field);
}


