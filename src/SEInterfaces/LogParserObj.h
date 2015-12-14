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
// 2014/07/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_LogParserObj_h
#define AOS_SEInterfaces_LogParserObj_h

#include "Jimo/Jimo.h"
#include "Jimo/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosLogParserObj : public AosJimo
{
protected:
	static AosLogParserObjPtr	smLogParserObj;

public:
	AosLogParserObj(const int version);

	~AosLogParserObj();

	static AosLogParserObjPtr getLogParserStatic(AosRundata *rdata);
	static AosLogParserObjPtr createLogParserStatic(
								const OmnString &type, 
								AosRundata *rdata);

	virtual AosLogParserObjPtr createLogParser(
								const OmnString &type,
								AosRundata *rdata) = 0;

private:
	static bool createJimoDocStatic(AosRundata *rdata);

};
#endif

