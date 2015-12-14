/*
 * User identify CLI module implementation
 * description: security Task 009,security Task 013
 * author	  :	<a href=mailto:xw_cn@163.com>bill xia</a>
 * directory  :	UserIdentify/user_identify_cli.c
 * date		  :	12/21/2006
 * history	  : created by bill xia, 12/21/2006
 */
#include "UserIdentify/user_identify_cli.h"
#include "UserIdentify/user_identify.h"
#include "aos/aosKernelApi.h"
#include <linux/string.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/sched.h>

#define BUFFER_SIZE 128
/*
 * convert the string format to integer in ip
 */
static uint32_t aos_security_inet_addr(char* ip_str)
{
	uint32_t ip = 0;
	
	int a1, a2, a3, a4;
	if (sscanf(ip_str, "%d.%d.%d.%d", &a1, &a2, &a3, &a4) == 4) {
		ip |= a1 << 24;
		ip |= a2 << 16;
		ip |= a3 << 8;
		ip |= a4;
	}

	return ip;
}

/*
 * convert the integer format to string in ip
 */
static char* aos_security_inet_ntoa(uint32_t ip)
{
	unsigned char* p = NULL;
	static char buff[18];
	
	p = (unsigned char*)&ip;
	snprintf(buff, 18, "%d.%d.%d.%d", p[3], p[2], p[1], p[0]);
	
	return buff;
}

/*
 * convert print format to the ethernet mac address
 */
static int aos_security_in_ether(char* bufp, char* dst)
{
	char *ptr;
	int i, j;
	unsigned char val;
	unsigned char c;

	ptr = dst;

	i = 0;
	do {
		j = val = 0;

		/* We might get a semicolon here - not required. */
		if (i && (*bufp == ':')) {
			bufp++;
		}

		do {
			c = *bufp;
			if (((unsigned char)(c - '0')) <= 9) {
				c -= '0';
			} else if (((unsigned char)((c|0x20) - 'a')) <= 5) {
				c = (c|0x20) - ('a'-10);
			} else if (j && (c == ':' || c == 0)) {
				break;
			} else {
				return -1;
			}
			++bufp;
			val <<= 4;
			val += c;
		} while (++j < 2);
		*ptr++ = val;
	} while (++i < 6);
	
	return 0;
}

/*
 * convert the ethernet mac address to print format
 */
static char* aos_security_ether_ntoa(unsigned char *ptr)
{
	static char buff[64];

	snprintf(buff, sizeof(buff), "%02X:%02X:%02X:%02X:%02X:%02X",
			 (ptr[0] & 0377), (ptr[1] & 0377), (ptr[2] & 0377),
			 (ptr[3] & 0377), (ptr[4] & 0377), (ptr[5] & 0377));
	
	return (buff);
}

/*
 * add the ip address by user name
 */
int aos_security_cli_add_user_ip(char *data,
								 unsigned int *length, 
								 struct aosKernelApiParms *parms,
								 char *errmsg, 
								 const int errlen)
{
	char* user_name = parms->mStrings[0];
	char* ip = parms->mStrings[1];
	int ret = 0;
	
	ret = aos_security_add_user_ip(user_name, aos_security_inet_addr(ip));
	switch(ret) {
		case S_SUCCESS:
			*length = 0;
			break;
		case S_USER_NOT_FOUND:
			sprintf(data, "the user name %s is not existed.", user_name);
			*length = strlen(data);
			break;
		case S_SAME_IP:
			sprintf(data, "same ip %s is existed.", ip);
			*length = strlen(data);
			break;
		default:
			*length = 0;
			break;
	}
	
	return 0;
}

/*
 * remove the ip address by user name
 */
int aos_security_cli_remove_user_ip(char *data,
								 	unsigned int *length, 
								 	struct aosKernelApiParms *parms,
								 	char *errmsg, 
								 	const int errlen)
{
	
	char* user_name = parms->mStrings[0];
	char* ip = parms->mStrings[1];
	int ret = 0;
	
	ret = aos_security_remove_user_ip(user_name, aos_security_inet_addr(ip));
	switch(ret) {
		case S_SUCCESS:
			*length = 0;
			break;
		case S_USER_NOT_FOUND:
			sprintf(data, "the user name %s is not existed.", user_name);
			*length = strlen(data);
			break;
		default:
			*length = 0;
			break;
	}
	
	return 0;
}

/*
 * remove all ip addresses by user name
 */
int aos_security_cli_remove_user_all_ips(char *data,
								 		 unsigned int *length, 
								 		 struct aosKernelApiParms *parms,
								 		 char *errmsg, 
								 		 const int errlen)
{
	char* user_name = parms->mStrings[0];
	int ret = 0;
	
	ret = aos_security_remove_user_all_ips(user_name);
	switch(ret) {
		case S_SUCCESS:
			*length = 0;
			break;
		case S_USER_NOT_FOUND:
			sprintf(data, "the user name %s is not existed.", user_name);
			*length = strlen(data);
			break;
		default:
			*length = 0;
			break;
	}
	
	return 0;
}

/*
 * add the mac address by user name
 */
int aos_security_cli_add_user_mac(char *data,
								  unsigned int *length, 
								  struct aosKernelApiParms *parms,
								  char *errmsg, 
								  const int errlen)
{
	char* user_name = parms->mStrings[0];
	char* mac = parms->mStrings[1];
	int ret = 0;
	char* buffer;
	
	buffer = (char*)kmalloc(BUFFER_SIZE, GFP_KERNEL);
	if (buffer == NULL) {
		printk(KERN_ERR "can not allocate memory.\n");
		return -1;
	}
	
	aos_security_in_ether(mac, buffer);
	ret = aos_security_add_user_mac(user_name, buffer);
	kfree(buffer);
	switch(ret) {
		case S_SUCCESS:
			*length = 0;
			break;
		case S_USER_NOT_FOUND:
			sprintf(data, "the user name %s is not existed.", user_name);
			*length = strlen(data);
			break;
		case S_SAME_MAC:
			sprintf(data, "same mac %s is existed.", mac);
			*length = strlen(data);
			break;
		default:
			*length = 0;
			break;
	}
	
	return 0;
}

/*
 * remove the mac address by user name
 */
int aos_security_cli_remove_user_mac(char *data,
								 	 unsigned int *length, 
								 	 struct aosKernelApiParms *parms,
								 	 char *errmsg, 
								 	 const int errlen)
{
	char* user_name = parms->mStrings[0];
	char* mac = parms->mStrings[1];
	int ret = 0;
	
	ret = aos_security_remove_user_mac(user_name, mac);
	switch(ret) {
		case S_SUCCESS:
			*length = 0;
			break;
		case S_USER_NOT_FOUND:
			sprintf(data, "the user name %s is not existed.", user_name);
			*length = strlen(data);
			break;
		default:
			*length = 0;
			break;
	}
	
	return 0;
}

/*
 * remove all mac addresses by user name
 */
int aos_security_cli_remove_user_all_macs(char *data,
								 		  unsigned int *length, 
								 		  struct aosKernelApiParms *parms,
								 		  char *errmsg, 
								 		  const int errlen)
{
	char* user_name = parms->mStrings[0];
	int ret = 0;
	
	ret = aos_security_remove_user_all_macs(user_name);
	switch(ret) {
		case S_SUCCESS:
			*length = 0;
			break;
		case S_USER_NOT_FOUND:
			sprintf(data, "the user name %s is not existed.", user_name);
			*length = strlen(data);
			break;
		default:
			*length = 0;
			break;
	}
	
	return 0;
}

/*
 * add user
 */
int aos_security_cli_add_user(char *data,
							  unsigned int *length, 
							  struct aosKernelApiParms *parms,
							  char *errmsg, 
							  const int errlen)
{
	char* user_name = parms->mStrings[0];
	int ret = 0;
	
	ret = aos_security_add_user(user_name);
	switch(ret) {
		case S_SUCCESS:
			*length = 0;
			break;
		case S_SAME_USER_NAME:
			sprintf(data, "same user name %s is existed.", user_name);
			*length = strlen(data);
			break;
		default:
			*length = 0;
			break;
	}
	
	return 0;
}

/*
 * remove user
 */
int aos_security_cli_remove_user(char *data,
								 unsigned int *length, 
								 struct aosKernelApiParms *parms,
								 char *errmsg, 
								 const int errlen)
{
	char* user_name = parms->mStrings[0];
	int ret = 0;
	
	ret = aos_security_remove_user(user_name);
	switch(ret) {
		case S_SUCCESS:
			*length = 0;
			break;
		case S_USER_NOT_FOUND:
			sprintf(data, "the user name %s is not existed.", user_name);
			*length = strlen(data);
			break;
		default:
			*length = 0;
			break;
	}
	
	return 0;
}

/*
 * remove all users
 */
int aos_security_cli_remove_all_users(char *data,
								 	  unsigned int *length, 
								 	  struct aosKernelApiParms *parms,
								 	  char *errmsg, 
								 	  const int errlen)
{
	aos_security_remove_all_users();
	*length = 0;
	
	return 0;
}

/*
 * show the user information by user name
 */
int aos_security_cli_show_user(char *data,
							   unsigned int *length, 
							   struct aosKernelApiParms *parms,
							   char *errmsg, 
							   const int errlen)
{
	aos_security_user_t* user = NULL;
	aos_security_ip_t* ip_list = NULL;
	aos_security_mac_t* mac_list = NULL;
	char* user_name = parms->mStrings[0];
	char buffer[BUFFER_SIZE];
	int i = 0;
	
	user = aos_security_show_user(user_name);
	if (user == NULL) {
		sprintf(data, "the user name %s is not existed.", user_name);
	} else {
		sprintf(buffer, "User Information\n--------------------\nUser Name: %s\nIP List: ", user_name);
		strcpy(data, "");
		strcpy(data, buffer);
		i = 0;
		list_for_each_entry(ip_list, &(user->ips), list) {
			if (i == 0) {
				strcat(data, aos_security_inet_ntoa(ip_list->ip));
				strcat(data, "\n");
			}
			else {
				strcat(data, "         ");
				strcat(data, aos_security_inet_ntoa(ip_list->ip));
				strcat(data, "\n");	
			}
			i++;
		}
		if (i == 0)
			strcat(data, "\n");

		strcat(data, "Mac List: ");
		i = 0;
		list_for_each_entry(mac_list, &(user->macs), list) {
			if (i == 0) {
				strcat(data, aos_security_ether_ntoa(mac_list->mac));
				strcat(data, "\n");
			}
			else {
				strcat(data, "          ");
				strcat(data, aos_security_ether_ntoa(mac_list->mac));	
			}
			i++;
		}
	}
	*length = strlen(data);
	
	return 0;
}
