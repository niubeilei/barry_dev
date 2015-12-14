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
// 2015 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AosConf_DataRecordCSV_h
#define Aos_AosConf_DataRecordCSV_h

#include "AosConf/DataField.h"
#include "AosConf/DataRecord.h"
#include "AosConf/AosConfig.h"
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

namespace AosConf
{
class DataRecordCSV: public AosConf::DataRecord
{
public:
	DataRecordCSV() { mAttrs["type"] = "csv"; }
	~DataRecordCSV() {}

	string	getConfig();
	virtual void setField(const string &name, const string &type);
};
}

#endif



