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
#ifndef Aos_AosConf_DataAssembler_h
#define Aos_AosConf_DataAssembler_h

#include "AosConf/DataField.h"
#include "AosConf/AosConfig.h"

#include <map>
#include <vector>
#include <sstream>
#include <boost/shared_ptr.hpp>

using boost::shared_ptr;

namespace AosConf
{
class DataAssembler: public AosConf::AosConfBase
{
	struct CmpField
	{
		string size;
		string data_type;
		string pos;
		string reserve;
		string field_type;
	};
	struct AggrField
	{
		string aggr_type;
		string data_type;
		string pos;
		string field_type;
	};

private:
	boost::shared_ptr<DataRecord>	mRecord;
	int 							mCmpSize;
	string							mCmpType;
	string							mCmpRecordType;
	string 							mCmpFunReserve;

	vector<CmpField> 				mCmpFields;
	vector<AggrField> 				mAggrFields;
	vector<string>					mDataFields;
	bool							mIsUseCmp;
protected:

public:
	DataAssembler()
	:mIsUseCmp(true) 
	{
		setAttribute("zky_type", "sorted_file");
	}
	~DataAssembler() {};

	void 	setDataRecord(const boost::shared_ptr<DataRecord> &rcd){mRecord = rcd;}

	void 	setCmpFun(const string &cmpfun_type, 
			const int cmpfun_size, 
			const string &record_type = "",
			const string &cmpfun_reserve = "false")
	{
		mCmpType = cmpfun_type;
		mCmpSize = cmpfun_size;
		mCmpRecordType = record_type;
		mCmpFunReserve = cmpfun_reserve;
	}

	void 	setFieldType(const string &type)
	{
		mDataFields.push_back(type);
	}

	void 	setCmpField(const string &type, 
			const string &pos, 
			const string size = "-1",
			const string &cmp_reserve = "false") 
	{
		CmpField f = {size, type, pos, cmp_reserve};
		mCmpFields.push_back(f);
	}

	void 	setCmpField(const string &type, 
			const int pos, 
			const int size = -1,
			const string &cmp_reserve = "false") 
	{
		char buff[10];
		sprintf(buff, "%d", pos);
		string posstr = buff;
		sprintf(buff, "%d", size);
		string sizestr = buff;
		CmpField f = {sizestr, type, posstr, cmp_reserve};
		mCmpFields.push_back(f);
	}

	void 	setCmpField(
			const string &cmp_type, 
			const string &field_type,
			const int pos, 
			const int size = -1,
			const string &cmp_reserve = "false") 
	{
		char buff[10];
		sprintf(buff, "%d", pos);
		string posstr = buff;
		sprintf(buff, "%d", size);
		string sizestr = buff;
		CmpField f = {sizestr, cmp_type, posstr, cmp_reserve, field_type};
		mCmpFields.push_back(f);
	}


	void 	setAggrField(
			const string &datatype, 
			const string &pos, 
			const string &aggrtype) 
	{
		AggrField f = {aggrtype, datatype, pos};
		mAggrFields.push_back(f);
	}

	void 	setAggrField(
			const string &datatype, 
			const int pos, 
			const string &aggrtype) 
	{
		char buff[10];
		sprintf(buff, "%d", pos);
		string posstr = buff;
		AggrField f = {aggrtype, datatype, posstr};
		mAggrFields.push_back(f);
	}

	void 	setAggrField(
			const string &datatype, 
			const string &fieldtype,
			const int pos, 
			const string &aggrtype) 
	{
		char buff[10];
		sprintf(buff, "%d", pos);
		string posstr = buff;
		AggrField f = {aggrtype, datatype, posstr, fieldtype};
		mAggrFields.push_back(f);
	}

	void setUseCmp(const bool use) 
	{ 
		mIsUseCmp = use; 
		if (mIsUseCmp)
		{
			setAttribute("zky_type", "sorted_file");
		}
		else
		{
			setAttribute("zky_type", "file");
		}
	}

	string	getConfig()
	{
		if (mTagName == "")
			mTagName = "asm";
		// Ketty 2014/10/30
		string config = "<" + mTagName + " ";
		for (map<string, string>::iterator itr=mAttrs.begin();
				itr!=mAttrs.end(); itr++)
		{
			config += " " + itr->first + "=\"" + itr->second + "\"";
		}
		config += ">";

		//datarecord
		if (mRecord) config += mRecord->getConfig();

		if (mIsUseCmp)
		{
			config += "<CompareFun cmpfun_reserve=\"" + mCmpFunReserve + "\"";
			ostringstream ss;
			ss << mCmpSize;
			config += "cmpfun_size=\"" + ss.str() + "\"";
			if (mCmpRecordType != "")
				config += "record_type=\"" + mCmpRecordType + "\"";
			if (mCmpType == "")
				mCmpType = "custom";
			config += "cmpfun_type=\"" + mCmpType + "\">";

			if (mDataFields.size() > 0)
			{
				config+= "<datafields>";
				for (size_t i=0; i<mDataFields.size(); i++)
				{
					config += "<field type=\"" + mDataFields[i] + "\"/>";
				}
				config+= "</datafields>";
			}

			if (mCmpFields.size() > 0)
			{
				config += "<cmp_fields>";
				for (size_t i = 0; i < mCmpFields.size(); i++)
				{
					config += "<field cmp_size=\"" + mCmpFields[i].size + 
						"\" cmp_datatype=\"" + mCmpFields[i].data_type + 
						"\" field_type=\"" + mCmpFields[i].field_type + 
						"\" cmp_pos=\"" + mCmpFields[i].pos +
						"\" cmp_reserve=\"" + mCmpFields[i].reserve + "\"/>";
				}
				config += "</cmp_fields>";
			}

			if (mAggrFields.size() > 0)
			{
				config += "<aggregations>";
				for (size_t i=0; i<mAggrFields.size(); i++)
				{
					config += "<aggregation agr_pos=\"" + mAggrFields[i].pos + 
						"\" agr_type=\"" + mAggrFields[i].data_type + 
						"\" field_type=\"" + mAggrFields[i].field_type + 
						"\" agr_fun=\"" + mAggrFields[i].aggr_type + "\"/>";
				}
				config += "</aggregations>";
			}

			config += "</CompareFun>";
		}

		config += "</" + mTagName + ">";
		return config;
	}
};
}

#endif



