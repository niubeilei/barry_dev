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
#ifndef Aos_AosConf_DataProcStatIndex_h
#define Aos_AosConf_DataProcStatIndex_h

#include "AosConf/DataProc.h"
#include "AosConf/DataFieldStr.h"

using AosConf::DataProc;

namespace AosConf
{
class DataProcStatIndex : public DataProc
{
private:
	vector<string>				mKeyOutputFieldNames;
	vector<string>				mDocidOutputFieldNames;
	string						mKeyInputFieldName;
	string						mDocidInputFieldName;

public:
	DataProcStatIndex() {
		mAttrs["type"] = "statindex";
	}
	~DataProcStatIndex() {}

	void setKeyInputFieldName(const string &name){mKeyInputFieldName = name;}
	
	void setDocidInputFieldName(const string &name){mDocidInputFieldName = name;}
	
	void setKeyOutputFieldName(const string &name) {                         
		mKeyOutputFieldNames.push_back(name);
	}

	void setDocidOutputFieldName(const string &name) {                         
		mDocidOutputFieldNames.push_back(name);
	}


	string 	getConfig() {
		// 1. dataproc statindex node
		string conf = "";
		conf += "<dataproc";
		for (map<string, string>::iterator itr=mAttrs.begin(); 
				itr!=mAttrs.end(); itr++)
		{
			conf += " " + itr->first + "=\"" + itr->second +  "\"";
		}
		conf += ">";
		
		conf += "<fields>";

		// 2. keys node 
		conf += "<keys zky_input_field_name=\"" + mKeyInputFieldName + "\">";
		for (size_t i=0; i<mKeyOutputFieldNames.size(); i++)
		{
			conf += "<key zky_output_field_name=\"" + mKeyOutputFieldNames[i] + "\"/>";
		}
		conf += "</keys>";

		// 3. docids node
		conf += "<docids zky_input_field_name=\"" + mDocidInputFieldName +"\">";
		for (size_t i=0; i<mDocidOutputFieldNames.size(); i++)
		{
			conf += "<docid zky_output_field_name=\"" + mDocidOutputFieldNames[i] + "\"/>";
		}
		conf += "</docids>";
		conf += "</fields>";
		conf += "</dataproc>";
		return conf;
	}

};
}


#endif
