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
#ifndef AosSengTester_ModifyObjTester_h
#define AosSengTester_ModifyObjTester_h

#include "Rundata/Ptrs.h"
#include "SengTorturer/SengTester.h"
#include "SengTorturer/XmlTagRand.h"
#include "Tester/TestMgr.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosModifyObjTester : virtual public AosSengTester
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
	bool			mCanModify;

	u64				mServerDocid;
	OmnString		mObjid;
	AosXmlTagPtr	mResp;
	AosXmlTagPtr	mServerDoc;
	AosXmlTagPtr	mOld;

	OmnString		mName;
	OmnString		mOldname;
	OmnString 		mNewname;
	OmnString		mTagname;
	int 			mThreadId;
	AosXmlTagRand   mTag;

public:
	AosModifyObjTester(const bool regflag);
	AosModifyObjTester();
	~AosModifyObjTester();

	virtual bool test();
	AosSengTesterPtr clone()
			{
				return OmnNew AosModifyObjTester();
			}
private:
	bool	pickDoc();
	bool	modifyXml();
	bool	modifyDoc(const AosRundataPtr &rdata);
	bool	checkModify();
	bool	check();
	bool	modifyDocToMemory();
	bool	canAttrDelete(const OmnString &aname);
	bool	canAttrModify(const OmnString &aname);
	bool	canModify();
};
#endif

