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
//   
//
// Modification History:
// 2014/11/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAPI_Jimo_h
#define Aos_JimoAPI_Jimo_h

#include "CounterUtil/Ptrs.h"
#include "IILUtil/IILUtil.h"
#include "JimoAPI/JimoRundata.h"
#include "JimoCall/Ptrs.h"
#include "Util/String.h"
#include "Util/Buff.h"
#include "XmlUtil/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Util/Opr.h"

#include <vector>
using namespace std;

class AosJimoCall;
class AosRundata;

namespace Jimo
{
	bool		jimoInit(AosRundata *rdata, const AosXmlTagPtr &conf);

};

/*
class Jimo
{
public:
	static bool		init(AosRundata *rdata, const AosXmlTagPtr &conf);

	// Hello World Package
	static bool		helloJimo(AosRundata *rdata);
	static bool		helloJimo(
						AosRundata *rdata,
						const OmnString &message,
						AosJimoCallPtr &jimo_call);

	static bool		helloJimo(AosJimoRundata *jimo_rdata);
	static bool		helloJimo(
						AosJimoRundata *jimo_rdata,
						const OmnString &message,
						AosJimoCallPtr &jimo_call);

	// Doc Client Package
	static bool	createDoc(
						AosRundata *rdata,
						const OmnString &doc,
						u64 &docid);

	static bool addReq(
						AosRundata *rdata,
						const AosTransPtr &trans);
	
	
	static bool appendBinaryDoc(
						AosRundata *rdata,
						const OmnString &objid, 
						const AosBuffPtr &buff);
	
	static bool batchDataReceived(
						AosRundata *rdata,
						const OmnString &scannerid,
						const AosBuffPtr &contents,
						const int serverid,
						const bool finished,
						const int num);
	
	static void batchGetDocs(
						AosRundata *rdata,
						const int vid, 
						const u32 num_docs,
						const u32 blocksize,
						const AosBuffPtr &docid_buff,
						const AosBuffPtr &sizeid_len,
						const AosAsyncRespCallerPtr &resp_caller);
	
	
	static AosBuffPtr batchGetDocs(
						AosRundata *rdata,
						const OmnString scanner_id,
						const int server_id);
	
	static bool batchSendDocids(
						AosRundata *rdata,
						const OmnString &scanner_id,
						const int serverid,
						const AosBitmapObjPtr &bitmap);
	
	static bool batchSendDocids(
						AosRundata *rdata,
						const OmnString &scanner_id,
						const int serverid,
						const AosBitmapObjPtr &bitmap,
						const AosDocClientCallerPtr &caller);
	
	static bool bindCloudid_doc(   
						AosRundata *rdata,                   
						const OmnString &cid,
						const u64 &docid,
						const bool &flag);
	
	static AosXmlTagPtr createDoc(
						AosRundata *rdata,
						const AosXmlTagPtr &doc);
	
	
	static bool createDoc1(                            
						AosRundata *rdata,
						const AosXmlTagPtr &cmd,
						const OmnString &target_cid,
						const AosXmlTagPtr &newdoc,
						const bool resolveObjid,
							const AosDocSvrCbObjPtr &caller,
						void *userdata,
						const bool keepDocid);
	
	static AosXmlTagPtr createDocSafe1(
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
	
	static AosXmlTagPtr createDocSafe3(            
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
	
	static bool deleteObj(
						AosRundata *rdata,
						const OmnString &docid,
						const OmnString &theobjid,
						const OmnString &container,
						const bool deleteFlag);
	
	static AosAccessRcdPtr getAccessRcd(
						AosRundata *rdata,
						const AosXmlTagPtr &ref_doc,
						const u64 &ref_docid, 
						const OmnString &ref_objid,
						const bool parent_flag);
	
	static AosXmlTagPtr getDoc(
						AosRundata *rdata,
						const OmnString &docid, 
						const OmnString &objid, 
						bool &duplicated);
		
	static u64 getDocidByCloudid(
						AosRundata *rdata,
						const u32 siteid,
						const OmnString &cid);
	
	static AosXmlTagPtr getDocByObjid(
						AosRundata *rdata,
						const OmnString &objid);
		
	static u64 getDocidByObjid(
						AosRundata *rdata,
						const OmnString &objid,
						const bool &flag);
		
	static AosXmlTagPtr getDocByDocid(
						AosRundata *rdata,
						const u64 &docid);
	    
	static AosXmlTagPtr getDocByDocid(
						AosRundata *rdata,
						const OmnString &docid);
	
	static AosXmlTagPtr getDocByCloudid(
						AosRundata *rdata,
						const OmnString &cid);
	
	static AosUserAcctObjPtr getUserAcct(
						AosRundata *rdata,
						const u64 &user_docid);
	
	static AosXmlTagPtr getDeletedDoc(
						AosRundata *rdata,
						const OmnString &objid,
						const u64 &docid);
	
	static OmnString getObjidByDocid(
						AosRundata *rdata,
						const u64 &docid);
	
	static bool isCloudidBound(
						AosRundata *rdata,
						const OmnString &cid,
						u64 &userid);
	
	static bool isShowLog();
								
	static AosXmlTagPtr getDoc(                   
						AosRundata *rdata,
						const u64 &docid,
						const OmnString &objid);
	
	static OmnString getCloudid(
						AosRundata *rdata,
						const u64 &user_docid);
	
	static AosAccessRcdPtr getAccessRecord(
						AosRundata *rdata,
						const u64 &docid,
						const bool parent_flag);
								
	static OmnString incrementValue(                
						AosRundata *rdata,
						const u64 &docid,
						const OmnString &objid,
						const OmnString &aname,
						const OmnString &initvalue,
						const bool value_unique,
						const bool docid_unique);
	
	static bool isLog(const u64 &docid);

	static bool isRepairing();
	
	static bool modifyAttrStr1(
						AosRundata *rdata,
						const u64 &docid,
						const OmnString &objid,	
						const OmnString &aname,
						const OmnString &newvalue,
						const bool value_unique,
						const bool docid_unique,
						const bool checkSecurity,
						const bool need_trigger);
	
	static bool modifyAttrStr(
						AosRundata *rdata,
						const AosXmlTagPtr &doc,
						const OmnString &aname, 
						const OmnString &newvalue, 
						const bool value_unique, 
						const bool docid_unique, 
						const bool checkSecurity,
						const bool need_trigger);
	
	static bool modifyAttrU64(
						AosRundata *rdata,
						const AosXmlTagPtr &doc,
						const OmnString &aname, 
						const u64 &newvalue, 
						const bool value_unique, 
						const bool docid_unique, 
						const bool checkSecurity,
						const bool need_trigger);
	
	static bool modifyAttrXPath(
						AosRundata *rdata,
						const AosXmlTagPtr &doc,
						const OmnString &aname, 
						const OmnString &newvalue, 
						const bool value_unique, 
						const bool docid_unique, 
						const bool checkSecurity,
						const bool need_trigger);
	
	static bool modifyObj(
						AosRundata *rdata,
						const AosXmlTagPtr &doc);
	
	static bool modifyObj(
						AosRundata *rdata,
						const AosXmlTagPtr &newxml,
						const OmnString &rename,
						const bool synobj);        
	
	static bool purgeDoc(
						AosRundata *rdata,
						const OmnString &objid);
		
	static bool purgeDoc(
						AosRundata *rdata,
						const u64 &docid);
		
	static bool purgeDocByCloudid(
						AosRundata *rdata,
						const OmnString &cid);
	
	static bool writeLockDocByDocid(
						AosRundata *rdata, 
						const u64 &docid,
						const int waittimer,
						const int holdtimer);
	
	static bool writeUnlockDocByDocid(
						AosRundata *rdata, 
						const u64 &docid,
						const AosXmlTagPtr &newdoc);
	
	static AosXmlTagPtr writeLockDocByObjid(
						AosRundata *rdata,
						const OmnString &objid);
	
	static AosXmlTagPtr writeLockDocByDocid(
						AosRundata *rdata,
						const u64 &docid);
	
	static bool writeAccessDoc(
						AosRundata *rdata,
						const u64 &owndocid, 
						AosXmlTagPtr &arcd);
	
	static AosXmlTagPtr releaseWriteLock(
						AosRundata *rdata,
						const AosXmlTagPtr &doc);
	
	static AosXmlTagPtr releaseWriteLockDocByObjid(
						AosRundata *rdata,
						const OmnString &objid);
	
	static AosXmlTagPtr releaseWriteLockDocByDocid(
						AosRundata *rdata,
						const u64 &docid);
	
	static AosXmlTagPtr createBinaryDoc(
						AosRundata *rdata,
						const OmnString &docstr,
						const bool is_public, 
						const AosBuffPtr &buff,
						const int &vid);
	
	static bool deleteBinaryDoc(
						AosRundata *rdata,
						const OmnString &objid);
	
	static bool deleteBinaryDoc(
						AosRundata *rdata,
						const u64 &docid);
	
	static bool modifyBinaryDocByStat(
						AosRundata *rdata,
						const AosXmlTagPtr &doc,
						const AosBuffPtr &buff,
						const u64 &normal_snap_id,
						const u64 &binary_snap_id);
	
	static AosXmlTagPtr retrieveBinaryDocByStat(
						AosRundata *rdata,
						const u64 &docid, 
						AosBuffPtr &docBuff, 
						const u64 &normal_snap_id,
						const u64 &binary_snap_id);
		
	static AosXmlTagPtr retrieveBinaryDoc(
						AosRundata *rdata,
						const OmnString &objid, 
						AosBuffPtr &buff);
		
	static AosXmlTagPtr retrieveBinaryDoc(
						AosRundata *rdata,
						const u64 &docid, 
						AosBuffPtr &buff);
	
	static void retrieveBinaryDoc(
						AosRundata *rdata,
						const AosAsyncRespCallerPtr &resp_caller,
						const u64 &docid);
								
	static bool procDocLock(                         
						AosRundata *rdata,
						const AosXmlTagPtr &req,
						const u64 &docid);
	
	static void procNotifyMsg(
						AosRundata *rdata,
						const u64 &docid, 
						const u32 siteid);
	
	static AosXmlTagPtr getParentContainer(
						AosRundata *rdata,
						const OmnString &container_objid, 
						const bool ispublic);
	
	static bool getTableNames(
						AosRundata *rdata,
						vector<string> &values);
	
	static bool getJQLDocs(
						AosRundata *rdata,
						const JQLTypes::DocType	&type,
						vector<string> &values);
	
	static u64 getJQLDocidByTablename(
						AosRundata *rdata,
						const string &tablename);
	
	static bool dropJQLTable( 
						AosRundata *rdata,
						const OmnString &table_name);
	
	static bool createJQLTable(	
						AosRundata *rdata,
						const OmnString &table_def);
	
	static bool createDocByJQL( 
						AosRundata *rdata,
						const OmnString &container_objid,
						const OmnString &objid,
						const OmnString &doc);
	
	static bool deleteDocByDocid(	
						AosRundata *rdata,
						const int64_t docid);
	
	static bool deleteDocByObjid(	
						AosRundata *rdata,
						const string &objid);
								
	static bool deleteDoc(
						AosRundata *rdata,
						const AosXmlTagPtr &origdocroot);
	
	static bool deleteDoc(
						AosRundata *rdata,
						const AosXmlTagPtr &origdocroot,
						const OmnString &ctnr_objid,
						const bool deleteFlag);
	
	
	static bool getDocByJQL(	
						AosRundata *rdata,
						const string &objid,
						string &doc);
	
	static bool getParentContainerByObjid(	
						AosRundata *rdata,
						const string &objid,
						string &doc);
	
	static bool getParentContainerByDocid(	
						AosRundata *rdata,
						const int64_t docid,
						string &doc);
	
	static AosXmlTagPtr getDocByKey(
						AosRundata *rdata, 
						const OmnString &container, 
						const OmnString &key_name, 
						const OmnString &key_value);
	
	static AosXmlTagPtr getDocByKey(
						AosRundata *rdata, 
						const OmnString &tablename);
	
	static AosXmlTagPtr getDocByKeys(
						AosRundata *rdata, 
						const OmnString &container, 
						const vector<OmnString> &key_names, 
						const vector<OmnString> &key_values);
	
	static bool mergeSnapshot(
						AosRundata *rdata,
						const int virtual_id,
						const AosDocType::E doc_type,
						const u64 &target_snap_id,
						const u64 &merge_snap_id);
	
	static u64 createSnapshot(
						AosRundata *rdata,
						const int virtual_id,
						const u64 snap_id,
						const AosDocType::E doc_type,
						const u64 &task_docid);
	
	static bool commitSnapshot(
						AosRundata *rdata,
						const int virtual_id,
						const AosDocType::E doc_type,
						const u64 &snap_id,
						const u64 &task_docid);
	
	static bool rollbackSnapshot(
						AosRundata *rdata,
						const int virtual_id,
						const AosDocType::E doc_type,
						const u64 &snap_id,
						const u64 &task_docid);
	
	static AosXmlTagPtr createRootCtnr(              
						AosRundata *rdata,
						const OmnString &docstr);
	
	static AosXmlTagPtr createDocByTemplate1(             
						AosRundata *rdata,
						const OmnString &cid,
						const OmnString &objid,
						const OmnString &template_objid);
	
	static bool bindObjid(                     
						AosRundata *rdata,
						OmnString &objid,
						u64 &docid,
						bool &objid_changed,
						const bool resolve,
						const bool keepDocid);
	
	static u64 doesObjidExist(                 
						AosRundata *rdata,
						const u32 siteid,
						const OmnString &objid);
	
	static AosXmlTagPtr cloneDoc(                    
						AosRundata *rdata,
						const OmnString &cloudid,
						const OmnString &fromobjid,
						const OmnString &toobjid,
						const bool is_public);
	
	static bool readLockDocByDocid(
						AosRundata *rdata, 
						const u64 &docid,
						const int waittimer,
						const int holdtimer);
	
		
	static bool readUnlockDocByDocid(
						AosRundata *rdata, 
						const u64 &docid);
	
	// IIL Package					
	static u64 		getDocidByObjid(
						AosRundata *rdata,
						const OmnString objid);
	static bool 	bindObjid(
						AosRundata *rdata,
						const OmnString &objid, 
						const u64 &docid);
	static bool 	unbindObjid(
						AosRundata *rdata,
						const OmnString &objid, 
						const u64 &docid);

	static bool		addStrValueDoc(
						AosRundata *rdata,
						const u64 &iilid,
						const OmnString &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique);
	static bool		addStrValueDoc(
						AosRundata *rdata,
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique);
	static bool		removeStrValueDoc(
						AosRundata *rdata,
						const u64 &iilid,
						const OmnString &value,
						const u64 &docid);
	static bool		removeStrValueDoc(
						AosRundata *rdata,
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid);
	static bool		addU64ValueDoc(
						AosRundata *rdata,
						const u64 &iilid,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique);
	static bool		addU64ValueDoc(
						AosRundata *rdata,
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique);
	static bool		removeU64ValueDoc(
						AosRundata *rdata,
						const u64 &iilid,
						const u64 &value,
						const u64 &docid);
	static bool		removeU64ValueDoc(
						AosRundata *rdata,
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid);
	static bool		getDocidByObjid(
						AosRundata *rdata,
						const u32 siteid,
						const OmnString &objid,
						u64 &docid,
						bool &isunique);
	static bool		incrementKeyedValue(
						AosRundata *rdata,
						const OmnString &iilname,
						const OmnString &key,
						u64 &value,
						const bool persis_flag,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag);
	static bool		getDocid(
						AosRundata *rdata,
						const u64 &iilid,
						const OmnString &key,
						const AosOpr opr,
						const bool reverse,
						u64 &docid,
						bool &isunique);
	static bool		getDocid(
						AosRundata *rdata,
						const OmnString &iilname,
						const OmnString &key,
						const AosOpr opr,
						const bool reverse,
						u64 &docid,
						bool &isunique);
	static bool		getDocid(
						AosRundata *rdata,
						const u64 &iilid,
						const u64 &key,
						u64 &docid,
						const u64 &dft,
						bool &found,
						bool &isunique);
	static bool		getDocid(
						AosRundata *rdata,
						const OmnString &iilname,
						const u64 &key,
						u64 &docid,
						const u64 &dft,
						bool &found,
						bool &isunique);
	static bool		bindCloudid(
						AosRundata *rdata,
						const OmnString &cid,
						const u64 &docid);
	static bool		unbindCloudid(
						AosRundata *rdata,
						const OmnString &cid,
						const u64 &docid);
	static bool		rebindCloudid(
						AosRundata *rdata,
						const OmnString &old_cid,
						const OmnString &new_cid,
						const u64 &docid);
	static bool		incrementDocidToTable(
						AosRundata *rdata,
						const u64 &iilid,
						const OmnString &key,
						u64 &value,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag);
	static bool		incrementInlineSearchValue(
						AosRundata *rdata,
						const OmnString &iilname,
						const OmnString &value,
						const u64 &seqid,
						const u64 incValue);
	static bool		getDocidByCloudid(
						AosRundata *rdata,
						const u32 siteid,
						const OmnString &cid,
						u64 &docid,
						bool &isunique);
	static u64 		getCtnrMemDocid(
						AosRundata *rdata,
						const u32 siteid,
						const OmnString &ctnr_objid,
						const OmnString &keyname,
						const OmnString &keyvalue,
						bool &duplicated);
	static u64		getCtnrMemDocid(
						AosRundata *rdata,
						const u32 siteid,
						const OmnString &ctnr_objid,
						const OmnString &keyname1,
						const OmnString &keyvalue1,
						const OmnString &keyname2,
						const OmnString &keyvalue2,
						bool &duplicated);
	static bool		addHitDoc(
						AosRundata *rdata,
						const OmnString &iilname,
						const u64 &docid);
	static u64		nextId1(
						AosRundata *rdata,
						const u32 siteid,
						const u64 &iilid,
						const u64 &init_value,
						const OmnString &id_name,
						const u64 &incValue);
	static bool		addU64ValueDocToTable(
						AosRundata *rdata,
						const u64 &iilid,
						const u64 &key,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique);
	static bool		preQuerySafe(
						AosRundata *rdata,
						const OmnString &iilname,
						const AosQueryContextObjPtr &query_context);
	static bool		preQuerySafe(
						AosRundata *rdata,
						const u64 &iilid,
						const AosQueryContextObjPtr &query_context);
	static bool		querySafe(
						AosRundata *rdata,
						const OmnString &iilname,
				 		const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context);
	static bool 	removeHitDoc(
						AosRundata *rdata,
						const OmnString &iilname,
						const u64 &docid);

	// 01/29/2015
	static bool		addHitDoc(
						AosRundata *rdata,
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &docid,
						int &physical_id);
	static bool		addStrValueDoc(
						AosRundata *rdata,
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						int &physical_id);
	static bool		addStrValueDocToTable(
						AosRundata *rdata,
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique);
	static bool		addU64ValueDoc(
						AosRundata *rdata,
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						int &physical_id);
	static bool		removeHitDoc(
						AosRundata *rdata,
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &docid);
	static bool		removeStrValueDoc(
						AosRundata *rdata,
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const u64 &iilid,
						const OmnString &value,
						const u64 &docid);
	static bool		removeStrValueDoc(
						AosRundata *rdata,
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						int &physical_id);
	static bool		removeU64ValueDoc(
						AosRundata *rdata,
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const u64 &iilid,
						const u64 &value,
						const u64 &docid);
	static bool		removeU64ValueDoc(
						AosRundata *rdata,
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						int &physical_id);
	static bool		removeU64DocByValue(
						AosRundata *rdata,
						const u64 &iilid,
						const u64 &value,
						int &entriesRemoved);
	static bool		removeU64DocByValue(
						AosRundata *rdata,
						const OmnString &iilname,
						const u64 &value,
						int &entriesRemoved);
	static bool		modifyStrValueDoc(
						AosRundata *rdata,
						const u64 &iilid,
						const OmnString &oldvalue,
						const OmnString &newvalue,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const bool override);
	static bool		modifyStrValueDoc(
						AosRundata *rdata,
						const OmnString &iilname,
						const OmnString &oldvalue,
						const OmnString &newvalue,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const bool override);
	static u64		getIILID(
						AosRundata *rdata,
						const OmnString &iilname);
	static bool		counterRange(
						AosRundata *rdata,
						const u64 &iilid,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosCounterQueryPtr &counter_query);
	static bool		counterRange(
						AosRundata *rdata,
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosCounterQueryPtr &counter_query);
	static bool		rebuildBitmap(
						AosRundata *rdata,
						const OmnString &iilname);
	static bool		querySafe(
						AosRundata *rdata,
						const u64 &iilid,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context);
	static bool		querySafe(
						AosRundata *rdata,
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosAsyncRespCallerPtr &resp_caller,
						const u64 &reqId,
						const u64 &snapId);
	static bool		createTablePublic(
						AosRundata *rdata,
						u64 &iilid,
						const AosIILType iiltype);
	static bool		createTablePublic(
						AosRundata *rdata,
						const OmnString &iilname,
						u64 &iilid,
						const AosIILType iiltype);
	static bool		setU64ValueDocUnique(
						AosRundata *rdata,
						u64 &iilid,
						const bool createFlag,
						const u64 &key,
						const u64 &docid,
						const bool must_same);
	static bool		setU64ValueDocUniqueToTable(
						AosRundata *rdata,
						u64 &iilid,
						const bool createFlag,
						const u64 &key,
						const u64 &docid,
						const bool must_same);
	static bool		setStrValueDocUnique(
						AosRundata *rdata,
						u64 &iilid,
						const bool createFlag,
						const OmnString &key,
						const u64 &docid,
						const bool must_same);
	static bool		setStrValueDocUniqueToTable(
						AosRundata *rdata,
						u64 &iilid,
						const bool createFlag,
						const OmnString &key,
						const u64 &docid,
						const bool must_same);
	static bool		incrementDocid(
						AosRundata *rdata,
						const u64 &iilid,
						const OmnString &key,
						u64 &value,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag);
	static bool		incrementDocid(
						AosRundata *rdata,
						const OmnString &iilname,
						const OmnString &key,
						u64 &value,
						const bool persis,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag);
	static bool		incrementDocid(
						AosRundata *rdata,
						const u64 &iilid,
						const u64 &key,
						u64 &value,
						const bool persis,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag,
						const u64 &dftValue);
	static bool		incrementDocid(
						AosRundata *rdata,
						const OmnString &iilname,
						const u64 &key,
						u64 &value,
						const bool persis,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag,
						const u64 &dft_value);
	static bool		createIILPublic(
						AosRundata *rdata,
						u64 &iilid,
						const AosIILType iiltype);
	static bool		createIILPublic(
						AosRundata *rdata,
						const OmnString &iilname,
						u64 &iilid,
						const AosIILType iiltype);
	static bool		getDocidsByKeys(
						AosRundata *rdata,
						const u64&iilid,
						const vector<OmnString> &keys,
						const bool need_dft_value,
						const u64 &dft_value,
						AosBuffPtr &buff);
	static bool		getDocidsByKeys(
						AosRundata *rdata,
						const OmnString &iilname,
						const vector<OmnString> &keys,
						const bool need_dft_value,
						const u64 &dft_value,
						AosBuffPtr &buff);
	static bool		deleteIIL(
						AosRundata *rdata,
						const u64 &iilid,
						const bool true_delete);
	static bool		deleteIIL(
						AosRundata *rdata,
						const OmnString &iilname,
						const bool true_delete);
	static bool		getSplitValue(
						AosRundata *rdata,
						const OmnString &iilname,
						const AosQueryContextObjPtr &context,
						const int size,
						vector<AosQueryContextObjPtr> &contexts);
	static bool		resetKeyedValue(
						AosRundata *rdata,
						const OmnString &iilname,
						const OmnString &key,
						u64 &value,
						const bool persis,
						const u64 &incValue);
	static bool		StrBatchAdd(
						AosRundata *rdata,
						const OmnString &iilname,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid);
	static bool		StrBatchDel(
						AosRundata *rdata,
						const OmnString &iilname,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid);
	static bool		StrBatchInc(
						AosRundata *rdata,
						const OmnString &iilname,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &init_value,
						const AosIILUtil::AosIILIncType incType,
						const u64 &snap_id,
						const u64 &task_docid);
	static bool		mergeSnapshot(
						AosRundata *rdata,
						const u32 static_id,
						const u64 &target_snap_id,
						const u64 &merge_snap_id);
	static u64		createSnapshot(
						AosRundata *rdata,
						const u32 static_id,
						const u64 &snap_id,
						const u64 &task_docid);
	static bool		commitSnapshot(
						AosRundata *rdata,
						const u32 &static_id,
						const u64 &snap_id,
						const u64 &task_docid);
	static bool		rollBackSnapshot(
						AosRundata *rdata,
						const u32 static_id,
						const u64 &snap_id,
						const u64 &task_docid);
	static int64_t		getTotalNumDocs(
						AosRundata *rdata,
						const OmnString &iilname);
	static int64_t		getTotalNumDocs(
						AosRundata *rdata,
						const OmnString &iilname,
						const u64 &snap_id);
	static bool		StrBatchAddMerge(
						AosRundata *rdata,
						const OmnString &iilname,
						const int64_t &size,
						const u64 &executor_id,
						const bool true_delete);
	static bool		StrBatchIncMerge(
						AosRundata *rdata,
						const OmnString &iilname,
						const int64_t &size,
						const u64 &dftvalue,
						const AosIILUtil::AosIILIncType incType,
						const bool true_delete);
	static bool		HitBatchAdd(
						AosRundata *rdata,
						const OmnString &iilname,
						const vector<u64> &docids,
						const u64 &snap_id,
						const u64 &task_docid);
	static bool		U64BatchAdd(
						AosRundata *rdata,
						const OmnString &iilname,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid);
	static bool		JimoTableBatchAdd(
						AosRundata *rdata,
						const OmnString &iilname,
						const AosXmlTagPtr &cmp_tag,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid);
	static bool		querySafe(
						AosRundata *rdata,
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosValueRslt &start_value,
						const AosValueRslt &end_value,
						AosValueRslt &next_value,
						u64 &next_docid,
						const AosOpr opr,
						const bool reverse,
						const i64 start_pos,
						const int page_size);
	static bool		querySafe(
						AosRundata *rdata,
						const u64 iilid,
						const AosQueryRsltObjPtr &query_rslt,
						const AosValueRslt &start_value,
						const AosValueRslt &end_value,
						AosValueRslt &next_value,
						u64 &next_docid,
						const AosOpr opr,
						const bool reverse,
						const i64 start_pos,
						const int page_size);
	static bool		rebindObjid(
						AosRundata *rdata,
						const OmnString &old_objid,
						const OmnString &new_objid,
						const u64 &docid);
	static bool		bitmapQueryNewSafe(
						AosRundata *rdata,
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context);
	static bool		bitmapRsltQuerySafe(
						AosRundata *rdata,
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context);
	static bool		moveManualOrder(
						AosRundata *rdata,
						const OmnString &iilname,
						u64 &value1,
						const u64 &docid1,
						u64 &value2,
						const u64 &docid2,
						const OmnString flag);
	static bool		removeManualOrder(
						AosRundata *rdata,
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid);
	static bool		swapManualOrder(
						AosRundata *rdata,
						const OmnString &iilname,
						const u64 &value1,
						const u64 &docid1,
						const u64 &value2,
						const u64 &docid2);
	static bool		appendManualOrder(
						AosRundata *rdata,
						const OmnString &iilname,
						const u64 &docid,
						u64 &value);
	static bool		getDocids(
						AosRundata *rdata,
						const OmnString &iilname,
						int &iilidx,
						int &idx,
						u64 *docids,
						const int array_size);
	static bool		addInlineSearchValue(
						AosRundata *rdata,
						const OmnString &iilname,
						const OmnString &value,
						const u64 &seqid,
						const OmnString &splitType);
	static bool		modifyInlineSearchValue(
						AosRundata *rdata,
						const OmnString &iilname,
						const OmnString &old_value,
						const OmnString &new_value,
						const u64 &old_seqid,
						const u64 &new_seqid,
						const OmnString &splitType);
	static bool		removeInlineSearchValue(
						AosRundata *rdata,
						const OmnString &iilname,
						const OmnString &value,
						const u64 &seqid,
						const OmnString &splitType);
	static bool		getBatchDocids(
						AosRundata *rdata,
						const AosBuffPtr &column,
						const OmnString &iilname);
	static bool		updateKeyedValue(
						AosRundata *rdata,
						const OmnString &iilname,
						const u64 &key,
						const bool &flag,
						const u64 &delta,
						u64 &new_value);
	static bool		removeStrFirstValueDoc(
						AosRundata *rdata,
						const OmnString &iilname,
						const OmnString &value,
						u64 &docid,
						const bool reverse);
	static bool		addU64ValueDocToTable(
						AosRundata *rdata,
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique);
};
*/
#endif

