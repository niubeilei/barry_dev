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
// 02/21/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartObj/ValueMap/SobjValueMapClient.h"


SobjValueMapClient::SobjValueMapClient()
{
}


SobjValueMapClient::~SobjValueMapClient()
{
}


bool 
SobjValueMapClient::getMappedValue(
			const u64 &iilid,
			const int64_t &key, 
			int64_t &value,
			const AosRundataPtr &rdata)
{
	// In the current implementations, this is implemented through 
	// IIL. In the future, it will be implemented through a special
	// class.
	//
	// The IIL is a u64 IIL. Its value portion is the key and the 
	// value portion is the value being mapped.
	AosQueryRsltPtr results = AosGetTableContents(iilid, rdata);
	if (!results) return false;
	results->reset();
	OmnString kk;
	u64 docid;
	while (results->nextDocid(docid, kk))
	{
		int64_t vv = atoll(kk);
		if (key < vv) 
		{
			value = docid;
			return true;
		}
	}
	return false;
}

