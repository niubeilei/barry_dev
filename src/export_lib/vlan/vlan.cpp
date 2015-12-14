/*
 * generated by genAPI.py
 * author: bill xia<xw_cn@163.com>
 */
#include "common.h"
#include "aos/aosReturnCode.h"
#include "vlan.h"
#define MAX_BUFF_LEN 10240
/*
 * add vlan
 */
int aos_vlan_add(char* device, int vlan_tag)
{
	OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "vlan device add  %s  %d ", device, vlan_tag);

	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * remove a vlan tag from interface
 */
int aos_vlan_del(char* device, int vlan_tag)
{
	OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "vlan device del  %s  %d ", device, vlan_tag);

	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * show vlan config
 */
int aos_vlan_retrieve_config(char* result, int* resultlen)
{
	OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];
	int len;

	sprintf(cmd, "vlan show config ");

	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	len = strlen(rslt);
	if (len > *resultlen)
	{
		strncpy(result, rslt, *resultlen);
		return eAosRc_LenTooShort;
	}
	else
	{
		strcpy(result, rslt);
		*resultlen = len;}
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * save vlan config
 */
int aos_vlan_save_config()
{
	OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "vlan save config ");

	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * load current vlan config
 */
int aos_router_load_config()
{
	OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "vlan load config ");

	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * clear vlan config
 */
int aos_vlan_clear_config()
{
	OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "vlan clear config ");

	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * set a vlan tag ip
 */
int aos_vlan_add_addr(char* device, int vlan_tag, char* addr)
{
	OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "vlan set ip  %s  %d  %s ", device, vlan_tag, addr);

	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * remove a vlan tag ip
 */
int aos_vlan_del_addr(char* device, int vlan_tag)
{
	OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "vlan del ip  %s  %d ", device, vlan_tag);

	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * remove all vlan of a interface
 */
int aos_vlan_del_all(char* device)
{
	OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "vlan device del all_tag   %s ", device);

	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * remove all vlan ip of a interface
 */
int aos_vlan_del_addr_all(char* device)
{
	OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "vlan device alltag del all_ip  %s ", device);

	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 *  turn on/off the VLAN feature on the attached switch
 */
int aos_vlan_switch_set_status(int status)
{
	OmnString rslt = "";
	char* tmp;
	//char cmd[MAX_BUFF_LEN];

	if (status == 1)
		OmnCliProc::getSelf()->runCliAsClient("vlan switch status on", rslt);
	else
		OmnCliProc::getSelf()->runCliAsClient("vlan switch status off", rslt);

	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * add a VLAN tag to a specific port on that switch
 */
int aos_vlan_switch_add_vlan(int vlan_tag, int port)
{
	OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "vlan switch set  %d  %d ", vlan_tag, port);

	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 *  set the factory default
 */
int aos_vlan_switch_reset()
{
	OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "vlan switch reset ");

	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

