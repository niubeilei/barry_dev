
#include <linux/errno.h>
#include "aosUtil/StringUtil.h"
#include "QoS/qos_global.h"
#include "QoS/qos_rule.h"

#ifdef __KERNEL__
#define qos_atoi(str) simple_strtoul(((str != NULL) ? str : ""), NULL, 0)
#else
#include <stdlib.h>
#define qos_atoi(str) atoi(str)
#endif

int is_status_valid(char *status,
					int *set,
					char *errmsg)
{
	int ret;
	ret = 0;
	if (strcmp(status, "on") == 0) {
		*set = AOS_QOS_TRAFFIC_STATUS_ON;
	} else if (strcmp(status, "off") == 0) {
		*set = AOS_QOS_TRAFFIC_STATUS_OFF;
	} else if (strcmp(status, "stop") == 0) {
		*set = AOS_QOS_TRAFFIC_STATUS_STOP;
	} else if (strcmp(status, "ready") == 0) {
		ret = -1;
	} else {
		sprintf(errmsg, "Invalid parmseter: %s", status);
		ret = -1;
	}
	return ret;
}


int is_direction_valid(char *direction,int *value,char *errmsg)
{
	int ret;

	ret = 0;

	if (strcmp(direction,"in") == 0) {
		*value = direction_in;
	}else if (strcmp(direction,"out")== 0) {
		*value = direction_out;
	}else {
		ret = -EINVAL;
	}
	return ret;
}


int is_valid_proto(char *proto_type,u8 *proto)
{
	if (strcmp("tcp",proto_type)== 0) {
		*proto = AOS_QOS_TCP;
	} else if (strcmp("udp",proto_type) == 0){
		*proto = AOS_QOS_UDP;
	} else {
		*proto = AOS_QOS_NONE;
		return -EINVAL;
	}
	return 0;
}

/*
static int change_char_to_num(char *start_pos,char *end_pos,u8 *num)
{
	char *local_start;
	u16 value;

	value = 0;


	if (start_pos == end_pos || end_pos - start_pos > 4) {
		return -EINVAL;
	}
	local_start = start_pos + 1;
	while (local_start != end_pos) {
		if ((*local_start >= '0') && (*local_start <= '9')) {
			value = value * 10 + (*local_start - '0');
			++local_start;
		} else {
			return -EINVAL;
		}	
	}

	if (value > 32) {
		return -EINVAL;
	}

	*num = value;
	return 0;
}
*/
//Need to utilty
int qos_inet_addr(char* ip_str, u32 *ip)
{	
	int a1, a2, a3, a4;
	static char token[] = "255.255.255.255"; 
	
	if (strlen(ip_str) > strlen(token))
		return -EINVAL;
	 
	if (sscanf(ip_str, "%d.%d.%d.%d", &a1, &a2, &a3, &a4) == 4) {
		*ip = (a1 << 24) + (a2 << 16) + (a3 << 8) + a4;
		*ip = htonl(*ip);
	} else {
		return -EINVAL;
	}
	
	return 0;
}

int is_valid_ip_with_mask_bit(char* raw,u32 *src_ip,u8 *mask_bit)

{
	char* p;
	char* q;
	char* r;
	
	p = raw;
	q = strchr(p, '/');
	if (q != NULL) {
		*q = '\0';
		if (qos_inet_addr(p, src_ip) == -EINVAL)
			return -EINVAL;
		
		q++;
		if (*q == '\0')
			return -EINVAL;
		r = q;
		while (*r != '\0') {
			if (*r < '0' || *r > '9')
				return -EINVAL;
			r++;
		} 
		if ((*mask_bit = qos_atoi(q)) > 32)
			return -EINVAL;
	} else {
		if (qos_inet_addr(p, src_ip) == -EINVAL)
			return -EINVAL;
		*mask_bit = 0;
	}

	return 0;
/*
	char *start_positon;
	char *pos;
	int raw_length;
	int split_pos[5];
	u8 ip_raw[4];
	int ret;
	int i;

	pos = start_positon = raw;
	raw_length = strlen(raw);
	pos = strchr(pos,'/');       //Reviewed strchr
	if (pos != NULL){
		ret = change_char_to_num(pos,start_positon + raw_length,mask_bit);
		if (ret != 0) {
			return -EINVAL;
		}
		if (*mask_bit > 32) {
			return -EINVAL;
		}
		split_pos[4] = pos - start_positon;
	} else {
		split_pos[4] = raw_length;
		*mask_bit = 0;
	}

	pos = start_positon;
	split_pos[0] = -1;

	for (i = 0; i < 3; ++i) {
		pos = strstr(pos,".");
		if (pos == NULL) {
			return -EINVAL;
		}
		split_pos[i + 1] = pos - start_positon;
		++pos;
	}


	for (i = 0; i < 4; ++i) {
		ret = change_char_to_num(start_positon + split_pos[i],start_positon + split_pos[i + 1],ip_raw + i);
		if (ret != 0) {
			return -EINVAL;
		}
	}

	memcpy(src_ip,ip_raw,4);

	return 0;
*/
}


int is_valid_port_without_wild(int raw, u16 *port)
{
	if (raw > 0 && raw <= 0xFFFF) {
		*port = raw;
		return 0;
	}

	return -EINVAL;
}
//Reviewed 

int is_valid_port_with_wild(int raw_port,u16 *port)
{
	int ret;

	ret = is_valid_port_without_wild(raw_port,port);

	if (ret != 0 && raw_port == 0) {
		*port = 0;
		return 0;
	}

	return ret;
}


int is_valid_ip_address(int raw,u32 *src_ip)
{
	*src_ip = raw;

	return 0;
}



/*
int is_valid_past_time(int raw,u16 *past_time)
{
	if (raw < 0) {
		*past_time = -1 * raw;
		return 0;
	}	

	if (raw == 0) {
		*past_time = 0;
		return 0;
	}
	return -EINVAL;
} */

int is_valid_past_time(char *raw,u16 *past_time)
{
	int value;
	char* p;

	p = raw;
	if (*p == '\0') {
		return -EINVAL;
	}
	while (*p != '\0') {
		if (*p < '0' || *p > '9')
			return -EINVAL;
		p++;
	}
	value = qos_atoi(raw);
	if (value < 0)
		return -EINVAL;
	*past_time = value;
	
	return 0;
	/*
	value = 0;
	switch (*raw) {
		case '-':
			pos = (raw + 1);
			while (*pos != '\0') {
				if (*pos <= '9' && *pos >= '0') {
					value = 10 * value + *pos - '0';
				} else {
					return -EINVAL;
				}
				++pos;  //BUG
			}
			*past_time = value;
			break;
		case '0':
			pos = raw + 1;
			if (*pos == '\0') {
				*past_time = value;
				return 0;
			} else {
				return -EINVAL;
			}
		default:
			return -EINVAL;
	}
	return 0;
	*/
}

int is_valid_interval(int raw,u16 *interval)
{

	if (raw >= 0) {
		*interval = raw;
		return 0;
	}

	return -EINVAL;
}


int is_valid_dev_name(char *dev_name,int *devid)
{
	struct net_device *ndv = NULL;

	ndv = dev_get_by_name(dev_name); 
	if (ndv == NULL) {
		return -EINVAL;
	}
	*devid = ndv->ifindex;
	dev_put(ndv);
	return 0;
}


int is_valid_dev_direction(char *raw, u8 *direction)
{
	int ret;
	int flag;

	ret = 0;
	*direction = 0;
	flag = 0;

	if (raw == NULL) {
		*direction = monitor_direction_both;
		return 0;
	}

	if ((strcmp(raw,"in") == 0)) {
		*direction |= monitor_direction_in;
		++flag;
	}

	if ((strcmp(raw,"out") == 0)) {
		*direction |= monitor_direction_out;
		++flag;
	}


	if (flag == 0) {
		ret = -EINVAL;
	}

	return ret;
}



int find_app_in_list(char *app)
{
	return 0;
}


u8 change_char_to_u8(char x)
{
	u8 ret = 20;

	if (x >= '0' && x <= '9') {
		ret = x - '0';
		goto out;
	}
	if (x >= 'A' && x <= 'F') {
		ret = x - 'A' + 10; 
		goto out;
	}
	if (x >= 'a' && x <= 'f') {
		ret = x - 'a' + 10; 
		goto out;
	}
	if (ret > 15) {
		QOS_RULE_PRINTK("Ivalid MAC bit\n");
	}

out:
	QOS_RULE_PRINTK("Rule %c\n",x);
	QOS_RULE_PRINTK("Ret %u\n",ret);
	return ret;
}


int is_valid_priority(char *priority,u8 *level)
{
	*level = AOS_QOS_RULE_PRIORITY_LOW;

	if (strcmp("high",priority)==0) {
		QOS_RULE_PRINTK("High priority detected\n");
		*level = AOS_QOS_RULE_PRIORITY_HIGH;
	} else if (strcmp("medium",priority) == 0) {
		QOS_RULE_PRINTK("Medium priority detected\n");
		*level = AOS_QOS_RULE_PRIORITY_MEDIUM;
	} else if (strcmp("low",priority) == 0) {
		QOS_RULE_PRINTK("Low priority detected\n");
		*level = AOS_QOS_RULE_PRIORITY_LOW;
	} else {
		QOS_RULE_PRINTK("Ivalided priority detected\n");
		return -EINVAL;
	}
	return 0;
}


int is_valid_app(char *app)
{
	int inlst;

	inlst = find_app_in_list(app);    //0 in
	if (inlst != 0) {
		return -EINVAL;
	}
	return 0;
}


int is_valid_direction(char *ifsending,u8 *direction)
{
	if (strcmp("sending",ifsending)==0) {
		*direction = AOS_QOS_RULE_DIRECTION_SENDING;
	} else if (strcmp("receiving",ifsending) == 0) {
		*direction = AOS_QOS_RULE_DIRECTION_RECEIVING;
	} else {
		return -EINVAL;
	}
	return 0;
}


int is_valid_vlantag(u16 tag)
{
	if ((tag & 0xf000) != 0 ) {
		return -EINVAL;
	}
	return 0;
}


int is_valid_if(char *dev_name,int *devid)
{
	struct net_device *ndv = NULL;

	ndv = dev_get_by_name(dev_name); 
	if (ndv == NULL) {
		return -EINVAL;
	}
	*devid = ndv->ifindex;
	dev_put(ndv);
	return 0;
}

int is_valid_mac_address(char* str,u8 *mac)
{
	int i;
	int count = 0;
	int maci;
	u8 tmp;

	tmp = 0;
	maci = 0;
	memset(mac,0,ETH_ALEN);
	if (strlen(str) != 17) {
		QOS_RULE_PRINTK("mac string length %d\n",strlen(str));
		return -EINVAL;
	}
	for (i = 0; i < 17; ++i) {
		QOS_RULE_PRINTK("input char %c\n",str[i]);
		switch (i) {
			case 2:
			case 5:
			case 8:
			case 11:
			case 14:
				QOS_RULE_PRINTK("Should be:\n");
				if (str[i] != ':') {
					return -EINVAL;
				}
				break;
			default:
				tmp = change_char_to_u8(str[i]);
				QOS_RULE_PRINTK("char changed to interger %u\n",tmp);
				if (tmp > 15) {
					return -EINVAL;
				}
				++count;
				mac[maci] = (mac[maci] << 4) + tmp;
				QOS_RULE_PRINTK("cell %u\n",tmp);
		}
		if (count == 2) {
			++maci;
			count = 0;
		}
	}
	return 0;
}
