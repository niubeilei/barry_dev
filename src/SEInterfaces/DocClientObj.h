////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 01/01/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DocClientObj_h
#define Aos_SEInterfaces_DocClientObj_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DocSvrCbObj.h"
#include "TransUtil/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/DocTypes.h"
#include "SEUtil/JqlTypes.h"


class AosDocClientObj : virtual public OmnRCObject
{
private:
	static AosDocClientObj *	smDocClient;
	static AosDocClientObjPtr 	smDocClientPtr;

public:
    virtual bool	config(const AosXmlTagPtr &def) = 0;
    virtual bool   	start() = 0;
    virtual bool   	stop() = 0;

	virtual bool createDoc1(                            
			                const AosRundataPtr &rdata,
							const AosXmlTagPtr &cmd,
							const OmnString &target_cid,
							const AosXmlTagPtr &newdoc,
							const bool resolveObjid,
							const AosDocSvrCbObjPtr &caller,
							void *userdata,
							const bool keepDocid) = 0;

	virtual AosXmlTagPtr getDoc(
							const AosRundataPtr &rdata,
							const OmnString &docid, 
							const OmnString &objid, 
							bool &duplicated) = 0;
	
	virtual u64         getDocidByCloudid(          
			                const u32 siteid,
						    const OmnString &cid,
						    const AosRundataPtr &rdata) = 0;

	virtual AosXmlTagPtr getDocByObjid(
							const OmnString &objid,
							const AosRundataPtr &) = 0;
	
	virtual u64			 getDocidByObjid(
							const OmnString &objid,
							const AosRundataPtr &) = 0;
	
	virtual AosXmlTagPtr getDocByDocid(
			                const u64 &docid, 
							const AosRundataPtr &) = 0;
    
	virtual AosXmlTagPtr getDocByDocid(
			                const OmnString &docid,
						    const AosRundataPtr &rdata) = 0;

	virtual AosXmlTagPtr getDocByCloudid(
			                const OmnString &cid, 
							const AosRundataPtr &rdata) = 0;
	
	virtual AosXmlTagPtr createDoc(
			                const AosXmlTagPtr &doc, 
							const AosRundataPtr &rdata) = 0;
							
	virtual bool         createDoc(
			                const AosXmlTagPtr &doc, 
							const AosRundataPtr &rdata,
							u64 &docid) = 0;

    virtual OmnString incrementValue(                
	       		            const u64 &docid,
							const OmnString &objid,
							const OmnString &aname,
							const OmnString &initvalue,
							const bool value_unique,
							const bool docid_unique,
							const AosRundataPtr &rdata) = 0;

	virtual AosXmlTagPtr createDocSafe3(            
			                const AosRundataPtr &rdata,
						    const AosXmlTagPtr &doc,
							const OmnString &cloudid,
							const OmnString &objid_base,
							const bool is_public,
							const bool checkCreation,
							const bool keepDocid,
							const bool reserved,
							const bool cid_required,
							const bool check_ctnr,
							const bool saveDocFlag) = 0;

	virtual bool 		 isRepairing() const = 0;
	
	virtual bool 		 purgeDoc(
			                const OmnString &objid, 
							const AosRundataPtr &rdata) = 0;
	
	virtual bool 	     purgeDoc(
			                const u64 &docid, 
							const AosRundataPtr &rdata) = 0;
	
	virtual bool 		 purgeDocByCloudid(
			                const OmnString &cid, 
							const AosRundataPtr &rdata) = 0;

	virtual bool readLockDocByDocid(
							AosRundata *rdata, 
							const u64 &docid,
							const int waittimer,
							const int holdtimer) = 0;
	
	virtual bool writeLockDocByDocid(
							AosRundata *rdata, 
							const u64 &docid,
							const int waittimer,
							const int holdtimer) = 0;

	virtual bool readUnlockDocByDocid(
							AosRundata *rdata, 
							const u64 &docid) = 0;

	virtual bool writeUnlockDocByDocid(
							AosRundata *rdata, 
							const u64 &docid,
							const AosXmlTagPtr &newdoc) = 0;
	
	virtual AosUserAcctObjPtr getUserAcct(
							const u64 &user_docid, 
							const AosRundataPtr &rdata) = 0;

	virtual bool	modifyAttrStr1(
							const AosRundataPtr &rdata,
							const u64 &docid,
							const OmnString &objid,	
							const OmnString &aname,
							const OmnString &newvalue,
							const bool value_unique,
							const bool docid_unique,
							const bool checkSecurity,
							const bool need_trigger = true) = 0;

	virtual bool	modifyAttrStr(
							const AosXmlTagPtr &doc,
							const OmnString &aname, 
							const OmnString &newvalue, 
							const bool value_unique, 
							const bool docid_unique, 
							const bool checkSecurity,
							const AosRundataPtr &rdata,
							const bool need_trigger = true) = 0;

	virtual bool	modifyAttrU64(
							const AosXmlTagPtr &doc,
							const OmnString &aname, 
							const u64 &newvalue, 
							const bool value_unique, 
							const bool docid_unique, 
							const bool checkSecurity,
							const AosRundataPtr &rdata,
							const bool need_trigger = true) = 0;

	virtual bool	modifyAttrXPath(
							const AosRundataPtr &rdata,
							const AosXmlTagPtr &doc,
							const OmnString &aname, 
							const OmnString &newvalue, 
							const bool value_unique, 
							const bool docid_unique, 
							const bool checkSecurity,
							const bool need_trigger) = 0;

	virtual AosXmlTagPtr createDocSafe1(
							const AosRundataPtr &rdata,
							const OmnString &docstr,
							const OmnString &cloudid,
							const OmnString &objid_base,
							const bool is_public,
							const bool checkCreation,
							const bool keepDocid, 
							const bool reserved, 
							const bool cid_required, 
							const bool check_ctnr) = 0;

	virtual bool modifyObj( const AosXmlTagPtr &doc,
							const AosRundataPtr &rdata) = 0;

	virtual bool modifyObj(
			                const AosRundataPtr &rdata,
				            const AosXmlTagPtr &newxml,
					        const OmnString &rename,
						    const bool synobj) = 0;        

	virtual bool modifyObj(
							const AosRundataPtr &rdata,
							const AosXmlTagPtr &newxml,
							const OmnString &rename,
							const bool synobj,
							u64 &newdid) = 0;

	virtual AosAccessRcdPtr getAccessRcd(
							const AosRundataPtr &rdata,
							const AosXmlTagPtr &ref_doc,
							const u64 &ref_docid, 
							const OmnString &ref_objid,
							const bool parent_flag) = 0;

	virtual bool isCloudidBound(                     
			                const OmnString &cid,
						    u64 &userid,
						    const AosRundataPtr &rdata) = 0;
							
    virtual AosXmlTagPtr getDoc(                   
			                const u64 &docid,
							const OmnString &objid,
							const AosRundataPtr &rdata) = 0;

	virtual bool deleteObj(
							const AosRundataPtr &rdata,
							const OmnString &docid,
							const OmnString &theobjid,
							const OmnString &container,
							const bool deleteFlag) = 0;

	virtual OmnString   getCloudid(
							const u64 &user_docid,
							const AosRundataPtr &rdata) = 0;

	virtual AosAccessRcdPtr getAccessRecord(
							const u64 &docid,
							const bool parent_flag,
							const AosRundataPtr &rdata) = 0;

	virtual AosXmlTagPtr writeLockDocByObjid(
							const OmnString &objid,
							const AosRundataPtr &rdata) = 0;

	virtual AosXmlTagPtr writeLockDocByDocid(
							const u64 &docid,
							const AosRundataPtr &rdata) = 0;

	virtual bool writeAccessDoc(
							const u64 &owndocid, 
							AosXmlTagPtr &arcd, 
							const AosRundataPtr &rdata) = 0;

	virtual AosXmlTagPtr releaseWriteLock(
							const AosXmlTagPtr &doc,
							const AosRundataPtr &rdata) = 0;

	virtual AosXmlTagPtr releaseWriteLockDocByObjid(
							const OmnString &objid,
							const AosRundataPtr &rdata) = 0;

	virtual AosXmlTagPtr releaseWriteLockDocByDocid(
							const u64 &docid,
							const AosRundataPtr &rdata) = 0;


	virtual AosXmlTagPtr createBinaryDoc(
							const OmnString &docstr,
							const bool is_public, 
							const AosBuffPtr &buff,
							const AosRundataPtr &rdata) = 0;

	virtual AosXmlTagPtr createBinaryDoc(
							const OmnString &docstr,
							const bool is_public, 
							const AosBuffPtr &buff,
							const int &vid,
							const AosRundataPtr &rdata) = 0;

	virtual bool deleteBinaryDoc(
							const OmnString &objid, 
							const AosRundataPtr &rdata) = 0;

	virtual bool deleteBinaryDoc(
							const u64 &docid, 
							const AosRundataPtr &rdata) = 0;

	virtual bool modifyBinaryDocByStat(
							const AosXmlTagPtr &doc,
							const AosBuffPtr &buff,
							const u64 &normal_snap_id,
							const u64 &binary_snap_id,
							const AosRundataPtr &rdata) = 0;

	virtual AosXmlTagPtr retrieveBinaryDocByStat(
							const u64 &docid, 
							AosBuffPtr &docBuff, 
							const u64 &normal_snap_id,
							const u64 &binary_snap_id,
							const AosRundataPtr &rdata) = 0;
	
	virtual bool appendBinaryDoc(
							const OmnString &objid, 
							const AosBuffPtr &buff, 
							const AosRundataPtr &rdata) = 0;

	virtual AosXmlTagPtr retrieveBinaryDoc(
							const OmnString &objid, 
							AosBuffPtr &buff, 
							const AosRundataPtr &rdata) = 0;
	
	virtual AosXmlTagPtr retrieveBinaryDoc(
							const u64 &docid, 
							AosBuffPtr &buff, 
							const AosRundataPtr &rdata) = 0;

	virtual void  retrieveBinaryDoc(
							const AosAsyncRespCallerPtr &resp_caller,
							const u64 &docid,
							const AosRundataPtr &rdata) = 0;
							
    virtual bool    procDocLock(                         
			                const AosRundataPtr &rdata,
							const AosXmlTagPtr &req,
							const u64 &docid) = 0;

	virtual void procNotifyMsg(
							const u64 &docid, 
							const u32 siteid,
							const AosRundataPtr &rdata) = 0;

	virtual bool batchDataReceived(
							const OmnString &scannerid,
							const AosBuffPtr &contents,
							const int serverid,
							const bool finished,
							const int num_call_back_data_msgs,
							const AosRundataPtr &rdata) = 0;

	virtual AosXmlTagPtr getParentContainer(
							const AosRundataPtr &rdata,
							const OmnString &container_objid, 
							const bool ispublic) = 0;

	virtual bool getTableNames(
							const AosRundataPtr &rdata,
							vector<string> &values) = 0;

	virtual bool getJQLDocs(
							const AosRundataPtr &rdata,
							const JQLTypes::DocType	&type,
							vector<string> &values) = 0;

	virtual u64  getJQLDocidByTablename(
							const AosRundataPtr &rdata,
							const string &tablename) = 0;

	virtual bool dropJQLTable( 
							const AosRundataPtr &rdata,
							const OmnString &table_name) = 0;

	virtual bool createJQLTable(	
							const AosRundataPtr &rdata,
							const OmnString &table_def) = 0;

	virtual bool createDocByJQL( 
							const AosRundataPtr &rdata,
							const OmnString &container_objid,
							const OmnString &objid,
							const OmnString &doc) = 0;

	virtual bool deleteDocByDocid(	
							const AosRundataPtr &rdata,
							const int64_t docid) = 0;

	virtual bool deleteDocByObjid(	
							const AosRundataPtr &rdata,
							const string &objid) = 0;
							
    virtual bool deleteDoc(                         
			                const AosXmlTagPtr &origdocroot,
					        const AosRundataPtr &rdata) = 0;

	virtual bool getDocByJQL(	
							const AosRundataPtr &rdata,
							const string &objid,
							string &doc) = 0;

	virtual bool getParentContainerByObjid(	
							const AosRundataPtr &rdata,
							const string &objid,
							string &doc) = 0;

	virtual bool getParentContainerByDocid(	
							const AosRundataPtr &rdata,
							const int64_t docid,
							string &doc) = 0;

	virtual AosXmlTagPtr getDocByKey(
							const AosRundataPtr &rdata, 
							const OmnString &container, 
							const OmnString &key_name, 
							const OmnString &key_value) = 0;

	virtual AosXmlTagPtr getDocByKey(
							const AosRundataPtr &rdata, 
							const OmnString &tablename) = 0;

	 virtual AosXmlTagPtr getDocByKeys(
	 						const AosRundataPtr &rdata, 
	 						const OmnString &container, 
	 						const vector<OmnString> &key_names, 
	 						const vector<OmnString> &key_values) = 0;

	virtual bool mergeSnapshot(
							const AosRundataPtr &rdata,
							const int virtual_id,
							const AosDocType::E doc_type,
							const u64 &target_snap_id,
							const u64 &merge_snap_id) = 0;

	virtual u64 createSnapshot(
							const AosRundataPtr &rdata,
							const int virtual_id,
							const u64 snap_id,
							const AosDocType::E doc_type,
							const u64 &task_docid) = 0;

	virtual bool commitSnapshot(const AosRundataPtr &rdata,
							const int virtual_id,
							const AosDocType::E doc_type,
							const u64 &snap_id,
							const u64 &task_docid) = 0;

	virtual bool rollbackSnapshot(
							const AosRundataPtr &rdata,
							const int virtual_id,
							const AosDocType::E doc_type,
							const u64 &snap_id,
							const u64 &task_docid) = 0;

	virtual AosXmlTagPtr createRootCtnr(              
			                const OmnString &docstr,
						    const AosRundataPtr &rdata) = 0;

	virtual bool deleteDoc(
				            const AosXmlTagPtr &origdocroot, 
						    const AosRundataPtr &rdata, 
							const OmnString &ctnr_objid,
							const bool deleteFlag) = 0;

	virtual AosXmlTagPtr createDocByTemplate1(             
			    	        const AosRundataPtr &rdata,
							const OmnString &cid,
							const OmnString &objid,
							const OmnString &template_objid) =0;

	virtual bool bindObjid(                     
				            OmnString &objid,
						    u64 &docid,
							bool &objid_changed,
							const bool resolve,
							const bool keepDocid,
							const AosRundataPtr &rdata) = 0;

	virtual  u64 doesObjidExist(                 
				            const u32 siteid,
						    const OmnString &objid,
							const AosRundataPtr &rdata) = 0;

	virtual AosXmlTagPtr cloneDoc(                    
				            const AosRundataPtr &rdata,
							const OmnString &cloudid,
							const OmnString &fromobjid,
							const OmnString &toobjid,
							const bool is_public) = 0;

	virtual OmnString getObjidByDocid(const u64 &docid, const AosRundataPtr &rdata)= 0;
 
    virtual bool isLog(const u64 &docid) const =0;

	virtual AosXmlTagPtr getDeletedDoc(                
				            const OmnString &objid,
							const u64 &docid,
							const AosRundataPtr &rdata) =0 ;
	 
	virtual bool bindCloudid(                      
							const OmnString &cid,
							const u64 &docid,
							const AosRundataPtr &rdata) =0;

	virtual AosBuffPtr batchGetDocs(                               
				            const OmnString &scanner_id,
							const int server_id,
							const AosRundataPtr &rdata) =0;

   virtual	void    batchGetDocs(
				            const AosRundataPtr &rdata,
							const int vid,
							const u32 num_docs,
							const u32 blocksize,
							const AosBuffPtr &docid_buff,
							const AosBuffPtr &sizeid_len,
							const AosAsyncRespCallerPtr &resp_caller) =0;

	virtual bool insertInto(
							const AosXmlTagPtr &data,
							const AosRundataPtr &rdata) = 0;
	virtual bool batchSendDocids(
					const OmnString &scanner_id,
					const int serverid,
					const AosBitmapObjPtr &bitmap,
					const AosRundataPtr &rdata) = 0;

	virtual bool batchSendDocids(
					const OmnString &scanner_id,
					const int serverid,
					const AosBitmapObjPtr &bitmap,
					const AosDocClientCallerPtr &caller,
					const AosRundataPtr &rdata) = 0;


	virtual bool addCreateLogRequest(
					const u64 &userid,
					const AosXmlTagPtr &doc,
					const AosRundataPtr &rdata) = 0;

	virtual bool addDeleteLogRequest(
					const AosXmlTagPtr &doc,
					const u64 userid,
					const AosRundataPtr &rdata) = 0;

	// Chen Ding, 2015/06/02
	virtual bool createDoc(AosRundata *rdata,                                   
			        const AosXmlTagPtr &newdoc,
					const OmnString &objid,
					u64 &docid,
					const bool keep_objid,
					const u64 snap_id);

	static bool removeDocByDocidStatic(const AosRundataPtr &rdata, const u64 docid);
	static void setDocClient(const AosDocClientObjPtr &d);
	static AosDocClientObj *getDocClient() {return smDocClient;}
};

#endif
