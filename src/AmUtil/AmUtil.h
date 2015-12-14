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
// Modification History:
// 3/31/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AmUtil_AmUtil_h
#define Aos_AmUtil_AmUtil_h

#include "AmUtil/AmTagId.h"
#include "AmUtil/AmRespCode.h"
#include "aosUtil/Types.h"
#include "Util/String.h"

typedef bool (*AosAmApiCallback)(
				const bool status, 
				const AosAmRespCode::E &respCode, 
				const OmnString &errmsg);


class AosAmUtil
{

private: 

public:
	AosAmUtil();
	~AosAmUtil();

	static bool expandBuf(char *&buff, const u32 buflen);
	static bool	setU32(char *buff, const u32 value);
	static bool	setU16(char *buff, const u16 value);
	static u16	getU16(char *buff);
	static u32	getU32(char *buff);
	static int 	addTag(char *buff, const u32 buflen, AosAmTagId::E tagId, const OmnString &value);
};

#endif

