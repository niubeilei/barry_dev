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
#ifndef AosSengTester_CreateUserNewTester_h
#define AosSengTester_CreateUserNewTester_h

#include "Rundata/Ptrs.h"
#include "SengTorturer/SengTester.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/OmnNew.h"


class AosCreateUserNewTester : virtual public AosSengTester
{
private:
	int 			mUserParentCtnrWeight;
	int				mCreatePubCtnrWeight;
	int				mWithCidWeight;
	int				mParentWithCidWeight;
	int 			mGroupId;
	u64				mLocalDocid;
	bool			mParent_is_public;
	bool			mUser_is_public;
	bool			mParent_with_cid;
	u64				mThreadId;
	AosXmlTagPtr	mRawUser;
	OmnString 		mParentObjid;
	OmnString 		mObjid;
	AosXmlTagPtr	mServerUser;
	OmnString 		mPasswd;
	AosXmlTagPtr 	mResp;
	OmnString		mUserGroup;
	bool			mWithUserDomain;
	bool			mWithObjid;
	int				mRandomContainerWeight;
	OmnString		mUsername;
	bool			mUserIsPublic;

public:
	AosCreateUserNewTester(const bool regflag);
	AosCreateUserNewTester();
	~AosCreateUserNewTester();

	virtual bool test();

	bool	checkCreate(
			AosXmlTagPtr &ar,
			OmnString &ssid,
			AosXmlTagPtr &ctr,
			const AosSengTestThrdPtr &thread,
			const AosSengTesterMgrPtr &mgr);
	OmnString pickUsername(const u64 &ctn_docid);
	OmnString getRandomString();
	bool	createRawUserDoc();
	bool	pickParents();
	bool	determinePublic();
	bool	createUser();
	bool	checkCreation(const AosRundataPtr &rdata);
	bool	checkParents(const AosRundataPtr &rdata);
	bool	addUser();
	bool	addUserGroup();
	static bool createUserAccount(const AosSengTestThrdPtr &thread);
	static bool createUserAccount1(const AosSengTestThrdPtr &thread);
	AosSengTesterPtr clone()
			{
				return OmnNew AosCreateUserNewTester();
			}
private:
	bool oneTest(const AosSengTestThrdPtr &thread,
			const AosSengTesterMgrPtr &mgr);
	bool checkPermission(bool &allowed);
	bool checkObjid(const AosRundataPtr &rdata);
	bool	checkLogin();
};
#endif

