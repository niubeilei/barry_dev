#ifndef INET_CTRL_CLI_H
#define INET_CTRL_CLI_H

#include "aos/aosReturnCode.h" 

int reg_inetctrl_cli(void);

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

#endif

