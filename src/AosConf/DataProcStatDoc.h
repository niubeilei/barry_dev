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
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AosConf_DataProcStatDoc_h
#define Aos_AosConf_DataProcStatDoc_h

#include "AosConf/DataProc.h"
#include "AosConf/DataFieldStr.h"

#include <map>
#include <sstream>
using AosConf::DataProc;

namespace AosConf
{
class DataProcStatDoc : public DataProc
{
	struct StatInfo
	{
//		string value_output_field_name;
//		string docid_output_field_name;
		string  	mNodeName;
		string		mInputValueName;
	};

	//struct StatVt2D
	//{
	//	string				mInputValueName;
	//	string				mName;
	//};
	
	struct StatMeasure
	{
		string		mName;
		string 		mGrpbyTimeUnit;
		string		mTimeInputValueName;
		string		mValueInputValueName;
	};


private:
//	StatInfo							mNewValue;
//	StatInfo							mOldValue;
	string 								mInputFieldName;
	string								mLength;
	string								mKeyFieldLength;
	string								mDataFieldsStr;
	//vector<StatVt2D>					mStatVt2D;
	vector<StatMeasure>					mMeasures;
	vector<StatInfo>					mStatInfos;
	
	boost::shared_ptr<DataRecord> 				mInputRcd;

public:
	DataProcStatDoc() {
		mAttrs["type"] = "statdoc";
	}
	~DataProcStatDoc() {}

	void setDataFieldsStr(const string &str)
	{
		mDataFieldsStr = str;
	}

	void setInputFieldName(const string &input_field_name)
	{
		mInputFieldName = input_field_name;
	}

	void setStatInfo(
			const string &nodename,
			const string &input_value_name)
	{
		StatInfo stat;	
		stat.mInputValueName = input_value_name;
		stat.mNodeName = nodename;
		mStatInfos.push_back(stat);
	}
/*
	void setOldValue(
			const string &value_output_field_name,
			const string &docid_output_field_name)
	{                         
		mOldValue.value_output_field_name = value_output_field_name;
		mOldValue.docid_output_field_name = docid_output_field_name;

	}

	void setNewValue(
			const string &value_output_field_name,
			const string &docid_output_field_name)
	{                         
		mNewValue.value_output_field_name = value_output_field_name;
		mNewValue.docid_output_field_name = docid_output_field_name;

	}
*/
	void setLength(const string &len)
	{
		mLength = len;
	}

	void setKeyFieldLength(const string &len)
	{
		mKeyFieldLength = len;
	}

	void setLength(int len)
	{
		ostringstream oss;
		oss << len;
		mLength = oss.str();
	}

	void setStatMeasure(
			const string &measure_name,
			const string &grpby_time_unit,
			const string &time_input_name,
			const string &value_input_name)
	{
		StatMeasure ms;
		ms.mName = measure_name;
		ms.mGrpbyTimeUnit = grpby_time_unit;
		ms.mTimeInputValueName = time_input_name;
		ms.mValueInputValueName = value_input_name;
		mMeasures.push_back(ms);
	}

	//void setStatValue(
	//		const string &value_name,
	//		const string &name)
	//{
	//	StatVt2D stat;	
	//	stat.mInputValueName = value_name;
	//	stat.mName = name;
	//	mStatVt2D.push_back(stat);
	//}
	
	//void setStatTime(
	//		const string &nodename,
	//		const string &input_value_name)
	//{
	//	StatInfo stat;	
	//	stat.mInputValueName = input_value_name;
	//	stat.mNodeName = nodename;
	//	mStatTimes.push_back(stat);
	//}
	
	void	setInputRecord(boost::shared_ptr<DataRecord> &input_rcd)
	{
		mInputRcd = input_rcd; 
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
		
		conf += mInputRcd->getConfig();
		//conf += "<input zky_input_field_name=\"" + mInputFieldName + "\" ";
		//conf += "zky_length=\"" + mLength + "\">";
		//conf += mDataFieldsStr;
		//conf += "</input>";

		for(size_t i=0; i<mStatInfos.size(); i++)
		{
			conf += "<" + mStatInfos[i].mNodeName;
			conf += " zky_input_value_name=\"" + mStatInfos[i].mInputValueName + "\" />"; 
		}
	
		conf += "<measures>";
		for(size_t i=0; i<mMeasures.size(); i++)
		{
			conf += "<measure ";
			conf += "zky_name=\"" + mMeasures[i].mName + "\" ";
			if(mMeasures[i].mGrpbyTimeUnit != "")
			{
				conf += "grpby_time_unit=\"" + mMeasures[i].mGrpbyTimeUnit + "\" ";
			}
			conf += ">";
			
			if(mMeasures[i].mGrpbyTimeUnit != "")
			{
				conf += "<stat_time ";
				conf += "zky_input_value_name=\"" + mMeasures[i].mTimeInputValueName + "\" >";
				conf += "</stat_time>";
			}
			conf += "<stat_value ";
			conf += "zky_input_value_name=\"" + mMeasures[i].mValueInputValueName + "\" >";
			conf += "</stat_value>";
			conf += "</measure>";
		}
		conf += "</measures>";

		/*
		conf += "<stat_times>";
		for(size_t i=0; i<mStatTimes.size(); i++)
		{
			conf += "<" + mStatTimes[i].mNodeName;
			conf += " zky_input_value_name=\"" + mStatTimes[i].mInputValueName + "\" />"; 
		}
		conf += "</stat_times>";

		conf += "<stat_values>";
		for(size_t i=0; i<mStatVt2D.size(); i++)
		{
			conf += "<stat_value zky_name=\"" + mStatVt2D[i].mName + "\" ";
			conf += "zky_input_value_name=\"" + mStatVt2D[i].mInputValueName + "\" />";
		}
		conf += "</stat_values>";
		*/

		//ostringstream oss;
		//oss << mLength;
		//conf += "zky_length = \"" + oss.str() + "\" >";
		conf += "</dataproc>";
		return conf;
	}

};
}


#endif
