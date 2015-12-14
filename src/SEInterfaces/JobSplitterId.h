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
// 04/28/2012 Created by Chen Ding
// 2013/06/02 Moved from Job by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_SEInterfaces_JobSplitterId_h
#define AOS_SEInterfaces_JobSplitterId_h

#include "Util/String.h"


class AosJobSplitterId
{
public:
	enum E
	{
		eInvalid,

		eAuto,
		eDir,
		eFile, 
		eIILSize,
		eSubiil,
		eVirtualFile,
		eDoc,
		eDocIds,

		eMax
	};

	static bool isValid(const E id) {return id > eInvalid && id < eMax;}
	static E toEnum(const OmnString &reqid);
	static bool addName(const OmnString &name, const E e);
};

#endif
#endif
