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
// 01/29/2012 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Tracer_FileLog_h
#define Omn_Tracer_FileLog_h

#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/File.h"

#define AosFileLog1 AosFileTracer::getFileLog1() << "<" << __FILE__ << ":" << __LINE__ << ">: "
#define AosFileLog2 AosFileTracer::getFileLog2() << "<" << __FILE__ << ":" << __LINE__ << ">: "

class AosFileTracer
{
private:
	OmnFilePtr	 		 mFile;
	static bool			 smInited;
	static AosFileTracer smLog1;
	static AosFileTracer smLog2;

public:
	AosFileTracer();

	bool openFile(const OmnString &name);

	static bool init();

	static AosFileTracer & getFileLog1()
	{
		if (!smInited) init();
		return smLog1;
	}

	static AosFileTracer & getFileLog2()
	{
		if (!smInited) init();
		return smLog2;
	}

	AosFileTracer & operator << (const u64 &value)
	{
		OmnString vv;
		vv << value;
		return *this << vv;
	}
	AosFileTracer & operator << (const OmnString &str)
	{
		if (!mFile || !mFile->isGood()) return *this;
		mFile->append(str);
		return *this;
	}
	AosFileTracer & operator << (ostream & (*f)(ostream &outs))
	{
		return *this << "\n";
	}
};
#endif

