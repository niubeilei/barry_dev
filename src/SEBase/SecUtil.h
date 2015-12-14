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
// 07/19/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Snt_SEBase_SecUtil_h
#define Snt_SEBase_SecUtil_h

#include "Porting/Process.h"
#include "Util/String.h"

#include <sys/types.h>
#include <unistd.h>


class AosSecUtil 
{
public:
	static OmnString signValue(const OmnString &value);
	static OmnString signValue(const char *data, const int &size);
};

#endif

