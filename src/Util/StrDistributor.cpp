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
// 07/25/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/StrDistributor.h"

AosStrDistributor::AosStrDistributor()
{
}


AosStrDistributor::~AosStrDistributor()
{
}


bool 
AosStrDistributor::distribute(
		const AosBuffArrayPtr &data, 
		const int num_buckets,
		vector<AosBuffArrayPtr> &buckets,
		const int key_start, 
		const int key_len,
		const int record_len,
		AosRundata *rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosStrDistributor::createIndex(const vector<OmnString> keys, AosRundata *rdata)
{
	OmnNotImplementedYet;
	return false;
}

