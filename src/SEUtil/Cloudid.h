////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 10/22/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEUtil_Cloudid_h
#define AOS_SEUtil_Cloudid_h

#include "Util/RCObject.h"
#include "Util/String.h"

#define AOSCLOUDID_SYSTEM 		"50001"
#define AOSCLOUDID_UNKNOWN 		"50002"
#define AOSCLOUDID_ROOT 		"50003"
#define AOSCLOUDID_SYSROOT 		"50004"
#define AOSCLOUDID_APPROOT 		"50005"
#define AOSCLOUDID_DELOBJROOT 	"50006"
#define AOSCLOUDID_IILMGRROOT 	"50007"
#define AOSCLOUDID_GUEST		"50008"

class AosCloudid
{
public:
	
private:

	AosCloudid();
	~AosCloudid();

public:
	enum
	{
		eMinCloudidLen = 6
	};

	static OmnString
	composeCloudid(const u64 &cid)
	{
		// In the current implementations, there is no prefix for cids.
		OmnString ccc;
		ccc << cid;
		return ccc;
	}

	static bool isValidCloudid(const OmnString &cid)
	{
		// Currently we assume cloud ids are at least 'eMinCloudidLen' char long.
		if (cid.length() < eMinCloudidLen) return false;
		return true;
	}

	static bool isRootCloudid(const OmnString &cid)
	{
		return (cid == AOSCLOUDID_SYSTEM ||
				cid == AOSCLOUDID_ROOT ||
				cid == AOSCLOUDID_SYSROOT ||
				cid == AOSCLOUDID_APPROOT ||
				cid == AOSCLOUDID_DELOBJROOT ||
				cid == AOSCLOUDID_IILMGRROOT);
	}
};

#endif
