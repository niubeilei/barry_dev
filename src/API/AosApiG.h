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
#ifndef AOS_API_ApiG_h
#define AOS_API_ApiG_h

#include "API/AosApiI.h"
//#include "SEInterfaces/ReplicPolicyType.h"
#include "ReplicMgr/ReplicPolicy.h"
#include "SEInterfaces/NetworkMgrObj.h"
#include "SEInterfaces/NetListener.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/Ptrs.h"
#include "SEBase/SecOpr.h"
#include "UtilData/ModuleId.h"
#include "SEInterfaces/ServerInfo.h"
#include "TransClient/Ptrs.h"
#include "TransServer/Ptrs.h"
#include "UserMgmt/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "HDFS/hdfs.h"

#include <sys/sysinfo.h>

static const bool sgParityTable256[256] = 
{
#define P2(n) n, n^1, n^1, n
#define P4(n) P2(n), P2(n^1), P2(n^1), P2(n)
#define P6(n) P4(n), P4(n^1), P4(n^1), P4(n)
	    P6(0),P6(1),P6(1),P6(0)
}; 

extern OmnString AosGetCreator(const AosXmlTagPtr &doc);

extern inline OmnString	AosGetObjid(const AosXmlTagPtr &doc)
{
	aos_assert_r(doc, "");
	return doc->getAttrStr(AOSTAG_OBJID);
}

extern inline OmnString	AosGetSupervisors(
		const AosXmlTagPtr &user_account,
		AosRundata *rdata)
{
	aos_assert_r(user_account, "");
	if (!AosIsUserAccount(user_account)) return "";
	return user_account->getAttrStr(AOSTAG_SUPERVISOR);
}

extern AosUserDomainObjPtr AosGetUserDomain(
		const AosUserAcctObjPtr &user_acct,
		const AosRundataPtr &rdata);

extern OmnString AosGetUserGroups(
		const AosUserAcctObjPtr &user_acct,
		AosRundata *rdata);

// extern OmnString AosGetUserOprGroups(
// 		const AosAccessRcdPtr &accessed_ar,
// 		const AosSecOpr::E opr,
// 		AosRundata *rdata);

// extern OmnString AosGetUserOprRoles(
// 		const AosXmlTagPtr &accessed_ar,
// 		const AosSecOpr::E opr,
// 		AosRundata *rdata);

extern OmnString AosGetUserRoles(
		const AosUserAcctObjPtr &user_acct,
		AosRundata *rdata);

extern AosQueryRsltObjPtr AosGetTableContents(
		const u64 &iilid,
		AosRundata *rdata);

extern AosXmlTagPtr AosGetDocByDocid(
		const u64 &docid,
		const AosRundataPtr &rdata);

extern AosXmlTagPtr AosGetDocByObjid(
		const OmnString &objid,
		const AosRundataPtr &rdata);

extern AosXmlTagPtr AosGetDocByKeys(
		const AosRundataPtr &rdata,
		const OmnString &container,
		const vector<OmnString> &key_names,
		const vector<OmnString> &key_values);

///////////////////////////
// IIL API Start
extern u64 AosGetDocidByObjid(
		const OmnString &objid,
		const AosRundataPtr &rdata);

extern bool AosGetDocid(
		const AosRundataPtr &rdata,
		const OmnString &iilname,
		const OmnString &key,
		u64 &docid);

extern bool AosGetDocid(
		const u64 &iilid,
		const u64 &key,
		u64 &docid,
		const u64 &dft,
		bool &found,
		bool &is_unique,
		const AosRundataPtr &rdata);

extern u64 AosGetKeyedU64Value(
		const u64 &iilid,
		const u64 &key,
		const u64 &dft_value,
		const AosRundataPtr &rdata);

extern u64 AosGetKeyedU64Value(
		const u64 &iilid,
		const u64 &key,
		const u64 &dft_value,
		bool &is_unique,
		const AosRundataPtr &rdata);

extern u64 AosGetKeyedU64Value(
		const u64 &iilid,
		const u64 &key,
		const u64 &dft_value,
		bool &is_unique,
		bool &found,
		AosRundata *rdata);

extern u64 AosGetKeyedStrValue(
		const u64 &iilid,
		const OmnString &key,
		const u64 &dft_value,
		const AosRundataPtr &rdata);

extern u64 AosGetKeyedStrValue(
		const u64 &iilid,
		const OmnString &key,
		const u64 &dft_value,
		bool &is_unique,
		const AosRundataPtr &rdata);

extern u64 AosGetKeyedStrValue(
		const u64 &iilid,
		const OmnString &key,
		const u64 &dft_value,
		bool &is_unique,
		bool &found,
		const AosRundataPtr &rdata);
// IIL API End
///////////////////////////

extern bool AosGetDocids(
		const u64 &sizeid,
		u64 &start_docid,
		int &num_docids,
		bool &overflow,
		const AosRundataPtr &rdata);

extern bool AosGetKeyedStrValues(
		const u64 &iilid,
		const vector<OmnString> &keys,
		const bool need_dft_value,
		const u64 &dft_value,
		AosBuffPtr &buff,
		const AosRundataPtr &rdata);

extern bool AosGetKeyedStrValues(
		const OmnString &iilname,
		const vector<OmnString> &keys,
		const bool need_dft_value,
		const u64 &dft_value,
		AosBuffPtr &buff,
		const AosRundataPtr &rdata);

inline AosNetListenerPtr AosGetNetListener(const AosModuleId::E module_id)
{
	//AosNetworkMgrObjPtr theobj = AosNetworkMgrObj::getNetworkMgr();
	//aos_assert_r(theobj, 0);
	//return theobj->getNetListener(module_id);
	return 0;
}

//extern OmnString AosRetrieveServerIpAddr(const OmnString &name, AosRundata *rdata);

extern bool AosGetField(
		const char *&value,
		int &len,
		const char *data,
		const int data_len,
		const int field_idx,
		const OmnString &separator,
		AosRundata *rdata);

extern OmnString AosGetRemoteWorkingDir(const int server_id);
extern OmnString AosGetLocalWorkingDir();

extern int AosGetPhysicalIdFromFilename(const OmnString &fname);

extern OmnString AosGetBaseDirname();

extern int AosGetNumCpuCores();

extern u64 AosGetSystemTotalMemory();

extern bool AosGetIILDistributionMap(
		const AosQueryContextObjPtr &query_context,
		const int num_cores,
		AosBuffPtr &buff,
		AosRundata *rdata);

extern int AosGetNetFileNextPartition();

extern bool getLocalVirtualMachines(
		vector<int> &vecVirtualMachines,
		AosRundata *rdata);

//ken 2012/08/02
extern bool AosGetSplitValue(
		const OmnString &iilname,
		const AosQueryContextObjPtr &context,
		const int size,
		vector<AosQueryContextObjPtr> &contexts,
		const AosRundataPtr &rdata);

extern u64 AosGetSizeIdByDocid(const u64 &docid);
extern u64 AosGetLocalIdByDocid(const u64 &docid);

extern int AosGetSelfServerId();
extern int AosGetSelfClientId();
extern u32 AosGetSelfProcId();		// Ketty 2013/07/11
extern u32 AosGetSelfCubeGrpId();
extern AosProcessType::E AosGetSelfProcType();
extern vector<u32> & AosGetServerIds();
extern vector<u32> & AosGetTotalCubeIds();
extern bool AosGetTotalCubeGrps(vector<u32> &cube_grps);
extern vector<u32> & AosGetCubeIdsByGrpId(const u32 grp_id);
extern vector<u32> & AosGetCubeSvrIds(const u32 cube_grp_id);
extern int AosGetNumPhysicals();
extern int AosGetNumCubes();
extern int AosGetNextSvrId(const u32 cube_grp_id, const int crt_sid);
extern int AosGetConfigMaster(const u32 cube_grp_id);
extern AosServerInfoPtr AosGetSvrInfo(const u32 svr_id);
extern int AosGetCubeGrpIdByCubeId(const u32 cube_id);
extern int AosGetSelfConfigMaster();
extern int	AosGetSelfCrtMaster();
extern bool AosGetLocalVirtualIds(vector<u32> &vids);
extern u32 AosGetCubeBkpNum();
extern OmnString AosGetProcExeName(const AosProcessType::E tp);
extern AosReplicPolicyType::E AosGetReplicPolicyType();
extern int AosGetBroadcastPort();
extern int AosGetIdealSvrIdByCubeId(const u32 cube_id);

//extern u32 AosGetFrontEndPIdBySvrId(const int svr_id);
//extern AosReplicPolicyObjPtr AosGetReplicPolicy();
//extern u32	AosGetSendProcId(const u32 cube_grp_id, const int svr_id);
//extern bool AosGetNextSvrInfo(const u32 cube_grp_id, int &svr_id, u32 &proc_id);
//extern bool AosGetConfigSvrInfo(const u32 cube_grp_id, int &svr_id, u32 &proc_id);
//extern AosProcInfoPtr AosGetProcInfo(const u32 svr_id, const u32 proc_id);

extern OmnString AosGetFileName(
		const u64 &virtual_dir,
		const u32 seqno,
		const OmnString &prefix,
		AosRundata *rdata);

extern AosUserAcctObjPtr AosGetUserAcct(
		const u64 &user_docid,
		const AosRundataPtr &rdata);

// Chen Ding, 2013/02/06
extern OmnString AosGetWorkingDirname();
extern u64 AosGetDftSiteId();
extern u32 AosGetNumRegions();
extern u32 AosGetRegionId(const u64 &generic_id);
extern int AosGetPhysicalIdByGenericId(
		AosRundata *rdata,
		const u64 &generic_id);
extern AosBitmapObjPtr AosGetBitmap();
extern AosQueryContextObjPtr AosGetQueryContext();

extern int AosGetPhysicalId(const u64 &dist_id);
extern int AosGetPhysicalId(
		char * str,
		const int &str_len,
		const bool use_manual_hash = true);
extern int AosGetPhysicalId(
		const OmnString &str,
		const bool use_manual_hash = true);
extern int AosGetCubeId(const u64 &dist_id);
extern int AosGetCubeId(
		char * str,
		const int &str_len,
		const bool use_manual_hash = true);
extern int AosGetCubeId(
		const OmnString &str,
		const bool use_manual_hash = true);

extern AosCappletPtr AosGetCapplet(
		AosRundata *rdata,
		const OmnString &capplet_id);

extern AosXmlTagPtr AosGetConfig();

// Ketty 2013/05/03
extern int AosGetSvrIdByIpAddr(const OmnString &addr, const int port);

extern u64 AosGetCrtSizeId(
		const u64 &record_docid,
		const AosRundataPtr &rdata);

extern u64 AosGetNextDocid(
		const u64 &record_docid,
		const AosRundataPtr &rdata);

// Ken Lee, 2013/04/21
extern int AosGetDataRecordLenBySizeId(
		const u32 siteid,
		const u64 &sizeid,
		AosRundata *rdata);

extern int AosGetDataRecordLenByDocid(
		const u32 siteid,
		const u64 &docid,
		const AosRundataPtr &rdata);

extern u64 AosGetDataRecordDocidBySizeId(
		const u32 siteid,
		const u64 &sizeid,
		AosRundata *rdata);
	
extern AosDataRecordObjPtr AosGetDataRecordBySizeId(
		const u32 siteid,
		const u64 &sizeid,
		AosRundata *rdata);
	
extern AosDataRecordObjPtr AosGetDataRecordByDocid(
		const u32 siteid,
		const u64 &docid,
		const AosRundataPtr &rdata);

extern OmnString& AosGetCurrentThreadLog();

extern OmnThreadPtr AosGetCurrentThread();
/*
extern AosSchedulerObjPtr AosGetScheduler(
				AosRundata *rdata,
				const AosXmlTagPtr &def);
*/
extern AosAccessRcdPtr AosGetAccessRcd(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &ref_doc,
		const u64 &ref_docid,
		const OmnString &ref_objid,
		const bool parent_flag);

extern AosXmlTagPtr AosGetDocByCloudid(
		const OmnString &cloudid,
		const AosRundataPtr &rdata);

extern AosAccessRcdPtr AosGetAccessRecord(
		const AosRundataPtr &rdata,
		const u64 &docid,
		const bool parent_flag);

extern AosValueRslt AosGetValue(
		AosRundata *rdata,
		const AosXmlTagPtr &sdoc,
		const OmnString &tagname);

extern AosValueRslt AosGetValue(
		AosRundata *rdata,
		const AosXmlTagPtr &sdoc,
		const OmnString &aname,
		const OmnString &tagname);

extern OmnString AosGetValueStr(
		AosRundata *rdata,
		const AosXmlTagPtr &sdoc,
		const OmnString &tagname,
		const OmnString &aname,
		const OmnString &dft);

extern u64 AosGetValueU64(
		AosRundata *rdata,
		const AosXmlTagPtr &sdoc,
		const OmnString &tagname,
		const OmnString &aname,
		const u64 dft);

extern int64_t AosGetValueInt64(
		AosRundata *rdata,
		const AosXmlTagPtr &sdoc,
		const OmnString &tagname,
		const OmnString &aname,
		const int64_t dft);

extern bool AosGetValueBool(
		AosRundata *rdata,
		const AosXmlTagPtr &sdoc,
		const OmnString &tagname,
		const OmnString &aname,
		const bool dft);

extern AosXmlTagPtr AosGetParentContainer(
		const AosRundataPtr &rdata,
		const OmnString &container_objid,
		const bool ispublic);

extern AosSchemaSelectorObjPtr AosGetSchemaSelector(
		AosRundata *rdata,
		const AosXmlTagPtr &worker);

extern bool AosIsDiskErrTest();
extern void AosSetDiskErrTest();
extern AosQueryRsltObjPtr AosGetQueryRslt();

extern bool AosGetFileLength(
		AosRundata *rdata,
		const OmnString &filename,
		const int phy_id,
		int64_t &filelen,
		AosDiskStat &disk_stat);

extern u64 AosGetSchemaDocid(const OmnString &type);

// Chen Ding, 2014/01/01
extern AosXmlTagPtr AosGetDocByKey(
		const AosRundataPtr &rdata,
		const OmnString &container,
		const OmnString &key_name,
		const OmnString &key_value);

// Chen Ding, 2014/01/13
extern AosXmlTagPtr AosGetDocByKey(
		const AosRundataPtr &rdata,
		const OmnString &container,
		const vector<OmnString> &key_names,
		const vector<OmnString> &key_values);

extern u32 AosGenerateParity(const char *data, const u32 data_len);

// Chen Ding, 2014/01/15
extern AosIndexMgrObjPtr AosGetIndexMgr(AosRundata *rdata);

// Chen Ding, 2014/01/30
extern u32 AosGetCrtDay();
extern void AosSetCrtDay(const int day); // This function is for testing only

extern bool AosGetIILNames(
		AosRundata *rdata,
		const AosQueryReqObjPtr &query_req,
		const OmnString &iilname,
		vector<OmnString> &iilnames);


struct AosDiskInfo
{
	u64					mAll;
	u64 				mUsed;
	u64					mFree;
	OmnString 			mFSName;
	OmnString			mMountedDir;
};

extern bool AosGetDiskInfo(vector<AosDiskInfo> &dsInfos);

// Chen Ding, 2015/01/22
extern OmnString AosGetUsername();

#endif

