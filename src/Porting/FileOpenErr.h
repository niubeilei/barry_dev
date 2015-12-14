////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FileOpenErr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_FileOpenErr_h
#define Omn_Porting_FileOpenErr_h

#include "Util/String.h"


#ifdef OMN_PLATFORM_UNIX
OmnString OmnGetFileOpenError(const int c);

#elif OMN_PLATFORM_MICROSOFT
#define OMN_E_RESOURCE_N_A EDEADLOCK
#endif


#endif

