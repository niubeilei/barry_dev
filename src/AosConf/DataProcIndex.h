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
#ifndef Aos_AosConf_DataProcIndex_h
#define Aos_AosConf_DataProcIndex_h

#include "AosConf/DataProc.h"
#include "AosConf/DataFieldStr.h"

#include <map>
using AosConf::DataProc;

namespace AosConf
{
class DataProcIndex : public DataProc
{
private:
	pair<string, string >		mKey;
	pair<string, string>		mValue;

public:
	DataProcIndex() {
	}
	~DataProcIndex() {}

	void setKey(const string &input, const string &output) {                         
		mKey.first = input;
		mKey.second = output;
	}

	void setValue(const string &input, const string &output) {                         
		mValue.first = input;
		mValue.second = output;
	}

	string 	getConfig() {
		// 1. dataproc compose node
		string conf = "";
		conf += "<dataproc";
		for (map<string, string>::iterator itr=mAttrs.begin(); 
				itr!=mAttrs.end(); itr++)
		{
			conf += " " + itr->first + "=\"" + itr->second +  "\"";
		}
		conf += ">";

		// 2. input node 
		conf += "<fields>";
		conf += "<key zky_input_field_name=\"" + mKey.first + "\" zky_output_field_name=\"";
		conf += mKey.second + "\"/>";
		conf += "<value zky_input_field_name=\"" + mValue.first + "\" zky_output_field_name=\"";
		conf += mValue.second + "\"/>";
		conf += "</fields>";
		conf += "</dataproc>";
		return conf;
	}

};
}


#endif
