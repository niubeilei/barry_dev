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
#ifndef Aos_AosConf_DataRecordMulti_h
#define Aos_AosConf_DataRecordMulti_h

#include "AosConf/DataSchema.h"
#include "AosConf/DataFieldStr.h"
#include "AosConf/DataRecord.h"
#include "AosConf/AosConfig.h"

#include <iostream>
#include <map>
#include <vector>

namespace AosConf
{
class DataRecordMulti: public AosConf::DataRecord
{
protected:
	vector<string>						mDataFields;
	vector<string>						mRecordPicks;
	string								mSchemaPicker;
	//AosXmlTagPtr						mThisConf;
	
public:
	DataRecordMulti(const AosXmlTagPtr &def);
	DataRecordMulti() {}
	~DataRecordMulti() {}
	
	void setSchemaPicker(string picker)
	{
		mSchemaPicker = string("<picker>") + string("<match_expr><![CDATA[")
			+ picker + string(";]]></match_expr></picker>");
	}

	void appendRecordPick(const string &pick) {
		mRecordPicks.push_back(pick);
	}

	void appendRecordPick(const boost::shared_ptr<DataRecord> &record) {
		mRecordPicks.push_back(record->getConfig());
	}
	
	void setDataField(const string &fieldconf) {
		mDataFields.push_back(fieldconf);			
	}

	string	getConfig();
};
}

#endif



