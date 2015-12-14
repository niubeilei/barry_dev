////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosDebug.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_aosDebug_h
#define aos_aosDebug_h

#include "aos/aosFuncId.h"


extern void aosEnterFunc(int code, int parm1, int parm2, int parm3, int parm4);
extern void aosExitFunc(int code, int parm1, int parm2);

#endif
