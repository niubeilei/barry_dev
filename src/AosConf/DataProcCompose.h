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
#ifndef Aos_AosConf_DataProcCompose_h
#define Aos_AosConf_DataProcCompose_h

#include "AosConf/DataProc.h"
#include "AosConf/DataFieldStr.h"

#include <map>
using AosConf::DataProc;

namespace AosConf
{
class DataProcCompose : public DataProc
{
private:
	vector<pair<string, string> >		mInputs;
	vector<string>						mOutputs;

public:
	DataProcCompose() {
		mAttrs["type"] = "compose";
	}
	~DataProcCompose() {}

	void setInput(const string &config, string sep = "") {                         
		mInputs.push_back(make_pair(config, sep));
	}

	void setOutput(const string &config) {                         
		mOutputs.push_back(config);
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
		conf += "<input>";
		for (size_t i=0; i<mInputs.size(); i++)
		{
			conf += "<field zky_input_field_name=\"" + mInputs[i].first + "\" zky_sep=\"" + mInputs[i].second + 
				"\" ></field>";
		}
		conf += "</input>";

		// 3. output node
		conf += "<output>";
		for (size_t i=0; i<mOutputs.size(); i++)
		{
			conf += "<field zky_output_field_name=\"" + mOutputs[i] + "\"></field>";
		}
		conf += "</output>";

		conf += "</dataproc>";
		return conf;
	}

};
}


#endif
