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
// 09/06/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSengTester_SengTestThrd_h
#define AosSengTester_SengTestThrd_h

#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SengTorturer/SengTester.h"
#include "SengTorturer/Ptrs.h"
#include "SengTorturer/StUtil.h"
#include "SengTorUtil/Ptrs.h"
#include "SengTorUtil/StContainer.h"
#include "Thread/ThreadedObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "SEBase/SecOpr.h"
#include "SEUtil/Ptrs.h"



#define AOS_SENGTORTURER_LOCAL_DOCID 			"zky_sengtorturer_docid"

typedef hash_map<const u64, AosStAccessRecord*, AosU64Hash1, u64_cmp>	AosU642StArcd_t;
typedef hash_map<const u64, AosStAccessRecord*, AosU64Hash1, u64_cmp>::iterator	AosU642StArcdItr_t;
typedef hash_map<const OmnString, AosStDocPtr, Omn_Str_hash, compare_str> AosStr2Stdoc_t;
typedef hash_map<const OmnString, AosStDocPtr, Omn_Str_hash, compare_str>::iterator AosStr2StdocItr_t;
// typedef hash_map<const u64, u64, AosU64Hash1, u64_cmp> AosU642U64_t1;
// typedef hash_map<const u64, u64, AosU64Hash1, u64_cmp>::iterator AosU642U64Itr_t1;
typedef hash_map<const u64, AosStDocPtr, AosU64Hash1, u64_cmp> AosU642Stdoc_t;
typedef hash_map<const u64, AosStDocPtr, AosU64Hash1, u64_cmp>::iterator AosU642StdocItr_t;


class AosSengTestThrd : virtual public OmnThreadedObj 
{
	OmnDefineRCObject;

	friend class AosTester;
public:
	enum
	{
	 	eMaxDepth = 4
	};

private:
	typedef map<u64, AosStContainerPtr> CtnrMap_t;
	typedef map<u64, AosStContainerPtr>::iterator CtnrMapItr_t;
	typedef map<u64, AosStUser*> UserMap_t;
	typedef map<u64, AosStUser*>::iterator UserMapItr_t;


	AosSengTesterMgrPtr	mMgr;
	OmnMutexPtr			mLock;
	OmnThreadPtr		mThread;
	int					mThreadId;
	u32					mTries;
	int					mTotalSuccess;
	int					mTotalFail;

	// Session Data
	OmnString			mCrtSsid;
	u64					mCrtUserid;
	OmnString			mCrtCid;
	AosXmlTagPtr		mCrtUserdoc;
	u64 				mCrtUrlDocid;

	u64 				mCrtDocid;

	u32					mNumObjid;
	u32					mNumCtnrObjid;
	u32					mNumUserName;

	// The data to keep the created docs
	CtnrMap_t			mCtnrs1;
	UserMap_t			mUsers1;
	vector<AosStDocPtr>			mDocs;
	AosStr2U64_t 		mUserCidMap;
	AosStr2Stdoc_t		mDocsByObjid;
	AosU642Stdoc_t		mDocsByLocdid;
	AosU642U64_t		mServerDidMap;
	AosU642StArcd_t		mLocalDidArcds;
	AosU642StArcd_t		mOwnDocidArcds;	
	AosRundataPtr		mRundata;
	AosXmlTagPtr		mOlddoc;
	AosXmlTagPtr		mNewdoc;
	AosSengTesterPtr	mTesters[AosTesterId::eMax];

public:
	AosSengTestThrd(
			const AosSengTesterMgrPtr &mgr, 
			const int tries, 
			const int thrdid);
	~AosSengTestThrd();

	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool			start();

public:
	void 			resetSession();
	bool 			loginAsRoot();
	void			setSession(
						const OmnString &ssid, 
						const u64 &user_docid,
						const AosXmlTagPtr &session_doc,
						const u64 &urldocid);

	// All 'add' functions
	bool            addContainer(
						const AosStContainer1::Type type, 
						const AosXmlTagPtr &ctnr, 
						const AosRundataPtr &rdata);
	bool            addContainer(const u64 &docid, const AosStContainerPtr &ctnr);
	bool			addContainer(
						const OmnString &objid, 
						const AosStContainer1::Type type, 
						const AosRundataPtr &rdata);
	bool            addUser(const AosXmlTagPtr &userdoc, const AosRundataPtr &rdata);
	bool            addUser(const u64 &local_docid, const AosXmlTagPtr &userdoc);

	bool            addDoc1(const u64 &locdid, const AosXmlTagPtr &doc);

	// All 'create' functions
	AosXmlTagPtr		createDoc();
	OmnString			createInvalidUsername();
	AosStAccessRecord *	createAccessRecord(const u64 &locdid);
	AosStAccessRecord *	createAccessRecord(const AosXmlTagPtr &svr_ard, const u64 &locdid);
	OmnString 			createArdObjid(const u64 docid);
	AosXmlTagPtr		createDocOnServer(
							const AosXmlTagPtr &doc, 
							const OmnString &resolve_objid,
							const OmnString &saveas);
	inline AosXmlTagPtr	createDocOnServer(const AosXmlTagPtr &doc)
	{
		return createDocOnServer(doc, "", "");
	}

	// Random Pickers
	AosStUser* 			pickUser(); 
	OmnString 			pickCid(); 
	OmnString			pickInvalidcid();
	const AosStDocPtr	pickDoc(const OmnString &cid) const;
	const AosStDocPtr 	pickDoc(const bool own_doc = false);
	OmnString			pickNewObjid();
	OmnString			pickUsedObjid();
	AosStContainerPtr 	pickContainer();
	AosStContainerPtr 	pickContainer(const AosStContainer1::Type type);
	AosStContainerPtr 	pickContainers();
	AosStContainerPtr 	pickUserContainer();
	u64					pickUser(const AosStContainerPtr &container);
	AosXmlTagPtr		pickLocalDoc();
	OmnString			pickNewCtnrObjid();
	OmnString			pickNewUsername();
	OmnString			pickOtype();
	OmnString			pickStype();
	AosStContainerPtr 	pickValidContainer();
	const AosStUser*	pickValidUser(const u64 &ctnr_docid); 
	OmnString 			pickInvalidCid();
	AosStContainer1::Type	pickContainerType();

	// All get's functions
	u64					getUrlDocid() const {return mCrtUrlDocid;}
	u64					getCrtUrlDocid() const {return mCrtUrlDocid;}
	u64             	getNewDocId();
	int             	getThreadId() const;
	OmnString			getCrtSsid() const {return mCrtSsid;}
	OmnString			getCrtCid() const {return mCrtCid;}
	AosStContainerPtr	getRequesterUserDomain();
	u64					getCrtUserid() const {return mCrtUserid;}
	AosXmlTagPtr		getCrtUserDoc() const {return mCrtUserdoc;}
	u32 				getSiteid() const;
	AosStAccessRecord*	getLocalAccessRecordByOwndocid(const u64 &owndid);
	AosStAccessRecord* 	getLocalAccessRecord(const u64 &local_docid);
	AosStContainerPtr	getContainer(const OmnString &objid);
	AosStContainerPtr	getContainer(const u64 &locdid);
	AosStUser* 			getCrtUser() {return getUserByCid(mCrtCid);} 
	AosStUser* 			getUser(const u64 &user_docid); 
	AosStUser* 			getUserByCid(const OmnString &cid); 
	AosStDocPtr			getDocByObjid(const OmnString &objid);
	AosStDocPtr			getDocByLocalDid(const u64 &local_docid);
	u64 				getTotalUser();
	u64					getServeridByLocalid(const OmnString &server_docid);
	u64 				getLocalidByCid(const OmnString &cid);
	int					getStDocSize();
	int					getNumContainers() const {return mCtnrs1.size();}
	OmnString			getNewUserName();
	AosXmlTagPtr 		getDefaultAccessObj();
	int					getTries() const {return mTries;}
	int					getThreadid() const {return mThreadId;}
	OmnString 			getUserCid(const u64 &user_localid);
	AosStAccessRecord *	getAcd(const AosStDocPtr &doc, bool &is_parent);
	OmnString 			getRootSsid();
	AosStDocPtr			getStDoc(int idx);
	OmnString			getRequesterRoles() const;
	AosXmlTagPtr		getOlddoc() const;
	AosXmlTagPtr		getNewdoc() const;
	AosXmlTagPtr		getUserOprArd() const;
	AosRundataPtr		getRundata() const;
	AosSengTesterPtr *	getTesters() {return mTesters;}

	// All set's functions
	void				setCtnrMembers(const AosStContainerPtr &ctnr, const u64 &ldocid);
	void				setSession(
							const OmnString &ssid, 
							const u64 &user_docid,
							const AosXmlTagPtr &session_doc);

	// All check functions
	bool			checkSaveNewDoc(const u64 local_docid);
	bool			checkCreate(const OmnString &container_objid);
	bool			checkDocThreadId(const AosXmlTagPtr &doc);
	bool			checkSaveSdocFlag();

	// All Retrieve functions
	AosXmlTagPtr	retrieveAccessRcdOrParent(const u64 &svrdid);
	AosXmlTagPtr	retrieveDoc(const u64 &svrdid);
	AosXmlTagPtr	retrieveDocByObjid(const OmnString &objid);
	AosXmlTagPtr	retrieveDocByCloudid(const OmnString &cid);
	AosXmlTagPtr	retrieveAccessRcd(  
				        const u64 &svrdid,
					    const bool create_flag,
					    const bool parent_flag);
	
	// All 'modify' functions
	bool			modifyDocToMemory(const u64 &local_docid, const AosXmlTagPtr &xmldoc);
	bool			modifyARToServer(const AosXmlTagPtr &doc);
	bool			modifyDocToFile(const u64 local_docid, const AosXmlTagPtr &doc);
	bool			modifyAccessRecord(AosStAccessRecord *ard);
	inline bool		modifyObj(const AosXmlTagPtr &doc){ return modifyObj(doc, true); }	
	bool			modifyObj(const AosXmlTagPtr &doc, 
						const bool check_error = false);

	// All is functions
	bool			isDeleteToFile(const u64 local_docid);
	bool			isUserValid(const AosStContainerPtr &container, const u64 &user_docid);
	bool			isRoot();
	bool 			isValidObjid(const OmnString &objid);
	bool			isSameDomain(const AosStDocPtr &local_doc);
	bool isObjidUsed(const OmnString &objid)
	{
		AosXmlTagPtr doc = retrieveDocByObjid(objid);
		if (doc) return true;
		return false;
	}

	// All 'remove' functions
	bool			removeDocToFile(const u64 local_docid);
	bool			removeCid(OmnString &objid);
	bool			removeDocFromServer(const AosXmlTagPtr &doc1);
	bool			removeDocFromServer(const AosStDocPtr &doc1);
	bool			removeDoc(
						const u64 &local_docid,
						const OmnString objid,
						const u64 &server_docid);

	AosXmlTagPtr	readLocalDocFromFile(const u64 &ldocid);
	bool 			saveNewDoc(const u64 local_docid, const AosXmlTagPtr &doc);
	AosStContainerPtr	generateNewCtnrs();

	bool 			userInContainer(const AosStContainerPtr &ctnr, const u64 &user_locdid);

	AosXmlTagPtr	loadLocalDoc(const u64 &local_docid);
	bool			saveDoc(const AosXmlTagPtr &doc, 
						const bool check_error = false)
	{
		return modifyObj(doc, check_error);
	}

	bool			syncAccesses(
						AosStAccessRecord *ard, 
						const AosXmlTagPtr &svr_ard); 
	bool			randomLogin();
	bool			login(const OmnString &create);
	bool			logout();
	u64				JudgeLocalDocid(const AosXmlTagPtr &doc);
	bool 			canDoOpr(const u64 &local_did, const AosSecOpr::E opr);
	bool			loginAsNormalUser();
	AosXmlTagPtr	getOwnerDomainDoc(const OmnString &objid);
	AosXmlTagPtr	getOwnerAccountDoc(const OmnString &objid);
	AosRundataPtr 	getRundata(){return mRundata;}

private:
	bool			basicTest();
	AosXmlTagPtr	createArcd();
	bool			modifyVector(const AosStDocPtr &doc1);
	void			addStDoc(const AosStDocPtr &doc);
	void			addDocByObjid(const OmnString objid, const AosStDocPtr &doc);
	void			addDocsByLocdid(const u64 &local_docid, const AosStDocPtr &doc);
	void			addServerDidMap(const u64 &server_docid, const u64 &local_docid);
};
#endif

