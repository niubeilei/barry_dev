////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Except.h
// Description:
//    This class defines the exception that all MMSN program throws.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Debug_Except_h
#define Omn_Debug_Except_h

#include "Debug/ErrId.h"
#include "Debug/ExceptId.h"
#include "Debug/Rslt.h"
#include "Porting/StrUtil.h"
#include "Util/BasicTypes.h"
#include "Util/String.h"

#ifndef OmnCreateException
#define OmnCreateException OmnExcept except(__FILE__, __LINE__); except 
#endif

#ifndef OmnThrowException
#define OmnThrowException(errmsg) throw OmnExcept(__FILE__, __LINE__, errmsg);
#endif

#ifndef OmnThrowException2
#define OmnThrowException2(errId, errmsg) \
		OmnExcept e(__FILE__, __LINE__, errId, errmsg); \
		throw e
#endif

class OmnExcept
{
public:
	enum ExceptRslt
	{
		eInvalid,
		eContinueTry
	};

private:
	OmnString			mFile;			// Where the exception was generated
	int					mLine;			// Where the exception was generated
	OmnString			mErrmsg;		// The error messages
	OmnErrId::E			mErrid;

public:
	OmnExcept(const OmnString &file,
		const int line,
		const OmnErrId::E errId,
		const OmnString &errMsg);

	OmnExcept(const OmnString &file, const int line, const OmnString &errmsg);

	~OmnExcept();

	OmnString		getErrmsg() const 
					{
						OmnString str;
						/*
						str << "<" << mFile << ":" << mLine << "> " 
							<< OmnErrId::toStr(mErrid) << ": " << mErrmsg;
							*/
						return str;
					}

	OmnString		toString() const 
					{
						OmnString str;
						/*
						str << "[" << mFile << ":" << mLine
							<< "] " << OmnErrId::toStr(mErrid) << ":" << mErrmsg;
							*/
						return str;
					}

	OmnErrId::E		getErrid() const {return mErrid;}
	OmnRslt			getRslt() const {return OmnRslt(mErrid, mErrmsg);}

	ExceptRslt		procException() const;

	OmnExcept &	operator << (const OmnString &msg)
	{
		mErrmsg << msg; 
		return *this;
	}

	OmnExcept & operator << (const int &value)
	{
		mErrmsg << value;
		return *this;
	}

	OmnExcept & operator << (const int64_t value)
	{
		mErrmsg << value;
		return *this;
	}

	OmnExcept & operator << (const u64 &value)
	{
		mErrmsg << value;
		return *this;
	}
};

#ifndef OmnExceptionBegin
#define OmnExceptionBegin							\
	{												\
		int __omnNumberOfExceptionTries = 0;		\
		while (__omnNumberOfExceptionTries < 3)		\
		{
#endif

#ifndef OmnStatExceptionRetry
#define OmnStatExceptionRetry __omnNumberOfExceptionTries++;
#endif

#ifndef OmnExceptionEnd
#define OmnExceptionEnd } }
#endif

#ifndef OmnThrowException
#define OmnThrowException(x) \
		OmnExcept e(__FILE__, __LINE__, OmnErrId::eAlarmProgramError, x); \
		throw e
#endif
#endif
