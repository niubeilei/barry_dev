////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: VersionInfo.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_NMS_Version_h
#define Omn_NMS_Version_h

#include "Util/String.h"



class OmnVersionInfo
{
private:
	static OmnString	mVersion;
	static OmnString	mPatch;
	static int			mBuildNo;
	static OmnString	mBuildTime;
	static OmnString	mDefaultHostName;

public:
	static OmnString	getVersion() {return mVersion;}
	static OmnString	getPatch()  {return mPatch;}
	static int			getBuildNo() {return mBuildNo;}
	static OmnString	getBuildTime() {return mBuildTime;}
	static OmnString	getDefaultHostName() {return mDefaultHostName;}
};

#endif
