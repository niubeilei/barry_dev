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
// 06/28/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocClient_DocClient_h
#define AOS_DocClient_DocClient_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "DocClient/Entry.h"
#include "DocClient/Ptrs.h"
#include "IdGen/Ptrs.h"
#include "IdGen/U64IdGen.h"
#include "Rundata/Rundata.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DocSvrCbObj.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/DocClientCaller.h"
#include "SEInterfaces/UserAcctObj.h"
#include "Security/Session.h"
#include "Security/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "TransClient/Ptrs.h"
#include "TransUtil/IdRobin.h"
#include "TransUtil/Ptrs.h"
#include "XmlUtil/AccessRcd.h"
#include "XmlUtil/DocTypes.h"
#include "SEUtil/JqlTypes.h"

#ifndef AOSDOCSVRLOG_DEFINE
#define AOSDOCSVRLOG_DEFINE OmnString _local_log; _local_log << __FILE__ << ":" << __LINE__
#endif

#ifndef AOSDOCSVRLOG
#define AOSDOCSVRLOG _local_log << ":" << __LINE__ << ":"
#endif

#ifndef AOSDOCSVRLOG_CLOSE
#define AOSDOCSVRLOG_CLOSE(x, rdata) _local_log << ":" << __LINE__ << "::"; createTempDoc((x), (rdata))
#endif

#ifndef TTLMINUS
#define TTLMINUS (rdata->checkTTL(__FILE__, __LINE__ ,__func__))
#endif

OmnDefineSingletonClass(AosDocClientSingleton,
                        AosDocClient,
                        AosDocClientSelf,
                        OmnSingletonObjId::eDocClient,
                        "DocClient");

class AosDocClient : public AosDocClientObj
{
	OmnDefineRCObject;
	
public:
	enum
	{
		eDftMaxObjidTries = 500,
		eMaxCtnrs = 30
	};

	typedef hash_map<const OmnString, AosEntry, Omn_Str_hash, compare_str> cmap_t;
	typedef hash_map<const OmnString, AosEntry, Omn_Str_hash, compare_str>::iterator cmapitr_t;

private:
	OmnMutexPtr         	mLock;

	bool                	mIsStopping;
	bool                	mIsRepairing;
	u64                 	mRepairingMaxDocid;
	cmap_t					mCallerMap;
	AosDocTransHandlerPtr	mTransHandler;			// Chen Ding, 2014/11/09
	AosDocTransHandler*		mTransHandlerRaw;		// Chen Ding, 2014/11/09
	

	// The reference of other module
	static AosSeLogClientObjPtr		mSeLogClientObj;
	static AosObjMgrObjPtr			mObjMgrObj;
	static AosLoginMgrObjPtr		mLoginMgrObj;
	static AosVersionServerObjPtr	mVersionServerObj;
	static AosDocMgrObjPtr			mDocMgrObj;
	
	static int          mTotalDeleted;
	static int          mNumModifies;
	static bool			mShowLog;

	AosDocClient();
	~AosDocClient();

public:
    // Singleton class interface
    static AosDocClient*    getSelf();
    virtual bool        	start();
    virtual bool        	stop();
    virtual bool			config(const AosXmlTagPtr &def);

	// Creation
	AosXmlTagPtr cloneDoc(
				const AosRundataPtr &rdata,
				const OmnString &cloudid,
				const OmnString &fromobjid, 
				const OmnString &toobjid, 
				const bool is_public);

	bool createDoc1(
				const AosRundataPtr &rdata, 
				const AosXmlTagPtr &cmd, 
				const OmnString &target_cid,
				const AosXmlTagPtr &newdoc,
				const bool resolveObjid,
				const AosDocSvrCbObjPtr &caller, 
				void *userdata,
				const bool keepDocid);

	AosXmlTagPtr createDocSafe1(
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

	AosXmlTagPtr createDocSafe3(
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

	AosXmlTagPtr createRootCtnr(
				const OmnString &docstr,
				const AosRundataPtr &rdata);

	AosXmlTagPtr createDocByTemplate1(
				const AosRundataPtr &rdata,     
				const OmnString &cid,           
				const OmnString &objid,               
				const OmnString &template_objid);

	AosXmlTagPtr createDoc(
				const AosXmlTagPtr &doc, 
				const AosRundataPtr &rdata)
	{
		return createDocSafe3(rdata, doc, "", "", false, true, false, false, true, true, true);
	}

	bool addCopy(
			const u64 &docid,
			const AosXmlTagPtr &doc, 
			const AosRundataPtr &rdata);

	AosXmlTagPtr createDoc(
			const OmnString &docstr, 
			const bool is_public, 
			const AosRundataPtr &rdata)
	{
		return createDocSafe1(rdata, docstr, 
						"", 			// Cloudid
						"", 			// objid_base
						is_public,
						true, 			// checkCreation
						false, 			// keepDocid
						false,			// reserved
						true,			// cid_required
						true);			// check_ctnr
	}

	virtual AosXmlTagPtr createBinaryDoc(
				const OmnString &docstr,
				const bool is_public, 
				const AosBuffPtr &buff,
				const AosRundataPtr &rdata)
	{
		return createBinaryDoc(docstr, is_public, buff, -1, rdata); 
	}

	virtual AosXmlTagPtr createBinaryDoc(
				const OmnString &docstr,
				const bool is_public, 
				const AosBuffPtr &buff,
				const int &vid,
				const AosRundataPtr &rdata);

	bool createJQLTable(	
				const AosRundataPtr &rdata,
				const OmnString &table_def);

	virtual AosAccessRcdPtr getAccessRecord(
				const u64 &docid, 
				const bool parent_flag,
				const AosRundataPtr &rdata)
	{
		return getAccessRcd(rdata, 0, docid, "", parent_flag);
	}

	AosXmlTagPtr getParentDoc(
				const OmnString &objid, 
				const u64 &docid, 
				const AosRundataPtr &rdata);

	AosAccessRcdPtr getAccessRcd(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &ref_doc,
				const u64 &ref_docid, 
				const OmnString &ref_objid,
				const bool parent_flag);

	AosXmlTagPtr getDeletedDoc(
				const OmnString &objid, 
				const u64 &docid, 
				const AosRundataPtr &rdata);

	AosXmlTagPtr getDoc(
				const u64 &docid, 
				const OmnString &objid, 
				const AosRundataPtr &rdata)
	{
		if (docid) return getDocByDocid(docid, rdata);
		aos_assert_r(objid != "", 0);
		return getDocByObjid(objid, rdata);
	}

	AosXmlTagPtr getDoc(
				const AosRundataPtr &rdata,
				const OmnString &docid, 
				const OmnString &objid, 
				bool &duplicated);

	//felicia peng, 2014/10/10
	bool 		readLockDocByDocid(
				AosRundata *rdata,
				const u64 &docid,
				const int waittimer,
				const int holdtimer);
	
	bool 		writeLockDocByDocid(
				AosRundata *rdata,
				const u64 &docid,
				const int waittimer,
				const int holdtimer);
	
	bool		readUnlockDocByDocid(
				AosRundata *rdata, 
				const u64 &docid);

	bool		writeUnlockDocByDocid(
				AosRundata *rdata, 
				const u64 &docid,
				const AosXmlTagPtr &newdoc);

	// Young. 2014/11/10
	AosXmlTagPtr getLogByDocid(
				const u64 &docid,
				const AosRundataPtr &rdata);

	AosXmlTagPtr getDocByDocid(
				const u64 &docid,
				const AosRundataPtr &rdata);

	AosXmlTagPtr getDocByDocid(
				const OmnString &docid,
				const AosRundataPtr &rdata)
	{
		u64 did = atoll(docid.data());
		aos_assert_r(did > 0, 0);
		return getDocByDocid(did, rdata);
	}

	AosXmlTagPtr getDocByCloudid(
				const OmnString &cid, 
				const AosRundataPtr &rdata);

	AosXmlTagPtr getDocByObjid(
				const OmnString &objid, 
				const AosRundataPtr &rdata);

	// Gets
	u64			getDocidByCloudid(
				const u32 siteid,
				const OmnString &cid,
				const AosRundataPtr &rdata);

	u64			getDocidByObjid(
				const OmnString &objid, 
				const AosRundataPtr &rdata);

	OmnString	getCloudid(
				const u64 &user_docid, 
				const AosRundataPtr &rdata);

	AosUserAcctObjPtr getUserAcct(
				const u64 &user_docid, 
				const AosRundataPtr &rdata);

	AosUserAcctObjPtr  getUserAcct(
				const OmnString &userid, 
				const AosRundataPtr &rdata)
				{
					u64 did = atoll(userid.data());
					return getUserAcct(did, rdata);
				}

	AosAccessRcdPtr getParentArcd(
				const OmnString &objid, 
				const u64 &docid, 
				const AosRundataPtr &rdata);

	OmnString getObjidByDocid(const u64 &docid, const AosRundataPtr &rdata);

	AosBuffPtr batchGetDocs(
				const OmnString &scanner_id,
				const int server_id,
				const AosRundataPtr &rdata);

	void batchGetDocs(
				const AosRundataPtr &rdata,
				const int vid, 
				const u32 num_docs,
				const u32 blocksize,
				const AosBuffPtr &docid_buff,
				const AosBuffPtr &sizeid_len,
				const AosAsyncRespCallerPtr &resp_caller);

	AosXmlTagPtr retrieveBinaryDoc(
				const OmnString &objid, 
				AosBuffPtr &buff, 
				const AosRundataPtr &rdata);
	
	AosXmlTagPtr retrieveBinaryDoc(
				const u64 &docid,  
				AosBuffPtr &buff,
				const AosRundataPtr &rdata);

	void retrieveBinaryDoc(
				const AosAsyncRespCallerPtr &resp_caller,
				const u64 &docid,
				const AosRundataPtr &rdata);

	bool batchDataReceived(
				const OmnString &scannerid,
				const AosBuffPtr &contents,
				const int serverid,
				const bool finished,
				const int num_call_back_data_msgs,
				const AosRundataPtr &rdata);

	bool getTableNames(
				const AosRundataPtr &rdata,
				vector<string> &values);

	bool getJQLDocs(
				const AosRundataPtr &rdata,
				const JQLTypes::DocType	&type,
				vector<string> &values);

	u64  getJQLDocidByTablename(
				const AosRundataPtr &rdata,
				const string &tablename);

	// Modifications
	bool modifyAttrStr(
					const AosXmlTagPtr &doc,
					const OmnString &aname, 
					const OmnString &newvalue, 
					const bool value_unique, 
					const bool docid_unique, 
					const bool checkSecurity,
					const AosRundataPtr &rdata,
					const bool need_trigger = true);

	bool modifyAttrU64(
	 				const AosXmlTagPtr &doc,
	 				const OmnString &aname, 
	 				const u64 &newvalue, 
	 				const bool value_unique, 
	 				const bool docid_unique, 
	 				const bool checkSecurity,
	 				const AosRundataPtr &rdata,
	 				const bool need_trigger = true);

	bool modifyAttrXPath(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &doc,
					const OmnString &aname, 
					const OmnString &newvalue, 
					const bool value_unique, 
					const bool docid_unique, 
					const bool checkSecurity,
					const bool need_trigger);

	OmnString incrementValue(
				const u64 &docid,
				const OmnString &objid,
				const OmnString &aname,
				const OmnString &initvalue,
				const bool value_unique,
				const bool docid_unique,
				const AosRundataPtr &rdata);

/*	u64		incrementValue(
				const u64 &docid, 
				const OmnString &objid,
				const OmnString &aname, 
				const u64 &initvalue, 
				const u64 &incValue,
				const u64 &dft_value,
				const bool value_unique, 
				const bool docid_unique, 
				const AosRundataPtr &rdata); */

	bool	modifyObj(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &newxml,
				const OmnString &rename,
				const bool synobj);

	bool	modifyObj(
				const AosXmlTagPtr &doc,
				const AosRundataPtr &rdata)
	{
		return modifyObj(rdata, doc, "", false);
	}

	bool	modifyAttrStr1(
				const AosRundataPtr &rdata,
				const u64 &docid,
				const OmnString &objid,
				const OmnString &aname,	
				const OmnString &newvalue,
				const bool value_unique,
				const bool docid_unique,
				const bool checkSecurity,
				const bool need_trigger = true);

	// AosXmlTagPtr createBinaryDocByStat(
	// 			const OmnString &docstr,
	// 			const AosBuffPtr &buff,
	// 			const int &vid,
	// 			const u64 &normal_snap_id,
	// 			const u64 &binary_snap_id,
	// 			const AosRundataPtr &rdata);

	bool modifyBinaryDocByStat(
				const AosXmlTagPtr &doc,
				const AosBuffPtr &buff,
				const u64 &normal_snap_id,
				const u64 &binary_snap_id,
				const AosRundataPtr &rdata);

	AosXmlTagPtr retrieveBinaryDocByStat(
				const u64 &docid, 
				AosBuffPtr &docBuff, 
				const u64 &normal_snap_id,
				const u64 &binary_snap_id,
				const AosRundataPtr &rdata);
	
	bool appendBinaryDoc(
				const OmnString &objid, 
				const AosBuffPtr &buff, 
				const AosRundataPtr &rdata);

	// Deletions
	bool deleteDoc(
			const AosXmlTagPtr &origdocroot, 
			const AosRundataPtr &rdata) 
	{
		aos_assert_rr(origdocroot, rdata, false);
		OmnString ctnr_objid = origdocroot->getAttrStr(AOSTAG_PARENTC);
		return deleteDoc(origdocroot, rdata, ctnr_objid, true);
	}

	bool deleteDoc(
			const AosXmlTagPtr &origdocroot, 
			const AosRundataPtr &rdata, 
			const OmnString &ctnr_objid,
			const bool deleteFlag);

	bool deleteBatchDocs(
			const AosRundataPtr &rdata,
			const OmnString &scanner_id);

	bool	deleteObj(
				const AosRundataPtr &rdata,
				const OmnString &docid,	
				const OmnString &theobjid,
				const OmnString &container,
				const bool deleteFlag);

	bool deleteBinaryDoc(
				const OmnString &objid, 
				const AosRundataPtr &rdata);

	bool deleteBinaryDoc(
				const u64 &docid, 
				const AosRundataPtr &rdata);

	// Miscellenous
	void procNotifyMsg(
				const u64 &docid,
				const u32 siteid,
				const AosRundataPtr &rdata);

	void	exitSearchEngine();

	static bool showLog() {return mShowLog;}
	u64		getRepairingMaxDocid() const {return mRepairingMaxDocid;}

	bool bindObjid(
	 		OmnString &objid,
	 		u64 &docid,          
	 		bool &objid_changed,
	 		const bool resolve,
	 		const bool keepDocid,
	 		const AosRundataPtr &rdata);

	bool bindCloudid(
			const OmnString &cid,
			const u64 &docid,
			const AosRundataPtr &rdata);

	u64	doesObjidExist(
			const u32 siteid,   
			const OmnString &objid,    
			const AosRundataPtr &rdata);

	bool isRepairing() const {return mIsRepairing;}

	bool isLog(const u64 &docid) const 
	{
		AosDocType::E type = AosDocType::getDocType(docid);
		if (type == AosDocType::eLogDoc) return true;
		return false;
	}

	bool isCloudidBound(
				const OmnString &cid, 
				u64 &userid, 
				const AosRundataPtr &rdata);

	void setRepairing(const bool f) {mIsRepairing = f;}

	bool	procDocLock(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &req,
					const u64 &docid);

	bool	checkDocLock(
					const AosRundataPtr &rdata,
					const u64 &docid,
					const OmnString &type);

	bool	batchSendDocids(
					const OmnString &scanner_id,
					const int serverid,
					const AosBitmapObjPtr &bitmap,
					const AosRundataPtr &rdata);

	bool	batchSendDocids(
					const OmnString &scanner_id,
					const int serverid,
					const AosBitmapObjPtr &bitmap,
					const AosDocClientCallerPtr &caller,
					const AosRundataPtr &rdata);

	bool	batchSendStart(
					const int serverid,
					const OmnString &scanner_id,
					const u32 &queue_size,
					const OmnString &read_policy,
					const u64 &blocksize,
					const OmnString &batch_type,
					const AosXmlTagPtr &fnames,
					const AosRundataPtr &rdata);

	bool	batchSendFinished(
					const int serverid,
					const OmnString &scanner_id,
					const int total_num_data_msgs,
					const AosRundataPtr &rdata);

	AosXmlTagPtr writeLockDocByObjid(
					const OmnString &objid,
					const AosRundataPtr &rdata);

	AosXmlTagPtr writeLockDocByDocid(
					const u64 &docid,
					const AosRundataPtr &rdata);

	AosXmlTagPtr releaseWriteLock(
					const AosXmlTagPtr &doc,
					const AosRundataPtr &rdata);

	AosXmlTagPtr releaseWriteLockDocByObjid(
					const OmnString &objid,
					const AosRundataPtr &rdata);

	AosXmlTagPtr releaseWriteLockDocByDocid(
					const u64 &docid,
					const AosRundataPtr &rdata);

	bool purgeDoc(const OmnString &objid, const AosRundataPtr &rdata);
	bool purgeDoc(const u64 &docid, const AosRundataPtr &rdata);
	bool purgeDocByCloudid(const OmnString &cid, const AosRundataPtr &rdata);

	AosXmlTagPtr getParentContainer(
					const AosRundataPtr &rdata,
					const OmnString &container_objid, 
					const bool ispublic);

	bool dropJQLTable( 
					const AosRundataPtr &rdata,
					const OmnString &table_name);

	bool createDocByJQL( 
					const AosRundataPtr &rdata,
					const OmnString &container_objid,
					const OmnString &objid,
					const OmnString &doc);

	bool deleteDocByDocid(	
					const AosRundataPtr &rdata,
					const int64_t docid);

	bool deleteDocByObjid(	
					const AosRundataPtr &rdata,
					const string &objid);

	bool getDocByJQL(	
					const AosRundataPtr &rdata,
					const string &objid,
					string &doc);

	bool getParentContainerByObjid(	
					const AosRundataPtr &rdata,
					const string &objid,
					string &doc);

	bool getParentContainerByDocid(	
					const AosRundataPtr &rdata,
					const int64_t docid, 
					string &doc);

	AosXmlTagPtr getDocByKey(
					const AosRundataPtr &rdata, 
					const OmnString &container, 
					const OmnString &key_name, 
					const OmnString &key_value);

	AosXmlTagPtr getDocByKey(
					const AosRundataPtr &rdata, 
					const OmnString &tablename);

	bool writeAccessDoc(
			const u64 &docid, 
			const AosXmlTagPtr &doc,
			const AosRundataPtr &rdata);

	// Chen Ding, 2014/01/29
	AosXmlTagPtr getDocByKeys(
			const AosRundataPtr &rdata, 
			const OmnString &container, 
			const vector<OmnString> &key_names, 
			const vector<OmnString> &key_values);

	bool mergeSnapshot(          
			const AosRundataPtr &rdata,
			const int virtual_id,
			const AosDocType::E doc_type,
			const u64 &target_snap_id,
			const u64 &merge_snap_id);

	u64 createSnapshot(
			const AosRundataPtr &rdata,
			const int virtual_id,
			const u64 snap_id,
			const AosDocType::E doc_type,
			const u64 &task_docid);

	bool commitSnapshot(
			const AosRundataPtr &rdata,
			const int virtual_id,
			const AosDocType::E doc_type,
			const u64 &snap_id,
			const u64 &task_docid);

	bool rollbackSnapshot(
			const AosRundataPtr &rdata,
			const int virtual_id,
			const AosDocType::E doc_type,
			const u64 &snap_id,
			const u64 &task_docid);

	bool insertInto(
			const AosXmlTagPtr &data,
			const AosRundataPtr &rdata);

	bool addReq(
			const AosRundataPtr &rdata,
			const AosTransPtr &trans);

private:
	AosXmlTagPtr createExclusiveDoc(
				const AosXmlTagPtr &doc, 
				const AosRundataPtr &rdata);

	bool handleObjidIssue(
			OmnString &objid, 
			const AosXmlTagPtr &newdoc,  
			const bool resolveObjid,
			const AosRundataPtr &rdata);

	bool initOtherModule();

	bool appendCidToParentCtnrObjid(
			const AosXmlTagPtr &doc, 
			const OmnString &cid, 
			const AosRundataPtr &rdata);

	bool addReq(
			const AosRundataPtr &rdata,
			const AosTransPtr &trans,
			AosBuffPtr &resp);
	

	AosXmlTagPtr	createMissingContainer(
				const AosRundataPtr &rdata,
				const OmnString &container_objid, 
				const bool ispublic);

	static bool createTempDoc(const AosXmlTagPtr &doc, const AosRundataPtr &rdata);

	bool checkHomeContainer(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &doc, 
			const AosXmlTagPtr &origdoc, 
			const OmnString &target_cid); 

	bool checkContainers(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &xml, 
			const AosXmlTagPtr &docorig, 
			const OmnString &cid, 
			const bool is_public);

	bool checkModifying(
			const AosXmlTagPtr &origdoc, 
			const AosXmlTagPtr &newdoc,
			const OmnString &cid, 
			const AosRundataPtr &rdata);

	bool determinePublic(
			const AosXmlTagPtr &doc, 
			const AosXmlTagPtr &cmd,
			const OmnString &tcid,
			OmnString &objid);

	AosAccessRcdPtr getParentAcrd(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &ref_doc);

	AosAccessRcdPtr getParentAR3(
				const AosRundataPtr &rdata,
				const u64 &ref_docid,
				const OmnString &ref_objid);

	AosAccessRcdPtr getUserHomeCtnrAR(
				const AosRundataPtr &rdata,
				const OmnString &cid);

	static AosXmlTagPtr getTempDoc(const u64 &docid, const AosRundataPtr &rdata);

	AosSystemDocPtr getSystemDocByObjid(
			const OmnString &ref_objid, 
			const AosRundataPtr &rdata);

	AosSystemDocPtr getSystemDocByDocid(
			const u64 &ref_docid, 
			const AosRundataPtr &rdata);

	bool modifyObjChecked(
			const AosRundataPtr &rdata,
			const OmnString &cid,
	 		const AosXmlTagPtr &newxml,
			u64 &newdid,
			const OmnString &rename,
			const bool synobj, 
			const bool is_public);

	bool modifyParentContainers1(
			const AosRundataPtr &rdata,
			const u32 siteid, 
			const AosXmlTagPtr &doc, 
			const OmnString &pctrs_new);

	bool parseCreateReqPriv(
			const AosXmlTagPtr &trans_doc,
			AosXmlTagPtr &root,
			AosXmlTagPtr &newxml);

	bool parseModifyReqPriv(
			const AosXmlTagPtr &trans_doc,
			AosXmlTagPtr &root,
			AosXmlTagPtr &newxml,
			AosXmlTagPtr &origdoc,
			bool &synobj);

	bool parseDeleteReqPriv(
			const AosXmlTagPtr &trans_doc,
			AosXmlTagPtr &origdoc);

	bool parseModifyAttrReqPriv(
			const AosXmlTagPtr &trans_doc, 
			OmnString &attrname,
			OmnString &oldvalue,
			OmnString &newvalue,
			bool &exist, 
			bool &value_unique, 
			bool &docid_unique, 
			u64 &docid, 
			OmnString &fname, 
			int &line);

	void	setShowLog(const bool);

	u64		getCtnrDocidByChildDocid(
			AosRundata *rdata,
			const u64 &docid);

	AosDataRecordObjPtr createDataRecord(
			const AosXmlTagPtr &table_doc,
			const AosXmlTagPtr &data,
			AosRundata* rdata_raw);

	bool insertDoc(
			const AosDataRecordObjPtr &record,
			AosRundata* rdata_raw);

	bool insertIndex(
			const AosXmlTagPtr &indexs_tag,
			const AosDataRecordObjPtr &record,
			AosRundata* rdata_raw);

	bool insertCmpIndex(
			const AosXmlTagPtr &cmp_indexs_tag,
			const AosDataRecordObjPtr &record,
			AosRundata* rdata_raw);

	bool insertMap(
			const AosXmlTagPtr &maps_tag,
			const AosDataRecordObjPtr &record,
			AosRundata* rdata_raw);

	bool insertInternalStat(
			const AosXmlTagPtr &stat_tags,
			const AosDataRecordObjPtr &record,
			AosRundata* rdata_raw);

	bool insertNormalStat(
			const OmnString &db_name,
			const AosXmlTagPtr &stat_tags,
			const AosDataRecordObjPtr &record,
			AosRundata* rdata_raw);

	bool addCreateLogRequest(
			const u64 &userid,
			const AosXmlTagPtr &doc,
			const AosRundataPtr &rdata);

	bool addDeleteLogRequest(
			const AosXmlTagPtr &doc,
			const u64 userid,
			const AosRundataPtr &rdata);

};

#endif

