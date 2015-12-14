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
#ifndef Aos_AosConf_DataSchema_h
#define Aos_AosConf_DataSchema_h

#include "AosConf/DataFieldStr.h"
#include "AosConf/DataRecord.h"
#include "AosConf/AosConfig.h"

#include <iostream>
#include <map>
#include <vector>

namespace AosConf
{
class DataSchema: public AosConf::AosConfBase
{
//private:
protected:
	boost::shared_ptr<DataRecord>		mRecord;
	vector<string>						mRecNames;
	string 								mRecordConf;
	
public:
	DataSchema() {
		mRecordConf = "";
	};
	DataSchema(const AosXmlTagPtr &xml);
	~DataSchema() {};

	void setRecord(boost::shared_ptr<DataRecord> record) {
		mRecord = record;
	}

	void setRecord(const string &recordConf) {
		mRecordConf = recordConf;
	}

	void setRecName(const string &name){
		mRecNames.push_back(name);
	}

	boost::shared_ptr<DataRecord> getRecord(){return mRecord;}
	
	string	getConfig();
};
}

#endif



