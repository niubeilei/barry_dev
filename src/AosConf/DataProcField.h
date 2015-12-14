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
#ifndef Aos_AosConf_DataProcField_h
#define Aos_AosConf_DataProcField_h

#include "AosConf/DataProc.h"
#include "AosConf/DataFieldStr.h"

#include <map>

namespace AosConf
{
class DataProcField : public DataProc
{
private:
	vector<boost::shared_ptr<DataFieldStr> >		mFields;

public:
	DataProcField() {
		mAttrs["type"] = "field";
	}
	~DataProcField(){}

	void 	setField(const boost::shared_ptr<DataFieldStr> &field) {
		mFields.push_back(field);
	}
		
	string 	getConfig() {
		// 1. dataproc compose node
		string conf = "<dataproc";
		for (map<string, string>::iterator itr=mAttrs.begin(); 
				itr!=mAttrs.end(); itr++)
		{
			conf += " " + itr->first + "=\"" + itr->second +  "\"";
		}
		conf += ">";

		// 2. fields node 
		conf += "<fields>";
		for (size_t i=0; i<mFields.size(); i++)
		{
			string field_conf = mFields[i]->getConfig();
			conf += field_conf;
		}
		conf += "</fields>";

		conf += "</dataproc>";
		return conf;
	}

};
}


#endif
