////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CreateSock.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_KernelPorting_CreateSock_h
#define Omn_KernelPorting_CreateSock_h

#ifdef AOS_KERNEL_2_4_30
#define aosSockCreateKern(x, y, z, w) sock_create((x), (y), (z), (w))
#else
#define aosSockCreateKern(x, y, z, w) sock_create_kern((x), (y), (z), (w))
#endif

/*
#elif AOS_KERNEL_2_6_11
#define aosSockCreateKern(x, y, z, w) sock_create_kern((x), (y), (z), (w))
#elif AOS_KERNEL_2_4_30
#else
#define aosSockCreateKern(x, y, z, w) Please Define AOS_KERNEL_x_y_z
#endif
*/
#endif

