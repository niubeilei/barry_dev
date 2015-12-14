////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: cout.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_cout_h
#define Omn_Porting_cout_h


#ifdef OMN_PLATFORM_UNIX
#define OmnSetbase10 setbase(10)


#elif OMN_PLATFORM_MICROSOFT
#define OmnSetbase10 dec

#endif

#endif

