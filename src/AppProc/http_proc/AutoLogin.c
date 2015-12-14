////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AutoLogin.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "AppProc/http_proc/AutoLogin.h"

#include "aos/aosKernelApi.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Mutex.h"
#include "aosUtil/Modules.h"
#include "aosUtil/Memory.h"
#include "aosUtil/StringUtil.h"
#include "aosUtil/Slab.h"
#include "AppProc/ReturnCode.h"
#include "KernelSimu/string.h"
#include "KernelUtil/KernelStr.h"
#include "Ktcpvs/aosTcpVsCntl.h"
#include "Ktcpvs/tcp_vs_def.h"
#include "PKCS/Certificate.h"

int aosHttpAutoLogin_setStatusCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	//
	// auto login set status <vsname> <on|off>
	//
	char *vsname = parms->mStrings[0];
	char *flag = parms->mStrings[1];
	
	*length = 0;

	if (!vsname || !flag)
	{
		strcpy(errmsg, "Missing paramters");
		return -eAosRc_MissingParms;
	}

	return 	aos_ktcpvs_set_autologin(vsname, flag, errmsg);
}

static struct AosHttp_AloginFileConf* 
aosHttpAutoLogin_findFile(
	struct AosHttp_AloginConf alogin_conf,
	char *alogin_filename)
{
	struct aos_list_head *entry;
	struct AosHttp_AloginFileConf *file_conf;
	aos_list_for_each (entry, &alogin_conf.alogin_file_list )
	{
		file_conf = list_entry(entry, struct AosHttp_AloginFileConf, list);
		if (strcmp(file_conf->alogin_filename, alogin_filename) == 0 )
			return file_conf;
	}
	return NULL;
}

static struct AosHttp_AloginParam*
aosHttpAutoLogin_findParam(
		struct AosHttp_AloginFileConf *file_conf, 
		uint8_t *param_name)
{
	struct aos_list_head *entry;
	struct AosHttp_AloginParam *param_conf;
	aos_list_for_each (entry, &file_conf.param_list )
	{
		param_conf = list_entry(entry, struct AosHttp_AloginParam, list);
		if (strcmp(param_conf->name, param_name) == 0 )
			return param_conf;
	}
	return NULL;
}

int aosHttpAutoLogin_configAutoLoginCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	//
	// auto login config <vs-name> <alogin-filename> <param-type> <param-name> 
	//		<cert-field> <param_name_size>
	//
	char *vsname =			parms->mStrings[0];
	char *alogin_file =		parms->mStrings[1];
	char *param_type =		parms->mStrings[2];
	char *param_name =		parms->mStrings[3];
	uint8_t cert_field =		parms->mIntegers[1];
	uint8_t param_name_size = parms->mIntegers[2];
	uint8_t type = eAosHttp_ParamType_None;
	struct tcp_vs_service *svc;
	struct AosHttp_AloginFileConf *file_conf;
	struct AosHttp_AloginParam *param_conf;
	int ret;

	*length = 0;
	ret = aosStrUtil_checkName(vsname, KTCPVS_IDENTNAME_MAXLEN, "Virtual Service Name", errmsg) ||
		  aosStrUtil_checkName(alogin_file, eAosHttp_MaxAloginFileNameSize, "Auto Login file", errmsg) ||
		  aosStrUtil_checkName(param_name, eAosHttp_MaxUsernameLen, "Auto Login Parameter Name", errmsg);
	if (ret)
	{
		return ret;
	}

	if (strcmp(param_type, "other") == 0)
	{
		type = eAosHttp_ParamType_Other;
	}
	else if (strcmp(param_type, "cert") == 0 )
	{
		type = eAosHttp_ParamType_Cert;
	}
	else
	{
		strcpy(errmsg, "Auto login file parameter error");
		return -1;
	}

	if (cert_field <= eAosHttp_CertField_None || cert_field >= eAosHttp_CertField_Max)
	{
		strcpy(errmsg, "cert_field should between %d and %d", 
			eAosHttp_CertField_None+1,
			eAosHttp_CertField_Max-1);
		return -1;
	}

	if (param_name_size <= 0 || param_name_size > eAosHttp_MaxUsernameLen)
	{
		strcpy(errmsg, "param format should less than %d", eAosHttp_MaxUsernameLen);
		return -1;
	}

	write_lock_bh(&__tcp_vs_svc_lock);

	svc = tcp_vs_get(vsname);
	if (!svc)
	{
		strcpy(errmsg, "Service not found");
		write_unlock_bh(&__tcp_vs_svc_lock);
		return -eAosRc_ServiceNotFound;
	}

	if (!(file_conf=aosHttpAutoLogin_findFile(svc->alogin_config, alogin_file)))
	{
		file_conf = (struct AosHttp_AloginFileConf *)aos_malloc(sizeof (struct AosHttp_AloginFileConf));
		if (!file_conf)
		{
			strcpy(errmsg, "Failed to allocate auto login file memory");
			write_unlock_bh(&__tcp_vs_svc_lock);
			return eAosAlarm_MemErr;
		}
		memset(file_conf, 0, sizeof(struct AosHttp_AloginFileConf));
		aos_list_add_tail(&file_conf->list, &svc->alogin_config.alogin_file_list);
	}

	if (!(param_conf = aosHttpAutoLogin_findParam(file_conf, param_name)))
	{
		param_conf = (struct AosHttp_AloginParam *)aos_malloc(sizeof (struct AosHttp_AloginParam));
		if (!param_conf)
		{
			strcpy(errmsg, "Failed to allocate auto login param memory");
			write_unlock_bh(&__tcp_vs_svc_lock);
			return eAosAlarm_MemErr;
		}
		memset(param_conf, 0, sizeof(struct AosHttp_AloginParam));
		aos_list_add_tail(&param_conf->list, &file_conf.param_list);
	}
	
	param_conf->type = type;
	strncpy(param_conf->name, param_name, sizeof(param_conf->name));
	param_conf->cert_field = cert_field;
	param_conf->name_size = param_name_size;

	write_unlock_bh(&__tcp_vs_svc_lock);
	
	return eAosRc_Success;
}

static int aosHttpAutoLogin_configShowOneSvcCli(
	struct tcp_vs_service *svc,
	char *data, 
	unsigned int length,
	unsigned int *index,
       char *errmsg)
{
	//
	// auto login config show <vs-name>
	//
	struct AosHttp_AloginFileConf *file_conf;
	struct AosHttp_AloginParam *param_conf;
	struct aos_list_head *file_entry, *param_entry;
	char local[256];
	unsigned int index = 0;
	int file_counts = 0;
	int param_counts = 0;
	
	sprintf(local, "---------------------------------------\n");
	aosCheckAndCopy(data, &index, length, local, strlen(local));
	sprintf(local, "Service Name: %s, \tAutoLogin: %s\n", svc->ident.name, 
		svc->alogin_config.auto_login?"on":"off");
	aosCheckAndCopy(data, &index, length, local, strlen(local));

	//if (!svc->alogin_config.auto_login)
	//{
	//	sprintf(local, "---------------------------------------\n");
	//	aosCheckAndCopy(data, &index, length, local, strlen(local));
	//	return eAosRc_Success;
	//}

	if (svc->alogin_config.auto_login)
	{
		aos_list_for_each(file_entry, &svc->alogin_config.alogin_file_list) 
		{
			file_conf = list_entry(file_entry, struct AosHttp_AloginFileConf, list);
			sprintf(local, "LoginFile: %s\n", file_conf->alogin_filename);
			aosCheckAndCopy(data, &index, length, local, strlen(local));
			aos_list_for_each(param_entry, &file_conf->param_list)
			{
				param_conf = aos_list_for_each(param_entry, struct AosHttp_AloginParam, list);
				sprintf(local,
					"Param Type: %d,\tLogin File: %s,\tCert Field: %d,\tFormat: %d\n", 
					param_conf->type, 
					param_conf->name, 
					param_conf->cert_field, 
					param_conf->name_size);
				aosCheckAndCopy(data, &index, length, local, strlen(local));
			}
		}
	}
	
	sprintf(local, "---------------------------------------\n");
	aosCheckAndCopy(data, &index, length, local, strlen(local));

	return eAosRc_Success;
}

// 
// Auto Login Processing Function. The caller should check the following 
// before calling this function:
//	1. Auto Login is turned on
//	2. Auto Login is configured
//  3. SSL is on
//
// Parms:
//	service: IN
//		The virtual service.
//
//  url: IN
//		The URL of the request
//
//  cert: IN
//		The client's certificate.
//
//  data: OUT
//		If successful, 'data' points to the ASP to be sent to the real server.
//
// Return Values:
//	eAosRc_ForwardToPeer:
//		The caller should forward 'data' to the peer (should be real server).
//
//	eAosRc_ProgErr
//		Internal error.
//
//	eAosRc_MemErr
//		Failed to allocate memory for the result data.
//
//	eAosRc_AutoLoginNotConfig:
//		The service is not configured with Auto Login
//
//	eAosRc_AutoLoginNotTurnedOn:
//		The Auto Login is not turned on
//	
int aosHttpAutoLogin_proc(struct tcp_vs_service *service, 
						  const char *url, 
						  const struct AosCertificate *cert,
						  char **data)
{
/*	char *d;
	char username[eAosHttp_MaxUsernameLen+1];
	char pwd[eAosHttp_MaxPwdLen+1];
	int ret;

	aos_assert1(service);
	aos_assert1(url);
	aos_assert1(cert);
	aos_assert1(data);
	
	*data = 0;
	if (!service->alogin_config || !service->auto_login)
	{
		return -eAosRc_AutoLoginNotConfig;
	}

	// 
	// If the URL does not match the login URL, do nothing.
	//
	if (strcmp(url, service->alogin_config->login_url) != 0)
	{
		return -eAosRc_NotLoginUrl;
	}

	d = (char *)AosSlab_get(sgSlab);
	aos_assert1(d);

	ret = AosCert_retrieve(cert, service->alogin_config->cert_username, username, eAosHttp_MaxUsernameLen);
	if (ret)
	{
		return ret;
	}

	ret = AosCert_retrieve(cert, service->alogin_config->cert_pwd, pwd, eAosHttp_MaxPwdLen);
	if (ret)
	{
		return ret;
	}

	sprintf(d, "%s %s %s %s %s", 
		service->alogin_config->part1,
		username, 
		service->alogin_config->part2,
		pwd, 
		service->alogin_config->part3);

	(*data) = d;*/
	return 0;
}


int aosHttpAutoLogin_saveConfCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	return aos_ktcpvs_saveAutologin(data, length, parms, errmsg, errlen);
	return 0;
}


int aosHttpAutoLogin_showAutoLoginCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	struct list_head *lsvc;
	struct tcp_vs_service *svc;
	char local[256];
	char *rsltBuff = aosKernelApi_getBuff(data);
	unsigned int index = 0;
	unsigned int optlen = *length;
	int ret = 0;

	*length = 0;

	write_lock_bh(&__tcp_vs_svc_lock);

	if (parms->mNumStrings == 0)
	{
		list_for_each(lsvc, &tcp_vs_svc_list) 
		{
			svc = list_entry(lsvc, struct tcp_vs_service, list);
			ret = aosHttpAutoLogin_configShowOneSvcCli(svc, rsltBuff, optlen, &index, errmsg);
			if (ret)
			{
				write_unlock_bh(&__tcp_vs_svc_lock);
				*length = index;
				return -1;
			}
		}
	}
	else
	{
		char *vsname = parms->mStrings[0];
		if (!vsname)
		{
			strcpy(errmsg, "Command incorrect");
			write_unlock_bh(&__tcp_vs_svc_lock);
			*length = index;
			return -eAosRc_ProgErr;
		}
		svc = tcp_vs_get(vsname);
		if (!svc)
		{
			strcpy(errmsg, "Service %s not found", vsname);
			write_unlock_bh(&__tcp_vs_svc_lock);
			*length = index;
			return -eAosRc_ServiceNotFound;
		}
		ret = aosHttpAutoLogin_configShowOneSvcCli(svc, rsltBuff, optlen, &index, errmsg);
	}

	write_unlock_bh(&__tcp_vs_svc_lock);

	*length = index;

	return ret;
}}

