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
// 2015/11/10 Modify  by Xuqi
////////////////////////////////////////////////////////////////////////////
#include "DocClientWrapper/DocClientWrapper.h"

#include "JimoAPI/JimoDocEngine.h"
#include "SEInterfaces/UserAcctObj.h"
#include "Util/Buff.h"
#include "XmlUtil/AccessRcd.h"
#include "UtilData/JPID.h"
#include "UtilData/JSID.h"

using namespace Jimo;

bool	
AosDocClientWrapper::config(const AosXmlTagPtr &def)
{
	return true;
}

bool   	
AosDocClientWrapper::start()
{
	return true;
}

bool   	
AosDocClientWrapper::stop()
{
	return true;
}


bool 
AosDocClientWrapper::createDoc1(                            
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &cmd,
		const OmnString &target_cid,
		const AosXmlTagPtr &newdoc,
		const bool resolveObjid,
		const AosDocSvrCbObjPtr &caller,
		void *userdata,
		const bool keepDocid)
{
	return Jimo::jimoCreateDoc1(rdata.getPtrNoLock(),
				cmd, target_cid, newdoc, resolveObjid, caller, userdata, keepDocid);
}


AosXmlTagPtr 
AosDocClientWrapper::getDoc(
		const AosRundataPtr &rdata,
		const OmnString &docid, 
		const OmnString &objid, 
		bool &duplicated)
{
	return Jimo::jimoGetDoc(rdata.getPtrNoLock(),
			docid.toU64(), objid, duplicated);
}

	
u64         
AosDocClientWrapper::getDocidByCloudid(          
		const u32 siteid,
		const OmnString &cid,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoGetDocidByCloudid(rdata.getPtrNoLock(),
			siteid, cid);
}


AosXmlTagPtr 
AosDocClientWrapper::getDocByObjid(
		const OmnString &objid,
		const AosRundataPtr &rdata)
{
	rdata->setJPID(AOSJPID_SYSTEM);
	rdata->setJSID(AOSJSID_SYSTEM);

	return Jimo::jimoGetDocByObjid(rdata.getPtrNoLock(),
		   objid);
}

	
u64			 
AosDocClientWrapper::getDocidByObjid(
		const OmnString &objid,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoGetDocidByObjid(rdata.getPtrNoLock(),
			objid);
}

	
AosXmlTagPtr 
AosDocClientWrapper::getDocByDocid(
		const u64 &docid, 
		const AosRundataPtr &rdata)
{
	rdata->setJPID(AOSJPID_SYSTEM);
	rdata->setJSID(AOSJSID_SYSTEM);
	
	return Jimo::jimoGetDocByDocid(rdata.getPtrNoLock(),
			docid);
}

    
AosXmlTagPtr 
AosDocClientWrapper::getDocByCloudid(
		const OmnString &cid, 
		const AosRundataPtr &rdata)
{
	return Jimo::jimoGetDocByCloudid(rdata.getPtrNoLock(), cid);
}

	
AosXmlTagPtr
AosDocClientWrapper::createDoc(
		const AosXmlTagPtr &doc, 
		const AosRundataPtr &rdata)
{
	//use the following function instead
	//AosDocClientWrapper::createDoc(    
	//        const AosXmlTagPtr &doc,   
	//        const AosRundataPtr &rdata,
	//        u64 &docid)                
	OmnNotImplementedYet;
	return 0;
}

bool
AosDocClientWrapper::createDoc(
		const AosXmlTagPtr &doc,
		const AosRundataPtr &rdata,
        u64 &docid)
{
	aos_assert_rr(doc, rdata, false);
	docid = Jimo::jimoCreateDoc(rdata.getPtrNoLock(), doc);
	if( docid == 0 )
	{
		OmnAlarm << "createDoc failed, docstr: " << doc->toString() << enderr; 
	    return false;
	}
	return true;
}

							
OmnString 
AosDocClientWrapper::incrementValue(                
		const u64 &docid,
		const OmnString &objid,
		const OmnString &aname,
		const OmnString &initvalue,
		const bool value_unique,
		const bool docid_unique,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoIncrementValue(rdata.getPtrNoLock(),
			docid, objid, aname, initvalue, value_unique, docid_unique);
}


AosXmlTagPtr 
AosDocClientWrapper::createDocSafe3(            
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
		const bool saveDocFlag)
{
	return Jimo::jimoCreateDocSafe3(rdata.getPtrNoLock(),
			doc, cloudid, objid_base, is_public, checkCreation, keepDocid,
			reserved, cid_required, check_ctnr, saveDocFlag);
}


bool 		 
AosDocClientWrapper::isRepairing() const
{
	return Jimo::jimoIsRepairing();
}

	
bool 		 
AosDocClientWrapper::purgeDoc(
		const OmnString &objid, 
		const AosRundataPtr &rdata)
{
	return Jimo::jimoPurgeDoc(rdata.getPtrNoLock(), objid);
}

	
bool 	     
AosDocClientWrapper::purgeDoc(
		const u64 &docid, 
		const AosRundataPtr &rdata)
{
	return Jimo::jimoPurgeDoc(rdata.getPtrNoLock(), docid);
}

	
bool 		 
AosDocClientWrapper::purgeDocByCloudid(
		const OmnString &cid, 
		const AosRundataPtr &rdata)
{
	return Jimo::jimoPurgeDocByCloudid(rdata.getPtrNoLock(), cid);
}


	
AosUserAcctObjPtr 
AosDocClientWrapper::getUserAcct(
		const u64 &user_docid, 
		const AosRundataPtr &rdata)
{
	return Jimo::jimoGetUserAcct(rdata.getPtrNoLock(), user_docid);
}


bool	
AosDocClientWrapper::modifyAttrStr1(
		const AosRundataPtr &rdata,
		const u64 &docid,
		const OmnString &objid,	
		const OmnString &aname,
		const OmnString &newvalue,
		const bool value_unique,
		const bool docid_unique,
		const bool checkSecurity,
		const bool need_trigger)
{
	return Jimo::jimoModifyAttrStr1(rdata.getPtrNoLock(),
			docid, objid, aname, newvalue, value_unique, docid_unique,
			checkSecurity, need_trigger);
}


bool	
AosDocClientWrapper::modifyAttrStr(
		const AosXmlTagPtr &doc,
		const OmnString &aname, 
		const OmnString &newvalue, 
		const bool value_unique, 
		const bool docid_unique, 
		const bool checkSecurity,
		const AosRundataPtr &rdata,
		const bool need_trigger)
{
	return Jimo::jimoModifyAttrStr(rdata.getPtrNoLock(),
			doc, aname, newvalue, value_unique, docid_unique, 
			checkSecurity, need_trigger);
}


bool	
AosDocClientWrapper::modifyAttrU64(
		const AosXmlTagPtr &doc,
		const OmnString &aname, 
		const u64 &newvalue, 
		const bool value_unique, 
		const bool docid_unique, 
		const bool checkSecurity,
		const AosRundataPtr &rdata,
		const bool need_trigger)
{
	return Jimo::jimoModifyAttrU64(rdata.getPtrNoLock(),
			doc, aname, newvalue, value_unique, docid_unique,
			checkSecurity, need_trigger);
}


AosXmlTagPtr 
AosDocClientWrapper::createDocSafe1(
		const AosRundataPtr &rdata,
		const OmnString &docstr,
		const OmnString &cloudid,
		const OmnString &objid_base,
		const bool is_public,
		const bool checkCreation,
		const bool keepDocid, 
		const bool reserved, 
		const bool cid_required, 
		const bool check_ctnr)
{
	return Jimo::jimoCreateDocSafe1(rdata.getPtrNoLock(),
			docstr, cloudid, objid_base, is_public, checkCreation,
			keepDocid, reserved, cid_required, check_ctnr);
}


bool 
AosDocClientWrapper::modifyObj( 
		const AosXmlTagPtr &doc,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoModifyObj(rdata.getPtrNoLock(), doc);
}


bool 
AosDocClientWrapper::modifyObj(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &newxml,
		const OmnString &rename,
		const bool synobj)        
{
	return Jimo::jimoModifyObj(rdata.getPtrNoLock(),
			newxml, rename, synobj);
}



AosAccessRcdPtr 
AosDocClientWrapper::getAccessRcd(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &ref_doc,
		const u64 &ref_docid, 
		const OmnString &ref_objid,
		const bool parent_flag)
{
	//Xuqi 2015/11/4
	return Jimo::jimoGetAccessRcd(rdata.getPtrNoLock(),
			ref_doc, ref_docid, ref_objid, parent_flag);
}


bool 
AosDocClientWrapper::isCloudidBound(                     
		const OmnString &cid,
		u64 &userid,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoIsCloudidBound(rdata.getPtrNoLock(),
			cid, userid);
}

							
AosXmlTagPtr 
AosDocClientWrapper::getDoc(                   
		const u64 &docid,
		const OmnString &objid,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}


bool 
AosDocClientWrapper::deleteObj(
		const AosRundataPtr &rdata,
		const OmnString &docid,
		const OmnString &theobjid,
		const OmnString &container,
		const bool deleteFlag)
{
	return Jimo::jimoDeleteObj(rdata.getPtrNoLock(),
			docid, theobjid, container, deleteFlag);
}


OmnString   
AosDocClientWrapper::getCloudid(
		const u64 &user_docid,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoGetCloudid(rdata.getPtrNoLock(), 
			user_docid);
}


AosAccessRcdPtr 
AosDocClientWrapper::getAccessRecord(
		const u64 &docid,
		const bool parent_flag,
		const AosRundataPtr &rdata)
{
	//Xuqi 2015/11/4
	return Jimo::jimoGetAccessRecord(rdata.getPtrNoLock(),
			docid, parent_flag);
}


bool 
AosDocClientWrapper::writeAccessDoc(
		const u64 &owndocid, 
		AosXmlTagPtr &arcd, 
		const AosRundataPtr &rdata)
{
	//Xuqi 2015/11/4
	return Jimo::jimoWriteAccessDoc(rdata.getPtrNoLock(), 
			owndocid, arcd);
}


						
AosXmlTagPtr 
AosDocClientWrapper::createBinaryDoc(
		const OmnString &docstr,
		const bool is_public, 
		const AosBuffPtr &buff,
		const int &vid,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoCreateBinaryDoc(rdata.getPtrNoLock(),
			docstr, is_public, buff, vid);
}

AosXmlTagPtr 
AosDocClientWrapper::retrieveBinaryDoc(
		const OmnString &objid, 
		AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	return Jimo::jimoRetrieveBinaryDoc(rdata.getPtrNoLock(), 
			objid, buff);
}

	
AosXmlTagPtr 
AosDocClientWrapper::retrieveBinaryDoc(
		const u64 &docid, 
		AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	return Jimo::jimoRetrieveBinaryDoc(rdata.getPtrNoLock(),
			docid, buff);
}

							
bool    
AosDocClientWrapper::procDocLock(                         
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &req,
		const u64 &docid)
{
	return Jimo::jimoProcDocLock(rdata.getPtrNoLock(), req, 
			docid);
}


void 
AosDocClientWrapper::procNotifyMsg(
		const u64 &docid, 
		const u32 siteid,
		const AosRundataPtr &rdata)
{
	Jimo::jimoProcNotifyMsg(rdata.getPtrNoLock(), docid, siteid);
}


AosXmlTagPtr 
AosDocClientWrapper::getParentContainer(
		const AosRundataPtr &rdata,
		const OmnString &container_objid, 
		const bool ispublic)
{
	return Jimo::jimoGetParentContainer(rdata.getPtrNoLock(),
			container_objid, ispublic);
}


bool 
AosDocClientWrapper::getJQLDocs(
		const AosRundataPtr &rdata,
		const JQLTypes::DocType	&type,
		vector<string> &values)
{
	return Jimo::jimoGetJQLDocs(rdata.getPtrNoLock(), type, 
			values);
}



bool 
AosDocClientWrapper::createJQLTable(	
		const AosRundataPtr &rdata,
		const OmnString &table_def)
{
	return Jimo::jimoCreateJQLTable(rdata.getPtrNoLock(), 
			table_def);
}



bool 
AosDocClientWrapper::deleteDocByDocid(	
		const AosRundataPtr &rdata,
		const int64_t docid)
{
	return Jimo::jimoDeleteDocByDocid(rdata.getPtrNoLock(), 
			docid);
}


bool 
AosDocClientWrapper::deleteDocByObjid(	
		const AosRundataPtr &rdata,
		const string &objid)
{
	return Jimo::jimoDeleteDocByObjid(rdata.getPtrNoLock(), 
			objid);
}

							
bool 
AosDocClientWrapper::deleteDoc(                         
		const AosXmlTagPtr &origdocroot,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoDeleteDoc(rdata.getPtrNoLock(), origdocroot);
}


bool 
AosDocClientWrapper::getDocByJQL(	
		const AosRundataPtr &rdata,
		const string &objid,
		string &doc)
{
	return Jimo::jimoGetDocByJQL(rdata.getPtrNoLock(), objid, 
			doc);
}


AosXmlTagPtr 
AosDocClientWrapper::getDocByKey(
		const AosRundataPtr &rdata, 
		const OmnString &container, 
		const OmnString &key_name, 
		const OmnString &key_value)
{
	return Jimo::jimoGetDocByKey(rdata.getPtrNoLock(), container,
			key_name, key_value);
}


AosXmlTagPtr 
AosDocClientWrapper::getDocByKey(
		const AosRundataPtr &rdata, 
		const OmnString &tablename)
{
	return Jimo::jimoGetDocByKey(rdata.getPtrNoLock(), tablename);
}


AosXmlTagPtr 
AosDocClientWrapper::getDocByKeys(
		const AosRundataPtr &rdata, 
	 	const OmnString &container, 
	 	const vector<OmnString> &key_names, 
	 	const vector<OmnString> &key_values)
{
	return Jimo::jimoGetDocByKeys(rdata.getPtrNoLock(), container,
			key_names, key_values);
}


bool 
AosDocClientWrapper::mergeSnapshot(
		const AosRundataPtr &rdata,
		const int virtual_id,
		const AosDocType::E doc_type,
		const u64 &target_snap_id,
		const u64 &merge_snap_id)
{
	return Jimo::jimoMergeSnapshot(rdata.getPtrNoLock(), 
			virtual_id, doc_type, target_snap_id, merge_snap_id);
}


u64 
AosDocClientWrapper::createSnapshot(
		const AosRundataPtr &rdata,
		const int virtual_id,
		const u64 snap_id,
		const AosDocType::E doc_type,
		const u64 &task_docid)
{
	return Jimo::jimoCreateSnapshot(rdata.getPtrNoLock(),
			virtual_id, snap_id, doc_type, task_docid);
}


bool 
AosDocClientWrapper::commitSnapshot(const AosRundataPtr &rdata,
		const int virtual_id,
		const AosDocType::E doc_type,
		const u64 &snap_id,
		const u64 &task_docid)
{
	return Jimo::jimoCommitSnapshot(rdata.getPtrNoLock(),
			virtual_id, doc_type, snap_id, task_docid);
}


bool 
AosDocClientWrapper::rollbackSnapshot(
		const AosRundataPtr &rdata,
		const int virtual_id,
		const AosDocType::E doc_type,
		const u64 &snap_id,
		const u64 &task_docid)
{
	return Jimo::jimoRollbackSnapshot(rdata.getPtrNoLock(),
			virtual_id, doc_type, snap_id, task_docid);
}


AosXmlTagPtr 
AosDocClientWrapper::createRootCtnr(              
		const OmnString &docstr,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoCreateRootCtnr(rdata.getPtrNoLock(), docstr);
}


bool 
AosDocClientWrapper::deleteDoc(
		const AosXmlTagPtr &origdocroot, 
		const AosRundataPtr &rdata, 
		const OmnString &ctnr_objid,
		const bool deleteFlag)
{
	return Jimo::jimoDeleteDoc(rdata.getPtrNoLock(), origdocroot,
			ctnr_objid, deleteFlag);
}


AosXmlTagPtr 
AosDocClientWrapper::createDocByTemplate1(             
		const AosRundataPtr &rdata,
		const OmnString &cid,
		const OmnString &objid,
		const OmnString &template_objid)
{
	return Jimo::jimoCreateDocByTemplate1(rdata.getPtrNoLock(),
			cid, objid, template_objid);
}


bool 
AosDocClientWrapper::bindObjid(                     
		OmnString &objid,
		u64 &docid,
		bool &objid_changed,
		const bool resolve,
		const bool keepDocid,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoBindObjid(rdata.getPtrNoLock(), objid,
			docid, objid_changed, resolve, keepDocid);
}


u64 
AosDocClientWrapper::doesObjidExist(                 
		const u32 siteid,
		const OmnString &objid,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoDoesObjidExist(rdata.getPtrNoLock(), siteid,
			objid);
}


AosXmlTagPtr 
AosDocClientWrapper::cloneDoc(                    
		const AosRundataPtr &rdata,
		const OmnString &cloudid,
		const OmnString &fromobjid,
		const OmnString &toobjid,
		const bool is_public)
{
	return Jimo::jimoCloneDoc(rdata.getPtrNoLock(), cloudid, 
			fromobjid, toobjid, is_public);
}


OmnString 
AosDocClientWrapper::getObjidByDocid(
		const u64 &docid, 
		const AosRundataPtr &rdata)
{
	return Jimo::jimoGetObjidByDocid(rdata.getPtrNoLock(), docid);
}

 
bool 
AosDocClientWrapper::isLog(const u64 &docid) const
{
	return Jimo::jimoIsLog(docid);
}


AosXmlTagPtr 
AosDocClientWrapper::getDeletedDoc(                
		const OmnString &objid,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoGetDeletedDoc(rdata.getPtrNoLock(), objid,
			docid);
}

	 
bool 
AosDocClientWrapper::bindCloudid(                      
		const OmnString &cid,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoBindCloudid_doc(rdata.getPtrNoLock(), cid,
			docid);
}


AosBuffPtr 
AosDocClientWrapper::batchGetDocs(                               
		const OmnString &scanner_id,
		const int server_id,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoBatchGetDocs(rdata.getPtrNoLock(), 
			scanner_id, server_id);
}


void    
AosDocClientWrapper::batchGetDocs(
		const AosRundataPtr &rdata,
		const int vid,
		const u32 num_docs,
		const u32 blocksize,
		const AosBuffPtr &docid_buff,
		const AosBuffPtr &sizeid_len,
		const AosAsyncRespCallerPtr &resp_caller)
{
	Jimo::jimoBatchGetDocs(rdata.getPtrNoLock(), vid, num_docs,
			blocksize, docid_buff, sizeid_len, resp_caller);
}


bool 
AosDocClientWrapper::insertInto(
		const AosXmlTagPtr &data,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoInsertInto(rdata.getPtrNoLock(), data);
}

/*
bool 
AosDocClientWrapper::createDoc(
		AosRundata *rdata,                                   
		const AosXmlTagPtr &newdoc,
		const OmnString &objid,
		u64 &docid,
		const bool keep_objid,
		const u64 snap_id)
{
	OmnNotImplementedYet;
	return false;
}
*/


bool 
AosDocClientWrapper::batchSendDocids(
	 	const OmnString &scanner_id,
	 	const int serverid,
	 	const AosBitmapObjPtr &bitmap,
	 	const AosRundataPtr &rdata)
{
	return Jimo::jimoBatchSendDocids(rdata.getPtrNoLock(), 
			scanner_id, serverid, bitmap);
}


bool 
AosDocClientWrapper::batchSendDocids(
	 	const OmnString &scanner_id,
	 	const int serverid,
	 	const AosBitmapObjPtr &bitmap,
	 	const AosDocClientCallerPtr &caller,
	 	const AosRundataPtr &rdata)
{
	return Jimo::jimoBatchSendDocids(rdata.getPtrNoLock(),
			scanner_id, serverid, bitmap, caller);
}

