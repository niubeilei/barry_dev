////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 04/30/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Job_JobSchedulerId_h
#define AOS_Job_JobSchedulerId_h

#include "Util/String.h"

#define AOSJOBSCHEDULER_NORM			"norm"

class AosJobSchedulerId
{
public:
	enum E
	{
		eInvalid,

		eNorm,

		eMax
	};

	static bool isValid(const E id) {return id > eInvalid && id < eMax;}
	static E toEnum(const OmnString &reqid);
	static bool addName(const OmnString &name, const E e);
};
#endif
