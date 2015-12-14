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
// 2014/12/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAPI_JimoPackage_h
#define Aos_JimoAPI_JimoPackage_h

#include "Util/String.h"

class JimoPackageID
{
public:
	enum E
	{
		eInvalid,

		eHelloWorld = 2,
		eDocPackage = 3,
		eIILPackage = 4,		// Yazong Ma, 2014/12/20
		eRaft = 5,		// Phil, 2015/05/11

		eLastEntry
	};

	static OmnString getClassname(const E package_id);
};

#endif
