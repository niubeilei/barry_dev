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
// 06/21/2011: Created by  Brian Zhang 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ShortMsgUtil_ShmPneids_h
#define Aos_ShortMsgUtil_ShmPneids_h

#include "Util/String.h"

#define AOSNINEZEROFIVEEIGHT			"13915411643"

enum AosPhoneId
{
	eAosPhoneId_Invalid = 0,
	
	eAosPhoneId_NineZeroFiveEight,

	eAosPhoneId_SplitLine,

	eAosPhoneId_Max
};

extern AosPhoneId AosPhoneids_strToCode(const OmnString &typeName);
extern AosPhoneId AosPhoneids_getLineCode();
#endif
