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
// 2015/05/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_JimoPodObj_h
#define AOS_SEInterfaces_JimoPodObj_h

#include "aosUtil/Types.h"


class AosJimoPodObj
{
public:
	virtual u32 getJSID(const u64 docid) const = 0;
};
#endif
