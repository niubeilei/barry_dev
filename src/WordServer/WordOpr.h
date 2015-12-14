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
// 09/28/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_WordSrver_WordOpr_h
#define Aos_WordSrver_WordOpr_h

#include "Util/String.h" 

#define AOSDOCOPR_ADD		"ADD"

struct AosWordOpr
{
	enum E
	{
		eInvalid, 

		eAdd,

		eMax
	};

	static E toEnum(const OmnString &name)
	{
		if (name == AOSDOCOPR_ADD) return eAdd;

		return eInvalid;
	}
};
#endif

