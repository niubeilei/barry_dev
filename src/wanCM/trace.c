////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: trace.c
// Description:
//   
//
// Modification History:
// 2007-03-07 Created
////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <syslog.h>
#include <time.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <netinet/ip_icmp.h>
#include <netinet/udp.h>

struct rec 
{					/* format of outgoing UDP data */
  u_short	rec_seq;			/* sequence number */
  u_short	rec_ttl;			/* TTL packet left with */
};

#define	BUFSIZE		1500

static void err_print(const char *fmt, va_list ap)
{
	int		errno_save, n;
	char	buf[4096];

	errno_save = errno;		/* value caller might want printed */
	vsnprintf(buf, 4095, fmt, ap);	/* safe */
	n = strlen(buf);
	snprintf(buf + n, 4095 - n, ": %s", strerror(errno_save));
	strcat(buf, "\n");
	fputs(buf, stderr);
	return;
}

void err_quit(const char *fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	err_print(fmt, ap);
	va_end(ap);
	exit(1);
}

int Socket(int family, int type, int protocol)
{
	int		n;

	if ( (n = socket(family, type, protocol)) < 0)
		err_quit("socket error");
	return(n);
}

void sock_set_port(struct sockaddr *sa, socklen_t salen, int port)
{
	struct sockaddr_in	*sin = (struct sockaddr_in *) sa;

	sin->sin_port = port;
	return;
}

void Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (bind(fd, sa, salen) < 0)
		err_quit("bind error");
}

void Setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
	if (setsockopt(fd, level, optname, optval, optlen) < 0)
		err_quit("setsockopt error");
}


void Sendto(int fd, const void *ptr, size_t nbytes, int flags,
	   const struct sockaddr *sa, socklen_t salen)
{
	int error;

	error = sendto(fd, ptr, nbytes, flags, sa, salen);
	printf("error code is %d\n", error);
}

struct addrinfo *
Host_serv(const char *host, const char *serv, int family, int socktype)
{
	int				n;
	struct addrinfo	hints, *res;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_CANONNAME;	/* always return canonical name */
	hints.ai_family = family;		/* 0, AF_INET, AF_INET6, etc. */
	hints.ai_socktype = socktype;	/* 0, SOCK_STREAM, SOCK_DGRAM, etc. */

	if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
		err_quit("host_serv error for %s, %s: %s",
				 (host == NULL) ? "(no hostname)" : host,
				 (serv == NULL) ? "(no service name)" : serv,
				 gai_strerror(n));

	return(res);	/* return pointer to first on linked list */
}

int
recv_v4(struct addrinfo * ai, int recvfd, unsigned short sport, unsigned short dport, int seq)
{
	int				hlen1, hlen2, icmplen, ret;
	socklen_t		len;
	ssize_t			n;
	struct ip		*ip, *hip;
	struct icmp		*icmp;
	struct udphdr	*udp;
	char	 recvbuf[BUFSIZE];
	struct timeval timeout;
	struct sockaddr sa_recv;

	for ( ; ; ) 
	{
		len = ai->ai_addrlen;
		timeout.tv_sec = 3;
		timeout.tv_usec = 0;
		Setsockopt(recvfd, SOL_SOCKET, SO_RCVTIMEO, (void *)&timeout, sizeof(timeout));

		n = recvfrom(recvfd, recvbuf, sizeof(recvbuf), 0, &sa_recv, &len);
		if (n < 0) 
		{
			return -3;
		}

		ip = (struct ip *) recvbuf;	/* start of IP header */
		hlen1 = ip->ip_hl << 2;		/* length of IP header */
	
		icmp = (struct icmp *) (recvbuf + hlen1); /* start of ICMP header */
		if ( (icmplen = n - hlen1) < 8)
			continue;				/* not enough to look at ICMP header */
	
		if (icmp->icmp_type == ICMP_TIMXCEED &&
			icmp->icmp_code == ICMP_TIMXCEED_INTRANS) {
			if (icmplen < 8 + sizeof(struct ip))
				continue;			/* not enough data to look at inner IP */

			hip = (struct ip *) (recvbuf + hlen1 + 8);
			hlen2 = hip->ip_hl << 2;
			if (icmplen < 8 + hlen2 + 4)
				continue;			/* not enough data to look at UDP ports */

			udp = (struct udphdr *) (recvbuf + hlen1 + 8 + hlen2);
 			if (hip->ip_p == IPPROTO_UDP &&
				udp->source == htons(sport) &&
				udp->dest== htons(dport + seq)) {
				ret = -2;		/* we hit an intermediate router */
				break;
			}

		} else if (icmp->icmp_type == ICMP_UNREACH) {
			if (icmplen < 8 + sizeof(struct ip))
				continue;			/* not enough data to look at inner IP */

			hip = (struct ip *) (recvbuf + hlen1 + 8);
			hlen2 = hip->ip_hl << 2;
			if (icmplen < 8 + hlen2 + 4)
				continue;			/* not enough data to look at UDP ports */

			udp = (struct udphdr *) (recvbuf + hlen1 + 8 + hlen2);
 			if (hip->ip_p == IPPROTO_UDP &&
				udp->source == htons(sport) &&
				udp->dest == htons(dport + seq)) {
				if (icmp->icmp_code == ICMP_UNREACH_PORT)
					ret = -1;	/* have reached destination */
				else
					ret = icmp->icmp_code;	/* 0, 1, 2, ... */
				break;
			}
		}
		/* Some other ICMP error, recvfrom() again */
	}
	return(ret);
}

int 
traceloop(char * remote_addr)
{
	int					seq, code ;
	struct rec			*rec;
	int recvfd, sendfd;
	unsigned short sport,dport= 32768 + 666;
	char	 sendbuf[BUFSIZE];
	struct sockaddr sa_bind;
	struct addrinfo *ai;

	ai = Host_serv(remote_addr, NULL, 0, 0);
	if (ai == NULL)
		return -1;
	sendfd = Socket(AF_INET, SOCK_DGRAM, 0);
	if (sendfd < 0)
		return -1;
	bzero(&sa_bind,sizeof(struct sockaddr));
	sa_bind.sa_family = AF_INET;
	sport = (getpid() & 0xffff) | 0x8000;	/* our source UDP port # */
	sock_set_port(&sa_bind, ai->ai_addrlen, htons(sport));
	Bind(sendfd, &sa_bind, ai->ai_addrlen);

	seq=1;
	Setsockopt(sendfd, IPPROTO_IP, IP_TTL, &seq, sizeof(int));

	recvfd = Socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	for (seq = 1; seq<4; seq++) 
	{
		rec = (struct rec *) sendbuf;
		rec->rec_seq = seq;
		rec->rec_ttl = 1;

		sock_set_port(ai->ai_addr, ai->ai_addrlen, htons(dport + seq));
		Sendto(sendfd, sendbuf, sizeof(struct rec), 0, ai->ai_addr, ai->ai_addrlen);

		if ( (code = recv_v4(ai,recvfd, sport, dport, seq)) == -3)
		{
//			printf(" *");		/* timeout, no reply */
		}
		else 
		{
			if (ai != NULL) {
				freeaddrinfo( ai );
				ai = NULL;
			}
			close( recvfd );
			close( sendfd );
			return 0;
		}
	}

	freeaddrinfo( ai );
	close( recvfd );
	close( sendfd );
	return -1;
}
