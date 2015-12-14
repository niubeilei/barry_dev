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
// 06/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataShuffler/IILShufflerProc.h"

#include "Rundata/Rundata.h"

AosIILShuffler::AosIILShuffler(const bool flag)
:
AosDataShuffler(AOSDATASHUFFLER_IIL, AosDataShufflerType::eIIL, flag),
mStartPos(-1),
mLength(-1)
{
}

AosIILShuffler::AosIILShuffler(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
:
AosDataShuffler(AOSDATASHUFFLER_IIL, AosDataShufflerType::eIIL, false)
{
	try
	{
		// 1. Retrieve mStartPos
		mStartPos = conf->getAttrInt(AOSTAG_START_POS, 0);
		if (mStartPos < 0)
		{
			AosSetErrorU(rdata, "invalid_config") << ": " << conf->toString() << enderr;
			return;
		}

		// 2. Retrieve mLength
		mLength = conf->getAttrInt(AOSTAG_LENGTH, -1);
		if (mLength <= 0)
		{
			AosSetErrorU(rdata, "invalid_config") << ": " << conf->toString() << enderr;
			return;
		}

	}
	catch(...)
	{
		OmnAlarm << "Failed creating object" << enderr;
	}
}


AosIILShuffler::~AosIILShuffler()
{
}


int 
AosIILShuffler::getRoute(const char *data, const int len, const AosRundataPtr &rdata)
{
	aos_assert_r(data, -1);
	aos_assert_r(len > 0, -1);
	aos_assert_r(mStartPos > 0 && mStartPos < len, -1);
	aos_assert_r(mLength > 0 && mLength <= len, -1);
	OmnString docid(&data[mStartPos], mLength);
	aos_assert_r(docid != "", -1);
	u64 distid = docid.parseU64(0);
	int idx = route(distid); 
	return idx;
}


int
AosIILShuffler::route(const u64 &distid)
{
	return -1;
}


AosDataShufflerPtr
AosIILShuffler::clone(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosIILShuffler(conf, rdata);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object: " << conf->toString() << enderr;
		return 0;
	}
}
#endif
