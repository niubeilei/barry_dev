/*
 * User identify module implementation
 * description: security Task 009,security Task 013
 * author	  :	<a href=mailto:xw_cn@163.com>bill xia</a>
 * directory  :	UserIdentify/user_identify.c
 * date		  :	12/21/2006
 * history	  : created by bill xia, 12/21/2006
 */

#include "UserIdentify/user_identify.h"
#include <linux/list.h>
#include <linux/types.h>
#include <linux/sched.h>

#define USER_LIST_LOCK() \
		do { \
			up(&user_list_sem); \
		} while (0)

#define USER_LIST_UNLOCK() \
		do { \
			down(&user_list_sem); \
		} while(0)

/* global variables */
static LIST_HEAD(user_list);
static DECLARE_MUTEX(user_list_sem);

/*
 * find all user name
 * @users
 *
 * now the function is dummy prototype.
 * I assume the function return the number of users.
 */
int get_users(char** users)
{
	return 0;
}

/*
 * get the user information by user name
 * @user_name
 * @return the user information if success,otherwise NULL
 */
aos_security_user_t* aos_security_get_user(char* user_name)
{
	aos_security_user_t* tmp = NULL;
	
	USER_LIST_LOCK();
	list_for_each_entry(tmp, &(user_list), list) {
		if ((strcmp(tmp->user_name, user_name)) == 0) {
			return tmp;
		}
	}
	USER_LIST_UNLOCK();
	
	return NULL;
}
/*
 * add the user ip
 * @user_name
 * @ip
 * @return S_SUCCESS if success
 *		   S_USER_NOT_FOUND if the user name is not existed
 *		   S_SAME_IP if the same ip is added already
 */
int aos_security_add_user_ip(char* user_name, uint32_t ip)
{
	aos_security_user_t* user = NULL;
	aos_security_user_t* tmp_user = NULL;
	aos_security_ip_t* ip_list = NULL;
	
	if ((user = aos_security_get_user(user_name)) == NULL) {
		return S_USER_NOT_FOUND;
	}
	
	list_for_each_entry(tmp_user, &(user_list), list) {
		list_for_each_entry(ip_list, &(tmp_user->ips), list) {
			if (ip_list->ip == ip) {
				return S_SAME_IP;
			}
		}
	}
	
	ip_list = NULL;
	ip_list = (aos_security_ip_t*)kmalloc(sizeof(aos_security_ip_t), GFP_KERNEL);
	if (ip_list == NULL) {
		printk(KERN_ERR "can not allocate memory.\n");
		return S_ERROR;
	}
	ip_list->ip = ip;
	USER_LIST_LOCK();
	list_add_tail(&(ip_list->list), &(user->ips));
	USER_LIST_UNLOCK();
	
	return S_SUCCESS;
}

/*
 * remove the ip by user name
 * @user_name
 * @ip
 * @return S_SUCCESS if success
 *		   S_USER_NOT_FOUND if the user name is not existed
 */
int aos_security_remove_user_ip(char* user_name, uint32_t ip)
{
	aos_security_user_t* user = NULL;
	aos_security_ip_t* ip_list = NULL;
	aos_security_ip_t* tmp_ip = NULL;
	
	if ((user = aos_security_get_user(user_name)) == NULL) {
		return S_USER_NOT_FOUND;
	}
	
	USER_LIST_LOCK();
	list_for_each_entry_safe(ip_list, tmp_ip, &(user->ips), list) {
		if (ip_list->ip == ip) {
			list_del(&(ip_list->list));
			USER_LIST_UNLOCK();
			return S_SUCCESS;
		}
	}
	USER_LIST_UNLOCK();
	
	return S_IP_NOT_FOUND;
}

/*
 * remove the all ip by user name
 * @user_name
 * @return S_SUCCESS if success
 *		   S_USER_NOT_FOUND if the user name is not existed
 */
int aos_security_remove_user_all_ips(char* user_name)
{
	aos_security_user_t* user = NULL;
	aos_security_ip_t* ip_list = NULL;
	aos_security_ip_t* tmp_ip = NULL;
	
	if ((user = aos_security_get_user(user_name)) == NULL) {
		return S_USER_NOT_FOUND;
	}
	
	USER_LIST_LOCK();
	list_for_each_entry_safe(ip_list, tmp_ip, &(user->ips), list) {
		list_del(&(ip_list->list));
	}
	USER_LIST_UNLOCK();
	
	return S_SUCCESS;
}

/*
 * add the user mac address
 * @user_name
 * @mac
 * @return S_SUCCESS if success
 *		   S_USER_NOT_FOUND if the user name is not existed
 * 		   S_SAME_MAC if the same mac is added already
 */
int aos_security_add_user_mac(char* user_name, char* mac)
{
	aos_security_user_t* user = NULL;
	aos_security_user_t* tmp_user = NULL;
	aos_security_mac_t* mac_list = NULL;
	
	if ((user = aos_security_get_user(user_name)) == NULL) {
		return S_USER_FOUND;
	}
	
	USER_LIST_LOCK();
	list_for_each_entry(tmp_user, &(user_list), list) {
		list_for_each_entry(mac_list, &(tmp_user->macs), list) {
			if ((strcmp(mac_list->mac, mac)) == 0) {
				return S_SAME_MAC;
			}
		}
	}
	USER_LIST_UNLOCK();
	
	mac_list = NULL;
	mac_list = (aos_security_mac_t*)kmalloc(sizeof(aos_security_mac_t), GFP_KERNEL);
	if (mac_list == NULL) {
		printk(KERN_ERR "can not allocate memeory.\n");
	}
	strcpy(mac_list->mac, mac);
	USER_LIST_LOCK();
	list_add_tail(&(mac_list->list), &(user->macs));
	USER_LIST_UNLOCK();
	
	return S_SUCCESS;
}

/*
 * remove the mac address by user name
 * @user_name
 * return S_SUCCESS if success
 *		  S_USER_NOT_FOUND if the user name is not existed
 */
int aos_security_remove_user_mac(char* user_name, uint8_t* mac)
{
	aos_security_user_t* user = NULL;
	aos_security_mac_t* mac_list = NULL;
	aos_security_mac_t* tmp_mac = NULL;
	
	if ((user = aos_security_get_user(user_name)) == NULL) {
		return S_USER_FOUND;
	}
	
	USER_LIST_LOCK();
	list_for_each_entry_safe(mac_list, tmp_mac, &(user->macs), list) {
		if ((strcmp(mac_list->mac, mac)) == 0) {
			list_del(&(mac_list->list));
			USER_LIST_UNLOCK();
			return S_SUCCESS;
		}
	}
	USER_LIST_UNLOCK();
	
	return S_ERROR;
}

/*
 * remove the all mac address by user name
 * @user_name
 * return S_SUCCESS if success
 *		  S_USER_NOT_FOUND if the user name is not existed
 */
int aos_security_remove_user_all_macs(char* user_name)
{
	aos_security_user_t* user = NULL;
	aos_security_mac_t* mac_list = NULL;
	aos_security_mac_t* tmp_mac = NULL;
	
	if ((user = aos_security_get_user(user_name)) == NULL) {
		return S_USER_FOUND;
	}
	
	USER_LIST_LOCK();
	list_for_each_entry_safe(mac_list, tmp_mac, &(user->macs), list) {
		list_del(&(mac_list->list));
	}
	USER_LIST_UNLOCK();
	
	return S_SUCCESS;
}

/*
 * show the user information
 * @user_name
 * @return the user information if success,otherwise NULL
 */
aos_security_user_t* aos_security_show_user(char* user_name)
{
	return aos_security_get_user(user_name);
}

/*
 * add user
 * @user_name
 */
int aos_security_add_user(char* user_name)
{
	aos_security_user_t* user = NULL;
	
	if ((user = aos_security_get_user(user_name)) != NULL) {
		return S_SAME_USER_NAME;
	}
	
	user = (aos_security_user_t*)kmalloc(sizeof(aos_security_user_t), GFP_KERNEL);
	if (user == NULL) {
		printk(KERN_ERR "can not allocate memory.\n");
		return S_ERROR;
	}
	strcpy(user->user_name, user_name);
	INIT_LIST_HEAD(&(user->macs));
	INIT_LIST_HEAD(&(user->ips));
	USER_LIST_LOCK();
	list_add_tail(&(user->list), &user_list);
	USER_LIST_UNLOCK();
	
	return S_SUCCESS;
}

/*
 * remove user
 * @user_name
 */
int aos_security_remove_user(char* user_name)
{
	aos_security_user_t* user = NULL;
	aos_security_ip_t* ip_list = NULL;
	aos_security_ip_t* tmp_ip = NULL;
	aos_security_mac_t* mac_list = NULL;
	aos_security_mac_t* tmp_mac = NULL;
	
	if ((user = aos_security_get_user(user_name)) == NULL) {
		return S_USER_NOT_FOUND;
	}
	
	USER_LIST_LOCK();
	list_for_each_entry_safe(ip_list, tmp_ip, &(user->ips), list) {
		list_del(&(ip_list->list));
		kfree(ip_list);
	}
	list_for_each_entry_safe(mac_list, tmp_mac, &(user->macs), list) {
		list_del(&(mac_list->list));
		kfree(mac_list);
	}
	list_del(&(user->list));
	kfree(user);
	USER_LIST_UNLOCK();
	
	return S_SUCCESS;
}

/*
 * remove all users
 */
int aos_security_remove_all_users(void)
{
	aos_security_user_t* user = NULL;
	aos_security_user_t* tmp_user = NULL;
	
	USER_LIST_LOCK();
	list_for_each_entry_safe(user, tmp_user, &(user_list), list) {
		list_del(&(user->list));
		aos_security_remove_user(user->user_name);
		kfree(user);
	}
	USER_LIST_UNLOCK();
	
	return S_SUCCESS;
}

/*
 * determine user by user ip address or user mac address
 * @user_name
 * @name_len
 * @sender_ip
 * @sender_mac
 * @return S_SUCCESS if success,otherwise S_ERROR
 */
int aos_security_determine_user(char* user_name, uint32_t name_len, uint32_t sender_ip, char* sender_mac)
{
	aos_security_user_t* user = NULL;
	aos_security_ip_t* ip_list = NULL;
	aos_security_mac_t* mac_list = NULL;
	uint32_t len = 0;
	
	if (sender_mac != NULL) {
		list_for_each_entry(user, &(user_list), list) {
			list_for_each_entry(mac_list, &(user->macs), list) {
				if ((strcmp(mac_list->mac, sender_mac)) == 0) {
					len = strlen(user->user_name);
					if (len > name_len) {
						strncpy(user_name, user->user_name, name_len);
					} else {
						strcpy(user_name, user->user_name);
					}
					return S_SUCCESS;
				}
			}
		}
	}
	if (sender_ip != 0) {
		list_for_each_entry(user, &(user_list), list) {
			list_for_each_entry(ip_list, &(user->ips), list) {
				if (ip_list->ip == sender_ip) {
					len = strlen(user->user_name);
					if (len > name_len) {
						strncpy(user_name, user->user_name, name_len);
					} else {
						strcpy(user_name, user->user_name);
					}
					return S_SUCCESS;
				}
			}
		}
	}
	
	return S_ERROR;
}
