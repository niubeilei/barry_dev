////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 01/18/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////

#include "alarm/Alarm.h"
#include "event/event_mgr.h"
#include "util/tracer.h"
#include "util/global_data.h"
#include "util/memory.h"
#include "util/tracer.h"

#include "action/tester/recv_msg_tester.h"
#include "action/tester/send_msg_tester.h"

#include <stdio.h>


int 
main(int argc, char **argv)
{
	aos_global_data_init();
	aos_event_mgr_init();
	aos_tracer_set_filter(eAosMD_Tracer, eAosLogLevel_Debug);

	aos_recv_msg_tester();
	//aos_send_msg_tester();
	return 0;
} 

