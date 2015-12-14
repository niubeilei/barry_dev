/*
 * User identify  module entry
 * description: security Task 009,security Task 013
 * author	  :	<a href=mailto:xw_cn@163.com>bill xia</a>
 * directory  :	UserIdentify/user_identify_module.c
 * date		  :	12/22/2006
 * history	  : created by bill xia, 12/22/2006
 */
#include <linux/module.h>
#include "aos/aosKernelApi.h"
#include "KernelSimu/module.h"
#include "KernelSimu/list.h"
#include "UserIdentify/user_identify.h"
#include "UserIdentify/user_identify_cli.h"

#define AOS_SECURITY_CLI_NAME_LEN 128
typedef struct aos_security_cli
{
	char name[AOS_SECURITY_CLI_NAME_LEN];
	int (*invoke)(char*, unsigned int*, struct aosKernelApiParms*, char*, const int);
} aos_security_cli_t;

/*
 * cli list
 */
static aos_security_cli_t clis[] = {
	{"SecurityUserIdentifyAddUser", aos_security_cli_add_user},
	{"SecurityUserIdentifyRemoveUser", aos_security_cli_remove_user},
	{"SecurityUserIdentifyRemoveAllUser", aos_security_cli_remove_all_users},
	{"SecurityUserIdentifyAddUserIp", aos_security_cli_add_user_ip},
	{"SecurityUserIdentifyRemoveUserIp", aos_security_cli_remove_user_ip},
	{"SecurityUserIdentifyRemoveAllUserIp", aos_security_cli_remove_user_all_ips},
	{"SecurityUserIdentifyAddUserMac", aos_security_cli_add_user_mac},
	{"SecurityUserIdentifyRemoveUserMac", aos_security_cli_remove_user_mac},
	{"SecurityUserIdentifyRemoveAllUserMac", aos_security_cli_remove_user_all_macs},
	{"SecurityUserIdentifyShowUser", aos_security_cli_show_user}
};
#define CLI_LEN sizeof(clis)/sizeof(aos_security_cli_t)

/*
 * register cli
 */
static int aos_security_register_cli(void)
{
	int ret = 0;
	int i = 0;
	
	for (i = 0; i <CLI_LEN; i++) {
		ret |= OmnKernelApi_addCliCmd(clis[i].name, clis[i].invoke);	
	}
	
	return ret;
}

/*
 * unregister cli
 */
static int aos_security_unregister_cli(void)
{
	int ret = 0;
	int i = 0;
	
	for (i = 0; i <CLI_LEN; i++) {
		ret |= OmnKernelApi_delCliCmd(clis[i].name);	
	}
	
	return ret;
}


static __init int
aos_security_user_identify_initmodule(void)
{
	AOS_SECURITY_PRINTK("initialize the aos security module.\n");
	aos_security_register_cli();
	
	return 0;
}

static __exit void
aos_security_user_identify_exitmodule(void)
{
	AOS_SECURITY_PRINTK("destroy the aos security module.\n");
	aos_security_unregister_cli();
}

module_init(aos_security_user_identify_initmodule);
module_exit(aos_security_user_identify_exitmodule);

MODULE_AUTHOR("Bill xia <xw_cn@163.com>");
MODULE_LICENSE("GPL");

