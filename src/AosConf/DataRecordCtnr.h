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
#ifndef Aos_AosConf_DataRecordCtnr_h
#define Aos_AosConf_DataRecordCtnr_h

#include "AosConf/DataRecord.h"
#include "AosConf/AosConfig.h"

#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

namespace AosConf
{
class DataRecordCtnr: public DataRecord
{
private:
	boost::shared_ptr<DataRecord> 		mRecord;
	string 								mRecordConf;

public:
	DataRecordCtnr()
	{
		// Ketty 2014/05/12
		setAttribute("type", "ctnr");
		mRecordConf = "";
	};
	~DataRecordCtnr() {};

	void setRecord(const boost::shared_ptr<DataRecord> &record) 
	{
		mRecord = record;
	}

	void setRecord(const string &conf)
	{
		mRecordConf += conf;
	}

	virtual void setField(const boost::shared_ptr<DataField> &field)
	{
		mRecord->setField(field);
	}

	virtual void setField(const string &fieldname)
	{
		mRecord->setField(fieldname);
	}


	virtual vector<boost::shared_ptr<DataField> >& getFields() 
	{
		return mRecord->getFields();
	}

	virtual void setField(
			const string &name, 
			const string &type, 
			const int length)
	{
		mRecord->setField(name, type, length);
	}

	virtual string	getConfig() 
	{
		string conf;
		if (mTagName == "")
			mTagName = "datarecord";
		if (conf != "") return conf; 
		conf = "<" + mTagName + " ";
		for (map<string, string>::iterator itr=mAttrs.begin();      
				        itr != mAttrs.end(); itr++)                         
		{   
			conf += " " + itr->first + "=\"" + itr->second + "\""; 
		}   
		conf += " >";                                               

		if (mRecord) conf += mRecord->getConfig();
		conf += mRecordConf;
		conf += "</" + mTagName + ">";
		return conf;
	}

	virtual int getLength()
	{
		return mRecord->getLength();
	}

	virtual bool setConfig(const AosXmlTagPtr &xml){ return mRecord->setConfig(xml); }
};
}

#endif



