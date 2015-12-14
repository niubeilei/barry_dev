#include "ParentCtrl/webwall_cli.h"
#include "ParentCtrl/webwall_common.h"
#include "ParentCtrl/webwall_impl.h"
#include "CliUtil/CliUtil.h"
#include "CliUtil/CliUtilProc.h"
#include "Util/String.h"
#include "KernelConfMgr/SystemSaveConfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DESCRIPTION_LEN 128

/*
 * save config structure
 * 
 */
static struct 
{
	char description[MAX_DESCRIPTION_LEN];
	char* (*save_func)(void); /* must use 'malloc' function to allocate memory */
} save_table[] = {
	{"//webwall save config\n", ww_cli_save_config},
};
#define SAVE_TABLE_LENGTH (int)(sizeof(save_table)/sizeof(save_table[0]))

/*
 * clear config structure
 */
static struct
{
	char description[MAX_DESCRIPTION_LEN];
	int (*clear_func)(void);
} clear_table[] = {
	{"//webwall clear config", ww_cli_clear_config},
};
#define CLEAR_TABLE_LENGTH (int)(sizeof(clear_table)/sizeof(clear_table[0]))


int system_save_config_register_clis(void)
{
	int ret;
	
	ret = CliUtil_regCliCmd("system save config", cli_system_save_config);
	ret |= CliUtil_regCliCmd("system load config", cli_system_load_config);
	ret |= CliUtil_regCliCmd("system clear all config", cli_system_clear_config);
	ret |= CliUtil_regCliCmd("webwall clear config", cli_system_clear_config);
	ret |= CliUtil_regCliCmd("webwall save config", cli_system_save_config_for_webwall);
	
	return ret;
}

int cli_system_save_config(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	FILE* fp;
	int i;
	char* p = NULL;
	
	if ((fp = fopen(CLI_FILE_LOCATION, "w+")) == NULL) {
		return -1;
	}

	for (i = 0; i < SAVE_TABLE_LENGTH; i++) {
		p = save_table[i].save_func();
		if (p != NULL) {
			fwrite(save_table[i].description, 1, strlen(save_table[i].description), fp);
			fwrite(p, 1, strlen(p), fp);
			free(p);
		}
	}
	
	fclose(fp);
	
	*length = 0;
	
	return 0;
}

int cli_system_save_config_for_webwall(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* p = NULL;
	char* dst;
	unsigned int index = 0;
	
	p = ww_cli_save_config();
	if (p != NULL) {
		dst = CliUtil_getBuff(data);
		CliUtil_checkAndCopy(dst, &index, *length, p, strlen(p));
		*length = index;
		free(p);
		return 0;
	}
	
	*length = 0;
	
	return 0;
}
/* following initalize function */
static char* ww_trim(char* str)
{
	int len = strlen(str);
	char* p;
	char* q;
	
	q = str;
	while (*q == ' ' || *q == '\t' || *q == '\n') {
		q++;
	}
	p = str + len - 1;
	while (p != str) {
		if (*p == '\n' || *p == ' ' || *p == '\t') {
			*p = '\0';
		} else
			break;
		p--;
	}
	
	return q;
}

static inline int is_save_entry_description(char* str)
{
	char* p;
	
	p = str;
	if (strncmp(p, "//", 2) == 0) {
		return 1;
	}
	
	return 0;
}

static inline int exe_cmd(char* cmd)
{
	char* p;
	char* q;
	char buff[MAX_BUFF_LEN];
	int ret;
	OmnString strCmd;
	OmnString strResult;
	
#define CMD_PREFIX "<cmd>"
#define CMD_SUFFIX "</cmd>"
	
	p = strstr(cmd, CMD_PREFIX);
	if (p == NULL)
		return -1;
	p += strlen(CMD_PREFIX);
	
	q = strstr(cmd, CMD_SUFFIX);
	if (q == NULL)
		return -1;
	*q = '\0';
	
	sprintf(buff, "%s \"%s\"", CLI_EXE_LOCATION, p);
	printf("%s\n", buff);
	strCmd<<p;
	//ret = system(buff);
	OmnCliUtilProc::getSelf()->runCli(strCmd, strResult);
	printf("ok\n\n");
#ifdef SAVE_CONFIG_DEBUG
	printf("execute cli command: %s\n", buff);
#endif 

	return ret;
}

int cli_system_load_config(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	FILE* fp;
	char buff[MAX_BUFF_LEN];
	char* p = NULL;
	int ret = 0;
	
	/* webwall initialize section*/
#ifdef SAVE_CONFIG_DEBUG
	printf("start to initialize webwall default setting\n");
#endif 
	ww_init();
	/* end*/
	
	if ((fp = fopen(CLI_FILE_LOCATION, "r+")) == NULL) {
		return -1;
	}
	
	while (!feof(fp)) {
		memset(buff, 0, MAX_BUFF_LEN);
		fgets(buff, MAX_BUFF_LEN, fp);
		p = ww_trim(buff);
		if (strlen(p) > 0) {
			if (is_save_entry_description(p) == 1)
				continue; 
			ret |= exe_cmd(p);
		}
	}
	
	fclose(fp);
#ifdef SAVE_CONFIG_DEBUG
	if (ret < 0) {
		printf("Failed to finish the loading config\n");
	}
#endif 
	
	*length = 0;
	
	return 0;
}

int cli_system_clear_config(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	int i;
	
	for (i = 0; i < CLEAR_TABLE_LENGTH; i++) {
		if (clear_table[i].clear_func != NULL) {
			clear_table[i].clear_func();
#ifdef SAVE_CONFIG_DEBUG
			if (clear_table[i].description != NULL)
				printf("clear config: %s\n", clear_table[i].description);
#endif
		}
	}
	*length = 0;
	
	return 0;
}



