#include "aosUtil/Types.h"
#include "QoS/QoSTester/QosTestUtil.h"
#include "KernelSimu/skbuff.h"
#include "KernelSimu/netdevice.h"
#include "KernelSimu/if_ether.h"
#include "KernelSimu/ip.h"
#include "Thread/Thread.h"
#include "aosUtil/Memory.h"
#include "aosUtil/Alarm.h"
#include "QoS/qos_global.h"

static struct sk_buff* create_skb_buff(int buffer_len)
{
	struct sk_buff *skb;

	skb = (struct sk_buff*)aos_malloc(sizeof(struct sk_buff)+buffer_len);
	if (!skb) return NULL;

	skb->dev = (struct net_device*) aos_malloc(sizeof(struct net_device));
	if(!(skb->dev))
	{
		aos_free(skb);
		return NULL;
	}

	skb->head = (u8*)skb + sizeof(struct sk_buff);
	skb->tail = skb->head + buffer_len;
	skb->end = skb->tail;
	skb->data = skb->head + ETH_HLEN;

	skb->mac.raw = skb->head;

	return skb;
}

static inline void release_skb_buff(struct sk_buff *skb)
{
	aos_free(skb->dev);
	aos_free(skb);
}

static int set_skb(
	struct sk_buff *skb, 
	int if_id, 
	u8 *ifname,
	u8 *src_mac, 
	u8 *dst_mac, 
	u32 src_ip, 
	u32 dst_ip, 
	u16 src_port, 
	u16 dst_port, 
	u8 proto,
	int skb_len)
{
	struct ethhdr *head;
	struct iphdr *iphead;
	// set proto
	skb->data[PROTO_OFFSET] = proto;

	// set src ip, dst ip, src port, dst port
	memcpy(skb->data+SRC_IP_OFFSET, &src_ip, sizeof(u32));
	memcpy(skb->data+DST_IP_OFFSET, &dst_ip, sizeof(u32));
	memcpy(skb->data+SRC_PORT_OFFSET, &src_port, sizeof(u16));
	memcpy(skb->data+DST_PORT_OFFSET, &dst_port, sizeof(u16));

	iphead = (struct iphdr*)skb->data;
	iphead->ihl = 5;
	
	//set skb len
	skb->len =  skb_len;

	// set src mac and dst mac
	head = eth_hdr(skb);
	memcpy(head->h_source, src_mac, ETH_ALEN);
	memcpy(head->h_dest, dst_mac, ETH_ALEN);
	head->h_proto = 0x0008; // ip protocol

	// set interface name and id
	strcpy(skb->dev->name, (const char*)ifname);
	skb->dev->ifindex = if_id;

	return eAosRc_Success;
}
void all_qos_rcv_skb(int sip_rang, int dip_rang, int sport_rang, int dport_rang, int skb_len_rang,int ifid,int mac_rang,int protocol)
{
	struct sk_buff *skb;
	int interface;
	in_addr_t addr;
	u8 src_mac[ETH_ALEN], dst_mac[ETH_ALEN];
	u8  ifname[IFNAMSIZ] = "eth0";
	u32 src_ip, dst_ip;
	u16 src_port, dst_port;
	u8 proto;
	int skb_len;

	memset(ifname,0,IFNAMSIZ);
	switch (ifid) {
		case 0:
			strcpy((char *)ifname,"eth0");
			break;
		case 1:
			strcpy((char *)ifname,"eth1");
			break;
		case 2:
			strcpy((char *)ifname,"eth2");
			break;
		case 3:
			strcpy((char *)ifname,"eth3");
			break;
		case 4:
			strcpy((char *)ifname,"eth4");
			break;
		case 5:
			strcpy((char *)ifname,"eth5");
			break;
		case 6:
			strcpy((char *)ifname,"eth6");
			break;
		case 7:
			strcpy((char *)ifname,"eth7");
			break;
		default:
			break;
	}
	memset(src_mac,0,ETH_ALEN);
	memset(dst_mac,1,ETH_ALEN);
	interface = ifid;
    src_ip = inet_addr("192.168.0.1")+htonl(sip_rang);
	dst_ip = inet_addr("192.168.0.2")+htonl(dip_rang);

	src_port = htons(180+sport_rang);
	dst_port = htons(80+dport_rang);

	proto = protocol;
	skb_len = skb_len_rang;
	src_mac[ETH_ALEN - 1] += mac_rang;

	skb = create_skb_buff(ETH_FRAME_LEN);
	if (!skb)
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "failed to create skb buffer");
		return;
	}

	set_skb(skb, interface, ifname, src_mac, dst_mac, src_ip, dst_ip, src_port, dst_port, proto, skb_len);

//	aos_trace("receive packet if_id = %d, ifname = %s, src_ip = 0x%02x," 
//			   "dst_ip = 0x%02x, src_port = %d, dst_port = %d, proto = %d, skb_len = %d",
//			   interface, ifname, src_ip, dst_ip, src_port, dst_port, proto, skb_len);

	qos_fnin(0, &skb, NULL, NULL, NULL); 

	release_skb_buff(skb);
}
void all_qos_rcv_skb1(u32 sip_rang, u32 dip_rang, u16 sport_rang, u16 dport_rang, u16 skb_len_rang,int ifid,u8 protocol)
{
	struct sk_buff *skb;
	struct ros_qos_skb_info skb_info;
	int interface;
	in_addr_t addr;
	u8 src_mac[ETH_ALEN], dst_mac[ETH_ALEN];
	u8  ifname[IFNAMSIZ] = "eth0";
	u32 src_ip, dst_ip;
	u16 src_port, dst_port;
	u8 proto;
	int mac_rang = 0, skb_len, ret;

	memset(ifname,0,IFNAMSIZ);
	switch (ifid) {
		case 0:
			strcpy((char *)ifname,"eth0");
			break;
		case 1:
			strcpy((char *)ifname,"eth1");
			break;
		case 2:
			strcpy((char *)ifname,"eth2");
			break;
		case 3:
			strcpy((char *)ifname,"eth3");
			break;
		case 4:
			strcpy((char *)ifname,"eth4");
			break;
		case 5:
			strcpy((char *)ifname,"eth5");
			break;
		case 6:
			strcpy((char *)ifname,"eth6");
			break;
		case 7:
			strcpy((char *)ifname,"eth7");
			break;
		default:
			break;
	}
	memset(src_mac,0,ETH_ALEN);
	memset(dst_mac,1,ETH_ALEN);
	interface = ifid;
   /* src_ip = htonl(sip_rang);
	dst_ip = htonl(dip_rang);

	src_port = htons(sport_rang);
	dst_port = htons(dport_rang);*/

	src_ip = sip_rang;
	dst_ip = dip_rang;
	src_port = sport_rang;
	dst_port = dport_rang;

	proto = protocol;
	skb_len = skb_len_rang;
	src_mac[ETH_ALEN - 1] += mac_rang;

	skb = create_skb_buff(ETH_FRAME_LEN);
	if (!skb)
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "failed to create skb buffer");
		return;
	}

	set_skb(skb, interface, ifname, src_mac, dst_mac, src_ip, dst_ip, src_port, dst_port, proto, skb_len);

//	aos_trace("receive packet if_id = %d, ifname = %s, src_ip = 0x%02x," 
//			   "dst_ip = 0x%02x, src_port = %d, dst_port = %d, proto = %d, skb_len = %d",
//			   interface, ifname, src_ip, dst_ip, src_port, dst_port, proto, skb_len);

	qos_fnin(0, &skb, NULL, NULL, NULL); 

	skb->data = skb->head;
//aos_trace("skb %x skb->len %d", skb, skb->len);
	u8 priority;

	ret = qos_traffic_parse_parm(skb, &skb_info.src_ip, &skb_info.dst_ip, 
								 &skb_info.src_port, &skb_info.dst_port, &skb_info.protocol, 
								 (char*)skb_info.dev_name, &skb_info.ifid, skb_info.src_mac, skb_info.dst_mac, 
								 &skb_info.vlan, &skb_info.len, 1);
	if (ret == 0) 
		skb_info.mac_proto = 0x0008;
	else 
		skb_info.mac_proto = 0;
	
	qos_bandwidth_monitor(&skb_info, 1, &priority);
	u32 band;
	
	//qos_get_channel_bw(ntohl(src_ip), ntohs(src_port), ntohl(dst_ip), ntohs(dst_port), proto, &band);
	qos_get_channel_bw(src_ip, src_port, dst_ip, dst_port, proto, &band);
	
//	aos_trace("band is %u", band);
	release_skb_buff(skb);
}

void qos_rcv_skb(int sip_rang, int dip_rang, int sport_rang, int dport_rang, int skb_len_rang)
{
	struct ros_qos_skb_info skb_info;
	struct sk_buff *skb;
	int interface;
	in_addr_t addr;
	u8 src_mac[ETH_ALEN], dst_mac[ETH_ALEN];
	u8 ifname[IFNAMSIZ] = "eth0";
	u32 src_ip, dst_ip, band = 0;
	u16 src_port, dst_port;
	u8 proto, priority;
	int skb_len, ret;


	interface = 0;
    src_ip = inet_addr("192.168.0.1")+htonl(sip_rang);
	dst_ip = inet_addr("192.168.0.2")+htonl(dip_rang);

	src_port = htons(180+sport_rang);
	dst_port = htons(80+dport_rang);

	proto = AOS_QOS_TCP;
	skb_len = skb_len_rang;
	
	skb = create_skb_buff(ETH_FRAME_LEN);
	if (!skb)
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "failed to create skb buffer");
		return;
	}

	set_skb(skb, interface, ifname, src_mac, dst_mac, src_ip, dst_ip, src_port, dst_port, proto, skb_len);

//	aos_trace("receive packet if_id = %d, ifname = %s, src_ip = 0x%02x," 
//			   "dst_ip = 0x%02x, src_port = %d, dst_port = %d, proto = %d, skb_len = %d",
//			   interface, ifname, src_ip, dst_ip, src_port, dst_port, proto, skb_len);

	qos_fnin(0, &skb, NULL, NULL, NULL); 
	skb->data = skb->head;
//aos_trace("skb %x skb->len %d", skb, skb->len);

	ret = qos_traffic_parse_parm(skb, &skb_info.src_ip, &skb_info.dst_ip,
	                             &skb_info.src_port, &skb_info.dst_port, &skb_info.protocol,
	                             (char*)skb_info.dev_name, &skb_info.ifid, skb_info.src_mac, skb_info.dst_mac,
	                             &skb_info.vlan, &skb_info.len, 1);
    if (ret == 0)
		skb_info.mac_proto = 0x0008;
	else
		skb_info.mac_proto = 0;

	qos_bandwidth_monitor(&skb_info, 1, &priority);
	
	//qos_get_channel_bw(ntohl(src_ip), ntohs(src_port), ntohl(dst_ip), ntohs(dst_port), proto, &band);
	qos_get_channel_bw(src_ip, src_port, dst_ip, dst_port, proto, &band);
	
//	aos_trace("band is %u", band);
	release_skb_buff(skb);
}


void qos_rcv_skb1(u32 sip, u16 sport, u32 dip, u16 dport, u8 proto, u8 interface_id, const char *ifname, u32 skb_len)
{
	struct sk_buff *skb;
	u8 src_mac[ETH_ALEN], dst_mac[ETH_ALEN];

	skb = create_skb_buff(ETH_FRAME_LEN);
	if (!skb)
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "failed to create skb buffer");
		return;
	}

	set_skb(skb, interface_id, (u8*)ifname, src_mac, dst_mac, sip, dip, sport, dport, proto, skb_len);

//	aos_trace("receive packet if_id = %d, ifname = %s, src_ip = 0x%02x," 
//			   "dst_ip = 0x%02x, src_port = %d, dst_port = %d, proto = %d, skb_len = %d",
//			   interface, ifname, src_ip, dst_ip, src_port, dst_port, proto, skb_len);

	qos_fnin(0, &skb, NULL, NULL, NULL); 

	release_skb_buff(skb);
}

