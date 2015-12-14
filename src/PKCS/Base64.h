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

#ifndef Aos_PKCS_Base64_h
#define Aos_PKCS_Base64_h

#include "aosUtil/Types.h"

extern int AosBase64_decode(const char *input, 
					 u32 input_len, 
					 char **output);

extern int AosBase64_decode1(const char *input, 
					 u32 input_len, 
					 char *output);
#endif

