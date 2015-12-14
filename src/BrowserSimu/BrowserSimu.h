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
#ifndef AOS_SEARCHENGINEADMIN_BrowserSimu_h
#define AOS_SEARCHENGINEADMIN_BrowserSimu_h

#include "Util/Opr.h"
#include "SearchEngine/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/SeTypes.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/Ptrs.h"
#include "IdGen/U64IdGen.h"
#include "SEClient/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/DynArray.h"
#include "UtilComm/Ptrs.h"
#include "Util/IpAddr.h"
#include "WebServerTorturer/Ptrs.h"
#include "WebServerTorturer/WebClient.h"


class OmnFile;
OmnDefineSingletonClass(AosBrowserSimuSingleton,
						AosBrowserSimu,
						AosBrowserSimuSelf,
						OmnSingletonObjId::eBrowserSimu,
						"BrowserSimu");


class AosBrowserSimu : virtual public OmnRCObject, 
				  virtual public OmnThreadedObj
{
	OmnDefineRCObject;

	enum 
	{
		eMaxIdxFiles = 50,
		eMaxDocFiles = 50,
		eMaxVersionSeqno = 500,
		eMaxConns = 10
	};

private:
	enum
	{
		eInitTransId = 100
	};

	OmnMutexPtr		mLock;
	OmnString		mRemoteAddr;
	int				mRemotePort;
	AosSqlClientPtr	mConn;
	AosWebClientPtr	mClient;
	OmnFilePtr		mIdxFiles[eMaxIdxFiles];
	OmnFilePtr		mDocFiles[eMaxDocFiles];
	OmnString		mDocFname;
	OmnString		mIdxFname;
	u32				mTransId;
	OmnString		mDirname;
	OmnString		mTargetDirname;
	OmnString		mSiteid;
	AosDocFileMgrPtr	mXmlDocReader;
	int             mNumConns;
	OmnTcpClientPtr mConns[eMaxConns];
	bool            mConnIdle[eMaxConns];


public:
	AosBrowserSimu();
	~AosBrowserSimu();
	void	setDocFname(const OmnString &fname){mDocFname = fname;}
	void	setIdxFname(const OmnString &fname){mIdxFname = fname;}
    //
    // Singleton class interface
    //
    static AosBrowserSimu*  	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual OmnString   getSysObjName() const {return "AosBrowserSimu";}
    virtual OmnRslt     config(const OmnXmlParserPtr &def);
    virtual OmnSingletonObjId::E  
						getSysObjId() const 
						{
							return OmnSingletonObjId::eBrowserSimu;
						}

	bool		start(const AosXmlTagPtr &config);

	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool 	rebuildDb(
				const u64 &maxDocid,
				const OmnString &siteid);
	bool    showDocId(u64 did);
	bool	addOne(
			const u64 &docid,
			const OmnString &siteid,
			const OmnString &dirname,
			const OmnString &idxFname,
			const OmnString &docFname);
	bool 	rereadDocs(
				const OmnString &siteid, 
				const u64 &max_docid, 
				const OmnString &idxfname,
				const OmnString &docfname);
	AosXmlTagPtr	readDocByDocid(
				const u64 &docid, 
				const OmnString &siteid, 
				const OmnString &idxfname,
				const OmnString &docfname);

	AosXmlTagPtr queryDocByDocid(
				 const OmnString &siteid,
				 const OmnString &docid,
				 const int startidx,
				 const int queryid = -1);
	AosXmlTagPtr readDocByDocid(const u64 &docid);

	AosXmlTagPtr queryDocsByAttrs(
				 const OmnString &siteid,
				 const OmnString &attr,
				 const AosOpr opr,
				 const OmnString &value,
				 const int startidx,
				 const int queryid);

	AosXmlTagPtr queryDocsByAttrs(
				 const OmnString &siteid,
				 const OmnString &attr1,
				 const AosOpr opr1,
				 const OmnString &value1,
				 const OmnString &attr2,
				 const AosOpr opr2,
				 const OmnString &value2,
				 const int startidx,
				 const int queryid);

	AosXmlTagPtr queryDocsByTags(
				 const OmnString &siteid,
				 const OmnString &tag,
				 const int startidx,
				 const int queryid);

	AosXmlTagPtr queryDocsByContainer(
				 const OmnString &siteid,
				 const OmnString &container,
				 const int startidx,
				 const int queryid);

	AosXmlTagPtr queryDocByObjid(
				 const OmnString &siteid,
				 const OmnString &objid,
				 const int startidx,
				 const int queryid = -1);

	AosXmlTagPtr retrieveDocByObjid(
				 const OmnString &siteid, 
				 const OmnString &ssid, 
				 const OmnString &objid);

	AosXmlTagPtr retrieveDocByObjid(
				 const OmnString &siteid, 
				 const OmnString &ssid, 
				 const OmnString &objid,
				 OmnString &resp);

	AosXmlTagPtr retrieveDocByDocid(
				 const OmnString &siteid, 
				 const OmnString &ssid, 
				 const u64 &docid);
	AosXmlTagPtr retrieveDocByDocid(
				 const OmnString &siteid, 
				 const OmnString &ssid, 
				 const u64 &docid,
				 OmnString &resp);

	AosXmlTagPtr queryDocsByWords(
				 const OmnString &siteid,
				 const OmnString &words,
				 const int startidx,
				 const int queryid);
		

	AosXmlTagPtr queryDocs(
				const OmnString &siteid,
				const int startidx,
				const int psize,
				const OmnString &order,
				const OmnString &fields,
				const OmnString &query,
				const OmnString &container,
				const int queryid);
	

	AosXmlTagPtr queryDocByWords(
				 const OmnString &siteid,
				 const OmnString &words,
				 const u64 &docid);

	AosXmlTagPtr queryDocByContainer(
				 const OmnString &siteid,
				 const OmnString &container,
				 const u64 &docid);

	AosXmlTagPtr queryDocByTags(
				 const OmnString &siteid,
				 const OmnString &tags,
				 const u64 &docid);

	AosXmlTagPtr queryDocByAttrs(
				 const OmnString &siteid,
				 const OmnString &attr,
				 const AosOpr opr,
				 const OmnString &value,
				 const u64 &docid);

	AosXmlTagPtr queryDocByAttrs(
				 const OmnString &siteid,
				 const OmnString &attr1,
				 const AosOpr opr1,
				 const OmnString &value1,
				 const OmnString &attr2,
				 const AosOpr opr2,
				 const OmnString &value2,
				 const u64 &docid);

	bool 	rebuildDoc(
				const u64 &docid,
				const OmnString &siteid,
				const OmnString &idxFname, 
				const OmnString &docFname);
	AosXmlTagPtr retrieveDoc(
					const OmnString &siteid, 
					const OmnString &objid,
					const OmnString &sessionId);
	bool     retrieveDocss(
					const OmnString &siteid,
					const OmnString &ssid,
					const OmnString &objid);
	bool 	readDocByObjid(
				const OmnString &objid,
				const u64 &maxDocid,
				const OmnString &siteid,
				const bool readDeleted);
	bool 	convertParentContners(
				const u64 &maxDocid,
				const OmnString &siteid,
				const OmnString &idxFname, 
				const OmnString &docFname);
	bool	setImagePath(const OmnString &path);
	bool 	readDocsByContainer(
				const OmnString &container,
				const u64 &maxDocid,
				const OmnString &siteid,
				const OmnString &idxFname, 
				const OmnString &docFname, 
				const bool deletedFlag);
	bool 	readDoc(
				const u64 &docid,
				OmnConnBuffPtr &docbuff, 
				const bool readDeleted = false);

	bool
	retrieveObj(
		const OmnString &siteid, 
		const OmnString &ssid,
		const OmnString &username,
		const OmnString &passwd,
		const OmnString &objid, 
		AosXmlTagPtr &xmlroot);

	bool
	retrieveObjs(
		const OmnString &siteid, 
		const OmnString &ssid,
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
		const OmnString &siteid, 
		const OmnString &ssid,
		const OmnString &username,
		const OmnString &passwd,
		const OmnString &command,
		AosXmlTagPtr &xmlroot);

	bool resolveUrl(
		const OmnString &siteid,
	    const OmnString &url,
	    AosXmlTagPtr &root,
	    AosXmlTagPtr &vpd,
	    AosXmlTagPtr &obj,
		const OmnString &sessionId);

	bool resolveUrl(
		const OmnString &siteid,
	    const OmnString &url,
	    AosXmlTagPtr &root,
	    AosXmlTagPtr &vpd,
	    AosXmlTagPtr &obj,
		const OmnString &sessionId,
	    AosXmlTagPtr &loginobj);

	bool requestNewId(
		const OmnString &siteid, 
		AosXmlTagPtr &idInfo
		);
	bool sendModifyReq(
		const OmnString &siteid, 
		const char *docstr);

	bool rebuildVersions(const u32 max_seqno);
	bool rebuildLogs(const u32 max_seqno);
	AosXmlTagPtr createDoc(
		const OmnString &siteid, 
		const AosXmlTagPtr &doc,
		const OmnString &ssid, 
		const OmnString &owner_cid,
		const OmnString &resolve_objid, 
		const OmnString &saveas);

	AosXmlTagPtr readDocByDocid(const OmnString &siteid, const u64 &docid);
	bool vote(
			const int votetype,
			const OmnString &objid,
			const OmnString &mode,
			const OmnString &num,
			const OmnString &hour,
			const OmnString &siteid); 

	bool createSuperUser(
			const OmnString &passwd,
			AosXmlTagPtr &content,
			const OmnString &siteid
			);
	bool createUserCtnr(
			const OmnString &siteid, 
			const AosXmlTagPtr &doc,
			const OmnString &ssid,
			OmnString &new_objid, 
			u64 &server_docid);

	bool createAccessRcd(
			const OmnString &siteid, 
			const AosXmlTagPtr &doc,
			OmnString &objid,
			const OmnString &ssid,
			u64 &server_docid);

	bool createUserAcct(
			OmnString &objid,
			const AosXmlTagPtr &doc,
			const OmnString &ssid,
			u64 &server_docid,
			OmnString &passwd,
			OmnString &cid,
			const OmnString &siteid);

	AosXmlTagPtr getAccessRcd(
			const OmnString &siteid, 
			const u64 &docid,
			const OmnString &ssid);
		
	bool logout(const OmnString &siteid, const OmnString &ssid);
	bool addFriendResp(const OmnString &sited, const OmnString &requester, const OmnString &friendid);

	bool buildDocByObjid(
			const OmnString &objid,
			const u64 &maxDocid,
			const OmnString &siteid,
			const OmnString &idxFname, 
			const OmnString &docFname, 
			const bool readDeleted);

	bool login(
			const OmnString &siteid,
			const OmnString &username,
			const OmnString &hpvpd,
			const OmnString &login_vpdname,
			const OmnString &ctnr,
			const OmnString &passwd,
			OmnString &ssid);
	OmnTcpClientPtr		getConn();
 	bool  returnConn(const OmnTcpClientPtr &conn);
 	bool  test();


private:
	bool 	sendCreateObjReq(
				const OmnString &siteid,
				const char *doc);
	OmnFilePtr openIdxFile(const u32 seqno);
	OmnFilePtr openDocFile(const u32 seqno);
	bool 	convertDataForRebuild(const AosXmlTagPtr &doc);
	bool 	convertImageDoc(const AosXmlTagPtr &doc);
	bool 	cleanData06222010(const AosXmlTagPtr &doc);
	bool	rebuildVersion(const u32 seqno, OmnFile &file);
	bool	rebuildLog(const u32 seqno, OmnFile &file);
	bool
	sendServerReq(
		const OmnString &siteid, 
		const OmnString &reqid,
		const OmnString &args,
		const AosXmlTagPtr &obj);
};
#endif

