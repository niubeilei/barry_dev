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
#ifndef Aos_Torturers_XmlDocTurturer_CreateCtnrTester_h
#define Aos_Torturers_XmlDocTurturer_CreateCtnrTester_h

#include "Rundata/Ptrs.h"
#include "SengTorUtil/SengTester.h"
#include "SengTorUtil/StUtil.h"
#include "SengTorUtil/StContainer.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosCreateCtnrTester : virtual public AosSengTester
{
private:
	AosXmlTagPtr 	mRawCtnr;
	AosXmlTagPtr	mServerCtnr;
	OmnString 		mObjid;
	OmnString		mParentctnr;
	bool 			mParent_is_public;
	AosStContainer1::Type	mType;
	bool			mParent_is_cid;
	bool			mCtnr_is_public;
	bool 			mCtnr_with_cid;
	u64				mThreadId;
	AosXmlTagPtr 	mResp;
	bool			mCanCreate;

	
	int 			mUserParentCtnrWeight;
	int 			mDocCtnrWeight;
	int				mCreatePubCtnrWeight;
	int				mWithCidWeight;
	int				mUsePubParentWeight;
	int				mParentWithCidWeight;

public:
	AosCreateCtnrTester(const bool regflag);
	AosCreateCtnrTester();
	~AosCreateCtnrTester();

	virtual bool test();
	AosSengTesterPtr clone()
			{
				return OmnNew AosCreateCtnrTester();
			}

private:
	bool oneTest(const AosSengTestThrdPtr &thread,
			const AosSengTesterMgrPtr &mgr);
	bool	createRawCtnrDoc();
	bool	pickParents();
	bool	determinePublic();
	bool	createCtnr();
	bool	checkCreation(const AosRundataPtr &rdata);
	bool	checkObjid();
	bool	checkParents(const AosRundataPtr &rdata);
	bool	addCtnr(const AosRundataPtr &rdata);
	bool	checkServerDoc();
	bool	determineCanCreate();
};
#endif

