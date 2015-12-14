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
// 2014/11/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoCall_SysDefinedArgs_h
#define Aos_JimoCall_SysDefinedArgs_h
#include "SEUtil/LogFieldNames.h"
	inline JimoCall & SiteId(const u32 siteid)
	{
		mBSON.appendU32(AosFieldName::eSiteid, siteid);
		return *this;
	}

#endif
