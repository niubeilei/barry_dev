////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AppInfo.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_AppMgr_AppInfo_h
#define Omn_AppMgr_AppInfo_h

#include "Obj/ObjDb.h"
#include "Util/String.h"
#include "Util/IpAddr.h"


class OmnAppInfo 
{

private:
	OmnString	mAppName;
	OmnString	mVersion;
	OmnString	mUOI;
	int64_t		mInstanceId;
	int			mBuildNo;
	OmnString	mBuildTime;
	OmnString	mPatch;
	OmnIpAddr	mSignalAddr;
	int			mSignalPort;
	OmnIpAddr	mStreamerAddr;
	int			mStreamerPort;

public:
	OmnAppInfo();

	virtual ~OmnAppInfo()
	{
	}

	virtual OmnClassId::E	getClassId() const {return OmnClassId::eOmnAppInfo;}
	virtual int				objSize() const {return sizeof(*this) + mAppName.objSize();}

	static OmnString	getTableName();
	OmnString	toString() const;
	bool		existInDb(const OmnString &appname);

	OmnIpAddr	getSignalAddr() const {return mSignalAddr;}
	int			getSignalPort() const {return mSignalPort;}
	OmnIpAddr	getStreamerAddr() const {return mStreamerAddr;}
	int			getStreamerPort() const {return mStreamerPort;}

	OmnString	getAppName() {return mAppName;}
	void		setAppName(const OmnString &appName) {mAppName = appName;}

	OmnString	getUOI() {return mUOI;}
	void		setUOI(const OmnString &uoi) {mUOI = uoi;}

	int64_t		getInstanceId() {return mInstanceId;}
	void		setInstanceId(const int64_t &n) {mInstanceId = n;}

	void		setVersionInfo(const OmnString &version, 
							   const int buildNo,
							   const OmnString &buildTime,
							   const OmnString &patch)
	{
		mVersion = version;
		mBuildNo = buildNo;
		mBuildTime = buildTime;
		mPatch   = patch;
	}
	OmnString	getVersion() const {return mVersion;}
	int			getBuildNo() const {return mBuildNo;}
	OmnString	getBuildTime() const {return mBuildTime;}
	OmnString	getPatch() const {return mPatch;}
};
#endif

