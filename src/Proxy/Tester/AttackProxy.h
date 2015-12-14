////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AttackProxy.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_TcpProxy_Tester_Attack_h
#define Aos_TcpProxy_Tester_Attack_h

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


#define DELIMITER "@"       /* to separate ips, e.g. host1@host2@host3 */
#define getrandom(min, max) ((rand() % (int)(((max)+1) - (min))) + (min))
#define WAITAMOMENT nanosleep(&small,NULL)
#define CHLD_MAX 6
#define DELIMITER "@"

extern void syn (int, u_long, u_short);

//                    from_ip    to_ip      port
extern void commence_syn (char *, char *, int);
extern void commence_fin (char *, char *, int);
extern void commence_rst (char *, char *, int);
extern void commence_ack (char *, char *, int);

extern void commence_icmp (char *, char *);

extern void commence_udp (char *, char *, int);
//struct timespec small = {0, 1337};
#endif
