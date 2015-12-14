////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// handle the SEServer send request to MsgServer
//
// Modification History:
// 08/13/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "API/AosApiG.h"

#include "API/AosApi.h"
#include "ErrorMgr/ErrorMgr.h"
#include "ErrorMgr/ErrmsgId.h"
#include "Rundata/Rundata.h"
#include "SEBase/SecReq.h"
#include "SEInterfaces/Capplet.h"
#include "SEInterfaces/Datalet.h"
#include "SEInterfaces/Scheduler.h"
#include "SEInterfaces/ValueSelObj.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/SchemaMgrObj.h"
#include "SEInterfaces/IndexMgrObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/SiteMgrObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEInterfaces/SiteMgrObj.h"
#include "SEInterfaces/StorageMgrObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/NetListener.h"
#include "SEInterfaces/NetFileCltObj.h"
#include "SEInterfaces/UserMgmtObj.h"
#include "SEInterfaces/SizeIdMgrObj.h"
#include "SEInterfaces/TransCltObj.h"
#include "SEInterfaces/TransSvrObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEInterfaces/ReplicMgrObj.h"
#include "SEInterfaces/IndexMgrObj.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadMgr.h"
#include "TransUtil/TransUtil.h"
#include "UserMgmt/UserDomain.h"
#include "Util/OmnNew.h"
#include "Util/ValueRslt.h"
#include "Util/SystemDefs.h"
#include "Util1/Time.h"
#include "XmlUtil/AccessRcd.h"
#include <signal.h>


#define _FILE_OFFSET_BITS 64
#define __USE_FILE_OFFSET64
#include <stdio.h>
#include <string.h>
#include <mntent.h>
#include <sys/statfs.h>


static AosUserMgmtObjPtr sgUserMgmt;
int	gCrtPhysicalId = -1;
static OmnMutex sgLock;
static bool	sgIsDiskErrTester = false;
static vector<u32> sInvalidVt;

OmnString AosGetCreator(const AosXmlTagPtr &doc)
{
	if (!doc) return "";
	return doc->getAttrStr(AOSTAG_CREATOR);
}


AosUserDomainObjPtr AosGetUserDomain(
		const AosUserAcctObjPtr &user_acct,
		const AosRundataPtr &rdata)
{
	if (!user_acct) return 0;
	OmnString objid = user_acct->getDomainObjid();
	if (objid == "")
	{
		AosSetError(rdata, AosErrmsgId::eMissingParentContainer)
			<< user_acct->getObjid();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	try
	{
		if (!sgUserMgmt) sgUserMgmt = AosUserMgmtObj::getUserMgmt();
		aos_assert_r(sgUserMgmt, 0);
		return sgUserMgmt->getUserDomain1(objid, user_acct, rdata);
	}

	catch (...)
	{
		AosSetError(rdata, AosErrmsgId::eExceptionCreateUserDomain)
			<< user_acct->getAttrStr(AOSTAG_OBJID);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}
}


OmnString AosGetUserGroups(
		const AosUserAcctObjPtr &user_acct,
		AosRundata *rdata)
{
	if (!user_acct) return "";
	return user_acct->getUserGroups();
}


/*
OmnString AosGetAccessedGroups(
		const AosAccessRcdPtr &accessed_ard,
		const AosSecOpr::E opr,
		AosRundata *rdata)
{
	if (!accessed_ard) return "";
	return accessed_ard->getGroups(opr);
}


OmnString AosGetAccessedRoles(
		const AosAccessRcdPtr &accessed_ard,
		const AosSecOpr::E opr,
		AosRundata *rdata)
{
	if (!accessed_ard) return "";
	return accessed_ard->getRoles(opr);
}
*/


OmnString AosGetUserRoles(
		const AosUserAcctObjPtr &user_acct,
		AosRundata *rdata)
{
	if (!user_acct) return "";
	return user_acct->getUserRoles();
}


AosQueryRsltObjPtr AosGetTableContents(
		const u64 &iilid,
		AosRundata *rdata)
{
	AosQueryRsltObjPtr queryrslt = AosQueryRsltObj::getQueryRsltStatic();
	aos_assert_r(queryrslt, 0);
	queryrslt->setWithValues(true);
	bool rslt = AosDbQuery(iilid, queryrslt, 0, eAosOpr_an, "*", rdata);
	if (!rslt) return 0;
	return queryrslt;
}

	
AosXmlTagPtr AosGetDocByDocid(
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, 0);
	return docclient->getDocByDocid(docid, rdata);
}


AosXmlTagPtr AosGetDocByObjid(
		const OmnString &objid,
		const AosRundataPtr &rdata)
{
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, 0);
	return docclient->getDocByObjid(objid, rdata);
}


AosXmlTagPtr AosGetDocByKeys(
	 	const AosRundataPtr &rdata,
	 	const OmnString &container,
	 	const vector<OmnString> &key_names,
	 	const vector<OmnString> &key_values)
{
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, 0);
	return docclient->getDocByKeys(rdata, container, key_names, key_values);
}


///////////////////////////
// IIL API Start
u64 AosGetDocidByObjid(
		const OmnString &objid,
		const AosRundataPtr &rdata)
{
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, false);
	return iilclient->getDocidByObjid(objid, rdata);
}


bool AosGetDocid(
		const AosRundataPtr &rdata,
		const OmnString &iilname,
		const OmnString &key,
		u64 &docid)
{
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, false);
	bool is_unique;
	bool rslt = iilclient->getDocid(iilname, key,
		eAosOpr_eq, false, docid, is_unique, rdata);
	aos_assert_rr(rslt, rdata, false);
	if (!is_unique)
	{
		AosSetErrorUser(rdata, "iilclientobj_docid_not_unique") << docid << enderr;
		return false;
	}
	return true;
}


bool AosGetDocid(
		const u64 &iilid,
		const u64 &key,
		u64 &value,
		const u64 &dft,
		bool &found,
		bool &is_unique,
		const AosRundataPtr &rdata)
{
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, false);
	return iilclient->getDocid(iilid, key,
		value, dft, found, is_unique, rdata);
}


u64 AosGetKeyedU64Value(
		const u64 &iilid,
		const u64 &key,
		const u64 &dft_value,
		const AosRundataPtr &rdata)
{
	bool is_unique;
	u64 value = AosGetKeyedU64Value(iilid, key, dft_value, is_unique, rdata);
	if (is_unique) return value;
	return dft_value;
}


u64 AosGetKeyedU64Value(
		const u64 &iilid,
		const u64 &key,
		const u64 &dft_value,
		bool &is_unique,
		const AosRundataPtr &rdata)
{
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, false);
	
	bool found;
	u64 docid = 0;
	bool rslt = iilclient->getDocid(iilid, key, docid,
		dft_value, found, is_unique, rdata);
	if (!rslt) return dft_value;
	return docid;
}	


u64 AosGetKeyedU64Value(
		const u64 &iilid,
		const u64 &key,
		const u64 &dft_value,
		bool &is_unique,
		bool &found,
		AosRundata *rdata)
{
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, false);
	
	u64 docid = 0;
	bool rslt = iilclient->getDocid(iilid, key, docid,
		dft_value, found, is_unique, rdata);
	if (!rslt || !found) return dft_value;
	return docid;
}	


u64 AosGetKeyedStrValue(
		const u64 &iilid,
		const OmnString &key,
		const u64 &dft_value,
		const AosRundataPtr &rdata)
{
	bool is_unique;
	u64 value = AosGetKeyedStrValue(iilid, key, dft_value, is_unique, rdata);
	if (is_unique) return value;
	return dft_value;
}


u64 AosGetKeyedStrValue(
		const u64 &iilid,
		const OmnString &key,
		const u64 &dft_value,
		bool &is_unique,
		const AosRundataPtr &rdata)
{
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, false);
	
	u64 docid = 0;
	bool rslt = iilclient->getDocid(iilid, key, docid, is_unique, rdata);
	if (!rslt) return dft_value;
	return docid;
}	


u64 AosGetKeyedStrValue(
		const u64 &iilid,
		const OmnString &key,
		const u64 &dft_value,
		bool &is_unique,
		bool &found,
		const AosRundataPtr &rdata)
{
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, false);
	
	u64 docid = 0;
	bool rslt = iilclient->getDocid(iilid, key, docid,
		dft_value, found, is_unique, rdata);
	if (!rslt || !found) return dft_value;
	return docid;
}	


bool AosGetKeyedStrValues(
			const u64 &iilid,
			const vector<OmnString> &keys,
			const bool need_dft_value,
			const u64 &dft_value,
			AosBuffPtr &buff,
			const AosRundataPtr &rdata)
{
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, false);
	
	return iilclient->getDocidsByKeys(iilid, keys, need_dft_value, dft_value, buff, rdata);
}


bool AosGetKeyedStrValues(
			const OmnString &iilname,
			const vector<OmnString> &keys,
			const bool need_dft_value,
			const u64 &dft_value,
			AosBuffPtr &buff,
			const AosRundataPtr &rdata)
{
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, false);
	
	return iilclient->getDocidsByKeys(iilname, keys, need_dft_value, dft_value, buff, rdata);
}
// IIL API End
///////////////////////////

u64 AosGetCrtSizeId(
		const u64 &record_docid,
		const AosRundataPtr &rdata)
{
	AosSizeIdMgrObjPtr obj = AosSizeIdMgrObj::getSizeIdMgr();
	aos_assert_r(obj, false);
	return obj->getCrtSizeId(record_docid, rdata);
}


u64 AosGetNextDocid(
		const u64 &record_docid,
		const AosRundataPtr &rdata)
{
	AosSizeIdMgrObjPtr obj = AosSizeIdMgrObj::getSizeIdMgr();
	aos_assert_r(obj, false);
	return obj->getNextDocid(record_docid, rdata);
}


bool AosGetDocids(
			const u64 &sizeid,
			u64 &start_docid,
			int &num_docids,
			bool &overflow,
			const AosRundataPtr &rdata)
{
	AosSizeIdMgrObjPtr obj = AosSizeIdMgrObj::getSizeIdMgr();
	aos_assert_r(obj, false);
	return obj->getDocids(sizeid, start_docid, num_docids, overflow, rdata);
}


bool
getLocalVirtualMachines(vector<int> &vecVirtualMachines, AosRundata *rdata)
{
	// Ketty 2013/07/13
	OmnNotImplementedYet;
	return false;
	/*
	vecVirtualMachines.clear();
	int numvirtual = AosGetNumCubes();
	int myPhusocalID = AosGetSelfServerId();
	aos_assert_r(myPhusocalID >= 0, false);
	aos_assert_r(numvirtual >= 0, false);
	for(int i = 0; i < numvirtual; i++)
	{
		if(myPhusocalID == AosGetPhysicalIdByVirtualId(i))
		{
			vecVirtualMachines.push_back(myPhusocalID);
		}
	}
	return true;
	*/
}


u64 AosGetSizeIdByDocid(const u64 &docid)
{
	AosSizeIdMgrObjPtr obj = AosSizeIdMgrObj::getSizeIdMgr();
	aos_assert_r(obj, 0);
	return obj->getSizeIdByDocid(docid);
}


u64 AosGetLocalIdByDocid(const u64 &docid)
{
	AosSizeIdMgrObjPtr obj = AosSizeIdMgrObj::getSizeIdMgr();
	aos_assert_r(obj, 0);
	return obj->getLocalIdByDocid(docid);
}


bool AosGetField(
		const char *&value,
		int &len,
		const char *data,
		const int data_len,
		const int field_idx,
		const OmnString &separator,
		AosRundata *rdata)
{
	// This function assumes 'data' is separated by 'separator' into a number
	// of fields. It retrieves the 'field_idx'-th field.
	aos_assert_r(field_idx >= 0, false);

	// Chen Ding, 08/01/2012
	if (!data || data_len <= 0)
	{
		value = 0;
		len = 0;
		return true;
	}

	aos_assert_r(data, false);
	int start_pos = 0;
	char *pp = 0;
	for (int i=0; i<=field_idx; i++)
	{
		if(start_pos >= data_len)
		{
			if(i == field_idx)
			{
				value = 0;
				len = 0;
				return true;
			}
			OmnAlarm << "Invalid operation: " << data << ":" << separator.data() << enderr;
			return false;
		}
		pp = strstr((char*)&data[start_pos], (char*)separator.data());

		if (!pp)
		{
			if (i == field_idx)
			{
				value = &data[start_pos];
				len = data_len - start_pos;
				return true;
			}
				
			OmnAlarm << "Invalid operation: " << data << ":" << separator.data() << enderr;
			return false;
		}
			
		if (i == field_idx)
		{
			value = &data[start_pos];
			if(pp - data >= data_len)
			{
				len = data_len - start_pos;
			}
			else
			{
				len = pp - data - start_pos;
			}
			return true;
		}

		start_pos = pp - data + separator.length();
	}

	OmnAlarm << "Invalid operation: " << data << ":" << separator.data() << enderr;
	return false;
}


// Ketty 2013/05/09
/*
AosTransClientObjPtr AosGetTransClient()
{
	// Chen Ding, 07/01/2012
	// if(sgTransClient)	return sgTransClient;
	// 	
	// AosXmlTagPtr config = OmnApp::getAppConfig()->getFirstChild(AOSCONFIG_TRANS_CLIENT);
	// sgTransClient = OmnNew AosTransClient(config);	
	// sgTransClient->recover();
	// return sgTransClient;
	return AosTransClientObj::getTransClient();
}


AosTransServerObjPtr AosGetTransServer()
{
	return AosTransServerObj::getTransServer();
}
*/

/*
vector<AosServerInfoPtr>& AosGetServers()
{
	return AosNetworkMgrObj::getServers();
}
*/


OmnString AosGetRemoteWorkingDir(const int server_id)
{
	return OmnApp::getRemoteWorkingDir(server_id);
}


OmnString AosGetLocalWorkingDir()
{
	return OmnApp::getLocalWorkingDir();
}


int AosGetPhysicalIdFromFilename(const OmnString &fname)
{
	// This function assumes the file is in the following
	// format:
	// 	basedir/AOSSYSTEMDEF_WORKINGDIRNAME/AOSSYSTEMDEF_WORKINGDIR_PREFIX + phyid + ...
	OmnString prefix = AosGetBaseDirname();
	prefix << "/" << AOSSYSTEMDEF_WORKINGDIRNAME
		<< "/" << AOSSYSTEMDEF_WORKINGDIR_PREFIX;
	if (fname.length() <= prefix.length()) return -1;
	if (strncmp(fname.data(), prefix.data(), prefix.length() != 0)) return -1;

	int idx = prefix.length();
	const char *data = fname.data();
	while (idx < fname.length())
	{
		char c = data[idx];
		if (c >= '0' && c <= '9') continue;

		aos_assert_r(c == '/', -1);
		return atoi(&fname.data()[prefix.length()]);
	}

	return -1;
}


OmnString AosGetBaseDirname()
{
	return OmnApp::getAppBaseDir();
}

u64 AosGetSystemTotalMemory()
{
	struct sysinfo info;
	sysinfo(&info);
	return info.totalram;
}

int AosGetNumCpuCores()
{
	/*
	 *  _SC_NPROCESSORS_CONF
	 *  The number of processors configured.
	 *
	 *  _SC_NPROCESSORS_ONLN
	 *  The number of processors currently online (available).
	 */
	//return ::sysconf(_SC_NPROCESSORS_CONF);
	return ::sysconf(_SC_NPROCESSORS_ONLN);
}


int AosGetNetFileNextPartition()
{
	return 0;
}


// ken 2012/08/02
bool
AosGetSplitValue(
		const OmnString &iilname,
		const AosQueryContextObjPtr &context,
		const int size,
		vector<AosQueryContextObjPtr> &contexts,
		const AosRundataPtr &rdata)
{
	AosIILClientObjPtr obj = AosIILClientObj::getIILClient();
	aos_assert_r(obj, false);
	return obj->getSplitValue(iilname, context, size, contexts, rdata);
}

AosUserAcctObjPtr
AosGetUserAcct(
		const u64 &user_docid,
		const AosRundataPtr &rdata)
{
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, 0);
	return docclient->getUserAcct(user_docid, rdata);
}


// Chen Ding, 2013/02/06
OmnString AosGetWorkingDirname()
{
	static OmnString lsDirname;

	if (lsDirname == "")
	{
		lsDirname = OmnApp::getAppBaseDir();
		aos_assert_r(lsDirname != "", "");
		lsDirname << "/working";
		OmnString command = "mkdir -p ";
		command << lsDirname;
		system(command.data());
	}

	return lsDirname;
}


AosBitmapObjPtr AosGetBitmap()
{
	AosBitmapObjPtr obj = AosBitmapObj::getObject();
	aos_assert_r(obj, 0);
	return obj->createBitmap();
}


u64 AosGetDftSiteId()
{
	AosSiteMgrObjPtr obj = AosSiteMgrObj::getSiteMgr();
	aos_assert_r(obj, 0);
	return obj->getDftSiteId();
}


u32 AosGetNumRegions()
{
	// Ketty 2013/07/13
	OmnNotImplementedYet;
	return false;
	//return AosNetworkMgrObj::getNumRegions();
}


u32 AosGetRegionId(const u64 &id)
{
	// Ketty 2013/07/13
	OmnNotImplementedYet;
	return false;
	//return AosNetworkMgrObj::getRegionId(id);
}


int AosGetPhysicalIdByGenericId(
		AosRundata *rdata,
		const u64 &generic_id)
{
	// Ketty 2013/07/13
	OmnNotImplementedYet;
	return false;
	//return AosNetworkMgrObj::getPhysicalIdByGenericId(rdata, generic_id);
}


AosQueryContextObjPtr AosGetQueryContext()
{
	return AosQueryContextObj::createQueryContextStatic();
}


int AosGetPhysicalId(const u64 &dist_id)
{
	u32 cube_id = AosGetCubeId(dist_id);
	int cube_grp_id = AosGetCubeGrpIdByCubeId(cube_id);
	aos_assert_r(cube_grp_id >=0, false);

	return AosGetConfigMaster(cube_grp_id);
}


int AosGetCubeId(const u64 &dist_id)
{
	int cube_num = AosGetNumCubes();
	aos_assert_r(cube_num > 0, -1);
	return dist_id % cube_num;
}


int AosGetPhysicalId(char *str, const int &len, const bool use_manual_hash)
{
	u32 hashkey = AosGetHashKey(str, len, use_manual_hash);
	return AosGetPhysicalId(hashkey);
}


int AosGetPhysicalId(const OmnString &str, const bool use_manual_hash)
{
	return AosGetPhysicalId((char*)str.data(), str.length(), use_manual_hash);
}



int AosGetCubeId(const OmnString &str, const bool use_manual_hash)
{
	return AosGetCubeId((char*)str.data(), str.length(), use_manual_hash);
}


int AosGetCubeId(char *str, const int &len, const bool use_manual_hash)
{
	u32 hashkey = AosGetHashKey(str, len, use_manual_hash);
	return AosGetCubeId(hashkey);
}


int AosGetIdealSvrIdByCubeId(const u32 cube_id)
{
	return AosNetworkMgrObj::getPhysicalId(cube_id);	
}


// Chen Ding, 2013/03/28
AosCappletPtr
AosGetCapplet(AosRundata *rdata, const OmnString &capplet_id)
{
	OmnNotImplementedYet;
	return 0;
}


AosXmlTagPtr
AosGetConfig()
{
	return OmnApp::getAppConfig();
}


int AosGetDataRecordLenBySizeId(
		const u32 siteid,
		const u64 &sizeid,
		AosRundata *rdata)
{
	AosSizeIdMgrObjPtr obj = AosSizeIdMgrObj::getSizeIdMgr();
	aos_assert_r(obj, 0);
	return obj->getDataRecordLenBySizeId(siteid, sizeid, rdata);
}


int
AosGetDataRecordLenByDocid(
		const u32 siteid,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	return 1024;
	AosSizeIdMgrObjPtr obj = AosSizeIdMgrObj::getSizeIdMgr();
	aos_assert_r(obj, 0);
	return obj->getDataRecordLenByDocid(siteid, docid, rdata);
}


u64
AosGetDataRecordDocidBySizeId(
		const u32 siteid,
		const u64 &sizeid,
		AosRundata *rdata)
{
	AosSizeIdMgrObjPtr obj = AosSizeIdMgrObj::getSizeIdMgr();
	aos_assert_r(obj, 0);
	return obj->getDataRecordDocidBySizeId(siteid, sizeid, rdata);
}


AosDataRecordObjPtr
AosGetDataRecordBySizeId(
		const u32 siteid,
		const u64 &sizeid,
		AosRundata *rdata)
{
	AosSizeIdMgrObjPtr obj = AosSizeIdMgrObj::getSizeIdMgr();
	aos_assert_r(obj, 0);
	return obj->getDataRecordBySizeId(siteid, sizeid, rdata);
}


AosDataRecordObjPtr
AosGetDataRecordByDocid(
		const u32 siteid,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	AosSizeIdMgrObjPtr obj = AosSizeIdMgrObj::getSizeIdMgr();
	aos_assert_r(obj, 0);
	return obj->getDataRecordByDocid(siteid, docid, rdata);
}


OmnThreadPtr AosGetCurrentThread()
{
	return OmnThreadMgr::getCurrentThread();
}


OmnString& AosGetCurrentThreadLog()
{
	static OmnString lsLog;

	OmnThreadPtr thread = AosGetCurrentThread();
	if (thread) return thread->getLog();
	OmnAlarm << "Failed retrieving the current thread" << enderr;
	return lsLog;
}


/*
AosSchedulerPtr AosGetScheduler(
		AosRundata *rdata,
		const AosXmlTagPtr &def)
{
	return AosScheduler::getSchedulerStatic(rdata, def);
}
*/

AosAccessRcdPtr AosGetAccessRcd(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &ref_doc,
		const u64 &ref_docid,
		const OmnString &ref_objid,
		const bool parent_flag)
{
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, 0);
	return docclient->getAccessRcd(rdata, ref_doc, ref_docid, ref_objid, parent_flag);
}


AosAccessRcdPtr AosGetAccessRecord(
				const AosRundataPtr &rdata,
				const u64 &docid,
				const bool parent_flag)
{
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, 0);
	return docclient->getAccessRecord(docid, parent_flag, rdata);
}


AosXmlTagPtr AosGetDocByCloudid(
		const OmnString &cloudid,
		const AosRundataPtr &rdata)
{
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, 0);
	return docclient->getDocByCloudid(cloudid, rdata);
}


AosValueRslt AosGetValue(
		AosRundata *rdata,
		const AosXmlTagPtr &sdoc,
		const OmnString &tagname)
{
	AosValueRslt value;
	AosValueSelObj::getValueStatic(value, sdoc, tagname, rdata);
	return value;
}


AosValueRslt AosGetValue(
		AosRundata *rdata,
		const AosXmlTagPtr &sdoc,
		const OmnString &tagname,
		const OmnString &aname)
{
	AosValueRslt value;
	aos_assert_rr(sdoc, rdata, value);
	if (aname != "")
	{
		OmnString ss = sdoc->getAttrStr(aname);
		if (ss != "")
		{
			value.setStr(ss);
			return value;
		}
	}

	AosValueSelObj::getValueStatic(value, sdoc, tagname, rdata);
	return value;
}


bool AosGetValueBool(
		AosRundata *rdata,
		const AosXmlTagPtr &sdoc,
		const OmnString &tagname,
		const OmnString &aname,
		const bool dft)
{
	AosValueRslt value;
	aos_assert_rr(sdoc, rdata, dft);
	if (aname != "" && sdoc->hasAttribute(aname, false))
	{
		return sdoc->getAttrBool(aname, dft);
	}

	AosValueSelObj::getValueStatic(value, sdoc, tagname, rdata);
	return value.getBool();
}


OmnString AosGetValueStr(
		AosRundata *rdata,
		const AosXmlTagPtr &sdoc,
		const OmnString &tagname,
		const OmnString &aname,
		const OmnString &dft)
{
	AosValueRslt value;
	aos_assert_rr(sdoc, rdata, dft);
	if (aname != "" && sdoc->hasAttribute(aname, false))
	{
		return sdoc->getAttrStr(aname, dft);
	}

	AosValueSelObj::getValueStatic(value, sdoc, tagname, rdata);
	return value.getStr();
}


u64 AosGetValueU64(
		AosRundata *rdata,
		const AosXmlTagPtr &sdoc,
		const OmnString &tagname,
		const OmnString &aname,
		const u64 dft)
{
	AosValueRslt value;
	aos_assert_rr(sdoc, rdata, dft);
	if (aname != "" && sdoc->hasAttribute(aname, false))
	{
		return sdoc->getAttrU64(aname, dft);
	}

	AosValueSelObj::getValueStatic(value, sdoc, tagname, rdata);
	u64 vv;
	bool rslt = value.getU64();
	aos_assert_rr(rslt, rdata, dft);
	return vv;
}


int64_t AosGetValueInt64(
		AosRundata *rdata,
		const AosXmlTagPtr &sdoc,
		const OmnString &tagname,
		const OmnString &aname,
		const int64_t dft)
{
	AosValueRslt value;
	aos_assert_rr(sdoc, rdata, dft);
	if (aname != "" && sdoc->hasAttribute(aname, false))
	{
		return sdoc->getAttrInt64(aname, dft);
	}

	AosValueSelObj::getValueStatic(value, sdoc, tagname, rdata);
	return value.getI64();
}


AosXmlTagPtr AosGetParentContainer(
		const AosRundataPtr &rdata,
		const OmnString &container_objid,
		const bool ispublic)
{
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, 0);
	return docclient->getParentContainer(rdata, container_objid, ispublic);
}


int AosGetSelfServerId()
{
	AosNetworkMgrObjPtr obj = AosNetworkMgrObj::getNetworkMgr();
	aos_assert_r(obj, false);
	return obj->getSelfServerId();
}


int AosGetSelfClientId()
{
	AosNetworkMgrObjPtr obj = AosNetworkMgrObj::getNetworkMgr();
	aos_assert_r(obj, false);
	return obj->getSelfClientId();
}


// Ketty 2013/07/11
u32 AosGetSelfProcId()
{
	AosNetworkMgrObjPtr obj = AosNetworkMgrObj::getNetworkMgr();
	aos_assert_r(obj, false);
	return obj->getSelfProcId();
}


AosReplicPolicyType::E AosGetReplicPolicyType()
{
	AosReplicMgrObjPtr obj = AosReplicMgrObj::getReplicMgr();
	aos_assert_r(obj, AosReplicPolicyType::eInvalid);
	return obj->getPolicyType();
}


int AosGetBroadcastPort()
{
	AosNetworkMgrObjPtr obj = AosNetworkMgrObj::getNetworkMgr();
	aos_assert_r(obj, false);
	return obj->getBroadcastPort();
}


vector<u32> & AosGetServerIds()
{
	AosNetworkMgrObjPtr obj = AosNetworkMgrObj::getNetworkMgr();
	aos_assert_r(obj, sInvalidVt);
	return obj->getSvrIds();
}


vector<u32> & AosGetTotalCubeIds()
{
	AosNetworkMgrObjPtr obj = AosNetworkMgrObj::getNetworkMgr();
	aos_assert_r(obj, sInvalidVt);
	return obj->getTotalCubeIds();
}


bool AosGetTotalCubeGrps(vector<u32> &cube_grps)
{
	AosNetworkMgrObjPtr obj = AosNetworkMgrObj::getNetworkMgr();
	aos_assert_r(obj, false);
	return obj->getTotalCubeGrps(cube_grps);
}


vector<u32> & AosGetCubeIdsByGrpId(const u32 cube_grp_id)
{
	AosNetworkMgrObjPtr obj = AosNetworkMgrObj::getNetworkMgr();
	aos_assert_r(obj, sInvalidVt);
	return obj->getCubeIdsByGrpId(cube_grp_id);
}


int AosGetNumPhysicals()
{
	AosNetworkMgrObjPtr obj = AosNetworkMgrObj::getNetworkMgr();
	aos_assert_r(obj, false);
	return obj->getNumSvrs();
}


//int AosGetNumPhysicals(const AosServerInfo::Type type)
//{
//	return AosGetNumPhysicals();
//}

int AosGetNumCubes()
{
	int cube_num = AosNetworkMgrObj::getNumVirtuals();
	aos_assert_r(cube_num > 0, -1);
	return cube_num;
}

AosQueryRsltObjPtr AosGetQueryRslt()
{
	return AosQueryRsltObj::getQueryRsltStatic();
}

// Ketty 2013/05/03
int AosGetSvrIdByIpAddr(const OmnString &addr, const int port)
{
	// Ketty 2013/05/17 Not need.
	return -1;
	//return AosNetworkMgrObj::getSvrIdByIpAddr(addr, port);
}

vector<u32> & AosGetCubeSvrIds(const u32 cube_grp_id)
{
	AosNetworkMgrObjPtr obj = AosNetworkMgrObj::getNetworkMgr();
	aos_assert_r(obj, sInvalidVt);
	return obj->getCubeSendSvrIds(cube_grp_id);
}


//u32	AosGetSendProcId(const u32 cube_grp_id, const int svr_id)
//{
//AosNetworkMgrObjPtr obj = AosNetworkMgrObj::getNetworkMgr();
//aos_assert_r(obj, false);
//return obj->getSendProcId(cube_grp_id, svr_id);	
//}

int AosGetNextSvrId(const u32 cube_grp_id, const int crt_sid)
{
	AosNetworkMgrObjPtr obj = AosNetworkMgrObj::getNetworkMgr();
	aos_assert_r(obj, false);
	return obj->getNextSvrId(cube_grp_id, crt_sid);
}

int AosGetConfigMaster(const u32 cube_grp_id)
{
	AosNetworkMgrObjPtr obj = AosNetworkMgrObj::getNetworkMgr();
	aos_assert_r(obj, false);
	
	int svr_id = -1;
	return obj->getNextSvrId(cube_grp_id, svr_id);
}

AosServerInfoPtr AosGetSvrInfo(const u32 svr_id)
{
	AosNetworkMgrObjPtr obj = AosNetworkMgrObj::getNetworkMgr();
	aos_assert_r(obj, 0);
	return obj->getSvrInfo(svr_id);
}

//AosProcInfoPtr AosGetProcInfo(const u32 svr_id, const u32 proc_id)
//{
//	AosServerInfoPtr svr_info = AosGetSvrInfo(svr_id);
//	aos_assert_r(svr_info, 0);
//	return svr_info->getProc(proc_id);
//}

int AosGetCubeGrpIdByCubeId(const u32 cube_id)
{
	AosNetworkMgrObjPtr obj = AosNetworkMgrObj::getNetworkMgr();
	aos_assert_r(obj, false);
	return obj->getCubeGrpIdByCubeId(cube_id);
}


int AosGetSelfConfigMaster()
{
	if(!AosIsSelfCubeSvr()) return -1;
	AosNetworkMgrObjPtr obj = AosNetworkMgrObj::getNetworkMgr();
	aos_assert_r(obj, false);
	return obj->getSelfConfigMasterId();	
}

int	AosGetSelfCrtMaster()
{
	if(!AosIsSelfCubeSvr()) return -1;
	AosNetworkMgrObjPtr obj = AosNetworkMgrObj::getNetworkMgr();
	aos_assert_r(obj, false);
	return obj->getSelfCrtMasterId();
}

u32 AosGetSelfCubeGrpId()
{
	//aos_assert_r(AosIsSelfCubeSvr(), 0);
	if(!AosIsSelfCubeSvr()) return -1;
	
	AosNetworkMgrObjPtr obj = AosNetworkMgrObj::getNetworkMgr();
	aos_assert_r(obj, false);
	return obj->getSelfCubeGrpId();
}

// Ketty 2012/08/08
bool AosGetLocalVirtualIds(vector<u32> &vids)
{
	if(!AosIsSelfCubeSvr())	return true;
	AosNetworkMgrObjPtr obj = AosNetworkMgrObj::getNetworkMgr();
	aos_assert_r(obj, false);
	vids = obj->getSelfCubeIds();
	return true;
}

u32 AosGetCubeBkpNum()
{
	AosReplicMgrObjPtr obj = AosReplicMgrObj::getReplicMgr();
	aos_assert_r(obj, false);
	return obj->getCubeBkpNum();
}


AosProcessType::E AosGetSelfProcType()
{
	AosNetworkMgrObjPtr obj = AosNetworkMgrObj::getNetworkMgr();
	aos_assert_r(obj, AosProcessType::eInvalid);
	return obj->getSelfProcType();
}


bool
AosIsDiskErrTest()
{
	return sgIsDiskErrTester;
}

void
AosSetDiskErrTest()
{
	sgIsDiskErrTester = true;
}

OmnString
AosGetProcExeName(const AosProcessType::E tp)
{
	AosNetworkMgrObjPtr obj = AosNetworkMgrObj::getNetworkMgr();
	aos_assert_r(obj, "");
	return obj->getProcExeName(tp);
}

bool AosGetFileLength(
			AosRundata *rdata,
			const OmnString &filename,
			const int phy_id,
			int64_t &filelen,
			AosDiskStat &disk_stat)
{
	return AosNetFileCltObj::getFileLengthStatic(
		filename, phy_id, filelen, disk_stat, rdata);
}

u64 AosGetSchemaDocid(const OmnString &schema_type)
{
	AosSchemaMgrObjPtr obj = AosSchemaMgrObj::getSchemaMgr();
	aos_assert_r(obj, 0);
	return obj->getSchemaDocid(schema_type);
}

// Chen Ding, 2014/01/01
AosXmlTagPtr AosGetDocByKey(
		const AosRundataPtr &rdata,
		const OmnString &container,
		const OmnString &key_name,
		const OmnString &key_value)
{
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_rr(docclient, rdata, 0);
	return docclient->getDocByKey(rdata, container, key_name, key_value);
}


bool AosGetIILNames(
		AosRundata *rdata,
		const AosQueryReqObjPtr &query_req,
		const OmnString &iilname,
		vector<OmnString> &iilnames)
{
	return AosIndexMgrObj::getIILNamesStatic(rdata, query_req, iilname, iilnames);
}


// Chen Ding, 2014/01/13
// AosXmlTagPtr AosGetDocByKey(
// 		AosRundata *rdata,
// 		const OmnString &container,
// 		const vector<OmnString> &key_names,
// 		const vector<OmnString> &key_values)
// {
// 	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
// 	aos_assert_rr(docclient, rdata, 0);
// 	return docclient->getDocByKeys(rdata, container, key_names, key_values);
// }


// Chen Ding, 2014/01/15
AosIndexMgrObjPtr AosGetIndexMgr(AosRundata *rdata)
{
	return AosIndexMgrObj::getIndexMgr(rdata);
}


AosXmlTagPtr AosGetIndexDefDoc(
		AosRundata *rdata,
		const OmnString &table_name,
		const OmnString &field_name)
{
	return AosIndexMgrObj::getIndexDefDocStatic(rdata, table_name, field_name);
}


AosXmlTagPtr AosGetIndexDefDoc(
		AosRundata *rdata,
		const OmnString &iilname)
{
	return AosIndexMgrObj::getIndexDefDocStatic(rdata, iilname);
}


static int sgCrtDayForTestingOnly = 0;
u32 AosGetCrtDay()
{
	// This is for testing only
	return sgCrtDayForTestingOnly;

	// return OmnTime::getCrtDay();
}


// This function is for testing only
void AosSetCrtDay(const int day)
{
	sgCrtDayForTestingOnly = day;
}


bool
AosGetDiskInfo(vector<AosDiskInfo> &dsInfos)
{
	dsInfos.clear();

	struct mntent *ent;
	struct statfs64 stat;

	FILE *fp = setmntent(MOUNTED, "r");
	while ( (ent = getmntent(fp)) )
	{
		bzero(&stat, sizeof(struct statfs));
		if ( -1 == statfs64(ent->mnt_dir,&stat) )
		{
			//perror("statfs:");
			continue;
		}

		// Each directory's total size
		u64 all = stat.f_bsize * stat.f_blocks;

		// Ordinary user's free space
		u64 fre = stat.f_bsize * stat.f_bavail;

		// Super user's free space
		u64 free_to_root = stat.f_bsize * stat.f_bfree;
		u64 used = all - free_to_root;

		if (all == 0) continue;
	
		AosDiskInfo dsInfo;
		dsInfo.mAll = all;
		dsInfo.mUsed = used;
		dsInfo.mFree = fre;
		dsInfo.mFSName = ent->mnt_fsname;
		dsInfo.mMountedDir = ent->mnt_dir;

		dsInfos.push_back(dsInfo);
	}
	endmntent(fp);

	return true;
}


u32 
AosGenerateParity(const char *data, const u32 data_len)
{
	u32 parity_num = 0;
	for (u32 i = 0; i < data_len; i++)
	{
		if (sgParityTable256[*(u8 *)(data + i)])
		{
			parity_num++;
		}
	}
	return parity_num;
}                                                

OmnString AosGetUsername()
{
	OmnNotImplementedYet;
	return "no-name";
}

