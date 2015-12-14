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
// This class implements errors. When program encounters an error, an
// instance of this class is generated. Each instance may record multiple
// occurances of the same error, which is identified by (filename, lineno).
// This means that when a new error occurs, if the error occurred before,
// instead of creating a new instance, it adds the error message to the
// existing one. Each error message is associated with a counter. When 
// adding a new error message to an instance of this class, it checks
// whether the same error message is already in the instance. If yes, 
// it increments the error message's counter instead of inserting the
// error message to the instance. 
//
// Note that checking error messages can be time consuming. But we 
// consider a program should not generate too many errors. If it generates
// too many errors, whether it consumes too much CPU or not is not 
// a big concern for us anymore.
//
// Modification History:
// 12/05/2007	Modified by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Debug/Error.h"

#include "Debug/Ptrs.h"
#include "Util/a_string.h"

AosError::AosError(const std::string &fname,
			 	   const int lineno,
			 	   const std::string &errmsg, 
			 	   const std::string &thread_name, 
			 	   const int thread_id)
:
mFilename(fname),
mLine(lineno),
mThreadName(thread_name),
mThreadId(thread_id),
mTotal(0)
{
	mErrmsg.push_back(Errmsg_t(errmsg));
}


std::string 
AosError::toString()
{
	std::string tmp;
	tmp = "********** Error **********\n";
	tmp << "Location: " << mFilename << ":" << mLine << "\n";
	tmp << "Thread: " << mThreadName << ":" << mThreadId << "\n";
	tmp << "Errors: " << mTotal << "\n";

	std::list<Errmsg_t>::iterator itr;
	for (itr = mErrmsg.begin(); itr != mErrmsg.end(); itr++)
	{
		tmp << "   " << (*itr).mCount << ": " << (*itr).mErrmsg << "\n";
	}
	tmp << "**************************\n";

	return tmp;
}


bool
AosError::addNewInstance(const std::string &errmsg)
{
	mTotal++;
	std::list<Errmsg_t>::iterator itr;
	for (itr = mErrmsg.begin(); itr != mErrmsg.end(); itr++)
	{
		if ((*itr).mErrmsg == errmsg)
		{
			(*itr).mCount++;
			return true;
		}
	}

	// 
	// Not found. Check whether there are too many errors
	// already. If yes, return.
	//
	if (mErrmsg.size() >= eMaxInstances) return false;
	
	mErrmsg.push_back(Errmsg_t(errmsg));
	return true;
}


u32			
AosError::getHashKey()
{
	return (u32)(mLine + mThreadId);
}


bool		
AosError::isSameObj(const AosErrorPtr &rhs) const
{
	return (mFilename == rhs->mFilename && mLine == rhs->mLine);
}

#endif

