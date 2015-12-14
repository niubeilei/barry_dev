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
#ifndef Aos_AosConf_DataProcJoin_h
#define Aos_AosConf_DataProcJoin_h

#include "AosConf/DataProc.h"
#include "AosConf/DataFieldStr.h"

#include <map>
using AosConf::DataProc;

namespace AosConf
{
class DataProcJoin : public DataProc
{
private:
	string				mLhsInputName;
	string				mRhsInputName;

public:
	DataProcJoin() {
		mAttrs["type"] = "join";
	}
	~DataProcJoin() {}

	void setLhsInput(const string &name) {                         
		mLhsInputName = name;
	}

	void setRhsInput(const string &name) {                         
		mRhsInputName = name;
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

		conf += "<fields>";
		
		// 2. lhs_input
		conf += "<zky_lhs zky_input_field_name=\"" + mLhsInputName + "\"/>";

		// 3. rhs_input
		conf += "<zky_rhs zky_input_field_name=\"" + mRhsInputName + "\"/>";

		conf += "</fields>";
		conf += "</dataproc>";
		return conf;
	}

};
}


#endif
