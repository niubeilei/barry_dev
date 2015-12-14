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
#ifndef Aos_AosConf_DataRecordBuff_h
#define Aos_AosConf_DataRecordBuff_h

#include "AosConf/DataField.h"
#include "AosConf/DataRecord.h"
#include "AosConf/AosConfig.h"
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

namespace AosConf
{
class DataRecordBuff: public AosConf::DataRecord
{
private:

public:
	DataRecordBuff();
	DataRecordBuff(const AosXmlTagPtr &xml);
	~DataRecordBuff() {};

	string	getConfig();
	vector<boost::shared_ptr<DataField> >& getFields();
	virtual void setField(const string &name, const string &type, const int length);
	void setField(boost::shared_ptr<AosConf::DataField> &df);
};
}

#endif



