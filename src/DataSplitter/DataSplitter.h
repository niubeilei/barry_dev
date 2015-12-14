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
// 2013/11/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataSplitter_DataSplitter_h
#define Aos_DataSplitter_DataSplitter_h

#include "SEInterfaces/DataSplitterObj.h"


class AosDataSplitter : public AosDataSplitterObj
{
	OmnDefineRCObject;

protected:
	u64					mJobDocid;

public:
	AosDataSplitter(const OmnString &type, const int ver);
	~AosDataSplitter();
	virtual void setJobDocid(const u64 &job_docid);
};
#endif



