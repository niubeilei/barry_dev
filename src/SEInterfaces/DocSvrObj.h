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
// 2015/01/09 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DocSvrObj_h
#define Aos_SEInterfaces_DocSvrObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/TransId.h"


class AosDocSvrObj : virtual public OmnRCObject
{
	static AosDocSvrObjPtr	smDocSvr;

public:
	static void setDocSvr(const AosDocSvrObjPtr &d) {smDocSvr = d;}
	static AosDocSvrObjPtr getDocSvr() {return smDocSvr;}

	virtual AosXmlTagPtr getDoc(
			              const u64 &docid,           
						  const u64 snap_id,
						  const AosRundataPtr &rdata) =0 ;

	virtual bool    getDocs(                             
			              const AosRundataPtr &rdata,
						  const AosBuffPtr &buff,
						  const u32 num_docids,
						  const AosBuffPtr &docid_buff,
						  map<u64, int> &sizeid_len,
						  u64 &end_docid,
						  const u64 snap_id) =0;

	 virtual bool    retrieveBinaryDoc(             
			              const AosXmlTagPtr &doc,
						  AosBuffPtr &buff,
						  const AosRundataPtr &rdata,
						  const u64 snap_id) =0 ;

	 virtual bool    commitSnapshot(                  
			             const int virtual_id,
						 const AosDocType::E doc_type,
						 const u64 &snap_id,
						 const AosTransId &trans_id,
						 const AosRundataPtr &rdata) =0 ;
	 virtual bool    readLockDoc(                 
			             const u64 &docid,
						 const u64 &userid,
						 const int waittimer,
						 const int holdtimer,
						 const AosTransPtr &trans,
						 int &status,
						 AosRundata *rdata) =0 ;

	 virtual bool    batchDeleteCSVDoc(              
			             const AosTransPtr &trans,
						 const int virtual_id,
						 const u32 sizeid,
						 const u64 &num_docs,
						 const int record_len,
						 const u64 &data_len,
						 const AosBuffPtr &buff,
						 const u64 &snap_id,
						 const AosRundataPtr &rdata) =0 ;

	 virtual  bool    createDoc(
			             const AosRundataPtr &rdata,
						 const u64 &docid,
						 const char *doc,
						 const int64_t &doc_len,
						 const AosTransId &trans_id,
						 const u64 &snap_id) =0 ;

	 virtual bool    createDoc(  
			             const AosRundataPtr &rdata,
						 const AosXmlTagPtr &newdoc, 
						 const AosTransId &trans_id,
						 const u64 snap_id) =0 ;

	 virtual bool    deleteObj(
			             const AosRundataPtr &rdata,
						 const u64 &docid,
						 const AosTransId &trans_id,
						 const u64 snap_id) = 0;

	 virtual bool    deleteBinaryDoc(                 
			             const u64 &docid,
						 const AosRundataPtr &rdata,
						 const AosTransId &trans_id,
						 const u64 snap_id) =0 ;

	 virtual AosXmlTagPtr getFixedDoc(                
			             const u64 &docid,
						 const int record_len,
						 const u64 snap_id,
						 const AosRundataPtr &rdata) = 0;

	 virtual bool    saveBinaryDoc(                  
			             const AosXmlTagPtr &doc,
						 const AosBuffPtr &buff,
						 OmnString &signature,
						 const AosRundataPtr &rdata,
						 const AosTransId &trans_id,
						 const u64 snap_id) =0;
	 
	 virtual bool    isDocDeleted(                    
			             const u64 &docid,
						 bool &result,
						 const AosRundataPtr &rdata) = 0;

	 virtual  AosBuffPtr  getFixedDocs(                   
			             const AosRundataPtr &rdata,
						 const vector<u64> &docids,
						 const int record_size) =0;

	 virtual AosXmlTagPtr getCSVDoc(
			             const u64 &docid, 
						 const int record_len,
						 const u64 snap_id,
						 const AosRundataPtr &rdata) =0;

	virtual AosXmlTagPtr getCommonDoc(
			             const u64 &docid, 
						 const int record_len,
						 const u64 snap_id,
						 const AosRundataPtr &rdata) =0;

	 virtual bool    rollbackSnapshot(                
			             const int virtual_id,
						 const AosDocType::E doc_type,
						 const u64 &snap_id,
						 const AosTransId &trans_id,
						 const AosRundataPtr &rdata) = 0 ;

	 virtual  AosBuffPtr  getCommonDocs(                     
			             const AosRundataPtr &rdata,
						 const vector<u64> &docids,
						 const int record_size) = 0;


	 virtual  AosBuffPtr  getCSVDocs(                     
			             const AosRundataPtr &rdata,
						 const vector<u64> &docids,
						 const int record_size) = 0;

	 virtual bool    createDocSafe(                    
			             const AosRundataPtr &rdata,
						 const AosXmlTagPtr &doc,
						 const bool &saveDocFlag,
						 const AosTransId &trans_id,
						 const u64 snap_id) = 0;
	 virtual  bool        writeUnLockDoc(            
			             const u64 &docid,
						 const u64 &userid,
				         const AosXmlTagPtr &newdoc,
			             const AosTransId &trans_id,
						 AosRundata *rdata) = 0;

	 virtual bool    modifyObj(                          
			             const AosRundataPtr &rdata,
						 const AosXmlTagPtr &newxml,
						 const u64 &newdid,
						 const AosTransId &trans_id,
						 const u64 snap_id) = 0;

	 virtual  u64     createSnapshot(                  
			             const int virtual_id,
						 const u64 snap_id,
						 const AosDocType::E doc_type,
						 const AosTransId &trans_id,
						 const AosRundataPtr &rdata) = 0;

	 virtual bool    batchSaveCSVDoc(                 
			             const AosTransPtr &trans,
						 const int virtual_id,
						 const u32 sizeid,
						 const u64 &num_docs,
						 const int record_len,
						 const u64 &data_len,
						 const AosBuffPtr &buff,
						 const u64 &snap_id,
						 const AosRundataPtr &rdata) = 0;

	virtual bool	batchInsertDocNew(
						const AosTransPtr &trans,
						const AosBuffPtr &buff,
						const int virtual_id,
						const u64 group_id,
						const u64 &snap_id,
						const AosRundataPtr &rdata) = 0;

	virtual bool	batchUpdateDocNew(
						const AosTransPtr &trans,
						const AosBuffPtr &buff,
						const int virtual_id,
						const u64 group_id,
						const u64 &snap_id,
						const AosRundataPtr &rdata) = 0;

	 virtual bool	 batchInsertCommonDoc(
						 const AosTransPtr &trans,
						 const int virtual_id,
						 const u32 sizeid,
						 const u64 &num_docs,
						 const int record_len,
						 const u64 &data_len,
						 const AosBuffPtr &buff,
						 const u64 &snap_id,
						 const AosRundataPtr &rdata) = 0;

	 virtual  bool    writeLockDoc(                 
			              const u64 &docid,
						  const u64 &userid,
						  const int waittimer,
						  const int holdtimer,
						  const AosTransPtr &trans,
						  int &status,
						  AosRundata *rdata) = 0;

	 virtual  bool        readUnLockDoc(      
			              const u64 &docid,
						  const u64 &userid,
						  AosRundata *rdata) = 0;

	 virtual  bool    mergeSnapshot(                    
			              const int virtual_id,
						  const AosDocType::E doc_type,
						  const u64 target_snap_id,
						  const u64 merge_snap_id,
						  const AosTransId &trans_id,
						  const AosRundataPtr &rdata) = 0;

	 virtual bool    batchSaveGroupedDoc(                
			             const AosTransPtr &trans,
						 const int virtual_id,
						 const u32 sizeid,
						 const u64 &num_docs,
						 const int record_len,
						 const AosBuffPtr &docids_buff,
						 const AosBuffPtr &raw_data,
						 const u64 &snap_id,
						 const AosRundataPtr &rdata) =0;

	 virtual  bool    getGroupedDocWithComp()=0;

	 virtual AosDocFileMgrObjPtr getGroupedDocFileMgr( 
			             const int vid,
						 const bool create_flag,
						 const AosRundataPtr &rdata) = 0;

	 virtual bool    addSmallDocs(                   
			             const u64 &docid,
						 const AosBuffPtr &buff,
						 const AosTransId &trans_id,
						 const AosRundataPtr &rdata) =0;

	 virtual AosBuffPtr getSmallDocs(                   
			             const AosRundataPtr &rdata,
						 const u64 &docid) = 0;

	 virtual  bool    saveToFile(                      
			             const u64 &docid,
						 const AosXmlTagPtr &doc,
						 const AosRundataPtr &rdata,
						 const AosTransId &trans_id,
						 const u64 snap_id) =0;

};

#endif
