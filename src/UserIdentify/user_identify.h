/*
 * User identify module implementation
 * description: security Task 009,security Task 013
 * author	  :	<a href=mailto:xw_cn@163.com>bill xia</a>
 * directory  :	UserIdentify/user_identify.h
 * date		  :	12/21/2006
 * history	  : created by bill xia, 12/21/2006
 */
#include <linux/list.h>
#include <linux/types.h>

#ifndef AOS_USER_IDENTIFY
#define AOS_USER_IDENTIFY

#define AOS_SECURITY_MAX_USER_NAME_LEN 128
#define AOS_SECURITY_ETH_LEN 6	

#define AOS_SECURITY_USER_IDENTIFY_DEBUG

#ifndef AOS_SECURITY_USER_IDENTIFY_DEBUG
#define AOS_SECURITY_PRINTK(format, x...) { \
							printk("In %s:%i\n==>",__FILE__, __LINE__); \
							printk(format, ##x); \
						}
#else
#define AOS_SECURITY_PRINTK(format, x...) { \
						}
#endif
/*
 * status code
 */
enum
{
	S_USER_NOT_FOUND = -10,
	S_USER_FOUND,
	S_IP_NOT_FOUND,
	S_MAC_NOT_FOUND,
	S_SAME_IP,
	S_SAME_MAC,
	S_SAME_USER_NAME,
	S_ERROR = -1,
	S_SUCCESS,
};

/*
 * user account information
 */
typedef struct aos_security_ip 
{
	struct list_head list;
	uint32_t ip;
} aos_security_ip_t;

typedef struct aos_security_mac
{
	struct list_head list;
	uint8_t mac[AOS_SECURITY_ETH_LEN];
} aos_security_mac_t;

typedef struct aos_security_user
{
	struct list_head list;
	char user_name[AOS_SECURITY_MAX_USER_NAME_LEN];
	struct list_head ips;
	struct list_head macs;
} aos_security_user_t;
 
 
 
/*
 * find all user name
 * @users
 *
 * now the function is dummy prototype.
 * I assume the function return the number of users.
 */
extern int get_users(char** users);

/*
 * add the user ip
 * @user_name
 * @ip
 * @return S_SUCCESS if success
 *		   S_USER_NOT_FOUND if the user name is not existed
 *		   S_SAME_IP if the same ip is added already
 */
extern int aos_security_add_user_ip(char* user_name, uint32_t ip);

/*
 * remove the ip by user name
 * @user_name
 * @ip
 * @return S_SUCCESS if success
 *		   S_USER_NOT_FOUND if the user name is not existed
 */
extern int aos_security_remove_user_ip(char* user_name, uint32_t ip);

/*
 * remove the all ip by user name
 * @user_name
 * @return S_SUCCESS if success
 *		   S_USER_NOT_FOUND if the user name is not existed
 */
extern int aos_security_remove_user_all_ips(char* user_name);

/*
 * add the user mac address
 * @user_name
 * @mac
 * @return S_SUCCESS if success
 *		   S_USER_NOT_FOUND if the user name is not existed
 * 		   S_SAME_MAC if the same mac is added already
 */
extern int aos_security_add_user_mac(char* user_name, char* mac);

/*
 * remove the mac address by user name
 * @user_name
 * return S_SUCCESS if success
 *		  S_USER_NOT_FOUND if the user name is not existed
 */
extern int aos_security_remove_user_mac(char* user_name, uint8_t* mac);

/*
 * remove the all mac address by user name
 * @user_name
 * return S_SUCCESS if success
 *		  S_USER_NOT_FOUND if the user name is not existed
 */
extern int aos_security_remove_user_all_macs(char* user_name);

/*
 * show the user information
 * @user_name
 * @return the user information if success,otherwise NULL
 */
extern aos_security_user_t* aos_security_show_user(char* user_name);

/*
 * add user
 * @user_name
 */
extern int aos_security_add_user(char* user_name);

/*
 * remove user
 * @user_name
 */
extern int aos_security_remove_user(char* user_name);

/*
 * remove all users
 */
extern int aos_security_remove_all_users(void);
/*
 * determine user by user ip address or user mac address
 * @user_name
 * @name_len
 * @sender_ip
 * @sender_mac
 * @return S_SUCCESS if success,otherwise S_ERROR
 */
extern int aos_security_determine_user(char* user_name, uint32_t name_len, uint32_t sender_ip, char* sender_mac);

#endif
