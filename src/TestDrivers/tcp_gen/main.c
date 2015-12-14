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
// 02/28/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TestDrivers/tcp_gen/traffic_gen.h"

#include "porting/sleep.h"
#include "porting/addr.h"
#include "util/tracer.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	int index = 1;
	u32 local_addr = 0;
	int local_port = 0;
	int num_ports = 1;

	while (index < argc)
	{
		if (strcmp(argv[index], "-local_addr") == 0)
		{
			aos_assert_r(!aos_addr_to_u32(argv[index+1], &local_addr), -1);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-local_port") == 0)
		{
			local_port = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-num_ports") == 0)
		{
			num_ports = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-s") == 0)
		{
			aos_tracer_set_filter(eAosMD_Tracer, eAosLogLevel_Debug);
			index++;
			continue;
		}

		if (strcmp(argv[index], "-help") == 0)
		{
			printf("bouncer.exe -local_addr <addr>\n"
				   "            -local_port <port>\n"
				   "            [-num_ports <integer>]\n");
			exit(0);
		}

		index++;
	}

	if (local_addr == 0)
	{
		printf("Local address not specified\n");
		exit(0);
	}

	if (local_port <= 0 || local_port >= 65535)
	{
		printf("Local port is either not specified or incorrect\n");
		exit(0);
	}

	if (num_ports <= 0 || num_ports > 100)
	{
		printf("Invalid num_ports. It must be in the range [1, 100]\n");
		exit(0);
	}

	aos_assert_r(!aos_tcp_bouncer_create(local_addr, 
				local_port, num_ports), -1);

	while (1)
	{
		aos_sleep(1);
	}

	return 0;
} 

