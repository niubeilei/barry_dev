////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 09/26/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SearchEngine_DocServer_h
#define AOS_SearchEngine_DocServer_h

#include "ValueSel/ValueRslt.h"
#include "IdGen/Ptrs.h"
#include "IdGen/U64IdGen.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/SeTypes.h"
#include "SearchEngine/Ptrs.h"
#include "Security/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"


#ifndef AOSDOCSVRLOG_DEFINE
#define AOSDOCSVRLOG_DEFINE OmnString _local_log; _local_log << __FILE__ << ":" << __LINE__
#endif

#ifndef AOSDOCSVRLOG
#define AOSDOCSVRLOG _local_log << ":" << __LINE__ << ":"
#endif

#ifndef AOSDOCSVRLOG_CLOSE
#define AOSDOCSVRLOG_CLOSE(x, errmsg) _local_log << ":" << __LINE__ << "::"; AosDocServer::createTempDoc((x), _local_log, (errmsg))
#endif

#ifndef TTLMINUS
#define TTLMINUS (rdata->checkTTL(__FILE__, __LINE__ ,__func__))
#endif

OmnDefineSingletonClass(AosDocServerSingleton,
						AosDocServer,
						AosDocServerSelf,
						OmnSingletonObjId::eDocServer,
						"DocServer");

class AosXmlDoc;

class AosDocServer : virtual public OmnRCObject
{
	OmnDefineRCObject;
	friend class AosSeSysLog;
	friend class AosDocMgr;

public:
	enum
	{
		eMaxObjidTries = 100,
		eMaxCtnrs = 30,
		eDftMaxObjidTries = 500,
		eWordBuffSize = 1023,
		eMaxTags = 100,
		eMaxDocIdMem = 30000000,		// 30M

		eWordFlag_Normal = 1,
		eWordFlag_Tag = 2,
		eWordFlag_Ancestor = 4,
		eDocidsToTry = 5,
		eMaxTTL = 10,
		eMaxSortedLists = 20
	};

	enum ServerStatus
	{
		eNormal,
		eRepairing
	};


private:
	OmnMutexPtr			mLock;
	OmnCondVarPtr		mCondVar;
	OmnThreadPtr		mThread;
	AosU64IdGenPtr		mDocIdGen;
	OmnString			mDocIdGenName;
	u64					mMaxFilesize;
	u32					mMaxDocHeaderFiles;
	u32					mMaxIILHeaderFiles;
	u32					mMaxIILFiles;
//	bool				mIsStopping;
//	bool				mIsRepairing;
	int					mMaxObjidTries;
	AosLockMonitorPtr	mLockMonitor;
	AosRundataPtr		mRundata;			// Chen Ding, 2011/05/04

	static bool			mShowLog;

	AosDocServer();
	~AosDocServer();

public:
    // Singleton class interface
    static AosDocServer *    	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

//	const char *normalizeWord(char *word, int &wordLen);
//	u64			checkWord(const OmnString &word, bool &isIgnored);
//	const char *buffWord(const char *word, const u32 wordLen);
//	void		setShowLog(const bool);
	//static void	exitSearchEngine();
//	void		setRepairing(const bool f) {mIsRepairing = f;}
//	bool		isRepairing() const {return mIsRepairing;}
//	bool		isCloudidBound(const OmnString &cid, u64 &userid, const AosRundataPtr &rdata);
//	OmnString 	getCloudid(const u64 &user_docid, const AosRundataPtr &rdata);

//	u64 doesObjidExist(
//			const OmnString &siteid, 
//			const OmnString &objid, 
//			const AosRundataPtr &rdata);

//	bool determinePublic(
//			const AosXmlTagPtr &doc, 
//			const AosXmlTagPtr &cmd, 
//			const OmnString& cid,
//			OmnString &objid);

//	bool checkModifying(
//			const AosXmlTagPtr &origdoc, 
//			const AosXmlTagPtr &newdoc,
//			const OmnString &cid, 
//			const AosRundataPtr &rdata);

//	AosXmlTagPtr getUserHomeCtnrAR(
//			const AosRundataPtr &rdata,
//			const OmnString &user_homectnr_objid); 

//	bool bindCloudid(
//			const OmnString &cid, 
//			const u64 &docid, 
//			const AosRundataPtr &rdata);

//	static void createTempDoc(
//			const AosXmlTagPtr &doc,
//			const OmnString &log, 
//			OmnString &errmsg);

//	AosXmlTagPtr getTempDoc(
//			const OmnString &siteid,
//			const OmnString &objid);

//	bool bindObjid(
//			OmnString &objid, 
//			u64 &docid,
//			bool &objid_changed, 
//			const bool resolve,
//			const bool keepDocid,
//			const AosRundataPtr &rdata);

//	static bool	checkCtnrMember1(
//			const OmnString &event,
//			const AosRundataPtr &rdata);

//	static bool	showLog() {return mShowLog;}

//	bool addContainerMember(
//				const AosRundataPtr &rdata,
//				const OmnString &ctnr_objid,
//				const AosXmlTagPtr &doc,
//				const OmnString &objid,
//				const u64 &docid);

//	bool removeContainerMember(
//				const OmnString &ctnr_objid,
///				const AosXmlTagPtr &doc,
//				const u64 &docid, 
//				const AosRundataPtr &rdata);

//	static AosXmlTagPtr getTempDoc(const u32 seqno, const u64 &offset);

//	u64	getDocidByCloudid(
//				const OmnString &siteid,
//				const OmnString &cid,
//				const AosRundataPtr &rdata);

//	AosXmlTagPtr	getRawDoc(
//				const OmnString &siteid, 
//				const OmnString &objid, 
//				const bool deletedOnly, 
//			const AosRundataPtr &rdata) ;
private:
//	static	bool saveDocASC(
//				u32 &seqno, 
//				u64 &offset, 
//				u64 &docsize, 
//				const OmnString &str);

};
#endif
