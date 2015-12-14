///////////////////////////////////////////////////////////////
// Created by:	wu xia
// Email:<a href=mailto:xw_cn@163.com></a>
// Created:		09/19/2006	
// Comments:
//	   .ini file constants of CliDaemon tortureTest
// 
// Change History:
//	09/19/2006 File Created
///////////////////////////////////////////////////////////////
#ifndef AOS_CLIDAEMON_TESTER_CONSTANTS_H
#define AOS_CLIDAEMON_TESTER_CONSTANTS_H


#include "aosUtil/List.h"
#include "Util1/Time.h"
#define SECTION_USER "User"
#define SECTION_END_FLAG "EndFlag"
#define SECTION_LOGIN_CLI_PREFIX "LoginCLI_Prefix"
#define SECTION_LOGIN_CLI_BODY "LoginCLI_Body"
#define SECTION_LOGIN_CLI_SUCC "LoginCLI_Succ"
#define SECTION_LOGIN_CLI_FAIL "LoginCLI_Fail"
#define SECTION_OP_CLI_PREFIX "OpCLI_Prefix"
#define SECTION_OP_CLI_BODY "OpCLI_Body"
#define SECTION_LOGOUT_CLI_PREFIX "LogoutCLI_Prefix"
#define SECTION_LOGOUT_CLI_BODY "LogoutCLI_Body"

#define RIGHT_USER "user"
#define RIGHT_USER_INT 1

#define RIGHT_ENABLE "enable"
#define RIGHT_ENABLE_INT 2

#define RIGHT_CONFIG "config"
#define RIGHT_CONFIG_INT 3

#define RIGHT_ENGINEERING "engineering"
#define RIGHT_ENGINEERING_INT 4

#define DEFAULT_SESSION_ID "00000000"
#define DEFAULT_SIZE 256
#define DEFAULT_TIMEOUT 300
#define TIMEOUT_ENGINEERING 600

#define ERROR_TIMEOUT 1
#define ERROR_TIMEOUT_TEXT ""

#define ERROR_FORBID_ENGINEERING_CHANGE 2
#define ERROR_FORBID_ENGINEERING_CHANGE_TEXT ""

#define ERROR_FORBID_CONFIG_NUM 3
#define ERROR_RORBID_CONFIG_NUM_TEXT ""

#define ERROR_FORBID_NORMAL 4
#define ERROR_FORBID_NORMAL_TEXT ""

#define SWITCH_RANDOM 1
//user list definition
typedef struct _userListEntry
{
  char sessionId[DEFAULT_SIZE];
  char userName[DEFAULT_SIZE];
  char userPassword[DEFAULT_SIZE];
  char right[DEFAULT_SIZE];
  int timeout;
  int  lastTick;//last action time
  struct aos_list_head list;
} UserListEntry;
//end of user list definition

//command definition
typedef struct _CommandEntry
{
  char command[DEFAULT_SIZE];
  char result[DEFAULT_SIZE];
  char level[DEFAULT_SIZE];
  char user[DEFAULT_SIZE];
} CommandEntry;
//end
#endif
