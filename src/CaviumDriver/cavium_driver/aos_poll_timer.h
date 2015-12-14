////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aos_poll_timer.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef _AOS_POLL_TIMER_H_
#define _AOS_POLL_TIMER_H_

extern wait_queue_head_t cavium_poll;
extern int aos_init_poll_timer( void );
extern void aos_kill_poll_timer( void );

#endif // _AOS_POLL_TIMER_H_

