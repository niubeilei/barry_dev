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
// 2009/10/16	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEClient_Testers_SearchEngTester_h
#define Aos_SEClient_Testers_SearchEngTester_h

#include "SearchEngine/Ptrs.h"
#include "SEClient/Testers/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Thread/ThreadedObj.h"
#include "Util/DynArray.h"
#include "Util/Ptrs.h"
#include "UtilComm/Ptrs.h"


class AosXmlDoc;

class AosSearchEngTester : public OmnTestPkg
{
private:
	enum
	{
		eMaxSiteids = 1000,
		eMaxUnames = 1000,
		eMaxAppnames = 1000,
		eMaxCtnrnames = 1000,
		eMaxThrds = 100
	};

	OmnMutexPtr			mLock;
	OmnString			mDataDirname;
	int					mTotalDocsToCheck;
	OmnIpAddr			mRemoteAddr;
	int					mRemotePort;

	AosCreateDocThrdPtr	mCreateThrds[eMaxThrds];
	int					mNumCreateThrds;
	AosModifyDocThrdPtr	mModifyThrds[eMaxThrds];
	int					mNumModifyThrds;
	AosDeleteDocThrdPtr	mDeleteThrds[eMaxThrds];
	int					mNumDeleteThrds;
	AosQueryThrdPtr		mQueryThrds[eMaxThrds];
	int					mNumQueryThrds;
	AosDocThrdPtr		mDocThrds[eMaxThrds];
	int					mNumDocThrds;

	int					mTotalDocsCreated;
	u64					mStartTime;
	int					mUpdateStopped;
	OmnString			mSiteids[eMaxSiteids];
	OmnString			mUnames[eMaxUnames];
	OmnString			mAppnames[eMaxAppnames];
	OmnString			mCtnrnames[eMaxCtnrnames];

	OmnString			mDocTname1;
	OmnString			mDocFname;
	OmnString			mAttrTname1;
	OmnString			mAttrwdTname1;
	OmnString			mDocwdTname1;
	OmnString			mTagTname1;

public:
	AosSearchEngTester();
	~AosSearchEngTester() {}

	virtual bool	start();
	virtual void	procFinished(const bool status, 
						const u64 &docId,
						const OmnString &errmsg, 
						const void *userdata, 
						const int numWords);	

	bool 	isUpdateStopped();
	bool 	docCreated(const OmnString &siteid, 
					const OmnString &docId, 
					const OmnString &objid, 
					OmnString &doc);
	bool 	docModified(
					const OmnString &siteid,
					const OmnString &docid, 
					OmnString &contents);
	bool 	docDeleted(const OmnString &siteid, const OmnString &docId);
	bool 	stopUpdating();
	AosXmlTagPtr readDoc();
	AosXmlTagPtr readDoc(const OmnString &siteid, const OmnString &docid);
	OmnString	getSiteId();
	OmnString	getUsername();
	OmnString	getAppname();
	OmnString	getCtnrname();
	bool 		releaseUpdating();
	bool		getDocidToDelete(
					OmnString &siteid, 
					OmnString &docid, 
					OmnString &objid);

private:
	bool	prepareEnv();
	bool	basicTest();
	bool 	torturer();
	bool	saveDoc(const AosTestXmlDocPtr &, const u64 &);
	bool 	verifyQuery();
};
#endif

