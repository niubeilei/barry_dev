////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: GlobalVar.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_GlobalVar_h
#define Omn_Porting_GlobalVar_h

#ifdef OMN_PLATFORM_MICROSOFT
#define OmnOutputCriticalSession CRITICAL_SECTION sgCsOutput
#elif OMN_PLATFORM_UNIX
#define OmnOutputCriticalSession 
#endif

#ifndef DNSPORT  
#define DNSPORT 2000
#endif

#ifndef ROOTPORT  
#define ROOTPORT  3000
#endif

#endif


