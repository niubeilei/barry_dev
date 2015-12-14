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
// <dataproc type="statdocshuffle">
// 		<fields >
// 			<statid zky_input_field_name="xxxxx" zky_output_field_name="xxxxxxx"/>
// 			<stattime zky_input_field_name="xxxx" zky_output_field_name="xxx"/>
// 			<statvalue zky_input_field_name="xxx" zky_output_field_name="xxx"/>
// 			<statmeasure zky_input_field_name="xxx" zky_output_field_name="xxx"/>
// 		</fields>
// 	</dataproc>
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AosConf_DataProcStatDocShuffle_h
#define Aos_AosConf_DataProcStatDocShuffle_h

#include "AosConf/DataProc.h"
#include "AosConf/DataFieldStr.h"

#include <map>
using AosConf::DataProc;

namespace AosConf
{
class DataProcStatDocShuffle : public DataProc
{
	struct StatInfo
	{
		string nodename;
		string input_field_name;
		string output_field_name;

		StatInfo(const string &name, const string &input, const string &output)
		:
		nodename(name),
		input_field_name(input),
		output_field_name(output)
		{
		}
	};

private:
	vector<StatInfo>					mStatInfo;
public:
	DataProcStatDocShuffle() {
		mAttrs["type"] = "statdocshuffle";
	}
	~DataProcStatDocShuffle() {}

	void set(const string &key, 
			const string &input_field_name,
			const string &output_field_name)
	{
		StatInfo info(key, input_field_name, output_field_name);
		mStatInfo.push_back(info);

	}

	string 	getConfig() {
		string conf = "";
		conf += "<dataproc";
		for (map<string, string>::iterator itr=mAttrs.begin(); 
				itr!=mAttrs.end(); itr++)
		{
			conf += " " + itr->first + "=\"" + itr->second +  "\"";
		}
		conf += ">";
		string stat_values = "<stat_values>";
		string stat_times = "<stat_times>";
		for(size_t i=0; i<mStatInfo.size(); i++)
		{
			if(mStatInfo[i].nodename == "stat_value")
			{
				stat_values += "<" + mStatInfo[i].nodename + " zky_input_field_name=\"" + mStatInfo[i].input_field_name + "\" ";
				stat_values += "zky_output_field_name=\"" + mStatInfo[i].output_field_name + "\" />";
			}
			else if(mStatInfo[i].nodename == "stat_time")
			{
				stat_times += "<" + mStatInfo[i].nodename + " zky_input_field_name=\"" + mStatInfo[i].input_field_name + "\" ";
				stat_times += "zky_output_field_name=\"" + mStatInfo[i].output_field_name + "\" />";
			}
			else
			{
				conf += "<" + mStatInfo[i].nodename + " zky_input_field_name=\"" + mStatInfo[i].input_field_name + "\" ";
				conf += "zky_output_field_name=\"" + mStatInfo[i].output_field_name + "\" />";
			}
		}
		stat_values += "</stat_values>";
		stat_times += "</stat_times>";
		
		conf += stat_values;
		conf += stat_times;	

		conf += "</dataproc>";
		return conf;
	}

};
}


#endif
