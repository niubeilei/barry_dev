#include "iac.h"
#include "common.h"

int aos_iac_onoff(char *status)
{
	OmnString rslt = "";
	char* tmp;
	
	if (!strcmp(status, "on"))
		OmnCliProc::getSelf()->runCliAsClient("inetctrl on", rslt);
	else
		OmnCliProc::getSelf()->runCliAsClient("inetctrl off", rslt);
	
	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);	
}

int aos_iac_log_level(char *level)
{
	OmnString rslt = "";
	char* tmp;
	char cmdline[128];
	
	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "inetctrl log level %s", level);
	
	OmnCliProc::getSelf()->runCliAsClient(cmdline, rslt);
		
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_iac_set_policy(char *policy)
{
	OmnString rslt = "";
	char* tmp;
	char cmdline[128];
	
	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "inetctrl default policy %s", policy);

	OmnCliProc::getSelf()->runCliAsClient(cmdline, rslt);

	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_iac_retrieve(char *result, int *len)
{
	OmnString rslt = "";
	char* tmp;

	OmnCliProc::getSelf()->runCliAsClient("inetctrl show config", rslt);
    if (*len <= (int)0 || *len < (int)(rslt.length()+1))
        return -eAosRc_LenTooShort;

	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;
    
    strcpy(result, rslt);
    *len = strlen(rslt);
	return *((int*)tmp);	
}

int aos_iac_gen_app(char *name)
{
	OmnString rslt = "";
	char* tmp;
	char cmdline[128];
	
	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "inetctrl app name %s", name);
	
	OmnCliProc::getSelf()->runCliAsClient(cmdline, rslt);
		
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_iac_remove_app(char *name)
{
	OmnString rslt = "";
	char* tmp;
	char cmdline[128];
	
	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "inetctrl app remove %s", name);
	
	OmnCliProc::getSelf()->runCliAsClient(cmdline, rslt);
		
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_iac_add_flow(char *app_name, char *proto, char *ip, char *mask, int start_port, int end_port)
{
	OmnString rslt = "";
	char* tmp;
	char cmdline[128];
	
	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "inetctrl app addflow ip %s %s %s %s %d %d", 
			app_name, proto, ip, mask, start_port, end_port);
	
	OmnCliProc::getSelf()->runCliAsClient(cmdline, rslt);
		
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_iac_remove_flow(char *app_name, char *proto, char *ip, char *mask, int start_port, int end_port)
{
	OmnString rslt = "";
	char* tmp;
	char cmdline[128];
	
	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "inetctrl app revflow ip %s %s %s %s %d %d", 
			app_name, proto, ip, mask, start_port, end_port);
	
	OmnCliProc::getSelf()->runCliAsClient(cmdline, rslt);
		
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_iac_app_retrieve(char *result, int *len)
{
	OmnString rslt = "";
	char* tmp;
    
	OmnCliProc::getSelf()->runCliAsClient("inetctrl app show config", rslt);
    if (*len <= (int)0 || *len < (int)(rslt.length()+1))
        return -eAosRc_LenTooShort;

	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;
	
    strcpy(result, rslt);
    *len = strlen(rslt);
	return *((int*)tmp);
}

int aos_iac_app_clear()
{
	OmnString rslt = "";
	char* tmp;
	
	OmnCliProc::getSelf()->runCliAsClient("inetctrl app clear config", rslt);
		
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_iac_gen_timerule(char *name)
{
	OmnString rslt = "";
	char* tmp;
	char cmdline[128];
	
	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "inetctrl timerule name %s", name);

	OmnCliProc::getSelf()->runCliAsClient(cmdline, rslt);
		
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_iac_remove_timerule(char *name)
{
	OmnString rslt = "";
	char* tmp;
	char cmdline[128];
	
	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "inetctrl timerule remove %s", name);

	OmnCliProc::getSelf()->runCliAsClient(cmdline, rslt);
		
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_iac_add_timectrl(char *rule_name, char *start_time, char *end_time, char *day)
{
	OmnString rslt = "";
	char* tmp;
	char cmdline[128];
	
	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "inetctrl timerule addrule time %s %s %s %s",
			rule_name, start_time, end_time ,day);

	OmnCliProc::getSelf()->runCliAsClient(cmdline, rslt);
		
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_iac_remove_timectrl(char *rule_name, char *start_time, char *end_time, char *day)
{
	OmnString rslt = "";
	char* tmp;
	char cmdline[128];
	
	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "inetctrl timerule revrule time %s %s %s %s",
			rule_name, start_time, end_time ,day);

	OmnCliProc::getSelf()->runCliAsClient(cmdline, rslt);
		
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_iac_add_online(char *rule_name, char *online_type, int online_minutes, char *days)
{
	OmnString rslt = "";
	char* tmp;
	char cmdline[128];
	
	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "inetctrl timerule addrule online %s %s %d %s",
			rule_name, online_type, online_minutes ,days);

	OmnCliProc::getSelf()->runCliAsClient(cmdline, rslt);
		
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_iac_remove_online(char *rule_name, char *online_type, int online_minutes, char *days)
{
	OmnString rslt = "";
	char* tmp;
	char cmdline[128];
	
	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "inetctrl timerule revrule online %s %s %d %s",
			rule_name, online_type, online_minutes ,days);

	OmnCliProc::getSelf()->runCliAsClient(cmdline, rslt);
		
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_iac_timerule_retrieve(char *result, int *len)
{
	OmnString rslt = "";
	char* tmp;
    
	OmnCliProc::getSelf()->runCliAsClient("inetctrl timerule show config", rslt);
    if (*len <= (int)0 || *len < (int)(rslt.length()+1))
        return -eAosRc_LenTooShort;

	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;
    
    strcpy(result, rslt);
    *len = strlen(rslt);
	return *((int*)tmp);
}

int aos_iac_timerule_clear()
{
	OmnString rslt = "";
	char* tmp;

	OmnCliProc::getSelf()->runCliAsClient("inetctrl timerule clear config", rslt);
		
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_iac_gen_group(char *name)
{
	OmnString rslt = "";
	char* tmp;
	char cmdline[128];
	
	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "inetctrl group name %s", name);

	OmnCliProc::getSelf()->runCliAsClient(cmdline, rslt);
		
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_iac_remove_group(char *name)
{
	OmnString rslt = "";
	char* tmp;
	char cmdline[128];
	
	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "inetctrl group remove %s", name);

	OmnCliProc::getSelf()->runCliAsClient(cmdline, rslt);
		
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_iac_adduser_byip(char *grp_name, char *ip, char *mask)
{
	OmnString rslt = "";
	char* tmp;
	char cmdline[128];
	
	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "inetctrl group adduser byip %s %s %s", grp_name, ip, mask);

	OmnCliProc::getSelf()->runCliAsClient(cmdline, rslt);
		
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_iac_revuser_byip(char *grp_name, char *ip, char *mask)
{
	OmnString rslt = "";
	char* tmp;
	char cmdline[128];
	
	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "inetctrl group revuser byip %s %s %s", grp_name, ip, mask);

	OmnCliProc::getSelf()->runCliAsClient(cmdline, rslt);
		
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_iac_add_rule(char *grp_name, char*app_name, char *rule_name)
{
	OmnString rslt = "";
	char* tmp;
	char cmdline[128];
	
	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "inetctrl group addrule %s %s %s", grp_name, app_name, rule_name);

	OmnCliProc::getSelf()->runCliAsClient(cmdline, rslt);
		
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_iac_remove_rule(char *grp_name, char *app_name, char *rule_name)
{
	OmnString rslt = "";
	char* tmp;
	char cmdline[128];
	
	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "inetctrl group revrule %s %s %s", grp_name, app_name, rule_name);

	OmnCliProc::getSelf()->runCliAsClient(cmdline, rslt);
		
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_iac_group_retrieve(char *result, int *len)
{
	OmnString rslt = "";
	char* tmp;

	OmnCliProc::getSelf()->runCliAsClient("inetctrl group show config", rslt);
    if (*len <= (int)0 || *len < (int)(rslt.length()+1))
        return -eAosRc_LenTooShort;

	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

    strcpy(result, rslt);
    *len = strlen(rslt);
	return *((int*)tmp);
}

int aos_iac_group_clear()
{
	OmnString rslt = "";
	char* tmp;

	OmnCliProc::getSelf()->runCliAsClient("inetctrl group clear config", rslt);
		
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_iac_save_config(char *result, int *len)
{
	OmnString rslt = "";
	char* tmp;

	OmnCliProc::getSelf()->runCliAsClient("inetctrl save config", rslt);
    if (*len <= (int)0 || *len < (int)(rslt.length()+1))
        return -eAosRc_LenTooShort;

	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

    strcpy(result, rslt);
    *len = strlen(rslt);
	return *((int*)tmp);
}

int aos_iac_clear_config()
{
	OmnString rslt = "";
	char* tmp;

	OmnCliProc::getSelf()->runCliAsClient("inetctrl clear config", rslt);
		
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}
