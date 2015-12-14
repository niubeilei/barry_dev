////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/12/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_MySqlAdapterObj_h
#define Aos_SEInterfaces_MySqlAdapterObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/Buff.h"


class AosMySqlAdapterObj: virtual public OmnRCObject
{
public:

	virtual bool procCommand(const AosRundataPtr &rdata, const AosBuffPtr &data) = 0;
};
#endif
