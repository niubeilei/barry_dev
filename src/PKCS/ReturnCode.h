////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ReturnCode.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_PKCS_ReturnCode_h
#define Omn_PKCS_ReturnCode_h

#include "aos/aosReturnCode.h"
#include "aosUtil/Alarm.h"


enum aosPkcsReturnCode
{
	eAosRc_CertNotFound = eAosRc_CertMgrStart,
	eAosRc_CertErr,
	eAosRc_PkcsErr,
	eAosRc_PkcsCliErr,
};


enum AosPkcsAlarm
{
	eAosAlarm_CertDecodeErr,
	eAosAlarm_CertInvalid,
	eAosAlarm_PkcsErr,
	eAosAlarm_AsnErr,
};

#endif

