////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FileDesc.h
// Description:
//	This is the file descriptor.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Util_FileDesc_h
#define Snt_Util_FileDesc_h

#include "aosUtil/Types.h"
#include "Debug/Rslt.h"
#include "Porting/LongTypes.h"
#include "Util/FileObj.h"

class OmnRslt;

class OmnFileDesc : public OmnFileObj
{
private:
	OmnString		mFullName;		// Include path
	OmnString		mLastAccessTime;  
	OmnString		mLastWriteTime;  
	unsigned int 	mFileLength;  
	OmnString		mFileName;
	bool			mExist;

#ifdef OMN_PLATFORM_UNIX
	int				mUserOwner;
	int				mGroupOwner;
#else
	int				mFileAttributes;  
	OmnString		mCreationTime;  
#endif

public:
	OmnFileDesc()
		:
	OmnFileObj(eFile)
	{
#ifdef OMN_PLATFORM_UNIX
#else
	mFileAttributes = 0;
#endif
	}

	OmnFileDesc(const OmnString &fullname);

	virtual ~OmnFileDesc();
	
	static OmnRslt	getAbsoluteName(const OmnString &orig, 
									OmnString &absPath, 
									OmnString &filename,
									int64_t &filesize,
									bool &exist);

	OmnString		getPath() const;
	OmnString		getName();
	bool			exist() const { return mExist;}
	OmnString		getFullName() const {return mFullName;}
	OmnString		getLastAccessTime() const {return mLastAccessTime;}
	OmnString		getLastWriteTime() const {return mLastWriteTime;}
	unsigned int	getFileLength() const {return mFileLength;}

	static OmnString	getFileName(OmnString fullname);
	static OmnString	removeDotDot(OmnString fullname);

#ifdef OMN_PLATFORM_UNIX

#else
	OmnString		getCreationTime() const {return mCreationTime;}
	void set(WIN32_FIND_DATA findFileData);
#endif

	OmnString toString() const;
};
#endif

