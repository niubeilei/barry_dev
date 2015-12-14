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
#ifndef AosQuery_Tester_QueryTesterThrd_h
#define AosQuery_Tester_QueryTesterThrd_h


#include "Rundata/Ptrs.h"
#include "QueryTorturer/QueryTesterMgr.h"
#include "Thread/ThreadedObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "SecUtil/SecOpr.h"
#include "SEUtil/Ptrs.h"


class AosQueryTesterThrd : virtual public OmnThreadedObj 
{
	OmnDefineRCObject;

	friend class AosTester;
public:

private:
	AosQueryTesterMgrPtr	mMgr;
	OmnMutexPtr				mLock;
	OmnThreadPtr			mThread;
	int						mThreadId;
	u32						mTries;
	int						mTotalSuccess;
	int						mTotalFail;
	AosRundataPtr			mRundata;


	u32						mSiteId;
	OmnString				mSsid;
	u64						mUrldocid;	
		
	AosTesterQueryPtr		mTester;

public:
	// Constructor & Destructor	
	AosQueryTesterThrd(
			const AosQueryTesterMgrPtr &mgr, 
			const int tries, 
			const int thrdid);
	~AosQueryTesterThrd();

	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool			start();
	void			setSiteId(const u32 &siteid){mSiteId = siteid;}
	void			setSsid(const OmnString &ssid){mSsid = ssid;}
	void			setUrldocid(const u64 &id){mUrldocid = id;}

/*

	AosXmlTagPtr	retrieveDoc(const u64 &svrdid);
	AosXmlTagPtr	retrieveDocByObjid(const OmnString &objid);
	AosStDoc*		getDocFromMemory(const OmnString &objid);
	AosStDoc* 		getDocFromMemory(const u64 &local_docid);
	bool            addDoc(const u64 &locdid, const AosXmlTagPtr &doc);
	bool			modifyDocToMemory(const u64 &local_docid, const AosXmlTagPtr &xmldoc);
	u64 			getTotalUser();
	AosXmlTagPtr	createDoc();
	const AosStDoc*	pickDoc(const OmnString &cid) const;
	const AosStDoc* pickDoc(const bool own_doc = false);
	const AosStDoc*	pickByObjid(const OmnString &objid);
	u64				getServeridByLocalid(const OmnString &server_docid);
	u64 			getLocalidByCid(const OmnString &cid);
	bool			removeDoc(
						const u64 &local_docid,
						const OmnString objid,
						const u64 &server_docid);
	OmnString		pickNewObjid();
	AosXmlTagPtr	readLocalDocFromFile(const u64 &ldocid);
	bool 			saveNewDoc(const u64 local_docid, const AosXmlTagPtr &doc);
	bool			removeDocToFile(const u64 local_docid);
	bool			isDeleteToFile(const u64 local_docid);
	bool			modifyDocToFile(const u64 local_docid, const AosXmlTagPtr &doc);
	bool			checkSaveNewDoc(const u64 local_docid);
	AosStContainer*	generateNewCtnrs();
	OmnString		pickUsedObjid();
	bool			checkCreate(const OmnString &container_objid);
	OmnString			createInvalidUsername();
	OmnString			pickNewUsername();
	AosStContainer::Type	pickContainerType();
	void				setCtnrMembers(AosStContainer *ctnr, const u64 &ldocid);

	bool			isUserValid(const AosStContainer *container, const u64 &user_docid);
	bool 			userInContainer(const AosStContainer *ctnr, const u64 &user_locdid);
	int				getStDocSize();
	int				getNumContainers() const {return mCtnrs1.size();}

	// 10/26/2010
	AosXmlTagPtr	loadLocalDoc(const u64 &local_docid);
	AosXmlTagPtr	pickLocalDoc();
	OmnString		pickNewCtnrObjid();
	OmnString		getNewUserName();
	AosXmlTagPtr getDefaultAccessObj();
	bool		modifyARToServer(const AosXmlTagPtr &doc);
	bool		modifyObj(const AosXmlTagPtr &doc, const AosRundataPtr &rdata, const bool check_error ="false");
	AosXmlTagPtr	retrieveAccessRcd(  
				        const u64 &svrdid,
					    const bool create_flag,
					    const bool parent_flag);
	AosStAccessRecord *	createAccessRecord(const u64 &locdid);
	AosStAccessRecord *	createAccessRecord(const AosXmlTagPtr &svr_ard, const u64 &locdid);
	AosXmlTagPtr	retrieveAccessRcdOrParent(const u64 &svrdid);
	bool	modifyAccessRecord(AosStAccessRecord *ard);
	bool	syncAccesses(
				AosStAccessRecord *ard, 
				const AosXmlTagPtr &svr_ard); 
	bool	randomLogin();
	bool	login(const OmnString &create);
	bool	logout();
	bool	deleteDoc(const AosStDoc *doc1);
	OmnString createArdObjid(const u64 docid);
	bool	isRoot();

	u64		JudgeLocalDocid(const AosXmlTagPtr &doc);
	bool	checkDocThreadId(const AosXmlTagPtr &doc);
	int		getTries() const {return mTries;}
	int		getThreadid() const {return mThreadId;}
	OmnString getUserCid(const u64 &user_localid);
	bool 	canDoOpr(
	 			const u64 &local_did,
	 			const AosSecOpr::E opr, 
	 			const OmnString &owner_cid);
	AosStAccessRecord *getAcd(const AosStDoc *doc, bool &is_parent);
	OmnString getRootSsid();
	bool	checkSaveSdocFlag();
	bool	loginAsNormalUser();

private:
	bool			basicTest();
	AosXmlTagPtr	createArcd();
	bool			modifyVector(AosStDoc *doc1);
	void			addStDoc(AosStDoc *doc);
	AosStDoc *		getStDoc(int idx);
	void			addDocsByObjid(const OmnString objid, AosStDoc *doc);
	void			addDocsByLocdid(const u64 &local_docid, AosStDoc *doc);
	void			addServerDidMap(const u64 &server_docid, const u64 &local_docid);
	
*/
};
#endif

