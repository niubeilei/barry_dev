////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/04/27 Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_TaskUtil_TaskToken_h
#define Aos_TaskUtil_TaskToken_h

#include "Rundata/Ptrs.h"
#include "TaskUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"

#include <map>
#include <vector>

using namespace std;


class AosTaskToken : public OmnRCObject
{
	OmnDefineRCObject;

	OmnMutexPtr				mLock;
	OmnString				mTokenId;
	vector<OmnString>		mResources;
	vector<OmnString>		mControlled;
	map<OmnString, bool>	mStatus;

	AosTaskToken();
	~AosTaskToken();

public:
	OmnString			getTokenId() const {return mTokenId;}
	vector<OmnString> & getResources() {return mResources;}
	vector<OmnString> & getControlled() {return mControlled;}

	bool	resourceReady(const OmnString &id, const AosRundataPtr &rdata);
	bool	isTokenReady();

	static AosTaskTokenPtr createToken(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);
private:
	bool	config(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);
};

#endif

#endif
