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
#ifndef AosSengTester_DeleteObjTester_h
#define AosSengTester_DeleteObjTester_h

#include "SengTorturer/SengTester.h"
#include "SengTorturer/XmlTagRand.h"
#include "Tester/TestMgr.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosDeleteObjTester : virtual public AosSengTester
{
private:
	// Weights
	int				mUseKnownCtnrWeight;
	int				mUseDuplicatedCtnrWeight;
	int				mUsePubCtnrWeight;
	int				mPrivateDocWeight;
	int				mUseUniqueObjidWeight;
	int				mCreateDocWeight;
	int				mWithCidWeight;
	int				mNoParent;
	int				mOneParent;
	int				mTwoParent;
	int				mThreeParent;
	int				mUseAnotherCreator;

	// Working Data
	u64				mLocalDocid;
	AosXmlTagPtr	mRawDoc;
	OmnString		mRawData;

	u64				mServerDocid;
	OmnString		mObjid;
	AosXmlTagPtr	mResp;
	AosXmlTagPtr	mServerDoc;
	int				mThreadId;
	bool			mCanDelete;

public:
	AosDeleteObjTester(const bool regflag);
	AosDeleteObjTester();
	~AosDeleteObjTester();

	virtual bool test();
	AosSengTesterPtr clone()
			{
				return OmnNew AosDeleteObjTester();
			}
private:
	bool	pickDoc();
	bool	checkDoc();
	bool	deleteDoc();
	bool	checkDelete();
	bool	determineCanDelete();
};
#endif

