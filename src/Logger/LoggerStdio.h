////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: LoggerStdio.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Omn_Logger_LoggerStdio_h
#define Omn_Logger_LoggerStdio_h

#include "Logger/Logger.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlParser/Ptrs.h"


class OmnStdioLogger : public OmnLogger
{
	OmnDefineRCObject;
private:

public:
	OmnStdioLogger(const OmnString &name);
	virtual ~OmnStdioLogger();

	// Logger interface
	virtual bool 		openLogger();
	virtual bool 		closeLogger();
	virtual bool		config(const OmnXmlItemPtr &conf);

	void 	log(const OmnString &contents);
};

#endif
#endif
