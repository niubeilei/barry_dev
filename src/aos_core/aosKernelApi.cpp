////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosKernelApi.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos/aosKernelApi.h"

#include "AAA/AaaServer.h"
#include "aos/KernelApiStruct.h"
#include "aos/aosKernelAlarm.h"
#include "aos/aosTimer.h"
#include "aos/aosPkt.h"
#include "aos/aosDev.h"
#include "aos/aosBuiltinRules.h"
#include "aos/aosCorePcp.h"
#include "aos/aosArp.h"
#include "aos/aosBridge.h"
#include "aos/aosConfig.h"
#include "aos_core/aosMacCtlr.h"
#include "aos_core/aosSocket.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Modules.h"
#include "aosUtil/Util.h"
#include "AppProxy/AppProxy.h"
#include "aosUtil/AosPlatform.h"
#include "Servers/AppServer.h"
#include "Servers/ServerGroup.h"
#include "AppProxy/DenyPage.h"
#include <KernelSimu/aosKernelDebug.h>
#include <KernelSimu/timer.h>
#include <KernelSimu/jiffies.h>
#include <KernelSimu/uaccess.h>
#include <KernelSimu/module.h>

#include "aosUtil/CharPtree.h"
#include "KernelAPI/KernelWrapper/TestAosApi.h"
#include "KernelAPI/KernelWrapper/MutexTesterKnl.h"
#include "KernelAPI/KernelWrapper/RwlockTesterKnl.h"
#include "Ktcpvs/aosMemCheck.h"

#ifdef __KERNEL__
#include "linux/module.h"
#endif

#ifdef CONFIG_AOS_TEST
#include "aosUtil/Memory.h"
#endif


static struct aosCharPtree *sg_cli_tree = 0;
static int    				sg_module_init_flag = 0;


int aosInitKernelApiCli(char *data,
						unsigned int *optlen,
						struct aosKernelApiParms *parms,
						char *errmsg,
						const int errlen)
{
//	aosTestSkbInit();
	return 0;
}

extern int jmk_init(void);

int OmnKernelApi_init(void)
{
	if (sg_module_init_flag) return 0;

    sg_cli_tree = (struct aosCharPtree *)aosCharPtree_create();
	aos_assert1(sg_cli_tree);

	sg_module_init_flag = 1;
	return OmnKernelApi_addCliCmds();
}


static int aosKernelApi_convert(char *data,
						 unsigned int datalen,
						 struct aosKernelApiParms *parms)
{
	//
	// oprId
	// number of integers (integer)
	// number of strings (integer)
	// integer parm
	// integer parm
	// ...
	// string parm
	// string parm
	// ...
	//
	// Index counts from 1.
	//
	unsigned int i, ii, index;
	char *dd;
	int len = strlen(data);
	int *iv = (int *)&data[len+1];

	if ((ii = len + 1 + (iv[0] + 2) * 4) >datalen)
	{
		return eAosRc_IndexOutBound;
	}

	//
	// Retrieve the integers
	//
	parms->mNumIntegers = iv[0];
	parms->mNumStrings = iv[1];
	if (parms->mNumIntegers < 0 ||
		parms->mNumIntegers > eAosMaxKernelApiIntegers)
	{
		return eAosRc_IndexOutBound;
	}

	for (index = 0; index < parms->mNumIntegers; index++)
	{
		parms->mIntegers[index] = iv[index+2];
	}

	//
	// Retrieve the strings
	//

	dd = &data[ii];
	index = 0;
	
	for (i=0; i<parms->mNumStrings; i++)
	{
		parms->mStrings[index++] = &data[ii];

		ii += strlen(dd) + 1;
		if (ii > datalen)
		{
			return eAosRc_IndexOutBound;
		}
		dd = &data[ii];
	}

	// added by liqin 2007/02/27
	ii += strlen(dd) + 1;

	if (ii <= datalen)
	{
		parms->mCmd = dd;
	}
	else
	{
		parms->mCmd = NULL;
	}
	
	return 0;
}

int OmnKernelApi_addTestCmds(void)
{
	int ret = 0;
#ifdef CONFIG_AOS_TEST

	ret |= OmnKernelApi_addCliCmd("mutex_test",		aos_mutex_tester_cli);
	ret |= OmnKernelApi_addCliCmd("rwlock_test",	aos_rwlock_tester_cli);

	ret |= OmnKernelApi_addCliCmd("test_ocsp_api_start", aosTestOcspStartApi);
	ret |= OmnKernelApi_addCliCmd("test_ocsp_api_retrieve", aosTestOcspRetrieveApi);

	// Chen Ding, 11/01/2005
	ret |= OmnKernelApi_addCliCmd("memory_init", AosMemory_initCli);
	ret |= OmnKernelApi_addCliCmd("memory_getRslt", AosMemory_getRsltCli);
	ret |= OmnKernelApi_addCliCmd("memory_setStatus", AosMemory_setStatusCli);
	ret |= OmnKernelApi_addCliCmd("memory_show", AosMemory_showCli);
	ret |= OmnKernelApi_addCliCmd("MemoryCheckStatus", aosMem_checkStartCli);
	ret |= OmnKernelApi_addCliCmd("MemoryCheckTimeout", aosMem_checkTimeoutCli);

#endif

#ifdef CONFIG_AOS_INMEMORY_LOG
	ret |= OmnKernelApi_addCliCmd("InMemLogPrint", AosTracer_printLogCli);
	ret |= OmnKernelApi_addCliCmd("InMemLogSet",   AosTracer_setStatusCli);
	ret |= OmnKernelApi_addCliCmd("InMemLogReset",   AosTracer_resetCli);
	ret |= OmnKernelApi_addCliCmd("InMemLogWrap",   AosTracer_setWrapCli);
#endif

	return ret;
}


int OmnKernelApi_addCliCmds(void)
{
	int ret = 0;
	ret = OmnKernelApi_addTestCmds();

#ifdef __KERNEL__
#ifdef CONFIG_AOS_BRIDGE
	ret |= OmnKernelApi_addCliCmd("bridge-add",		aosBridge_createKapiCli);
	ret |= OmnKernelApi_addCliCmd("initkernelapi",	aosInitKernelApiCli);
	ret |= OmnKernelApi_addCliCmd("show-bridge",	aosBridge_showCli);
	ret |= OmnKernelApi_addCliCmd("bridge-remove",	aosBridge_removeCli);
	ret |= OmnKernelApi_addCliCmd("clear-bridge",	aosBridge_clearAllCli);
	ret |= OmnKernelApi_addCliCmd("bridge-reset",	aosBridge_resetCli);
	ret |= OmnKernelApi_addCliCmd("bridge-status",	aosBridge_statusCli);
	ret |= OmnKernelApi_addCliCmd("bridge-devadd",	aosBridge_addDevCli);
	ret |= OmnKernelApi_addCliCmd("bridge-devdel",	aosBridge_delDevCli);
	ret |= OmnKernelApi_addCliCmd("bridge-saveconf",aosBridge_saveConfigCli);
	ret |= OmnKernelApi_addCliCmd("bridge-hc",		aosBridge_healthcheckCli);

	ret |= OmnKernelApi_addCliCmd("pcp-status",		aosCorePcp_statusCli);
	ret |= OmnKernelApi_addCliCmd("show-pcp",		aosCorePcp_showPcpCli);
#endif

//	ret |= OmnKernelApi_addCliCmd("mac-status",		aosMacCtlr_statusCli);
//	ret |= OmnKernelApi_addCliCmd("mac-show",		aosMacCtlr_showCli);
//	ret |= OmnKernelApi_addCliCmd("mac-add",		aosMacCtlr_addCli);
//	ret |= OmnKernelApi_addCliCmd("mac-del",		aosMacCtlr_delCli);

#ifdef CONFIG_AOS_TCPVS
/*	ret |= OmnKernelApi_addCliCmd("KtcpvsInit",		aos_ktcpvs_init_cli);
    ret |= OmnKernelApi_addCliCmd("app_prox_rs_add", 	aos_ktcpvs_add_rs_cli);
    ret |= OmnKernelApi_addCliCmd("app_prox_rs_del", 	aos_ktcpvs_del_rs_cli);
	ret |= OmnKernelApi_addCliCmd("AppProxyAdd",		aosAppProxy_addCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyDel",		aosAppProxy_delCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyShow",		aosAppProxy_showCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyClearAll",	aosAppProxy_clearAllCli);
	ret |= OmnKernelApi_addCliCmd("AppProxySetStatus",	aosAppProxy_setStatusCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyAaaAdd",		aosAppProxy_aaaAddCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyAaaDel",		aosAppProxy_aaaDelCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyAaaClear",	aosAppProxy_aaaClearCli);
	ret |= OmnKernelApi_addCliCmd("AppProxySetSvrGrp",	aosAppProxy_setGroupCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyResetGroup",	aosAppProxy_resetGroupCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyShowConn",	aosAppProxy_showConnCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyConnDurSet",	aosAppProxy_setMaxDurationCli);
	ret |= OmnKernelApi_addCliCmd("AppProxySaveConfig",	aosAppProxy_saveConfigCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyClearConfig",aosAppProxy_clearConfigCli);
	ret |= OmnKernelApi_addCliCmd("AppProxySetPMI",		aosAppProxy_setPMICli);
	ret |= OmnKernelApi_addCliCmd("AppProxySslClientAuth",	aosAppProxy_sslClientAuthCli);
	ret |= OmnKernelApi_addCliCmd("AppProxySslFront",	aosAppProxy_sslFrontCli);
	ret |= OmnKernelApi_addCliCmd("AppProxySslBackend",	aosAppProxy_sslBackendCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyPmiSysname",	aosAppProxy_pmiSysnameCli);

	ret |= OmnKernelApi_addCliCmd("AppProxyForwardTable",aosAppProxy_forwardTableCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyMaxClients",aosAppProxy_maxClientsCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyMaxConns",	aosAppProxy_maxConnsCli);
*/
#endif

	ret |= OmnKernelApi_addCliCmd("AppServerCreate",	aosAppServer_createCli);
	ret |= OmnKernelApi_addCliCmd("AppServerShow",		aosAppServer_showCli);
	ret |= OmnKernelApi_addCliCmd("AppServerDel",		aosAppServer_delCli);
	ret |= OmnKernelApi_addCliCmd("AppServerClearAll",	aosAppServer_clearAllCli);
	ret |= OmnKernelApi_addCliCmd("AppServerSetStatus",	aosAppServer_setStatusCli);

	ret |= OmnKernelApi_addCliCmd("ServerGroupAdd",		aosServerGroup_addCli);
	ret |= OmnKernelApi_addCliCmd("ServerGroupShow",	aosServerGroup_showCli);
	ret |= OmnKernelApi_addCliCmd("ServerGroupDel",		aosServerGroup_delCli);
	ret |= OmnKernelApi_addCliCmd("ServerGroupClearAll",	aosServerGroup_clearAllCli);
	ret |= OmnKernelApi_addCliCmd("ServerGroupMemAdd",	aosServerGroup_addMemberCli);
	ret |= OmnKernelApi_addCliCmd("ServerGroupMemDel",	aosServerGroup_delMemberCli);
	ret |= OmnKernelApi_addCliCmd("ServerGroupMemClear",	aosServerGroup_clearMemberCli);

#ifdef CONFIG_AOS_AAA
	ret |= OmnKernelApi_addCliCmd("AaaServerAdd",		aosAaaServer_addCli);
	ret |= OmnKernelApi_addCliCmd("AaaServerShow",		aosAaaServer_showCli);
	ret |= OmnKernelApi_addCliCmd("AaaServerDel",		aosAaaServer_delCli);
	ret |= OmnKernelApi_addCliCmd("AaaServerClearAll",	aosAaaServer_clearAllCli);
	ret |= OmnKernelApi_addCliCmd("AaaServerStatReset",	aosAaaServer_resetStatCli);
	ret |= OmnKernelApi_addCliCmd("AaaServerSetStatus",	aosAaaServer_setStatusCli);
	ret |= OmnKernelApi_addCliCmd("AaaServerStatClearAll",	aosAaaServer_clearStatCli);
#endif

//	ret |= OmnKernelApi_addCliCmd("CreateTcpSock",		aosSocket_createAndConnectCli);

#ifdef CONFIG_CERT_VERIFY
	// ------------------------add by zhangyq -------------------------
	// OCSP cli Command
	ret |= OmnKernelApi_addCliCmd("OcspStatus",		aos_ocsp_status);
	ret |= OmnKernelApi_addCliCmd("OcspSetAddr",		aos_ocsp_set_addr);
	ret |= OmnKernelApi_addCliCmd("OcspSaveConfig",		aos_ocsp_save_config);
	ret |= OmnKernelApi_addCliCmd("OcspClearConfig",	aos_ocsp_clear_config);
	ret |= OmnKernelApi_addCliCmd("OcspShowConfig",		aos_show_ocsp_config);

	// CRL cli command
	ret |= OmnKernelApi_addCliCmd("CrlStatus",		aos_crl_status);
	ret |= OmnKernelApi_addCliCmd("CrlSetAddr",		aos_crl_set_addr);
	ret |= OmnKernelApi_addCliCmd("CrlSetUrl",		aos_crl_url_set);
	ret |= OmnKernelApi_addCliCmd("CrlFlush",		aos_crl_flush);
	ret |= OmnKernelApi_addCliCmd("CrlSetTimeout",		aos_crl_timeout_set);
	ret |= OmnKernelApi_addCliCmd("CrlUpdate",		aos_crl_update);
	ret |= OmnKernelApi_addCliCmd("CrlShowlist",		aos_crl_show_list);
	ret |= OmnKernelApi_addCliCmd("CrlShowConfig",		aos_show_crl_config);

	// amm cli command 
	ret |= OmnKernelApi_addCliCmd("AmmStatus",		aos_amm_set_status);
	ret |= OmnKernelApi_addCliCmd("AmmSetAddr",		aos_amm_set_proxy);
	ret |= OmnKernelApi_addCliCmd("AmmSetOrg",		aos_amm_set_org);
	ret |= OmnKernelApi_addCliCmd("AmmSetTimeout",		aos_amm_set_timeout);
	ret |= OmnKernelApi_addCliCmd("AmmSaveConfig",		aos_amm_save_config);
	ret |= OmnKernelApi_addCliCmd("AmmClearConfig",		aos_amm_clear_config);
	ret |= OmnKernelApi_addCliCmd("AmmShowConfig",		aos_amm_show_config);

#endif


#ifdef CONFIG_AOS_TCPAPI
	ret |= OmnKernelApi_addCliCmd("TestAosTcpApi",	aosTestTcpApi);
#endif

#ifdef CONFIG_AOS_TCPVS


	// deny page cli
	ret |= OmnKernelApi_addCliCmd("DenyPageAdd", 		aos_deny_page_add);
	ret |= OmnKernelApi_addCliCmd("DenyPageRemove", 		aos_deny_page_remove);
	ret |= OmnKernelApi_addCliCmd("DenyPageShow", 		aos_deny_page_show);
	ret |= OmnKernelApi_addCliCmd("DenyPageAssign", 		aos_deny_page_assign);
	ret |= OmnKernelApi_addCliCmd("DenyPageDeassign", 	aos_deny_page_deassign);
	ret |= OmnKernelApi_addCliCmd("DenyPageSaveConfig",	aos_deny_page_save_config);
	ret |= OmnKernelApi_addCliCmd("DenyPageClearAllConfig",	aos_deny_page_clearall_config);

	/*
	ret |= OmnKernelApi_addCliCmd("usbkey_add", 		aos_usbkey_add);
	ret |= OmnKernelApi_addCliCmd("usbkey_remove", 		aos_usbkey_remove);
	ret |= OmnKernelApi_addCliCmd("usbkey_show", 		aos_usbkey_show);
	ret |= OmnKernelApi_addCliCmd("usbkey_save_config", 	aos_usbkey_save_config);
	ret |= OmnKernelApi_addCliCmd("usbkey_clear_config",	aos_usbkey_clear_config);
	*/


#endif
	
#ifdef CONFIG_AOS_APP_HTTP_PROC
	ret |= OmnKernelApi_addCliCmd("HttpProcSet", aos_cert_module_set); 
	ret |= OmnKernelApi_addCliCmd("HttpProcTest", aos_httpproc_test_run);
	//ret |= OmnKernelApi_addCliCmd("aos_cert_module_set", aos_cert_module_set);
#endif

#ifdef CONFIG_AOS_SSL
	// ssl cli
	ret |= OmnKernelApi_addCliCmd("SSLHardwardSet", AosSsl_hardwareAcceleratorCli);
	ret |= OmnKernelApi_addCliCmd("SSLSessionShow", AosSsl_sessionShowCli);
	ret |= OmnKernelApi_addCliCmd("SSLSessionTimer", AosSsl_sessionTimerCli);
	ret |= OmnKernelApi_addCliCmd("SSLSessionLife", AosSsl_sessionLifeCli);
    ret |= OmnKernelApi_addCliCmd("set_sess_status", AosSsl_setSessionReuseCli);
    ret |= OmnKernelApi_addCliCmd("set_ciphers", AosSsl_setCiphersCli);
    ret |= OmnKernelApi_addCliCmd("show_ciphers", AosSsl_showCiphersCli);
    ret |= OmnKernelApi_addCliCmd("show_supported_ciphers", AosSsl_showSupportedCiphersCli);
    ret |= OmnKernelApi_addCliCmd("SSLClientAuth", AosSsl_setClientAuthCli);
    ret |= OmnKernelApi_addCliCmd("ssl_save_config", AosSsl_saveConfigCli);
    ret |= OmnKernelApi_addCliCmd("ssl_clear_config", AosSsl_clearConfigCli);
    ret |= OmnKernelApi_addCliCmd("SSLShowConfig", AosSsl_showConfigCli);
#endif

#endif


#ifdef CONFIG_AOS_APP_HTTP_PROC
	// Chen Ding, 09/04/2005
	ret |= OmnKernelApi_addCliCmd("fwdtbl_entry_add", aos_fwdtbl_entry_add_cli);
	ret |= OmnKernelApi_addCliCmd("fwdtbl_entry_del", aos_fwdtbl_entry_del_cli);
	ret |= OmnKernelApi_addCliCmd("fwdtbl_show", aos_fwdtbl_show_cli);
	ret |= OmnKernelApi_addCliCmd("fwdtbl_clearall", aos_fwdtbl_clearall_cli);
	ret |= OmnKernelApi_addCliCmd("fwdtbl_saveconf", aos_fwdtbl_saveconf_cli);
#endif


#ifdef CONFIG_CERT_VERIFY

	// Chen Ding, 10/09/2005
	// certificate manager cli
    ret |= OmnKernelApi_addCliCmd("cert_add", AosCertMgr_addCertCli);
    ret |= OmnKernelApi_addCliCmd("cert_del", AosCertMgr_delCertCli);
    ret |= OmnKernelApi_addCliCmd("cert_show", AosCertMgr_showCertCli);
    ret |= OmnKernelApi_addCliCmd("cert_clear", AosCertMgr_clearAllCli);
    ret |= OmnKernelApi_addCliCmd("cert_save", AosCertMgr_saveConfCli);
    ret |= OmnKernelApi_addCliCmd("import_cert", AosCertMgr_importCertCli);
    ret |= OmnKernelApi_addCliCmd("import_privkey", AosCertMgr_importPrivKeyCli);
    ret |= OmnKernelApi_addCliCmd("import_fprivkey", AosCertMgr_importFPrivKeyCli);
    ret |= OmnKernelApi_addCliCmd("import_fcert", AosCertMgr_importFCertCli);
    ret |= OmnKernelApi_addCliCmd("set_cert_type", AosCertMgr_setCertTypeCli);

	// certificate chain cli
    ret |= OmnKernelApi_addCliCmd("add_cert_chain", AosCertChain_addCli);
    ret |= OmnKernelApi_addCliCmd("del_cert_chain", AosCertChain_delCli);
    ret |= OmnKernelApi_addCliCmd("show_cert_chain", AosCertChain_showCli);
    ret |= OmnKernelApi_addCliCmd("clear_cert_chain", AosCertChain_clearallCli);
    ret |= OmnKernelApi_addCliCmd("save_cert_chain", AosCertChain_saveconfCli);

#endif

	// 
	// Chen Ding, 11/10/2005
	//
    ret |= OmnKernelApi_addCliCmd("log_setfilter", AosTracer_setFilterCli);
    ret |= OmnKernelApi_addCliCmd("log_show", AosTracer_showCli);
    ret |= OmnKernelApi_addCliCmd("module_show", AosModule_showCli);

	//
	// Yang Tao, 03/17/2006
	// 
	//ret |= OmnKernelApi_addCliCmd("sysstat_start",AosSystemStat_startCli);

	//
	// Chen Ding, 11/12/2005
	//
    //ret |= OmnKernelApi_addCliCmd("util_saveconf", AosUtil_saveConfCli);
    //ret |= OmnKernelApi_addCliCmd("util_clearconf", AosUtil_clearConfCli);
    //ret |= OmnKernelApi_addCliCmd("util_showconf", AosUtil_showCli);
	return ret;
}

int OmnKernelApi_addCliCmd(char *id, aosCliFunc func)
{
	if (!sg_cli_tree)
	{
		return aos_progerr_d(("Kernel API tree is null"));
	}

	return aosCharPtree_insert(sg_cli_tree, id, strlen(id), (void *)func, 0);
}
EXPORT_SYMBOL(OmnKernelApi_addCliCmd);

int OmnKernelApi_delCliCmd(char *id)
{
	if (!sg_cli_tree)
	{
		return aos_progerr_d(("Kernel API tree is null"));
	}

	return aosCharPtree_remove(sg_cli_tree, id, 0);
}
EXPORT_SYMBOL(OmnKernelApi_delCliCmd);


int OmnProcKernelApi(char *data,
					 unsigned int *optlen,
					 char *errmsg,
					 const int errlen)
{
	//
	// oprId
	// number of integers (integer)
	// number of strings (integer)
	// integer parm
	// integer parm
	// ...
	// string parm
	// string parm
	// ...
	//
	int ret = -1;
	aosCliFunc func;
	int index;
	struct aosKernelApiParms parms;

	memset(&parms, 0, sizeof(struct aosKernelApiParms));

	if (!sg_cli_tree)
	{
		*optlen = 0;
		strcpy(errmsg, "CLI Tree is null. System is not initialized");
		return eAosRc_CliTreeNull;
	}

	ret = aosCharPtree_get(sg_cli_tree, data, &index, (void **)&func);
	if (!func)
	{
		*optlen = 0;
		strcpy(errmsg, "Command not recognized in kernel");
		return eAosRc_CliCmdNotRecognized;
	}

	ret = aosKernelApi_convert(data, *optlen, &parms);
	if (ret)
	{
		*optlen = 0;
		strcpy(errmsg, "CD: Failed to parse parameters");
		return eAosRc_CliCmdParmError;
	}

	return func(data, optlen, &parms, errmsg, errlen);
}


int aosKernelApi_getMac(struct OmnKernelApi_GetMac *data)
{
//	return aosGetMacByName(data->mMac, data->mIpAddr, data->mDevName);
	return 0;
}


int aosKernelApi_getInt1(int index, int *value, char *data)
{
	//
	// oprId
	// number of integers (integer)
	// number of strings (integer)
	// integer parm
	// integer parm
	// ...
	// string parm
	// string parm
	// ...
	//
	// Index counts from 1.
	//
	int len = strlen(data);
	int *iv = (int *)&data[len+1];
	if (index > iv[0])
	{
		return eAosRc_IndexOutBound;
	}

	*value = iv[index+1];
	return 0;
}


int aosKernelApi_getInt2(int index,
						 int *value1,
						 int *value2,
						 char *data)
{
	int len = strlen(data);
	int *iv = (int *)&data[len+1];
	if (index +1 > iv[0])
	{
		return eAosRc_IndexOutBound;
	}

	*value1 = iv[index+1];
	*value2 = iv[index+2];
	return 0;
}


int aosKernelApi_getInt3(int index,
						 int *value1,
						 int *value2,
						 int *value3,
						 char *data)
{
	int len = strlen(data);
	int *iv = (int *)&data[len+1];
	if (index + 2 > iv[0])
	{
		return eAosRc_IndexOutBound;
	}

	*value1 = iv[index+1];
	*value2 = iv[index+2];
	*value3 = iv[index+3];
	return 0;
}


int aosKernelApi_getInt4(int index,
						 int *value1,
						 int *value2,
						 int *value3,
						 int *value4,
						 char *data)
{
	int len = strlen(data);
	int *iv = (int *)&data[len+1];
	if (index + 3 > iv[0])
	{
		return eAosRc_IndexOutBound;
	}

	*value1 = iv[index+1];
	*value2 = iv[index+2];
	*value3 = iv[index+3];
	*value4 = iv[index+4];
	return 0;
}


int aosKernelApi_getStr1(unsigned int index,
						 char **value,
						 char *data,
						 const unsigned int optlen)
{
	unsigned int len = strlen(data);
	unsigned int *iv = (unsigned int *)&data[len+1];
	unsigned int i, ii;
	char *dd;

	if (len >= optlen || index > iv[1] || (ii = len + 1 + (iv[0] + 2) * 4) >= optlen)
	{
		return eAosRc_IndexOutBound;
	}

	dd = &data[ii];
	for (i=0; i<index-1; i++)
	{
		ii += strlen(dd) + 1;
		if (ii >= optlen)
		{
			return eAosRc_IndexOutBound;
		}
		dd = &data[ii];
	}

	*value = &data[ii];
	return 0;
}


int aosKernelApi_getStr2(unsigned int index,
						 char **value1,
						 char **value2,
						 char *data,
						 const unsigned int optlen)
{
	unsigned int len = strlen(data);
	unsigned int *iv = (unsigned int *)&data[len+1];
	unsigned int i, ii;
	char *dd;

	if (len >= optlen || index + 1 > iv[1] || (ii = len + 1 + (iv[0] + 2) * 4) >= optlen)
	{
		return eAosRc_IndexOutBound;
	}

	dd = &data[ii];
	for (i=0; i<index-1; i++)
	{
		ii += strlen(dd) + 1;
		if (ii >= optlen)
		{
			return eAosRc_IndexOutBound;
		}
		dd = &data[ii];
	}

	*value1 = &data[ii];
	ii += strlen(*value1) + 1;
	if (ii >= optlen)
	{
		return eAosRc_IndexOutBound;
	}
	*value2 = &data[ii];
	return 0;
}


int aosKernelApi_getStr3(unsigned int index,
						 char **value1,
						 char **value2,
						 char **value3,
						 char *data,
						 const unsigned int optlen)
{
	unsigned int len = strlen(data);
	unsigned int *iv = (unsigned int *)&data[len+1];
	unsigned int i, ii;
	char *dd;

	if (len >= optlen || index + 2 > iv[1] || (ii = len + 1 + (iv[0] + 2) * 4) >= optlen)
	{
		return eAosRc_IndexOutBound;
	}

	dd = &data[ii];
	for (i=0; i<index-1; i++)
	{
		ii += strlen(dd) + 1;
		if (ii >= optlen)
		{
			return eAosRc_IndexOutBound;
		}
		dd = &data[ii];
	}

	*value1 = &data[ii];

	ii += strlen(*value1) + 1;
	if (ii >= optlen)
	{
		return eAosRc_IndexOutBound;
	}
	*value2 = &data[ii];

	ii += strlen(*value2) + 1;
	if (ii >= optlen)
	{
		return eAosRc_IndexOutBound;
	}
	*value3 = &data[ii];
	return 0;
}


int aosKernelApi_getStr4(unsigned int index,
						 char **value1,
						 char **value2,
						 char **value3,
						 char **value4,
						 char *data,
						 const unsigned int optlen)
{
	unsigned int len = strlen(data);
	unsigned int *iv = (unsigned int *)&data[len+1];
	unsigned int i, ii;
	char *dd;

	if (len >= optlen || index + 3 > iv[1] || (ii = len + 1 + (iv[0] + 2) * 4) >= optlen)
	{
		return eAosRc_IndexOutBound;
	}

	dd = &data[ii];
	for (i=0; i<index-1; i++)
	{
		ii += strlen(dd) + 1;
		if (ii >= optlen)
		{
			return eAosRc_IndexOutBound;
		}
		dd = &data[ii];
	}

	*value1 = &data[ii];

	ii += strlen(*value1) + 1;
	if (ii >= optlen)
	{
		return eAosRc_IndexOutBound;
	}
	*value2 = &data[ii];

	ii += strlen(*value2) + 1;
	if (ii >= optlen)
	{
		return eAosRc_IndexOutBound;
	}
	*value3 = &data[ii];

	ii += strlen(*value3) + 1;
	if (ii >= optlen)
	{
		return eAosRc_IndexOutBound;
	}
	*value4 = &data[ii];
	return 0;
}
#ifdef __KERNEL__
EXPORT_SYMBOL( aosKernelApi_getInt1 );
EXPORT_SYMBOL( aosKernelApi_getStr1 );
#endif
