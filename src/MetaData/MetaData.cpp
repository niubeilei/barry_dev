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
// 2015/03/24 Created by Jackie 
////////////////////////////////////////////////////////////////////////////
#include "MetaData/MetaData.h"

AosMetaData::AosMetaData()
{
	mRecordLength = 0;
}

AosMetaData::~AosMetaData()
{
}

void 
AosMetaData::setRecordLength(int64_t &length) 
{ 
	mRecordLength = length; 
}
