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
//
// Modification History:
// 2015/03/14 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAPI_IDODocidMgr_h
#define Aos_JimoAPI_IDODocidMgr_h

#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"

class AosRundata;

namespace Jimo
{


class IDODocidMgr
{
public:
	enum
	{
	};

public:
	u64 getCubeMapIDODocid();
};

extern IDODocidMgr gIDODocidMgr;

};
#endif

