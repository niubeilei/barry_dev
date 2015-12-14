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
// 07/18/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_ThreadShellRunners_ActionRunnerListener_h 
#define Omn_ThreadShellRunners_ActionRunnerListener_h

#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"

class AosValueRslt;

class AosActionRunnerListener : public OmnRCObject
{
public:
	virtual bool actionFinished(
					const bool status, 
					const AosValueRslt &user_data, 
					const AosRundataPtr &rdata) = 0;
};
#endif
