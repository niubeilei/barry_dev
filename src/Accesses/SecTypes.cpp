////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SecurityTypes.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Accesses/SecTypes.h"

#include "Alarm/Alarm.h"



OmnString 
OmnEncryptAlg::toStr(const E c)
{
	switch (c)
	{
	case eInvalid:
		 return "Invalid";

	case eNoEncrypt:
		 return "NoEncrypt";

	default:
		 OmnAlarm << "Unrecognized: " << c << enderr;
		 return "Unrecognized";
	}

	OmnShouldNeverComeToThisPoint;

	return "Incorrect";
}

#endif
