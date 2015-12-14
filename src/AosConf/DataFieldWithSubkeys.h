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
#ifndef Aos_AosConf_DataFieldWithSubkeys_h
#define Aos_AosConf_DataFieldWithSubkeys_h

#include "AosConf/AosConfig.h"
#include "AosConf/DataField.h"
#include <iostream>
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;
using AosConf::DataField;

namespace AosConf
{
class DataFieldWithSubkeys : public AosConf::DataField
{
private:
	vector<string>		mSubkeysDef;

public:
	DataFieldWithSubkeys(const AosXmlTagPtr &xml);
	DataFieldWithSubkeys();
	~DataFieldWithSubkeys();

	virtual string getConfig();

	void setSubkeyDef(const string &subkey_def)
	{
		mSubkeysDef.push_back(subkey_def);
	}

};
}

#endif



