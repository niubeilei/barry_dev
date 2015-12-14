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
#ifndef Aos_AosConf_DataRecordFixbin_h
#define Aos_AosConf_DataRecordFixbin_h

#include "AosConf/DataField.h"
#include "AosConf/DataRecord.h"
#include "AosConf/AosConfig.h"
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

namespace AosConf
{
class DataRecordFixbin: public AosConf::DataRecord
{
private:
	bool		mArrangeInit;

public:
	DataRecordFixbin() { mArrangeInit = false; mAttrs["type"] = "fixbin"; }
	DataRecordFixbin(const AosXmlTagPtr &xml);
	~DataRecordFixbin() { mArrangeInit = false; }

	string	getConfig();
	vector<boost::shared_ptr<DataField> >& getFields();
	virtual void setField(const string &name, const string &type, const int length);
	virtual void setField(const boost::shared_ptr<DataField> &field);
	virtual int getLength()
	{
		arrange();
		return atoi(getAttribute("zky_length").data());
	}
private:
	void arrange();
};
}

#endif



