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
// 02/21/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////

#include "event/event_mgr.h"
#include "statemachine/tester/state_tester.h"
#include "statemachine/tester/statemachine_tester.h"
#include "util/tracer.h"
#include "util/global_data.h"


int main(int argc, char **argv)
{
    aos_global_data_init();
	aos_tracer_set_filter(eAosMD_Tracer, eAosLogLevel_Debug);

	aos_statemachine_tester();
	return 0;
} 

