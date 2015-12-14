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
#ifndef Aos_AosConf_DataSchemaMultiRecord_h
#define Aos_AosConf_DataSchemaMultiRecord_h

#include "AosConf/DataSchema.h"
#include "AosConf/DataFieldStr.h"
#include "AosConf/DataRecord.h"
#include "AosConf/AosConfig.h"
#include "AosConf/DataRecordMulti.h"

#include <iostream>
#include <map>
#include <vector>

namespace AosConf
{
class DataSchemaMultiRecord: public AosConf::DataSchema
{
protected:
	struct RecordPick
	{
		string len;
		string key;
		boost::shared_ptr<DataRecord> record;
	};

protected:
	vector<RecordPick>					mRecordPicks;
	string								mSchemaPicker;

	// by young
	string								mRecordNode;
	
public:
	DataSchemaMultiRecord() {};
	DataSchemaMultiRecord(const AosXmlTagPtr &xml);
	~DataSchemaMultiRecord() {};
	
	void setSchemaPicker(string pos, string len)
	{
		mSchemaPicker = "<schema_picker jimo_objid=\"dataschemapicker_fixbin_jimodoc_v0\"";
		mSchemaPicker += " start_pos=\"0\" length=\"2\"></schema_picker>";
	}

	void appendRecordPick(const string &len, const string &key, const boost::shared_ptr<DataRecord> &record) {
		RecordPick pick;
		pick.len = len;
		pick.key = key;
		pick.record = record;
		mRecordPicks.push_back(pick);
	}

	string	getConfig();
};
}

#endif



