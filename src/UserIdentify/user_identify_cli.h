/*
 * User identify CLI module implementation
 * description: security Task 009,security Task 013
 * author	  :	<a href=mailto:xw_cn@163.com>bill xia</a>
 * directory  :	UserIdentify/user_identify_cli.h
 * date		  :	12/21/2006
 * history	  : created by bill xia, 12/21/2006
 */
#include "aos/aosKernelApi.h"

/*
 * add the ip address by user name
 */
extern int aos_security_cli_add_user_ip(char *data, unsigned int *length, struct aosKernelApiParms *parms, char *errmsg, const int errlen);

/*
 * remove the ip address by user name
 */
extern int aos_security_cli_remove_user_ip(char *data, unsigned int *length, struct aosKernelApiParms *parms, char *errmsg, const int errlen);

/*
 * remove all ip addresses by user name
 */
extern int aos_security_cli_remove_user_all_ips(char *data, unsigned int *length, struct aosKernelApiParms *parms, char *errmsg, const int errlen);

/*
 * add the mac address by user name
 */
extern int aos_security_cli_add_user_mac(char *data, unsigned int *length, struct aosKernelApiParms *parms, char *errmsg, const int errlen);

/*
 * remove the mac address by user name
 */
extern int aos_security_cli_remove_user_mac(char *data, unsigned int *length, struct aosKernelApiParms *parms, char *errmsg, const int errlen);

/*
 * remove all mac addresses by user name
 */
extern int aos_security_cli_remove_user_all_macs(char *data, unsigned int *length, struct aosKernelApiParms *parms, char *errmsg, const int errlen);

/*
 * add user
 */
extern int aos_security_cli_add_user(char *data, unsigned int *length, struct aosKernelApiParms *parms, char *errmsg, const int errlen);

/*
 * remove user
 */
extern int aos_security_cli_remove_user(char *data, unsigned int *length, struct aosKernelApiParms *parms, char *errmsg, const int errlen);

/*
 * remove all users
 */
extern int aos_security_cli_remove_all_users(char *data, unsigned int *length, struct aosKernelApiParms *parms, char *errmsg, const int errlen);

/*
 * show the user information by user name
 */
extern int aos_security_cli_show_user(char *data, unsigned int *length, struct aosKernelApiParms *parms, char *errmsg, const int errlen);
