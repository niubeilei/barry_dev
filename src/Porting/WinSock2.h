////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: WinSock2.h
// Description:
//	This is just <Winsock2.h>. This file is needed to define fd_set. 
//  For unit system, there is no need for this include file.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_WinSock2_h
#define Omn_Porting_WinSock2_h


//--------------------------------------------------------------
// Linux Portion
//--------------------------------------------------------------
#ifdef OMN_PLATFORM_UNIX


//--------------------------------------------------------------
// Microsoft Portion
//--------------------------------------------------------------
#elif OMN_PLATFORM_MICROSOFT
#include <WinSock2.h>

#endif
#endif

