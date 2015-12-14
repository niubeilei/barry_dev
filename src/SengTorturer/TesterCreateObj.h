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
#ifndef AosSengTester_CreateObjTester_h
#define AosSengTester_CreateObjTester_h

#include "Rundata/Ptrs.h"
#include "SengTorUtil/SengTester.h"
#include "Tester/TestMgr.h"
#include "Thread/Ptrs.h"
#include "SengTorUtil/StUtil.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosCreateObjTester : virtual public AosSengTester
{
public:
	enum
	{
		eMinCtnrLen = 2,
		eMaxCtnrLen = 30,
	};

private:
	enum
	{
		eAllowed = 0,

		eNotLogin,
		eObjidNotUnique,
		eObjidInvalid,
		eParentNotContainer,
		eParentIsPublic,
		eOperationReject
	};
	
	// Weights
	int				mWtUseKnownCtnr;
	int				mWtUsePubCtnr;
	int				mWtUsePrivateDoc;
	int				mWtUseUniqueObjid;
	int				mWtWithCid;
	int				mWtUseNewObjid;
	int				mWtUseExistingObjid;
	int				mWtUseRandomCtnr;
	int				mWtUseMultipleCtnrs;
	int				mWtUsePubDoc;
	int				mWtUseNoObjid;
	int				mWtUseValidCid;
	int				mWtUseInvalidCid;
	int				mWtUseOtherCid;
	int				mWtWithModifier;
	int				mWtCreatorAsCid;
	int				mWtCreatorAsUserid;
	int				mWtCreatorAsStr;
	int				mWtCreatorAsAnyUser;
	int				mWtNoCreator;
	int				mWtUseNoContainers;

	// Working Data
	AosXmlTagPtr	mRawDoc;
	OmnString		mRawData;
	OmnString		mCid;
	bool			mParentPublic;
	bool			mValidParent;
	OmnString		mParentStr;
	OmnString		mParentObjidStr;
	vector<OmnString>	mParentObjids;
	vector<bool>	mParentObjidTypes;
	bool			mDocPublic;
	OmnString		mObjid;
	bool			mObjidUnique;
	bool			mWithCid;
	AosXmlTagPtr	mResp;
	AosXmlTagPtr	mServerDoc;
	bool			mResolveObjid;
	OmnString		mCreatorCid;
	OmnString		mCreator;
	bool			mCanCreate;
	int				mThreadId;
	vector<OmnString>	mMemberOfObjids;
	vector<bool>	mMemberOfTypes;
	OmnString		mMemberOfStr;
	int				mRejectLine;

public:
	AosCreateObjTester(const bool regflag);
	AosCreateObjTester();
	~AosCreateObjTester();

	virtual bool test();
	AosSengTesterPtr clone() { return OmnNew AosCreateObjTester(); }

private:
	bool	setDefaultWeights();
	bool	createRawDoc();
	bool	determineCtnr();
	bool	determineCreator();
	bool	determineMetaData();
	OmnString determineCid();
	bool	determineMemberOf();
	bool	determinePublic();
	bool	resolveObjid();
	bool	determineObjid();
	bool	createDoc();
	bool	checkCreation(const AosRundataPtr &rdata);
	bool	determineCanCreate();
	bool	checkParents(const AosRundataPtr &rdata);
	bool	checkServerDoc();
	bool	checkObjid();
	bool	addDoc();
	bool	checkDuplicatedObjid();
	bool	decomposeDupObjid(
				const OmnString &objid,
				OmnString &prefix,
				OmnString &num);
	bool	pickMultipleContainers();
};
#endif

