////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 08/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_CompressUtil_GZip_h
#define AOS_CompressUtil_GZip_h

#include "CompressUtil/Compress.h"

class AosGZip : public AosCompress 
{
public:
	AosGZip(const bool flag);
	~AosGZip();

	virtual bool dataCompress(
			AosBuffPtr &destBuff,
			u64 &destLen,
			const char *sourceData,
			const int sourceLen,
			const AosRundataPtr &rdata AosMemoryCheckDecl); 

	virtual bool dataUncompress(
			char *destdata, 
			const u64 &destLen,
			const char *sourceData,
			const int sourceLen,
			const AosRundataPtr &rdata);
};
#endif

