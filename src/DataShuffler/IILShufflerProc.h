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
// 06/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_DataShuffler_IILShuffler_h
#define AOS_DataShuffler_IILShuffler_h

#include "DataShuffler/DataShuffler.h"

class AosIILShuffler : public AosDataShuffler
{
	OmnDefineRCObject;

private:
	int 		mStartPos;
	int 		mLength;
	int 		mNumServers;

public:
	AosIILShuffler(const bool flag);
	AosIILShuffler(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	~AosIILShuffler();

	virtual AosDataShufflerPtr clone(
					const AosXmlTagPtr &def, 
					const AosRundataPtr &rdata);

	virtual int getRoute(const char *data, const int len, const AosRundataPtr &rdata);
	int	route(const u64 &distid);
};
#endif
#endif
