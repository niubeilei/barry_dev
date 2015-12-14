////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: QoSTestUtil.h
// Description:
//   
//
// Modification History:
// 	created: 01/05/2007
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_QoSTester_QoSTestUtil_h
#define Omn_QoSTester_QoSTestUtil_h

#define SRC_IP_OFFSET 12
#define DST_IP_OFFSET 16
#define SRC_PORT_OFFSET 20
#define DST_PORT_OFFSET 22
#define PROTO_OFFSET 9

#define MAX_TEST_TIMES 10000

// extern void qos_rcv_skb(unsigned long);

extern void qos_rcv_skb(int sip_rang, int dip_rang, int sport_rang, int dport_rang, int skb_len_rang);
extern void all_qos_rcv_skb(int sip_rang, int dip_rang, int sport_rang, int dport_rang, int skb_len_rang,int ifid,int mac_rang,int protocol);
extern void qos_rcv_skb1(u32 sip, u16 sport, u32 dip, u16 dport, u8 proto, u8 interface_id, const char *ifname, u32 skb_len);
extern void all_qos_rcv_skb1(u32 sip_rang, u32 dip_rang, u16 sport_rang, u16 dport_rang, u16 skb_len_rang,int ifid,u8 protocol);
#endif

