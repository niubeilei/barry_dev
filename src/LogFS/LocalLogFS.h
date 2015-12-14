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
// 2014/09/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_LogFS_LocalLogFS_h
#define AOS_LogFS_LocalLogFS_h

#include "LogFS/LogFS.h"



class AosLocalLogFS : public AosLogFS
{
	OmnDefineRCObject;

public:
	AosLocalLogFS();
	~AosLocalLogFS();

	virtual bool config(AosRundata *rdata, const AosXmlTagPtr &def);

	virtual bool appendData(AosRundata *rdata, 
						const char *data, 
						const int len, 
						u64 &pos);

	virtual bool appendData(AosRundata *rdata, 
						const char *data, 
						const int len, 
						u64 &pos, 
						AosFmtPtr &fmt);

	virtual bool createNextLogFile(AosRundata *rdata);
};
#endif

