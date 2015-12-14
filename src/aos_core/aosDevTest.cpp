////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosDevTest.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos_core/aosDevTest.h"

#include <KernelSimu/skbuff.h>
#include "aos/aosCorePcp.h"

int netif_receive_skb(struct sk_buff *skb)
{
	if (aosNetworkInputPcpStatus && 
		aosNetInputPcp_proc(skb) == eAosRc_PktConsumed)
	{
	 	return 0;
	}

	kfree_skb(skb);
	return 0;
}

