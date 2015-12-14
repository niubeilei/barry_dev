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
#ifndef Aos_AosConf_DataSplit_h
#define Aos_AosConf_DataSplit_h

#include "AosConf/AosConfig.h"
#include <iostream>
#include <map>
#include <vector>
#include "Task.h"
#include "XmlUtil/Ptrs.h" 

namespace AosConf
{
class DataSplit: public AosConf::AosConfBase
{
private:

public:
	DataSplit() {};
	DataSplit(const AosXmlTagPtr &xml);
	~DataSplit() {};

	string	getConfig();
};
}



#endif



