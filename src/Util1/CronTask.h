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
// 08/09/2012, Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Util1_CronTask_h
#define Omn_Util1_CronTask_h

#include "Util/RCObject.h"

class AosCronTask : public virtual OmnRCObject
{
protected:
	int			mCronTaskFreq;

public:
	virtual bool runCronJob(const AosRundataPtr &rdata) = 0;
};

#endif
