////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Current.h
// Description:
//	
//	'nonagle'
//  There is a major difference between 2.4.30 and 2.6.7+. In 2.4.30, 
//  'nonagle' is defined in 'struct tcp_opt'. In 2.4.30, this is defined
//  as a member of the union 'tp_pinfo'. In 2.6.7+, it is defined in 
//  'struct tcp_sock'.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_KernelPorting_Current_h
#define Omn_KernelPorting_Current_h

#ifdef AOS_KERNEL_2_4_30
#define aosCurrent_Siglock(x) (x)->sigmask_lock
#define aos_recalc_sigpending(x) recalc_sigpending(x)
#else
#define aosCurrent_Siglock(x) (x)->sighand->siglock
#define aos_recalc_sigpending(x) recalc_sigpending()
#endif

#endif

