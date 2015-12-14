////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Base64.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_aosUtil_Base64_h
#define Aos_aosUtil_Base64_h

#include "aosUtil/Types.h"

extern int aos_base64_decode(const char *data, u32 data_len, 
							 char *dest, u32 dest_len);
extern int aos_base64_encode(const char *data, u32 data_len, 
							 char *dest, u32 dest_len);

#endif

