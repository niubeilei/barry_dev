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
//   DocStore API from DocPkg to Cube
//
// Modification History:
// 2014/11/24 Created by Chen Ding
// 2015/03/14 Copied from Jimo.h by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAPI_JimoDocEngine_h
#define Aos_JimoAPI_JimoDocEngine_h

#include "Rundata/Rundata.h"
#include "SearchEngine/DocReq.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/DocTypes.h"
#include "Util/String.h"

//class AosDocEngineObj;

namespace Jimo
{
extern bool jimoAppendBinaryDoc(
						AosRundata *rdata,
						const OmnString &objid,
						const AosBuffPtr &buff);

extern void jimoBatchGetDocs(
						AosRundata *rdata,
						const int vid,
						const u32 num_docs,
						const u32 blocksize,
						const AosBuffPtr &docid_buff,
						const AosBuffPtr &sizeid_len,
						const AosAsyncRespCallerPtr &resp_caller);

extern AosBuffPtr jimoBatchGetDocs(
						AosRundata *rdata,
						const OmnString scanner_id,
						const int server_id);

extern AosXmlTagPtr jimoGetDocByDocid(
						AosRundata *rdata,
						const OmnString &docid);

extern bool jimoIsShowLog();

extern bool jimoIsLog(const u64 &docid);

extern bool jimoModifyAttrXPath(
						AosRundata *rdata,
						const AosXmlTagPtr &doc,
						const OmnString &aname,
						const OmnString &newvalue,
						const bool value_unique,
						const bool docid_unique,
						const bool checkSecurity,
						const bool need_trigger);

extern bool jimoWriteLockDocByDocid(
						AosRundata *rdata,
						const u64 &docid,
						const int waittimer,
						const int holdtimer);

extern bool jimoWriteUnlockDocByDocid(
						AosRundata *rdata,
						const u64 &docid,
						const AosXmlTagPtr &newdoc);

extern AosXmlTagPtr jimoCreateBinaryDoc(
						AosRundata *rdata,
						const OmnString &docstr,
						const bool is_public,
						const AosBuffPtr &buff);

extern bool jimoModifyBinaryDocByStat(
						AosRundata *rdata,
						const AosXmlTagPtr &doc,
						const AosBuffPtr &buff,
						const u64 &normal_snap_id,
						const u64 &binary_snap_id);

extern AosXmlTagPtr jimoRetrieveBinaryDocByStat(
						AosRundata *rdata,
						const u64 &docid,
						AosBuffPtr &docBuff,
						const u64 &normal_snap_id,
						const u64 &binary_snap_id);

extern bool jimoGetTableNames(
						AosRundata *rdata,
						vector<string> &values);

extern u64 jimoGetJQLDocidByTablename(
						AosRundata *rdata,
						const string &tablename);

extern bool jimoDropJQLTable(
						AosRundata *rdata,
						const OmnString &table_name);

extern bool jimoCreateDocByJQL(
						AosRundata *rdata,
						const OmnString &container_objid,
						const OmnString &objid,
						const OmnString &doc);

extern bool jimoDeleteDocByDocid(
						AosRundata *rdata,
						const int64_t docid);

extern bool jimoDeleteDocByObjid(
						AosRundata *rdata,
						const string &objid);

extern bool jimoGetParentContainerByObjid(
						AosRundata *rdata,
						const string &objid,
						string &doc);

extern bool jimoGetParentContainerByDocid(
						AosRundata *rdata,
						const int64_t docid,
						string &doc);

extern bool jimoBatchSendDocids(
						AosRundata *rdata,
						const OmnString &scanner_id,
						const int serverid,
						const AosBitmapObjPtr &bitmap);

extern bool jimoBatchSendDocids(
						AosRundata *rdata,
						const OmnString &scanner_id,
						const int serverid,
						const AosBitmapObjPtr &bitmap,
						const AosDocClientCallerPtr &caller);

extern bool jimoBindObjid(
						AosRundata *rdata,
						OmnString &objid,
						u64 &docid,
						bool &objid_changed,
						const bool resolve,
						const bool keepDocid);

extern bool jimoBindCloudid_doc(
						AosRundata *rdata,
						const OmnString &cid,
						const u64 &docid);

extern AosXmlTagPtr jimoCreateBinaryDoc(
						AosRundata *rdata,
						const OmnString &docstr,
						const bool is_public,
						const AosBuffPtr &buff,
						const int &vid);

extern AosXmlTagPtr jimoCreateDocByTemplate1(
						AosRundata *rdata,
						const OmnString &cid,
						const OmnString &objid,
						const OmnString &template_objid);

extern bool jimoCreateDoc1(
						AosRundata *rdata,
						const AosXmlTagPtr &cmd,
						const OmnString &target_cid,
						const AosXmlTagPtr &newdoc,
						const bool resolveObjid,
						const AosDocSvrCbObjPtr &caller,
						void *userdata,
						const bool keepDocid);

extern AosXmlTagPtr jimoCreateDocSafe1(
						AosRundata *rdata,
						const OmnString &docstr,
						const OmnString &cloudid,
						const OmnString &objid_base,
						const bool is_public,
						const bool checkCreation,
						const bool keepDocid,
						const bool reserved,
						const bool cid_required,
						const bool check_ctnr);

extern u64 jimoCreateDoc(
						AosRundata *rdata,
						const AosXmlTagPtr &doc);


extern AosXmlTagPtr jimoCreateDocSafe3(
						AosRundata *rdata,
						const AosXmlTagPtr &doc,
						const OmnString &cloudid,
						const OmnString &objid_base,
						const bool is_public,
						const bool checkCreation,
						const bool keepDocid,
						const bool reserved,
						const bool cid_required,
						const bool check_ctnr,
						const bool saveDocFlag);

extern bool jimoCreateJQLTable(
						AosRundata *rdata,
						const OmnString &table_def);

extern u64 jimoCreateSnapshot(
						AosRundata *rdata,
						const int virtual_id,
						const u64 snap_id,
						const AosDocType::E doc_type,
						const u64 &task_docid);

extern bool jimoCommitSnapshot(
						AosRundata *rdata,
						const int virtual_id,
						const AosDocType::E doc_type,
						const u64 &snap_id,
						const u64 &task_docid);

extern AosXmlTagPtr jimoCreateRootCtnr(
						AosRundata *rdata,
						const OmnString &docstr);

extern AosXmlTagPtr jimoCloneDoc(
						AosRundata *rdata,
						const OmnString &cloudid,
						const OmnString &fromobjid,
						const OmnString &toobjid,
						const bool is_public);

extern bool jimoDeleteBinaryDoc(
						AosRundata *rdata,
						const OmnString &objid);

extern bool jimoDeleteBinaryDoc(
						AosRundata *rdata,
						const u64 &docid);

extern bool jimoDeleteDoc(
						AosRundata *rdata,
						const AosXmlTagPtr &origdocroot);

extern bool jimoDeleteDoc(
						AosRundata *rdata,
						const AosXmlTagPtr &origdocroot,
						const OmnString &ctnr_objid,
						const bool deleteFlag);

extern bool jimoDeleteObj(
						AosRundata *rdata,
						const OmnString &docid,
						const OmnString &theobjid,
						const OmnString &container,
						const bool deleteFlag);

extern u64 jimoDoesObjidExist(
						AosRundata *rdata,
						const u32 siteid,
						const OmnString &objid);

extern AosAccessRcdPtr jimoGetAccessRcd(
						AosRundata *rdata,
						const AosXmlTagPtr &ref_doc,
						const u64 &ref_docid,
						const OmnString &ref_objid,
						const bool parent_flag);

extern AosAccessRcdPtr jimoGetAccessRecord(
						AosRundata *rdata,
						const u64 &docid,
						const bool parent_flag);

extern OmnString jimoGetCloudid(
						AosRundata *rdata,
						const u64 &user_docid);

extern AosXmlTagPtr jimoGetDeletedDoc(
						AosRundata *rdata,
						const OmnString &objid,
						const u64 &docid);

extern AosXmlTagPtr jimoGetDoc(
						AosRundata *rdata,
						const u64 &docid,
						const OmnString &objid,
						bool &duplicated);

extern AosXmlTagPtr jimoGetDocByDocid(
						AosRundata *rdata,
						const u64 docid);

extern AosXmlTagPtr jimoGetDocByKey(
						AosRundata *rdata,
						const OmnString &container,
						const OmnString &key_name,
						const OmnString &key_value);

extern AosXmlTagPtr jimoGetDocByKeys(
						AosRundata *rdata,
						const OmnString &container,
						const vector<OmnString> &key_names,
						const vector<OmnString> &key_values);

extern AosXmlTagPtr jimoGetDocByObjid(
						AosRundata *rdata,
						const OmnString &objid);

extern AosXmlTagPtr jimoGetDocByCloudid(
						AosRundata *rdata,
						const OmnString &cid);

extern u64 jimoGetDocidByObjid(
						AosRundata *rdata,
						const OmnString &objid,
						const bool flag = true);

extern u64 jimoGetDocidByCloudid(
						AosRundata *rdata,
						const u32 siteid,
						const OmnString &cid);

extern OmnString jimoGetObjidByDocid(
						AosRundata *rdata,
						const u64 &docid);

extern AosXmlTagPtr jimoGetParentContainer(
						AosRundata *rdata,
						const OmnString &container_objid,
						const bool ispublic);

extern bool jimoGetJQLDocs(
						AosRundata *rdata,
						const JQLTypes::DocType	&type,
						vector<string> &values);

extern bool jimoGetDocByJQL(
						AosRundata *rdata,
						const string &objid,
						string &doc);

extern AosUserAcctObjPtr jimoGetUserAcct(
						AosRundata *rdata,
						const u64 &user_docid);

extern OmnString jimoIncrementValue(
						AosRundata *rdata,
						const u64 docid,
						const OmnString &objid,
						const OmnString &aname,
						const OmnString &initvalue,
						const bool value_unique,
						const bool docid_unique);

extern bool jimoIsRepairing();

extern bool jimoIsCloudidBound(
						AosRundata *rdata,
						const OmnString &cid,
						u64 &userid);

extern bool jimoInsertInto(
						AosRundata *rdata,
						const AosXmlTagPtr &data);

extern bool jimoMergeSnapshot(
						AosRundata *rdata,
						const int virtual_id,
						const AosDocType::E doc_type,
						const u64 &target_snap_id,
						const u64 &merge_snap_id);

extern bool jimoModifyAttrStr(
						AosRundata *rdata,
						const AosXmlTagPtr &doc,
						const OmnString &aname,
						const OmnString &newvalue,
						const bool value_unique,
						const bool docid_unique,
						const bool checkSecurity,
						const bool need_trigger = true);

extern bool jimoModifyAttrStr1(
						AosRundata *rdata,
						const u64 &docid,
						const OmnString &objid,
						const OmnString &aname,
						const OmnString &newvalue,
						const bool value_unique,
						const bool docid_unique,
						const bool checkSecurity,
						const bool need_trigger = true);

extern bool jimoModifyAttrU64(
						AosRundata *rdata,
						const AosXmlTagPtr &doc,
						const OmnString &aname,
						const u64 &newvalue,
						const bool value_unique,
						const bool docid_unique,
						const bool checkSecurity,
						const bool need_trigger = true);

extern bool jimoModifyObj(
						AosRundata *rdata,
						const AosXmlTagPtr &doc);

extern bool jimoModifyObj(
						AosRundata *rdata,
						const AosXmlTagPtr &newxml,
						const OmnString &rename,
						const bool synobj);

extern bool jimoProcDocLock(
						AosRundata *rdata,
						const AosXmlTagPtr &req,
						const u64 &docid);

extern void jimoProcNotifyMsg(
						AosRundata *rdata,
						const u64 &docid,
						const u32 siteid);

extern bool jimoPurgeDoc(
						AosRundata *rdata,
						const OmnString &objid);

extern bool jimoPurgeDoc(
						AosRundata *rdata,
						const u64 &docid);

extern bool jimoPurgeDocByCloudid(
						AosRundata *rdata,
						const OmnString &cid);

extern AosXmlTagPtr jimoReleaseWriteLock(
						AosRundata *rdata,
						const AosXmlTagPtr &doc);

extern AosXmlTagPtr jimoReleaseWriteLockDocByObjid(
						AosRundata *rdata,
						const OmnString &objid);

extern AosXmlTagPtr jimoReleaseWriteLockDocByDocid(
						AosRundata *rdata,
						const u64 &docid);

extern AosXmlTagPtr jimoRetrieveBinaryDoc(
						AosRundata *rdata,
						const OmnString &objid,
						AosBuffPtr &buff);

extern AosXmlTagPtr jimoRetrieveBinaryDoc(
						AosRundata *rdata,
						const u64 &docid,
						AosBuffPtr &buff);

extern void jimoRetrieveBinaryDoc(
						AosRundata *rdata,
						const AosAsyncRespCallerPtr &resp_caller,
						const u64 &docid);

extern bool jimoRollbackSnapshot(
						AosRundata *rdata,
						const int virtual_id,
						const AosDocType::E doc_type,
						const u64 &snap_id,
						const u64 &task_docid);

extern bool jimoWriteAccessDoc(
						AosRundata *rdata,
						const u64 &owndocid,
						AosXmlTagPtr &arcd);

extern AosXmlTagPtr jimoWriteLockDocByObjid(
						AosRundata *rdata,
						const OmnString &objid);

extern AosXmlTagPtr jimoWriteLockDocByDocid(
						AosRundata *rdata,
						const u64 &docid);

extern bool jimoAddCreateLogRequest(
						AosRundata *rdata,
						const u64 &userid,
						const AosXmlTagPtr &doc);

extern bool jimoAddDeleteLogRequest(
						AosRundata *rdata,
						const AosXmlTagPtr &doc,
						const u64 userid);

extern bool jimoBatchDataReceived(
						AosRundata *rdata,
						const OmnString &scannerid,
						const AosBuffPtr &contents,
						const int serverid,
						const bool finished,
						const int num);

extern bool jimoReadLockDocByDocid(
						AosRundata *rdata,
						const u64 &docid,
						const int waittimer,
						const int holdtimer);

extern bool jimoReadUnlockDocByDocid(
						AosRundata *rdata,
						const u64 &docid);

extern AosXmlTagPtr jimoGetDocByKey(
						AosRundata *rdata,
						const OmnString &tablename);
	/*
extern bool jimoGetDocByName(AosRundata *rdata,
						AosXmlTagPtr &doc,
						const OmnString &container_objid,
						const OmnString &key_fieldname,
						const OmnString &key_value);

extern AosXmlTagPtr jimoGetDocByDocid(
						AosRundata *rdata,
						const u64 &docid,
						const u32 rep_policy,
						AosClusterObj *cluster);


extern bool jimoAddReq(	AosRundata *rdata,
						const AosTransPtr &trans);


extern AosXmlTagPtr jimoGetDocByKey(
						AosRundata *rdata,
						const OmnString &tablename);


extern AosXmlTagPtr jimoGetDocByName(
						AosRundata *rdata,
						const OmnString &container_objid,
						const OmnString &key_field_name,
						const OmnString &key_value,
						const OmnString &cluster_name);

extern AosXmlTagPtr jimoGetAdminDocByName(
						AosRundata *rdata,
						const OmnString &container_objid,
						const OmnString &key_field_name,
						const OmnString &key_value);


*/
};
#endif


