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
// 2015/09/15 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocClientWrapper_DocClientWrapper_h
#define Aos_DocClientWrapper_DocClientWrapper_h

#include "SEInterfaces/DocClientObj.h"

class AosDocClientWrapper : virtual public AosDocClientObj
{

	OmnDefineRCObject;

public:
    virtual bool	config(const AosXmlTagPtr &def);
    virtual bool   	start();
    virtual bool   	stop();

	virtual bool createDoc1(                            
			                const AosRundataPtr &rdata,
							const AosXmlTagPtr &cmd,
							const OmnString &target_cid,
							const AosXmlTagPtr &newdoc,
							const bool resolveObjid,
							const AosDocSvrCbObjPtr &caller,
							void *userdata,
							const bool keepDocid);

	virtual AosXmlTagPtr getDoc(
							const AosRundataPtr &rdata,
							const OmnString &docid, 
							const OmnString &objid, 
							bool &duplicated);
	
	virtual u64         getDocidByCloudid(          
			                const u32 siteid,
						    const OmnString &cid,
						    const AosRundataPtr &rdata);

	virtual AosXmlTagPtr getDocByObjid(
							const OmnString &objid,
							const AosRundataPtr &);
	
	virtual u64			 getDocidByObjid(
							const OmnString &objid,
							const AosRundataPtr &);
	
	virtual AosXmlTagPtr getDocByDocid(
			                const u64 &docid, 
							const AosRundataPtr &);
    
	virtual AosXmlTagPtr getDocByDocid(
			                const OmnString &docid,
						    const AosRundataPtr &rdata)
	{
		u64 did = atoll(docid.data());   
		aos_assert_r(did > 0, 0);        
		return getDocByDocid(did, rdata);
	}

	virtual AosXmlTagPtr getDocByCloudid(
			                const OmnString &cid, 
							const AosRundataPtr &rdata);
	
	virtual AosXmlTagPtr createDoc(
			                const AosXmlTagPtr &doc, 
							const AosRundataPtr &rdata);
							
	virtual bool         createDoc(
			                const AosXmlTagPtr &doc, 
							const AosRundataPtr &rdata,
							u64 &docid);
    
	virtual OmnString incrementValue(                
	       		            const u64 &docid,
							const OmnString &objid,
							const OmnString &aname,
							const OmnString &initvalue,
							const bool value_unique,
							const bool docid_unique,
							const AosRundataPtr &rdata);

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
							const bool saveDocFlag);

	virtual bool 		 isRepairing() const;
	
	virtual bool 		 purgeDoc(
			                const OmnString &objid, 
							const AosRundataPtr &rdata);
	
	virtual bool 	     purgeDoc(
			                const u64 &docid, 
							const AosRundataPtr &rdata);
	
	virtual bool 		 purgeDocByCloudid(
			                const OmnString &cid, 
							const AosRundataPtr &rdata);

	
	virtual AosUserAcctObjPtr getUserAcct(
							const u64 &user_docid, 
							const AosRundataPtr &rdata);

	virtual bool	modifyAttrStr1(
							const AosRundataPtr &rdata,
							const u64 &docid,
							const OmnString &objid,	
							const OmnString &aname,
							const OmnString &newvalue,
							const bool value_unique,
							const bool docid_unique,
							const bool checkSecurity,
							const bool need_trigger = true);

	virtual bool	modifyAttrStr(
							const AosXmlTagPtr &doc,
							const OmnString &aname, 
							const OmnString &newvalue, 
							const bool value_unique, 
							const bool docid_unique, 
							const bool checkSecurity,
							const AosRundataPtr &rdata,
							const bool need_trigger = true);

	virtual bool	modifyAttrU64(
							const AosXmlTagPtr &doc,
							const OmnString &aname, 
							const u64 &newvalue, 
							const bool value_unique, 
							const bool docid_unique, 
							const bool checkSecurity,
							const AosRundataPtr &rdata,
							const bool need_trigger = true);


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
							const bool check_ctnr);

	virtual bool modifyObj( const AosXmlTagPtr &doc,
							const AosRundataPtr &rdata);

	virtual bool modifyObj(
			                const AosRundataPtr &rdata,
				            const AosXmlTagPtr &newxml,
					        const OmnString &rename,
						    const bool synobj);        


	virtual AosAccessRcdPtr getAccessRcd(
							const AosRundataPtr &rdata,
							const AosXmlTagPtr &ref_doc,
							const u64 &ref_docid, 
							const OmnString &ref_objid,
							const bool parent_flag);

	virtual bool isCloudidBound(                     
			                const OmnString &cid,
						    u64 &userid,
						    const AosRundataPtr &rdata);
							
    virtual AosXmlTagPtr getDoc(                   
			                const u64 &docid,
							const OmnString &objid,
							const AosRundataPtr &rdata);

	virtual bool deleteObj(
							const AosRundataPtr &rdata,
							const OmnString &docid,
							const OmnString &theobjid,
							const OmnString &container,
							const bool deleteFlag);

	virtual OmnString   getCloudid(
							const u64 &user_docid,
							const AosRundataPtr &rdata);

	virtual AosAccessRcdPtr getAccessRecord(
							const u64 &docid,
							const bool parent_flag,
							const AosRundataPtr &rdata);

	virtual bool writeAccessDoc(
							const u64 &owndocid, 
							AosXmlTagPtr &arcd, 
							const AosRundataPtr &rdata);

						
	virtual AosXmlTagPtr createBinaryDoc(
							const OmnString &docstr,
							const bool is_public, 
							const AosBuffPtr &buff,
							const int &vid,
							const AosRundataPtr &rdata);


	virtual AosXmlTagPtr retrieveBinaryDoc(
							const OmnString &objid, 
							AosBuffPtr &buff, 
							const AosRundataPtr &rdata);
	
	virtual AosXmlTagPtr retrieveBinaryDoc(
							const u64 &docid, 
							AosBuffPtr &buff, 
							const AosRundataPtr &rdata);

							
    virtual bool    procDocLock(                         
			                const AosRundataPtr &rdata,
							const AosXmlTagPtr &req,
							const u64 &docid);

	virtual void procNotifyMsg(
							const u64 &docid, 
							const u32 siteid,
							const AosRundataPtr &rdata);

	virtual AosXmlTagPtr getParentContainer(
							const AosRundataPtr &rdata,
							const OmnString &container_objid, 
							const bool ispublic);


	virtual bool getJQLDocs(
							const AosRundataPtr &rdata,
							const JQLTypes::DocType	&type,
							vector<string> &values);


	virtual bool createJQLTable(	
							const AosRundataPtr &rdata,
							const OmnString &table_def);


	virtual bool deleteDocByDocid(	
							const AosRundataPtr &rdata,
							const int64_t docid);

	virtual bool deleteDocByObjid(	
							const AosRundataPtr &rdata,
							const string &objid);
							
    virtual bool deleteDoc(                         
			                const AosXmlTagPtr &origdocroot,
					        const AosRundataPtr &rdata);

	virtual bool getDocByJQL(	
							const AosRundataPtr &rdata,
							const string &objid,
							string &doc);


	virtual AosXmlTagPtr getDocByKey(
							const AosRundataPtr &rdata, 
							const OmnString &container, 
							const OmnString &key_name, 
							const OmnString &key_value);

	virtual AosXmlTagPtr getDocByKey(
							const AosRundataPtr &rdata, 
							const OmnString &tablename);

	 virtual AosXmlTagPtr getDocByKeys(
	 						const AosRundataPtr &rdata, 
	 						const OmnString &container, 
	 						const vector<OmnString> &key_names, 
	 						const vector<OmnString> &key_values);

	virtual bool mergeSnapshot(
							const AosRundataPtr &rdata,
							const int virtual_id,
							const AosDocType::E doc_type,
							const u64 &target_snap_id,
							const u64 &merge_snap_id);

	virtual u64 createSnapshot(
							const AosRundataPtr &rdata,
							const int virtual_id,
							const u64 snap_id,
							const AosDocType::E doc_type,
							const u64 &task_docid);

	virtual bool commitSnapshot(const AosRundataPtr &rdata,
							const int virtual_id,
							const AosDocType::E doc_type,
							const u64 &snap_id,
							const u64 &task_docid);

	virtual bool rollbackSnapshot(
							const AosRundataPtr &rdata,
							const int virtual_id,
							const AosDocType::E doc_type,
							const u64 &snap_id,
							const u64 &task_docid);

	virtual AosXmlTagPtr createRootCtnr(              
			                const OmnString &docstr,
						    const AosRundataPtr &rdata);

	virtual bool deleteDoc(
				            const AosXmlTagPtr &origdocroot, 
						    const AosRundataPtr &rdata, 
							const OmnString &ctnr_objid,
							const bool deleteFlag);

	virtual AosXmlTagPtr createDocByTemplate1(             
			    	        const AosRundataPtr &rdata,
							const OmnString &cid,
							const OmnString &objid,
							const OmnString &template_objid);

	virtual bool bindObjid(                     
				            OmnString &objid,
						    u64 &docid,
							bool &objid_changed,
							const bool resolve,
							const bool keepDocid,
							const AosRundataPtr &rdata);

	virtual  u64 doesObjidExist(                 
				            const u32 siteid,
						    const OmnString &objid,
							const AosRundataPtr &rdata);

	virtual AosXmlTagPtr cloneDoc(                    
				            const AosRundataPtr &rdata,
							const OmnString &cloudid,
							const OmnString &fromobjid,
							const OmnString &toobjid,
							const bool is_public);

	virtual OmnString getObjidByDocid(const u64 &docid, const AosRundataPtr &rdata);
 
    virtual bool isLog(const u64 &docid) const;

	virtual AosXmlTagPtr getDeletedDoc(                
				            const OmnString &objid,
							const u64 &docid,
							const AosRundataPtr &rdata) ;
	 
	virtual bool bindCloudid(                      
							const OmnString &cid,
							const u64 &docid,
							const AosRundataPtr &rdata);

	virtual AosBuffPtr batchGetDocs(                               
				            const OmnString &scanner_id,
							const int server_id,
							const AosRundataPtr &rdata);

   virtual	void    batchGetDocs(
				            const AosRundataPtr &rdata,
							const int vid,
							const u32 num_docs,
							const u32 blocksize,
							const AosBuffPtr &docid_buff,
							const AosBuffPtr &sizeid_len,
							const AosAsyncRespCallerPtr &resp_caller);

	virtual bool insertInto(
							const AosXmlTagPtr &data,
							const AosRundataPtr &rdata);

	virtual bool batchSendDocids(
	 				const OmnString &scanner_id,
	 				const int serverid,
	 				const AosBitmapObjPtr &bitmap,
	 				const AosRundataPtr &rdata);

	virtual bool batchSendDocids(
	 				const OmnString &scanner_id,
	 				const int serverid,
	 				const AosBitmapObjPtr &bitmap,
	 				const AosDocClientCallerPtr &caller,
	 				const AosRundataPtr &rdata);


	virtual bool readLockDocByDocid(                 
					AosRundata *rdata,       
					const u64 &docid,        
					const int waittimer,    					
					const int holdtimer) {return true;}

	
	virtual bool writeLockDocByDocid(
							AosRundata *rdata, 
							const u64 &docid,
							const int waittimer,
							const int holdtimer) {return true;}

	virtual bool readUnlockDocByDocid(
							AosRundata *rdata, 
							const u64 &docid) {return true;}

	virtual bool writeUnlockDocByDocid(
							AosRundata *rdata, 
							const u64 &docid,
							const AosXmlTagPtr &newdoc) {return true;}

	virtual bool	modifyAttrXPath(
							const AosRundataPtr &rdata,
							const AosXmlTagPtr &doc,
							const OmnString &aname, 
							const OmnString &newvalue, 
							const bool value_unique, 
							const bool docid_unique, 
							const bool checkSecurity,
							const bool need_trigger) {return true;}

	virtual bool modifyObj(
			                const AosRundataPtr &rdata,
				            const AosXmlTagPtr &newxml,
					        const OmnString &rename,
						    const bool synobj,
							u64 &newdid) {return true;}

	virtual AosXmlTagPtr writeLockDocByObjid(
							const OmnString &objid,
							const AosRundataPtr &rdata) {return 0;}


	virtual AosXmlTagPtr writeLockDocByDocid(
							const u64 &docid,
							const AosRundataPtr &rdata) {return 0;}


	virtual AosXmlTagPtr releaseWriteLock(
							const AosXmlTagPtr &doc,
							const AosRundataPtr &rdata) {return 0;}


	virtual AosXmlTagPtr releaseWriteLockDocByObjid(
							const OmnString &objid,
							const AosRundataPtr &rdata) {return 0;}

	virtual AosXmlTagPtr releaseWriteLockDocByDocid(
							const u64 &docid,
							const AosRundataPtr &rdata) {return 0;}

	virtual AosXmlTagPtr createBinaryDoc(
							const OmnString &docstr,
							const bool is_public, 
							const AosBuffPtr &buff,
							const AosRundataPtr &rdata) {return 0;}

	virtual bool deleteBinaryDoc(
							const OmnString &objid, 
							const AosRundataPtr &rdata) {return true;}

	virtual bool deleteBinaryDoc(
							const u64 &docid, 
							const AosRundataPtr &rdata) {return true;}

	virtual bool modifyBinaryDocByStat(
							const AosXmlTagPtr &doc,
							const AosBuffPtr &buff,
							const u64 &normal_snap_id,
							const u64 &binary_snap_id,
							const AosRundataPtr &rdata) {return true;}

	virtual AosXmlTagPtr retrieveBinaryDocByStat(
							const u64 &docid, 
							AosBuffPtr &docBuff, 
							const u64 &normal_snap_id,
							const u64 &binary_snap_id,
							const AosRundataPtr &rdata) {return 0;}

	virtual bool appendBinaryDoc(
							const OmnString &objid, 
							const AosBuffPtr &buff, 
							const AosRundataPtr &rdata) {return true;}

	virtual void  retrieveBinaryDoc(
							const AosAsyncRespCallerPtr &resp_caller,
							const u64 &docid,
							const AosRundataPtr &rdata) {return;}
							
	virtual bool batchDataReceived(
							const OmnString &scannerid,
							const AosBuffPtr &contents,
							const int serverid,
							const bool finished,
							const int num_call_back_data_msgs,
							const AosRundataPtr &rdata) {return true;}

	virtual bool getTableNames(
							const AosRundataPtr &rdata,
							vector<string> &values) {return true;}

	virtual u64  getJQLDocidByTablename(
							const AosRundataPtr &rdata,
							const string &tablename) {return 0;}

	virtual bool dropJQLTable( 
							const AosRundataPtr &rdata,
							const OmnString &table_name) {return true;}

	virtual bool createDocByJQL( 
							const AosRundataPtr &rdata,
							const OmnString &container_objid,
							const OmnString &objid,
							const OmnString &doc) {return true;}

	virtual bool getParentContainerByObjid(	
							const AosRundataPtr &rdata,
							const string &objid,
							string &doc) {return true;}

	virtual bool getParentContainerByDocid(	
							const AosRundataPtr &rdata,
							const int64_t docid,
							string &doc) {return true;}

	virtual bool addCreateLogRequest(
					const u64 &userid,
					const AosXmlTagPtr &doc,
					const AosRundataPtr &rdata) {return true;}

	virtual bool addDeleteLogRequest(
					const AosXmlTagPtr &doc,
					const u64 userid,
					const AosRundataPtr &rdata) {return true;}



};
#endif
