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
// 	Created: 05/09/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEARCHENGINEADMIN_SeAdmin_h
#define AOS_SEARCHENGINEADMIN_SeAdmin_h

#include "Rundata/Ptrs.h"
#include "DocFileMgr/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "SEClient/SEClient.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/SeTypes.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/Ptrs.h"
#include "IdGen/U64IdGen.h"
#include "SEClient/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"
#include "Util/Opr.h"
#include "Util/String.h"
#include "Util/DynArray.h"
#include "UtilComm/Ptrs.h"
#include "Util/HashUtil.h"
#include <queue>
using namespace std;


class OmnFile;
OmnDefineSingletonClass(AosSengAdminSingleton,
						AosSengAdmin,
						AosSengAdminSelf,
						OmnSingletonObjId::eSengAdmin,
						"SengAdmin");

class AosSengAdmin : virtual public OmnRCObject, 
				  virtual public OmnThreadedObj
{
	OmnDefineRCObject;

public:
	enum 
	{
		eMaxIdxFiles = 50,
		eMaxDocFiles = 50,
		eMaxDocSize = 5000000,
		eMaxVersionSeqno = 500,
		eMaxThreads = 10
	};
	
	enum
	{
		eInitTransId = 100,
		eMaxQueryCond = 20
	};

	enum QueryType
	{
		eQueryHit = 1,
		eQueryStr = 2,
		eQueryU64 = 3
	};
	
	enum DocStatus
	{
		eInvalid, 
		eBad,
		eSyntaxError,
		eDeleted,
		eGood
	};

	struct CheckRslts
	{
		int 	num_docs;
		int 	num_deleted;
		int 	num_bad;
		int 	num_syntax;
		int 	num_invalid;
		int 	num_errors;
		int 	num_bad_docids;
		int 	num_no_objids;
		int 	num_docs_deleted;
		int 	num_dup_objids;
		int 	num_dup_cids;
		int 	num_no_creators;
		AosStr2U64_t objids;
		AosStr2U64_t cloudids;
		AosU642Char_t docids;
		AosU642Str_t docid2Cid;

		CheckRslts()
		:
		num_docs(0),
		num_deleted(0),
		num_bad(0),
		num_syntax(0),
		num_invalid(0),
		num_errors(0),
		num_bad_docids(0),
		num_no_objids(0),
		num_docs_deleted(0),
		num_dup_objids(0),
		num_dup_cids(0),
		num_no_creators(0)
		{
		}

		OmnString toString() const
		{
			OmnString str;
			str << "Number of good ones:     " << num_docs << "\n";
			str << "Number of Invalid:       " << num_invalid << "\n";
			str << "Number of bad ones:      " << num_bad << "\n";
			str << "Number of deleted:       " << num_deleted << "\n";
			str << "Number of synx errors:   " << num_syntax << "\n";
			str << "Number of Errors:        " << num_errors << "\n";
			str << "Number of Bad Docids:    " << num_bad_docids << "\n";
			str << "Number of Missing Objid: " << num_no_objids << "\n";
			str << "Number of Dup Objid:     " << num_dup_objids << "\n";
			str << "Number of Dup Cloudids:  " << num_dup_cids << "\n";
			str << "Number of No Creator:  	 " << num_no_creators << "\n";
			return str;
		}

		bool cloudidExists(const OmnString &cid)
		{
			AosStr2U64Itr_t itr = cloudids.find(cid);
			return (itr != cloudids.end());
		}

		bool docidExists(const u64 &docid)
		{
			AosU642CharItr_t itr = docids.find(docid);
			return (itr != docids.end());
		}

		OmnString getCidByDocid(const u64 &docid)
		{
			AosU642StrItr_t itr = docid2Cid.find(docid);
			if (itr == docid2Cid.end()) return "";
			return itr->second;
		}

		bool objidExists(const OmnString &objid)
		{
			AosStr2U64Itr_t itr = objids.find(objid);
			return (itr != objids.end());
		}
	};

	enum ReqType
	{
		eCreateDoc,
		eCreateArcd
	};

	struct ReqStruct
	{
		ReqType         mType;
		u32				mSiteid;
		AosXmlTagPtr    mDoc;
	};

private:

	OmnMutexPtr		mLock;
	OmnString		mRemoteAddr;
	int				mRemotePort;
	AosSEClientPtr	mConn;
	static OmnFilePtr		mIdxFiles[eMaxIdxFiles];
	static OmnFilePtr		mDocFiles[eMaxDocFiles];
	OmnFilePtr		mTargetIdxFiles[eMaxIdxFiles];
	u32				mTransId;
	OmnString		mTargetDirname;
	u32				mSiteid;
	u64				mLastDocid;

	static OmnString		mDirname;
	static OmnString		mIdxFname;
	static OmnString		mDocFname;
	// static AosDocFileMgrPtr	mOrigDocReader;
	// static AosDocFileMgrPtr	mTargetDocReader;

	OmnThreadPtr        mThreads[eMaxThreads];
	queue<ReqStruct>    mRequests;
	queue<ReqStruct>    mRequests1;
	OmnCondVarPtr       mCondVar;

public:
	AosSengAdmin();
	~AosSengAdmin();
    
    // Singleton class interface
    static AosSengAdmin*  	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool 	sendDocMultiTimes(
				const u64 &docid,
				const u64 &times,
				const int version_start,
				const int version_end,
				const int log_start,
				const int log_end,
				const u32 siteid);

	bool 	rebuildDb(
				const u64 &startDocid,
				const u64 &maxDocid,
				const int version_start,
				const int version_end,
				const int log_start,
				const int log_end,
				const u32 siteid);

	bool 	checkRebuild(
				const u32 siteid, 
				const u64 &startdocid, 
				const u64 &enddocid);

	bool	checkDoc(
		        const u32 siteid,
		        const u64 &startdocid,
		        const u64 &enddocid);

	bool    showDocId(
				const u64 &did, 
				const AosRundataPtr &rdata);
	bool	showOne(
			const u64 &docid,
			const u32 siteid,
			const OmnString &dirname,
			const OmnString &idxFname,
			const OmnString &docFname);
	bool	addOne(
			const u64 &docid,
			const u32 siteid,
			const OmnString &dirname,
			const OmnString &idxFname,
			const OmnString &docFname);
	bool 	rereadDocs(
				const u32 siteid, 
				const u64 &max_docid, 
				const OmnString &idxfname,
				const OmnString &docfname);
	static AosXmlTagPtr	readDocByDocid(
				const u64 &docid, 
				const u32 siteid); 

	AosXmlTagPtr queryDocByDocid(
				 const u32 siteid,
				 const OmnString &docid,
				 const int startidx,
				 const int queryid = -1);

	AosXmlTagPtr queryDocsByAttrs(
				 const u32 siteid,
				 const OmnString &attr,
				 const AosOpr opr,
				 const OmnString &value,
				 const int startidx,
				 const int queryid);


	AosXmlTagPtr queryDocsByAttrs(
				 const u32 siteid,
				 const OmnString &attr1,
				 const AosOpr opr1,
				 const OmnString &value1,
				 const OmnString &attr2,
				 const AosOpr opr2,
				 const OmnString &value2,
				 const int startidx,
				 const int queryid);

	AosXmlTagPtr queryDocsByAttrs(
				 const u32 siteid,
				 const OmnString &attr1,
				 const AosOpr opr1,
				 const OmnString &value1,
				 const OmnString &attr2,
				 const AosOpr opr2,
				 const OmnString &value2,
				 const OmnString &attr3,
				 const AosOpr opr3,
				 const OmnString &value3,
				 const int startidx,
				 const int queryid);

	AosXmlTagPtr queryDocsByAttrs(
				 const u32 siteid,
				 const int &numconds, 
				 const AosSengAdmin::QueryType *types,
				 const OmnString *attrs, 
				 const AosOpr *oprs,
				 const OmnString *values,
				 const bool *orders,
				 const bool *reverses,
				 const int psize,
				 const bool reverse,
				 const int startidx,
				 const int queryid);

	AosXmlTagPtr queryDocsByTags(
				 const u32 siteid,
				 const OmnString &tag,
				 const int startidx,
				 const int queryid);

	AosXmlTagPtr queryDocsByContainer(
				 const u32 siteid,
				 const OmnString &container,
				 const int startidx,
				 const int queryid);

	AosXmlTagPtr queryDocByObjid(
				 const u32 siteid,
				 const OmnString &objid,
				 const int startidx,
				 const int queryid = -1);

	AosXmlTagPtr retrieveDocByObjid(
				 const u32 siteid, 
				 const OmnString &ssid, 
				 const u64 &urldocid, 
				 const OmnString &objid,
				 const bool needLock = true);

	AosXmlTagPtr retrieveDocByObjid(
				 const u32 siteid, 
				 const OmnString &ssid, 
				 const u64 &urldocid, 
				 const OmnString &objid,
				 OmnString &resp,
				 const bool needLock = true);

	AosXmlTagPtr retrieveDocByDocid(
				 const u32 siteid, 
				 const OmnString &ssid, 
				 const u64 &urldocid, 
				 const u64 &docid,
				 const bool if_error = false);

			bool retrieveDocByDocid(
				 const u32 siteid,
				 const u64 &docid);

	AosXmlTagPtr retrieveDocByDocid(
				 const u32 siteid, 
				 const OmnString &ssid, 
				 const u64 &urldocid, 
				 const u64 &docid,
				 OmnString &resp);

	AosXmlTagPtr retrieveDocByCloudid(
				 const u32 siteid, 
				 const OmnString &cid, 
				 const u64 &urldoc_docid); 

	AosXmlTagPtr queryDocsByWords(
				 const u32 siteid,
				 const OmnString &words,
				 const int startidx,
				 const int queryid);
		
	AosXmlTagPtr	queryDocs(
					const u32 siteid, 
					const int startidx,
					const int psize,
					const OmnString &query,
					const OmnString &fields,
					const int queryid);

	AosXmlTagPtr queryDocs(
				const u32 siteid,
				const int startidx,
				const int psize,
				const OmnString &order,
				const OmnString &fields,
				const OmnString &query,
				const OmnString &container,
				const int queryid);

	AosXmlTagPtr queryDocByWords(
				 const u32 siteid,
				 const OmnString &words,
				 const u64 &docid);

	AosXmlTagPtr queryDocByContainer(
				 const u32 siteid,
				 const OmnString &container,
				 const u64 &docid);

	AosXmlTagPtr queryDocByTags(
				 const u32 siteid,
				 const OmnString &tags,
				 const u64 &docid);

	AosXmlTagPtr queryDocByAttrs(
				 const u32 siteid,
				 const OmnString &attr,
				 const AosOpr opr,
				 const OmnString &value,
				 const u64 &docid);

	AosXmlTagPtr queryDocByAttrs(
				 const u32 siteid,
				 const OmnString &attr1,
				 const AosOpr opr1,
				 const OmnString &value1,
				 const OmnString &attr2,
				 const AosOpr opr2,
				 const OmnString &value2,
				 const u64 &docid);

	AosXmlTagPtr queryDocByAttrs(
				 const u32 siteid,
				 const OmnString &attr1,
				 const AosOpr opr1,
				 const OmnString &value1,
				 const OmnString &attr2,
				 const AosOpr opr2,
				 const OmnString &value2,
				 const OmnString &attr3,
				 const AosOpr opr3,
				 const OmnString &value3,
				 const u64 &docid);

	bool 	rebuildDoc(
				const u64 &docid,
				const u32 siteid,
				const OmnString &idxFname, 
				const OmnString &docFname);
				
	AosXmlTagPtr retrieveDoc(
					const u32 siteid, 
					const OmnString &objid,
					const OmnString &ssid,
					const u64 &urldoc_docid,
					const OmnString &isInEditor,
					const AosXmlTagPtr &cookies);

	static bool readDocByObjid(
					const OmnString &objid,
					const u64 &maxDocid,
					const u32 siteid,
					const bool readDeleted);

	bool 	convertParentContners(
				const u64 &maxDocid,
				const u32 siteid,
				const OmnString &idxFname, 
				const OmnString &docFname);

	bool	setImagePath(const OmnString &path);

	bool 	readDocsByContainer(
				const OmnString &container,
				const u64 &maxDocid,
				const u32 siteid,
				const OmnString &idxFname, 
				const OmnString &docFname, 
				const bool deletedFlag);

	static bool 	readDoc(
				const u64 &docid,
				OmnConnBuffPtr &docbuff, 
				const bool readDeleted, 
				DocStatus &status, 
				const AosRundataPtr &rdata); 

	AosXmlTagPtr readArcd(
				const u64 &docid, 
				const AosRundataPtr &rdata);

	bool	readTargetDoc(
			    const u64 &docid,
			    OmnConnBuffPtr &docbuff,
			    const bool readDeleted = false);

	bool
	retrieveObj(
		const u32 siteid, 
		const OmnString &ssid,
		const u64 &urldocid,
		const OmnString &username,
		const OmnString &passwd,
		const OmnString &objid, 
		AosXmlTagPtr &xmlroot);

	bool
	retrieveObjs(
		const u32 siteid, 
		const OmnString &ssid,
		const u64 &urldocid,
		const OmnString &username,
		const OmnString &passwd,
		const int start_idx,
		const bool reverse,
		const int pagesize,
		const OmnString &fnames,
		const OmnString &order_fname,
		const OmnString &query, 
		const bool getTotal,
		AosXmlTagPtr &xmlroot);

	bool
	retrieveObjs(
		const u32 siteid, 
		const OmnString &ssid,
		const u64 &urldocid,
		const OmnString &username,
		const OmnString &passwd,
		const OmnString &command,
		AosXmlTagPtr &xmlroot);

	bool resolveUrl1(
		const u32 siteid,
	    const OmnString &url,
		const OmnString &full_url,
		const OmnString &query_str,
	    AosXmlTagPtr &root,
	    AosXmlTagPtr &vpd,
	    AosXmlTagPtr &obj,
		const OmnString &ssid,
		u64 &urldocid,
		const AosXmlTagPtr &cookies, 
		const bool isTablet);

	bool resolveUrl1(
		const u32 siteid,
	    const OmnString &url,
		const OmnString &full_url,
		const OmnString &query_str,
	    AosXmlTagPtr &root,
	    AosXmlTagPtr &vpd,
	    AosXmlTagPtr &obj,
		const OmnString &ssid,
		u64 &urldocid,
		const AosXmlTagPtr &cookies,
	    AosXmlTagPtr &loginobj, 
		const bool isTablet);

	// Chen Ding, 02/10/2012. Do not use it anymore. Use AosSeIdGenMgr::nextCompId(...)
	// bool requestNewId(
	// 	const u32 siteid, 
	// 	AosXmlTagPtr &idInfo);

	bool sendModifyReq(
		const u32 siteid, 
		const char *docstr);

	bool sendModifyReq(
		const u32 siteid, 
		const OmnString &ssid, 
		const u64 &urldocid,
		const OmnString &docstr, 
		const AosRundataPtr &rdata,
		const bool check_error = true);

	bool rebuildVersions(const int version_start, const int version_end);
	bool rebuildLogs(const int log_start, const int log_end);
	AosXmlTagPtr createDoc(
		const AosXmlTagPtr &doc,
		const u32 siteid,
		const OmnString &ssid, 
		const u64 &urldocid,
		const OmnString &owner_cid,
		const OmnString &resolve_objid, 
		const OmnString &saveas,
		const AosRundataPtr &rdata);

	AosXmlTagPtr createDoc(
		const AosXmlTagPtr &doc,
		const OmnString &ssid, 
		const u64 &urldocid,
		const OmnString &owner_cid,
		const OmnString &resolve_objid, 
		const OmnString &saveas,
		const AosRundataPtr &rdata);

	// Chen Ding, 2011/01/29
	// AosXmlTagPtr readDocByDocid(const u32 siteid, const u64 &docid);
	AosXmlTagPtr queryAndRetrieveByDocid(const u32 siteid, const u64 &docid);
	bool vote(
			const int votetype,
			const OmnString &objid,
			const OmnString &mode,
			const OmnString &num,
			const OmnString &hour,
			const u32 siteid); 

	bool createSuperUser(
			const OmnString &passwd,
			AosXmlTagPtr &content,
			const u32 siteid);

	AosXmlTagPtr createUserCtnr(
			const u32 siteid, 
			const AosXmlTagPtr &doc,
			const OmnString &ssid,
			const u64 &urldocid);

	AosXmlTagPtr createMsgCtnr(
			const u32 siteid,
			const AosXmlTagPtr &doc,
			const OmnString &ssid,
			const u64 &urldocid
			);
	AosXmlTagPtr createInbox(
			const u32 siteid, 
			const AosXmlTagPtr &doc,
			const OmnString &cloudid,
			const OmnString &ssid,
			const u64 &urldocid
			);
	AosXmlTagPtr moveMsg(
			const u32 siteid, 
			const OmnString &objid,
			const OmnString &containerid,
			const OmnString &ssid,
			const u64 &urldocid
			);
	AosXmlTagPtr createUserDomain(
			const AosXmlTagPtr &doc, 
			const OmnString &ssid, 
			const u64 &urldocid,
			const u32 siteid, 
			const AosRundataPtr &rdata);

	bool	 ModifyAccessRcd(
			const u32 siteid, 
			const AosXmlTagPtr &doc,
			const u64 &owndocid,
			const OmnString &ssid,
			const u64 &urldocid
			);

	AosXmlTagPtr createUserAcct(
			const AosXmlTagPtr &doc,
			const OmnString &ssid,
			const u64 &urldocid,
			const u32 siteid
			);

	bool  login(
			const OmnString &username,
			const OmnString &passwd,
			const OmnString &ctnr_name,
			OmnString &ssid,
			u64 &urldocid,
			u64 &userid,
			AosXmlTagPtr &userdoc,
			const u32 siteid,
			const OmnString &cid = "",
			const OmnString &rootssid = "");

	AosXmlTagPtr getAccessRcd(
			const u32 siteid, 
			const u64 &docid,
			const OmnString &ssid, 
			const u64 &urldocid,
			const bool create_flag, 
			const bool get_parent
			);
		
	bool logout(const u32 siteid, 
			const OmnString &ssid, 
			const u64 &urldocid,
			const AosRundataPtr &rdata);
	bool addFriendResp(
				const u32 siteid, 
				const OmnString &requester, 
				const OmnString &friendid);

	bool buildDocByObjid(
			const OmnString &objid,
			const u64 &maxDocid,
			const u32 siteid,
			const OmnString &idxFname, 
			const OmnString &docFname, 
			const bool readDeleted);

	bool tortureGetObjid(
			const u32 siteid, 
			const OmnString &tested_objid,
			const u64 &startdocid, 
			const u64 &enddocid);
	bool checkQuery(
			const u32 siteid, 
			const u64 &startdocid, 
			const u64 &enddocid,
			const u64 &max_num);
	bool query(
		    const AosXmlTagPtr &doc,
		    const u32 siteid,
		    const OmnString &ssid,
			const u64 &urldocid,
		    const u64 &docid);

	AosXmlTagPtr QueryEq(
			const OmnString &otype,
			const OmnString &ctime,
			const OmnString &creator,
			const u64 &docid);
	AosXmlTagPtr QueryLe(
			const OmnString &otype,
			const OmnString &ctime,
			const u64 &docid);
	AosXmlTagPtr QueryGe(
			const OmnString &otype,
			const OmnString &ctime,
			const u64 &docid);
	AosXmlTagPtr QueryNe(
			const OmnString &otype,
			const OmnString &ctime,
			const u64 &docid);
	AosXmlTagPtr retrieveVersionByObjid(
			const u32 siteid, 
			const OmnString &ssid, 
			const u64 &urldocid,
			const OmnString &objid, 
			const OmnString &version);
	bool	checkVersion(
			const u32 siteid, 
			const u64 &startdocid,
			const u64 &enddocid, 
			const u32 num_tries);
	bool	sendDeleteReq(
			const u32 &siteid,
		    const OmnString &ssid,
			const u64 &urldocid,
		    const OmnString &docid);
	bool	removeDocFromServer(
			const u32 siteid,
		    const OmnString &ssid,
			const u64 &urldocid,
			const OmnString &objid,
		    const u64 &docid);
	bool	sendDeleteMsgReq(
			const u32 siteid,
		    const OmnString &ssid,
			const u64 &urldocid,
		    const OmnString &objid,
			const OmnString &container);
	bool	sendDeleteMsgContainerReq(
			const u32 siteid,
		    const OmnString &ssid,
			const u64 &urldocid,
		    const OmnString &cname,
			const OmnString &container);
	bool	getDomain(OmnString &domain);
	bool 	sendCreateObjReq(
				const u32 siteid,
				const char *doc);
	bool	sendCreateObjReq2(const u32 siteid, 
							  const char *docstr,
							  const OmnString &ssid,
							  const u64 &urldocid
							  );
	u64		sendCreateObjReq2(const u32 siteid, 
							  const AosXmlTagPtr &doc,
							  const OmnString &ssid,
							  const u64 &urldocid
							  );
private:
	//Zky2675 Ketty 2011/02/14
	bool	sendDeleteObjReq(
				const u32 siteid,
				const OmnString &objid,
				const u64 &docid, 
				const u64 &urldocid);
	static OmnFilePtr openIdxFile(const u32 seqno);
	static OmnFilePtr openDocFile(const u32 seqno);
	OmnFilePtr openTargetIdxFile(const u32 seqno);
	bool 	convertDataForRebuild(const AosXmlTagPtr &doc);
	bool 	convertImageDoc(const AosXmlTagPtr &doc);
	bool 	cleanData(const AosXmlTagPtr &doc);
	bool	rebuildVersion(const u32 seqno, OmnFile &file);
	bool	rebuildLog(const u32 seqno, OmnFile &file);
	bool
	sendServerReq(
		const u32 siteid, 
		const OmnString &reqid,
		const OmnString &args,
		const AosXmlTagPtr &obj);
	static bool readHeaderVersion201012(
			const u64 &docid,
			u32 &seqno, 
			u64 &offset, 
			u64 &docsize, 
			DocStatus &status); 
	static bool readHeaderVersion201102(
			const u64 &docid,
			u32 &seqno, 
			u64 &offset, 
			u64 &docsize, 
			u64 &compressed_size,
			u32 &arcd_seqno, 
			u64 &arcd_offset, 
			u64 &arcd_size, 
			DocStatus &status, 
			const AosRundataPtr &rdata); 

public:
	static bool deleteDocVersion201012(const u64 &docid);
	static bool deleteDocVersion201102(
			const u64 &docid, 
			const AosRundataPtr &rdata);
	OmnString 	
	generateHtml(const OmnString &vpdname,
				 const OmnString &ssid,  
				 const u64 &urldocid,
				 bool fullPage = false); 
	bool 
	checkHtmlGenerate(
		const u32 siteid, 
		const u64 &startdocid,
		const u64 &enddocid);
	static void setDirname(const OmnString &dirname) {mDirname = dirname;}
	static void setIdxFname(const OmnString &f) {mIdxFname = f;}
	static void	setDocFname(const OmnString &fname){mDocFname = fname;}
	bool scanDocs(
			const u64 &startDocid,
			const u64 &maxdocid,
			const u32 siteid);
	bool scanDocs1(
			const u64 &startDocid,
			const u64 &maxdocid,
			const u32 siteid);
	bool modifyIILTypeAttr(OmnString &str);
	bool checkDoc(const u32 siteid, 
			const u64 &docid, 
			const OmnConnBuffPtr &buff,
			bool &delFlag,
			AosSengAdmin::CheckRslts &rslts,
			const OmnString &ssid,
			const u64 &urldocid);
	bool verifyUserAct(
			const u32 siteid,
			const u64 &docid, 
			const AosXmlTagPtr &doc,
			bool &delFlag);
	bool	verifyArcd(
			const u32 siteid,
			const u64 &docid,
			const AosXmlTagPtr &doc, 
			bool &delFlag);
	bool verifyDocid(
			const u32 siteid,
			const u64 &docid,
			const AosXmlTagPtr &doc,
			bool &delFlag,
			AosSengAdmin::CheckRslts &rslts);
	bool verifyCreator(
			const u32 siteid,
			const u64 &docid,
			const AosXmlTagPtr &doc,
			bool &delFlag,
			AosSengAdmin::CheckRslts &rslts,
			const OmnString &ssid,
			const u64 &urldocid
			);
	bool verifyMutiCtnr(
			const u32 siteid,
			const u64 &docid,
			const AosXmlTagPtr &doc,
			bool &delFlag); 
	bool verifyObjid(
			const u32 siteid,
			const u64 &docid,
			const AosXmlTagPtr &doc,
			bool &delFlag,
			AosSengAdmin::CheckRslts &rslts);
	bool verifyCloudid(
			const u32 siteid,
			const u64 &docid,
			const AosXmlTagPtr &doc,
			bool &delFlag,
			AosSengAdmin::CheckRslts &rslts);
	bool removeEmbeddedImg(
			AosXmlTagPtr &doc, 
			bool &delFlag, 
			const AosRundataPtr &rdata);
	static bool saveFileVersion201012(
			const AosXmlTagPtr &doc, 
			const AosRundataPtr &rdata);
	bool incrementalRebuild(
			const u32 siteid, 
			const u64 start_docid,
			const u64 max_docid,
			const int version_start, 
			const int version_end, 
			const int log_start, 
			const int log_end);
	OmnString retrieveCloudid(
			const u32 siteid, 
			const OmnString &ssid,
			const u64 &urldocid,
			const OmnString &userid);
	bool fixCreatorProblem(const u32 siteid, const u64 &start, const u64 &end);
	bool modifyAttr(
			const u32 siteid, 
			const OmnString &ssid,
			const u64 &urldocid,
			const OmnString &objid, 
			const OmnString &aname,
			const OmnString &value,
			const OmnString &dft,
			const OmnString &value_unique, 
			const OmnString &docid_unique);
	bool isValidCloudid(
			const u32 siteid, 
			const OmnString &ssid,
			const u64 &urldocid,
			const OmnString &cid);

	 bool getLoginObj(
		    const u32 siteid,
		    const OmnString &ssid,
			const u64 &urldocid,
		    AosXmlTagPtr &loginobj);
	 bool convertSystemDocs(
		    const u64 &startdocid,
		    const u64 &enddocid,
		    const char *fname,
		    const u32 siteid,
			const AosRundataPtr &rdata);
	bool
		changeCreator(
		const u64 &start_docid, 
		const u64 &end_docid,
		const OmnString &value,
		const u32 siteid,
		const AosRundataPtr &rdata);

	bool addFollower(
			const OmnString &ssid,
			const u64 &urldocid,
			const u32 siteid,
			const OmnString &frined_cid);

	bool removeFollower(
			const OmnString &ssid,
			const u64 &urldocid,
			const u32 siteid,
			const OmnString &frined_cid);

	bool addMicroBlogMsg(
			const OmnString &ssid,
			const u64 &urldocid,
			const u32 siteid,
			const OmnString &msg);

	 // Ketty 05/25/2011
	bool	checkCloudid(
				const u32 siteid,
				const OmnString &cloudid,
				bool	&exist);
	 
	// Tank 05/25/2011
	bool	manualOrder(
				const u32 siteid,
				const OmnString &reqid,
				const OmnString &args,
				OmnString &rslt); 

	bool verifyParentCtnr(
			const u32 siteid,
			const u64 &docid,
			const AosXmlTagPtr &doc,
			bool &delFlag,
			const OmnString &ssid,
			const u64 &urldocid);
	bool sendInMsg(
				const OmnString &ssid, 
				const u64 &urldocid,
				const u32 siteid, 
				const OmnString &friend_cid, 
				const OmnString &msg);

	bool addFriend(
				const OmnString &ssid, 
				const u64 &urldocid,
				const u32 siteid, 
				const OmnString &ctn_name, 
				const OmnString &friend_cid);

	bool inviteFriend(
				const OmnString &ssid, 
				const u64 &urldocid,
				const u32 siteid, 
				const OmnString &ctn_name, 
				const OmnString &friend_cid);

	bool denyFriend(
				const OmnString &ssid, 
				const u64 &urldocid,
				const u32 siteid, 
				const OmnString &ctn_name, 
				const OmnString &friend_cid);

	bool removeFriend(
				const OmnString &ssid, 
				const u64 &urldocid,
				const u32 siteid, 
				const OmnString &ctn_name, 
				const OmnString &friend_cid);
	bool checkLogin(
				const u32 siteid,
				const OmnString &cid,
				const OmnString &ssid,
				const u64 &urldocid);
	// Chen Ding, 08/07/2011
	bool convertVersionIILs();

	bool convertQuery(const AosXmlTagPtr &term, const AosRundataPtr &rdata);

	bool convertDatacolQuery(
				const AosXmlTagPtr &datacol_tag,
				const AosRundataPtr &rdata);

	bool convertOneCond(
		const OmnString &cond_str, 
		const AosXmlTagPtr &conds_tag, 
		const AosRundataPtr &rdata);

	bool convertFnames(
		const AosXmlTagPtr &datacol_tag,
		const AosRundataPtr &rdata);
	
	bool sendDocByContainerToRemServer(
			  const OmnString &ctnr,
			  const u64 &startdocid,
			  const u64 & enddocid,
			  const u32 siteid);
	bool getLanguageType(
			const u32 siteid, 
			const OmnString &ssid, 
			const u64 &urldocid,
			const OmnString &sendltype, 
			OmnString &receiveltype);

	void convertCtnrNamesInit();
	bool convertCtnrNames(const AosXmlTagPtr &doc);
	AosXmlTagPtr createLog(
			const u32 siteid,
			const OmnString &logname,
			const OmnString &ctnr_objid,
			const OmnString &contents);
	AosXmlTagPtr retrieveLog(
			const u32 siteid,
			const u64 &logid);
	bool addVersion(
			const AosXmlTagPtr &doc,
			const u32 siteid);
	bool sendShortMsg(
			const OmnString &msg,
			const OmnString &reveiver,
			const u32 siteid);
	bool addReq(const ReqType type,
			const u32 siteid,
			const AosXmlTagPtr &doc);

	bool modifyDocTest(
			const int &tries, 
			const OmnString &objid, 
			const u32 siteid);

	// Ketty 2011/11/28
	static AosDocFileMgrObjPtr getDocFileMgr(
			const u64 &docid, 
			const AosRundataPtr &rdata);

	bool scanParents(
			const u64 &startDocid,
			const u64 &maxDocid, 
			const u32 siteid);
	bool checkParent(
			const u32 siteid,
			const u64 &docid,
			const OmnConnBuffPtr &docbuff, 
			bool &delFlag,
			AosSengAdmin::CheckRslts &rslts,
			const OmnString &ssid,
			const u64 &urldocid);

	bool collectDataInfo(
			const u64 &startDocid,
			const u64 &endDocid,
			const u64 &startIILid,
			const u64 &endIILid);

	bool readIILHeaderBuff(
			const u64 &iilid, 
			AosBuff &buff,
			const AosRundataPtr &rdata);
	

	AosXmlTagPtr	sendDocLockReq(
			const u32 siteid, 
			const OmnString &ssid, 
			const u64 &urldocid,
			const OmnString &docstr);
	
	bool				backup(
						const u32 &siteid,
						const AosXmlTagPtr &data);
private:
	AosXmlTagPtr    	readDoc(
						const u64 &docid,
						const AosRundataPtr &rdata);
	
	static AosDocFileMgrObjPtr	getDocFileMgrNew(
        				const u64 &docid,
        				const AosRundataPtr &rdata);

	static AosDocFileMgrObjPtr   getDocFileMgrByVid(
						const int vid, 
						const AosRundataPtr &rdata); 
	
	u64					parseDocid(
        				const AosRundataPtr &rdata,
        				const u64 &docid);
};
#endif

