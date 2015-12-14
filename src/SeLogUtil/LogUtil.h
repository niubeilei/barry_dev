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
// 01/27/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SeLogUtil_LogUtil_h 
#define AOS_SeLogUtil_LogUtil_h 


class AosLogUtil
{
public:
	enum 
	{
		eMaxModules = 1024
	};

	inline static bool isValidModuleId(const int moduleid)
	{
		return (moduleid >= 0 && moduleid < eMaxModules);
	}
};
#endif

