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
// 08/24/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StorageMgrUtil_SignatureAlog_h
#define Aos_StorageMgrUtil_SignatureAlog_h

#include "Util/String.h" 

class AosSignatureAlgo
{

public:
	enum E 
	{                    
		eInvalid,
		
		eMD5,
		eSHA1,

		eMax
	};
	
	enum ELen 
	{                    
		eMD5Len = 32,
		eSHA1Len = 32,
	};

private:

public:
	static bool isValid(const E code) 
	{
		return code > eInvalid && code < eMax;
	}

	/*
	static E toEnum(const OmnString &name);
	static OmnString  toString(const E type);
	static bool addName(const OmnString &name, const E value, OmnString &errmsg);
	*/
};

#endif

