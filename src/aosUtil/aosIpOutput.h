////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosIpOutput.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aosUtil_ip_output_h
#define aosUtil_ip_output_h

struct sk_buff;
struct rtable;
struct ip_options;

extern int aosCopyDataToSkb(unsigned char *from, 
				 unsigned char *to, 
				 int offset, 
				 int len, 
				 struct sk_buff *skb);

extern int aosUdpSend(void *from,
               int length,
               int transhdrlen,
               int priority,
               struct rtable *rt,
               struct ip_options *opt,
               unsigned int saddr,
               unsigned int daddr,
               unsigned short sport,
               unsigned short dport,
               unsigned int flags,
               unsigned char doChecksum,
               unsigned char identity,
               unsigned char protocol,
               unsigned char tos);
extern int aosCreateSkb(struct sk_buff **skb, 
				 void *from, 
			     int length, 
			     int priority,
			     struct rtable *rt,
			     struct ip_options *opt,
			     unsigned int saddr, 
			     unsigned int daddr,
			     unsigned short sport,
			     unsigned short dport,
			     unsigned int flags, 
			     unsigned char doChecksum, 
			     unsigned char identity, 
			     unsigned char protocol,
			     unsigned char tos);
extern int aosSetIPHeader(struct sk_buff *skb, 
					  unsigned int saddr,
					  unsigned int daddr, 
					  struct ip_options *opt, 
					  struct dst_entry *dst,
					  int identity, 
					  int protocol,
					  int tos,
					  int priority);

#endif
