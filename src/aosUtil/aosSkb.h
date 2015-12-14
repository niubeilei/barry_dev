////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosSkb.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aosUtil_aosSkb_h
#define aosUtil_aosSkb_h

#include <KernelSimu/types.h>


struct sk_buff;
struct rtable;
struct ip_options;

extern int aosAllocSkb(u32 datalen);
extern int aosAllocL4Skb(u32 datalen, 
					u32 layer4len, 
					u32 layer3len, 
					u32 layer2len);
extern int aosCreateSkbs(void *from, 
					unsigned int length, 
					int transhdrlen,
					struct rtable *rt,
					struct ip_options *opt,
					unsigned short dport,
					unsigned int flags, 
					struct sk_buff_head *skbChain);
extern int aosInsertIPHdr(struct sk_buff *skb, 
				   u32 saddr,
				   u32 daddr, 
				   struct ip_options *opt, 
				   struct dst_entry *dst,
				   int identity, 
				   int protocol,
				   int tos,
				   int priority);
extern int aosSetIPHdr(struct sk_buff *skb, 
				    u32 saddr,
				    u32 daddr, 
				    struct ip_options *opt, 
				    struct dst_entry *dst,
				    int identity, 
				    int protocol,
				    int tos,
				    int priority);
extern int aosInsertTcpHdr(struct sk_buff_head *skbs,
				 u32 tcphdrlen,
				 u32 saddr, 
				 u32 daddr,
				 u16 sport,
				 u16 dport,
				 u16 datalen, 
				 u16 seq,
				 u16 ackSeq,
				 u16 doff,
				 u8  fin,
				 u8  ack,
				 u8  syn,
				 u8  rst,
				 u8  urg,
				 u8  ece,
				 u8  cwr,
				 u16 window,
				 u16 urgptr,
				 u8  doChecksum);;
extern int aosSetTcpHdr(struct sk_buff *skbs,
				 u32 tcphdrlen,
				 u32 saddr, 
				 u32 daddr,
				 u16 sport,
				 u16 dport,
				 u16 datalen, 
				 u16 seq,
				 u16 ackSeq,
				 u16 doff,
				 u8  fin,
				 u8  ack,
				 u8  syn,
				 u8  rst,
				 u8  urg,
				 u8  ece,
				 u8  cwr,
				 u16 window,
				 u16 urgptr,
				 u8  doChecksum);
extern int aosInsertUdpHdr(struct sk_buff *skb,
					u32 saddr, 
					u32 daddr,
					u16 sport,
					u16 dport,
					u16 datalen, 
					u16 hdrlen,
					int doChecksum);
extern int aosSetUdpHdr(struct sk_buff_head *skbs,
					unsigned int   transhdrlen,
					unsigned int   saddr, 
					unsigned int   daddr,
					unsigned short sport,
					unsigned short dport,
					unsigned short datalen, 
					int doChecksum);
extern int aosCopyDataToSkb(unsigned char *from, 
				 unsigned char *to, 
				 int offset, 
				 int len, 
				 struct sk_buff *skb);
extern int aosCreateUdpSkb(struct sk_buff **skb, 
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
#endif
