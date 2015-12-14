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
// 03/08/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "StorageEngClient/SotrageEngClient.h"


AosStorageEngClient::AosStorageEngClient()
{
}


AosStorageEngClient::~AosStorageEngClient()
{
}


bool 
AosStorageEngClient::config(const AosXmlTagPtr &conf)
{
	return true;
}


bool 
AosStorageEngClient::start()
{
	return true;
}


bool 
AosStorageEngClient::stop()
{
	return true;
}


u64 
AosStorageEngClient::getSizeId(const int size, const AosRundataPtr &rdata)
{
}


bool 
AosStorageEngClient::createSizeId(
		const int size, 
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
}


bool 
AosStorageEngClient::removeSizeId(
		const u64 size, 
		const AosRundataPtr &rdata)
{
}

