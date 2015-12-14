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

#if 0
#include "ActOpr/ValueRslt.h"
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
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DocSvrCbObj.h"


#ifndef AOSDOCSVRLOG_DEFINE
#define AOSDOCSVRLOG_DEFINE OmnString _local_log; _local_log << __FILE__ << ":" << __LINE__
#endif

#ifndef AOSDOCSVRLOG
#define AOSDOCSVRLOG _local_log << ":" << __LINE__ << ":"
#endif

#ifndef AOSDOCSVRLOG_CLOSE
#define AOSDOCSVRLOG_CLOSE(x, errmsg) _local_log << ":" << __LINE__ << "::"; createTempDoc((x), _local_log, (errmsg))
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
	bool				mIsStopping;
	bool				mIsRepairing;
	int					mMaxObjidTries;
	u64					mRepairingMaxDocid;		// Chen Ding, 10/11/2010
	AosLockMonitorPtr	mLockMonitor;
	bool				mIsLocal;

	static bool			mShowLog;
	static int			mNumModifies;
	static int			mTotalDeleted;

	AosDocServer();
	~AosDocServer();

public:
    // Singleton class interface
    static AosDocServer *    	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual OmnString   getSysObjName() const {return "AosDocServer";}
    virtual OmnSingletonObjId::E getSysObjId() const 
			{
				return OmnSingletonObjId::eDocServer;
			}
    virtual OmnRslt     config(const OmnXmlParserPtr &def);
	bool 	start(const AosXmlTagPtr &config);

	const char *normalizeWord(char *word, int &wordLen);
	u64			checkWord(const OmnString &word, bool &isIgnored);
	const char *buffWord(const char *word, const u32 wordLen);
	void		setShowLog(const bool);
	static void	exitSearchEngine();
	void		setRepairing(const bool f) {mIsRepairing = f;}
	void		setLocal(const bool local) {mIsLocal = local;}
	bool		isRepairing() const {return mIsRepairing;}
	//bool		isCloudidBound(const OmnString &cid, u64 &userid, const AosRundataPtr &rdata);
	u64			getRepairingMaxDocid() const {return mRepairingMaxDocid;}
	bool 		setDoc( const AosXmlTagPtr &doc, AosXmlDocPtr &header);
	OmnString 	getCloudid(const u64 &user_docid, const AosRundataPtr &rdata);

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

	bool createDoc1(	
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &cmd,
			const OmnString &target_cid,
			const AosXmlTagPtr &doc,
			const bool resolveObjid,
			const AosDocSvrCbObjPtr &caller, 
			void *userdata,
			const bool keepDocid); 

	bool deleteDocSafe(
			const OmnString &siteid,
			const OmnString &appname, 
			const u64 &userid,
			const AosXmlTagPtr &docroot);

	bool deleteObj(
			const AosRundataPtr &rdata,
			const OmnString &docid,
		    const OmnString &theobjid, 
			const OmnString &container, 
			const bool deleteFlag);

	bool modifyAttrStr1(
			const AosRundataPtr &rdata,
			const u64 &docid,
			const OmnString &objid,
			const OmnString &aname,
			const OmnString &newvalue,
			const OmnString &dft,
			const bool value_unique,
			const bool docid_unique,
			const OmnString &fname,
			const int line);

	bool modifyAttrU64(
			const AosRundataPtr &rdata,
			const u64 &docid,
			const OmnString &objid,
			const OmnString &aname,
			const u64 &newvalue,
			const u64 &dft,
			const bool value_unique,
			const bool docid_unique);

	bool modifyObj(
		    const AosRundataPtr &rdata,
		    const AosXmlTagPtr &newxml,
		    const OmnString &rename,
		    const bool synobj);

	u64	getCrtMaxDocid() const 
		{
			if (!mDocIdGen) return 0;
			return mDocIdGen->getCrtMaxId();
		}

	AosXmlTagPtr getDoc(
			const AosRundataPtr &rdata,
			const OmnString &docid,
			const OmnString &objid,
			bool &duplicated);

	u64 doesObjidExist(
			const OmnString &siteid, 
			const OmnString &objid, 
			const AosRundataPtr &rdata);

	bool isTagAttr(const OmnString &name)
			{
				if (name.length() != 7) return false;
				char *data = (char *)name.data();
				return (data[0] == 'z' &&
					data[1] == 'k' &&
					data[1] == 'y' &&
					data[1] == '_' &&
					data[1] == 't' &&
					data[1] == 'a' &&
					data[1] == 'g');
			}

	AosXmlTagPtr cloneDoc(
			const AosRundataPtr &rdata,
			const OmnString &cloudid,
			const OmnString &fromobjid,
			const OmnString &toobjid,
			const bool is_public);

	bool createObjByTemplate(
			const AosRundataPtr &rdata,
			const OmnString &cid,
			const AosXmlTagPtr &newxml,
			const AosXmlTagPtr &origdoc,
			const bool is_public);

	OmnString incrementValue(
			const u64 &docid, 
			const OmnString &objid,
			const OmnString &aname, 
			const OmnString &initvalue, 
			const bool value_unique,
			const bool docid_unique, 
			const OmnString &fname, 
			const int line, 
			const AosRundataPtr &rdata);

	AosXmlTagPtr getRawDoc(
			const OmnString &siteid, 
			const OmnString &objid, 
			const bool deletedOnly, 
			const AosRundataPtr &rdata);

//	u64 getDocidByCloudid(
//			const OmnString &siteid, 
//			const OmnString &cid, 
//			const AosRundataPtr &rdata);

	bool createDocPriv(
			const u64 &userid,
			const OmnString &docstr, 
			const bool needLock);

	bool createDocPriv1(
			const u64 &userid,
			const AosXmlTagPtr &doc, 
			const bool needLock);

	bool modifyDocPriv(
			const AosXmlTagPtr &origdoc,
			const AosXmlTagPtr &newdoc, 
			const u64 &uerid);

	AosXmlTagPtr resolveDuplicatedObjid(
			const OmnString &siteid, 
			const u64 &docid,
			bool &notFound);

	OmnString resolveObjidPriv(
			const OmnString &siteid,
			const OmnString &objid);

	bool determinePublic(
			const AosXmlTagPtr &doc, 
			const AosXmlTagPtr &cmd, 
			const OmnString& cid,
			OmnString &objid);

	bool checkModifying(
			const AosXmlTagPtr &origdoc, 
			const AosXmlTagPtr &newdoc,
			const OmnString &cid, 
			const AosRundataPtr &rdata);

	AosXmlTagPtr getUserHomeCtnrAR(
			const AosRundataPtr &rdata,
			const OmnString &user_homectnr_objid); 

	//bool bindCloudid(
	//		const OmnString &cid, 
	//		const u64 &docid, 
	//		const AosRundataPtr &rdata);

	AosXmlTagPtr createDocByTemplate1(
			const AosRundataPtr &rdata,
			const OmnString &cid,
			OmnString &objid,
			const OmnString &template_objid);

	void createTempDoc(
			const AosXmlTagPtr &doc,
			const OmnString &log, 
			OmnString &errmsg);

	AosXmlTagPtr getTempDoc(
			const OmnString &siteid,
			const OmnString &objid);

	bool modifyParentContainers1(
			const AosRundataPtr &rdata,
			const OmnString &siteid,
			const AosXmlTagPtr &doc,
			const OmnString &pctrs_new);

	AosXmlTagPtr createRootCtnr(
			const OmnString &siteid, 
			const OmnString &docstr, 
			const AosRundataPtr &rdata);
	
	bool modifyAttr(
			const AosXmlTagPtr &doc,
			const OmnString &xpath, 
			const AosValueRslt &value,
			const bool value_unique, 
			const bool docid_unique, 
			const AosRundataPtr &rdata,
			const char *fname, 
			const int line);

	bool deleteDoc(
			const AosXmlTagPtr &origdocroot, 
			const AosRundataPtr &rdata, 
			const OmnString &container,
			const bool deleteFlag);
	bool 
	writeAccessDoc(
			const u64 &docid, 
			const AosXmlTagPtr &doc,
			const AosRundataPtr &rdata);

	bool bindObjid(
			OmnString &objid, 
			u64 &docid,
			bool &objid_changed, 
			const bool resolve,
			const bool keepDocid,
			const AosRundataPtr &rdata);

private:
	bool checkHomeContainer(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &doc,
			const AosXmlTagPtr &origdoc,
			const OmnString &target_cid);

	bool checkContainers(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &xml,
				const AosXmlTagPtr &docorig,
				const OmnString &cid);

	AosXmlTagPtr createMissingContainer(
				const AosRundataPtr &rdata,
				const OmnString &container_objid,
				const bool ispublic); 

	AosXmlTagPtr getParentAcrd(
				const AosRundataPtr &rdata, 
				const AosXmlTagPtr &ref_doc);
	/*
	AosXmlTagPtr getParentAR1(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &ref_doc,
				const u64 &ref_docid, 
				const OmnString &objid,
				bool &is_parent);
	*/

	bool checkCreatingNew(
				const OmnString &siteid,
				const OmnString &newobjid, 
				const bool rename, 
				const AosRundataPtr &rdata);

	bool modifyObjChecked(
				const AosRundataPtr &rdata,
				const OmnString &cid,
				AosXmlTagPtr &newxml,
				u64 &newdid,
				const OmnString &rename,
				const bool synobj);

	bool modifyAttrPriv(
				const AosXmlTagPtr &doc,
				const OmnString &xpath, 
				const u64 &newvalue,
				const bool value_unique, 
				const bool docid_unique, 
				const AosRundataPtr &rdata,
				const char *fname, 
				const int line);

	bool modifyAttrPriv(
				const AosXmlTagPtr &doc,
				const OmnString &xpath, 
				const OmnString &newvalue,
				const bool value_unique, 
				const bool docid_unique, 
				const AosRundataPtr &rdata,
				const char *fname, 
				const int line);

	bool saveDocASC(
				u32 &seqno, 
				u64 &offset, 
				u64 &docsize, 
				const OmnString &str);

	bool addContainerMember(
				const AosRundataPtr &rdata,
				const OmnString &ctnr_objid,
				const AosXmlTagPtr &doc,
				const OmnString &objid,
				const u64 &docid);

	bool removeContainerMember(
				const OmnString &ctnr_objid,
				const AosXmlTagPtr &doc,
				const u64 &docid, 
				const AosRundataPtr &rdata);

	static AosXmlTagPtr getTempDoc(const u32 seqno, const u64 &offset);
	static bool	showLog() {return mShowLog;}
	static bool	checkCtnrMember(
			const OmnString &event,
			const AosRundataPtr &rdata);
};
#endif
#endif
