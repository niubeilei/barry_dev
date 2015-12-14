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
#ifndef AosQuery_Tester_QueryTesterMgr_h
#define AosQuery_Tester_QueryTesterMgr_h

#include "QueryTorturer/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/XmlTag.h"



class AosQueryTesterMgr : virtual public OmnRCObject
{
private:
	OmnDefineRCObject;

	enum
	{
		eMaxThrds = 10000
	};

	OmnMutexPtr				mLock;
	int						mNumThrds;
	int						mNumTries;
	AosQueryTesterThrdPtr *	mThrds;
	
public:
	AosQueryTesterMgr(const int num_thrds, const int num_tries);
	~AosQueryTesterMgr();

	bool					start();
	bool					config(const AosXmlTagPtr &config);

private:
	bool					createSuperUser();
};
#endif

