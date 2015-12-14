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
//
// Modification History:
// 2013/03/17 Commented out by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Omn_Logger_LoggerFile_h
#define Omn_Logger_LoggerFile_h

#include "Logger/Logger.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlParser/Ptrs.h"


class OmnFileLogger : public OmnLogger
{
	OmnDefineRCObject;
private:
	enum FlushType
	{
		eImmediate,
		eHouseKeep,
		eAuto
	};

	enum
	{
		eDefaultFlushFreq
	};

	FlushType		mFlushType;
	int				mFlushFreq;
	OmnString		mFileName;
	OmnString		mBanner;
	OmnFilePtr		mFile;
	bool			mAppend;

public:
	OmnFileLogger(const OmnString &logName,
				  const OmnString &fileName, 
				  const OmnString &banner);
	OmnFileLogger(const OmnString &name, 
				  const OmnXmlItemPtr &conf);
	virtual ~OmnFileLogger();

	// 
	// Logger interface
	//
	virtual bool 		openLogger();
	virtual bool 		closeLogger();
	virtual bool 		config(const OmnXmlItemPtr &conf);

	void	log(const OmnString &contents, const bool flush);
	void	checkFlush(const int crtSec);
};

#endif
#endif
