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
// 2014/07/27 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_LogParser_LogParserBinSingle_h
#define AOS_LogParser_LogParserBinSingle_h

#include "LogParserBinSingle/Ptrs.h"
#include "SEInterfaces/LogParserBinSingleObj.h"
#include "Thread/Ptrs.h"
#include <vector>

class AosLogParserBinSingle : public AosLogParserBinSingleObj
{
private:
	AosDataRecordObjPtr		mRecord;
	AosDataRecordObj *		mRecordRaw;

public:
	AosLogParserBinSingle(const int version);

	~AosLogParserBinSingle();

	// Jimo interface
	virtual bool config(	const AosRundataPtr &rdata, 
							const AosXmlTagPtr &worker_doc,
							const AosXmlTagPtr &jimo_doc);

	// LogParserBinSingle Interface
};
#endif

