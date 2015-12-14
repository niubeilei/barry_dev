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
#ifndef Aos_AosConf_CompFun_h
#define Aos_AosConf_CompFun_h

#include "AosConf/DataProc.h"
#include "AosConf/DataFieldStr.h"

#include <map>
#include <sstream>

namespace AosConf
{
class CompFun : public DataProc
{
	struct CmpField
	{
		string size;
		string data_type;
		string pos;
	};
	struct AggrField
	{
		string aggr_type;
		string data_type;
		string pos;
	};
private:
	vector<CmpField> mCmpFields;
	vector<AggrField> mAggrFields;
public:
	CompFun(const string &type, const int cmpsize) {
		mAttrs["cmpfun_type"] = type;
		mAttrs["cmpfun_reserve"] = "false";
		ostringstream oss;
		oss << cmpsize;
		mAttrs["cmpfun_size"] = oss.str();
	}
	~CompFun(){}

	void 	setCmpField(const string &type, const string &pos, const string size = "-1") {
		CmpField f = {size, type, pos};
		mCmpFields.push_back(f);
	}

	void 	setCmpField(const string &type, const int pos, const int size = -1) {
		char buff[10];
		sprintf(buff, "%d", pos);
		string posstr = buff;
		sprintf(buff, "%d", size);
		string sizestr = buff;
		CmpField f = {sizestr, type, posstr};
		mCmpFields.push_back(f);
	}
		
	void 	setAggrField(const string &datatype, const string &pos, const string &aggrtype) {
		AggrField f = {aggrtype, datatype, pos};
		mAggrFields.push_back(f);
	}

	void 	setAggrField(const string &datatype, const int pos, const string &aggrtype) {
		char buff[10];
		sprintf(buff, "%d", pos);
		string posstr = buff;
		AggrField f = {aggrtype, datatype, posstr};
		mAggrFields.push_back(f);
	}

	string 	getConfig() {
		// 1. dataproc compose node
		string conf = "<CompareFun";
		for (map<string, string>::iterator itr=mAttrs.begin(); 
				itr!=mAttrs.end(); itr++)
		{
			conf += " " + itr->first + "=\"" + itr->second +  "\"";
		}
		conf += ">";

		conf += "<cmp_fields>";
		for (size_t i=0; i<mCmpFields.size(); i++)
		{
			conf += "<field cmp_size=\"" + mCmpFields[i].size + 
				"\" cmp_datatype=\"" + mCmpFields[i].data_type + "\" cmp_pos=\"" + mCmpFields[i].pos + "\"/>";
		}
		conf += "</cmp_fields>";

		conf += "<aggregations>";
		for (size_t i=0; i<mAggrFields.size(); i++)
		{
			conf += "<aggregation agr_pos=\"" + mAggrFields[i].pos+ 
				"\" agr_type=\"" + mAggrFields[i].data_type + "\" agr_fun=\"" + mAggrFields[i].aggr_type + "\"/>";
		}
		conf += "</aggregations>";

		conf += "</CompareFun>";
		return conf;
	}

};
}


#endif
