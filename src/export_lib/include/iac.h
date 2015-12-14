#ifndef _INCLUDE_IAC_H
#define _INCLUDE_IAC_H

#include "aosReturnCode.h"

enum {
	eAosRc_NameExist = eAosRc_InetctrlStart+1,
	eAosRc_MallocFailed,
	eAosRc_InUsed,
	eAosRc_NameNotExist,
	eAosRc_LackFailed,
	eAosRc_ArgumentErr,
	eAosRc_FlowExist,
	eAosRc_MallocFailedAppNoExistFlowExist,
	eAosRc_FlowNotExist,
	eAosRc_RetriveBufLenTooShort,
	eAosRc_MallocFailedTimeCtrNoExistRuleExist,
	eAosRc_TimeCtrlNotExist,
	eAosRc_MallocFailedTimeCtrNoExistOnlineExist,
	eAosRc_OnlineNotExist,
	eAosRc_UserExist,
	eAosRc_UserNotExist,
	eAosRc_RuleExistAppNoExistTimeCtrNoExist,
	eAosRc_AlreadyOn,
	eAosRc_AlreadyOff,
	eAosRc_FailedToChangeStatus,
	eAosRc_RuleNotExist,
};

int aos_iac_onoff(char* onoff);
int aos_iac_log_level(char *level);
int aos_iac_set_policy(char *policy);
int aos_iac_retrieve(char *result, int *len);

int aos_iac_gen_app(char *name);
int aos_iac_remove_app(char *name);
int aos_iac_add_flow(char *app_name, char *proto, char *ip, char *mask, int start_port, int end_port);
int aos_iac_remove_flow(char *app_name, char *proto, char *ip, char *mask, int start_port, int end_port);
int aos_iac_app_retrieve(char *result, int *len);
int aos_iac_app_clear();

int aos_iac_gen_timerule(char *name);
int aos_iac_remove_timerule(char *name);
int aos_iac_add_timectrl(char *rule_name, char *start_time, char *end_time, char *day);
int aos_iac_remove_timectrl(char *rule_name, char *start_time, char *end_time, char *day);
int aos_iac_add_online(char *rule_name, char *online_type, int online_minutes, char *days);
int aos_iac_remove_online(char *rule_name, char *online_type, int online_minutes, char *days);
int aos_iac_timerule_retrieve(char *result, int *len);
int aos_iac_timerule_clear();

int aos_iac_gen_group(char *name);
int aos_iac_remove_group(char *name);
int aos_iac_adduser_byip(char *grp_name, char *ip, char *mask);
int aos_iac_revuser_byip(char *grp_name, char *ip, char *mask);
int aos_iac_add_rule(char *grp_name, char*app_name, char *rule_name);
int aos_iac_remove_rule(char *grp_name, char *app_name, char *rule_name);
int aos_iac_group_retrieve(char *result, int *len);
int aos_iac_group_clear();

int aos_iac_save_config(char *result, int *len);
int aos_iac_clear_config();


#endif

