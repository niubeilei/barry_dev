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
#ifndef Aos_AosConf_Task_h
#define Aos_AosConf_Task_h

#include "AosConf/AosConfig.h"

#include <map>
#include <vector>
#include <string>

namespace AosConf
{
class Task: public AosConf::AosConfBase
{
private:

public:

public:
	Task(){}
	~Task(){}

	virtual string	getConfig() = 0;

};
}


#endif



