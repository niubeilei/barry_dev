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
// Modification History:
// 12/05/2007	Modified by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Debug_Error_h
#define Omn_Debug_Error_h

#include "aosUtil/Types.h"
#include "Debug/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include <string>
#include <list>

class AosError : virtual public OmnRCObject
{
	OmnDefineRCObject;

	enum
	{
		eMaxInstances = 20 
	};

	struct Errmsg_t
	{
		std::string		mErrmsg;
		u32				mCount;

		Errmsg_t()
			:
		mCount(0)
		{
		}

		Errmsg_t(const std::string &errmsg)
			:
		mErrmsg(errmsg), 
		mCount(1)
		{
		}
	};

private:
	std::string			mFilename;
	int					mLine;
	std::list<Errmsg_t>	mErrmsg;
	std::string			mThreadName;
	int					mThreadId;
	u32					mTotal;

public:
	AosError(const std::string &fname,
			 const int lineno,
			 const std::string &errmsg, 
			 const std::string &threadname, 
			 const int thread_id);
	~AosError() {}

	std::string toString();
	bool		addNewInstance(const std::string &errmsg);
	u32			getHashKey();
	bool		isSameObj(const AosErrorPtr &rhs) const;
};

#endif

