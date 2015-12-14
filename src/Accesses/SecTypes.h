////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SecurityTypes.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef OMN_Accesses_SecTypes_h
#define OMN_Accesses_SecTypes_h

#include "Util/String.h"




class OmnEncryptAlg
{
public:
	enum
	{
		eMaxEncryptAlgs = 10
	};

	enum E
	{
		eFirstValidEntry,

		eInvalid,
		eNoEncrypt,

		eLastValidEntry
	};

	static bool isValid(const E c)
	{
		return c > eFirstValidEntry && c < eLastValidEntry;
	}

	static OmnString toStr(const E c);
};
#endif
#endif
