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
// 06/08/2011 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Torturers_XmlDocTurturer_CreateUserDomainTester_h
#define Aos_Torturers_XmlDocTurturer_CreateUserDomainTester_h

#include "Rundata/Ptrs.h"
#include "SengTorUtil/SengTester.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/OmnNew.h"


class AosCreateUserDomainTester : virtual public AosSengTester
{
private:
	int					mThreadId;
	bool				mAllowed;
	bool				mDocIsPublic;
	AosXmlTagPtr		mRawDoc;
	AosXmlTagPtr 		mResp;
	AosXmlTagPtr		mServerDoc;

	int					mDoNotUseRootWeight;
	int					mParentWithCidWeight;
	int					mCreatePubCtnrWeight;
	bool				mWithObjid;
	OmnString			mObjid;
	OmnString			mParentObjid;

public:
	AosCreateUserDomainTester(const bool regflag);
	AosCreateUserDomainTester();
	~AosCreateUserDomainTester();

	virtual bool test();
	virtual AosSengTesterPtr clone()
			{
				return OmnNew AosCreateUserDomainTester();
			}
	static AosXmlTagPtr createUserDomain(
				const AosSengTestThrdPtr &thread,
				const AosSengTesterMgrPtr &mgr, 
				const AosRundataPtr &rdata);

private:
	bool determineLogin();
	bool createRawDoc();
	bool pickParent();
	bool determineDocPublic();
	bool createUserDomain(const AosRundataPtr &rdata);
	bool checkCreation(const AosRundataPtr &rdata);
	bool checkObjid(const AosRundataPtr &rdata);
	bool checkParent(const AosRundataPtr &rdata);
	bool addContainer(const AosRundataPtr &rdata);
};
#endif

