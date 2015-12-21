////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DirDesc.h
// Description:
//	This is the file descriptor.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Util_DirDesc_h
#define Snt_Util_DirDesc_h

#ifdef OMN_PLATFORM_UNIX
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif


#include "Util/FileObj.h"
#include "Util/Ptrs.h"
#include "Util/ValList.h"


class OmnDirDesc : public OmnFileObj
{
private:
	OmnString	mDirName;
	int			mCount;
	bool		mStatus;

#ifdef OMN_PLATFORM_UNIX
	DIR	*mDirH;
#else
	WIN32_FIND_DATA mFindFileData;
	HANDLE mHandleFind;
#endif

public:
	OmnDirDesc()
		:
	OmnFileObj(eDirectory)
	{
	}

	OmnDirDesc(const OmnString &dir);
	virtual ~OmnDirDesc();

	//
	// File enumeration
	//
	bool	getFirstFile(OmnString &fullName);
	bool	getNextFile(OmnString &fullName);
	bool	close();
	int		numObjs() const;

	OmnString getDirName() const {return mDirName;}
	static bool dirExist(const OmnString &dirname);
	static bool createDir(const OmnString &dirname);

};
#endif

