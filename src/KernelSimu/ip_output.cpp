////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ip_output.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "KernelSimu/ip_output.h"

#include "aos/aosKernelAlarm.h"
#include "KernelSimu/ip.h"


void ip_send_check(struct iphdr *iph)
{
	iph->check = 0;

	aosAlarmStr(eAosAlarm, "ip_fast_csum() is called");
	// iph->check = ip_fast_csum((unsigned char *)iph, iph->ihl);
}

