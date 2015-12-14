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
#ifndef Aos_AosConf_DataFieldConst_h
#define Aos_AosConf_DataFieldConst_h

#include "AosConf/AosConfig.h"
#include "AosConf/DataField.h"
#include <iostream>
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

namespace AosConf
{
class DataFieldConst : public AosConf::DataField
{
	string mConstStr;
	public:
		DataFieldConst();
		~DataFieldConst();

		DataFieldConst(const AosXmlTagPtr &xml);
		void setConst(const string &str);
		virtual string getConfig();
};
}

#endif



