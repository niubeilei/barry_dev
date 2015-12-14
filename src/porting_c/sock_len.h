////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SockLen.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_porting_sock_len_h
#define Omn_porting_sock_len_h


//--------------------  Unix Portion ------------------------------
#ifdef OMN_PLATFORM_UNIX

typedef socklen_t	aos_sock_len_t;




//--------------------  Microsoft Portion  ------------------------------

#elif OMN_PLATFORM_MICROSOFT

typedef int		   aos_sock_len_t;

#endif 

#endif

