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
// 07/24/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_NetworkMgrUtil_CommandListener_h
#define Aos_NetworkMgrUtil_CommandListener_h

#include "Rundata/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"

class AosValueRslt;

class AosCommandListener : virtual public OmnRCObject
{
public:
	virtual bool commandResponded(
						const AosBuffPtr &buff, 
						const AosValueRslt &user_data, 
						const AosRundataPtr &rdata) = 0;
};
#endif

