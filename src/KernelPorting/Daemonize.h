////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Daemonize.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_KernelPorting_Daemonize_h
#define Omn_KernelPorting_Daemonize_h

#ifdef AOS_KERNEL_2_4_30
#define aosDaemonize(x) daemonize()
#else
#define aosDaemonize(fmt,x...) daemonize(fmt,##x)
#endif


#endif

