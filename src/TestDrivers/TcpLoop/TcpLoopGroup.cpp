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
// 08/05/2007	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TcpLoop/TcpLoopGroup.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"


OmnTcpLoopGroup::OmnTcpLoopGroup(const OmnIpAddr &remoteAddr, 
					   const OmnIpAddr &remoteMask, 
					   const OmnIpAddr &clientAddr, 
					   const OmnIpAddr &clientMask, 
					   const OmnIpAddr &serverAddr, 
					   const OmnIpAddr &serverMask, 
					   const u32 numLoops,
					   const OmnTcpLoop::BounceMethod method, 
					   const u32 sendBlockSize, 
					   const int sendSize,
					   const u32 sendWaitSec, 
					   const u32 sendWaitUsec)
{
	if (numLoops <= 0 || numLoops > eMaxLoops)
	{
		OmnAlarm << "Number of Loops incorrect: " << numLoops 
			<< ". Maximum allowed: " << eMaxLoops << enderr;
		return;
	}

	for (u32 i=0; i<numLoops; i++)
	{
		while (1)
		{
			OmnIpAddr saddr = aos_next_ip(serverAddr, serverMask);
			u32 sport = aos_next_u32(5000, 50000);
			OmnIpAddr caddr = aos_next_ip(clientAddr, clientMask);
			u32 cport = aos_next_u32(5000, 50000);
			OmnIpAddr raddr = aos_next_ip(remoteAddr, remoteMask);
			u32 rport = aos_next_u32(5000, 50000);

			bool duplicate = false;
			for (u32 j=0; j<i; j++)
			{
				if (mLoops[j]->getServerPort() == sport ||
					mLoops[j]->getClientPort() == cport)
				{
					duplicate = true;
					break;
				}
			}

			if (!duplicate)
			{
				mLoops.append(OmnNew OmnTcpLoop(raddr, rport, 
								caddr, cport, saddr, sport, method, 
								sendBlockSize, sendSize, 
								sendWaitSec, sendWaitUsec));
				if (!mLoops[i])
				{
					OmnAlarm << "Failed to allocate object" << enderr;
					return;
				}
				break;
			}
		}
	}

	return;
}


OmnTcpLoopGroup::~OmnTcpLoopGroup()
{
}


bool
OmnTcpLoopGroup::connect(OmnString &errmsg)
{
	for (int i=0; i<mLoops.entries(); i++)
	{
		if (!mLoops[i]->connect(errmsg))
		{
			OmnAlarm << "Failed to connect: " << i << ", " << errmsg << enderr;
			return false;
		}
	}

	return true;
}


bool
OmnTcpLoopGroup::start()
{
	for (int i=0; i<mLoops.entries(); i++)
	{
		if (!mLoops[i]->start())
		{
			OmnAlarm << "Failed to start: " << i << enderr;
			return false;
		}
	}

	return true;
}


u64 
OmnTcpLoopGroup::getTotalReceived() const
{
	u64 total = 0;
	for (int i=0; i<mLoops.entries(); i++)
	{
		total += mLoops[i]->getTotalReceived();
	}

	return total;
}


u64 
OmnTcpLoopGroup::getLastTotal() const
{
	u64 total = 0;
	for (int i=0; i<mLoops.entries(); i++)
	{
		total += mLoops[i]->getLastTotal();
	}

	return total;
}


void
OmnTcpLoopGroup::resetLastTotal()
{
	for (int i=0; i<mLoops.entries(); i++)
	{
		mLoops[i]->resetLastTotal();
	}
}

