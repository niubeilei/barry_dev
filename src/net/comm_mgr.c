////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 02/23/2008 : Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "net/comm_mgr.h"

#include "alarm_c/alarm.h"


// 
// This function starts reading the sock 'sock'. If something is read, 
// it will perform the action 'callback'. If anything happens to the
// sock, such as the sock being closed, it will also perform the action
// 'callback'. 
//
int aos_comm_mgr_recv_msg(
		const int sock, 
		struct aos_action *callback)
{
	aos_not_implemented_yet;
	return -1;
}

