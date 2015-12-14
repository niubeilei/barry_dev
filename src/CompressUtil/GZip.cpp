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
// 08/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CompressUtil/GZip.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"
#include <zlib.h>

AosGZip::AosGZip(const bool flag)
:
AosCompress(AOSCOMPRESS_GZIP, AosCompressType::eGZip, flag)
{
}


AosGZip::~AosGZip()
{
}


bool 
AosGZip::dataCompress(
		AosBuffPtr &destBuff,
		u64 &destLen,
		const char *sourceData,
		const int sourceLen,
		const AosRundataPtr &rdata AosMemoryCheckDecl)
{
	aos_assert_r(sourceLen > 0, false);
	uLong len = compressBound(sourceLen);
	aos_assert_r(len, false);
	destBuff = OmnNew AosBuff(len AosMemoryCheckerFileLine);
	int r = Z_BUF_ERROR;
	try
	{
		r = compress((unsigned char*)destBuff->data(), &len, (unsigned char*)sourceData, sourceLen);
	}
	catch (...)
	{
		OmnAlarm << "failed to compress!" << enderr;
		return false;
	}

	destLen = len;
	destBuff->setDataLen(destLen);
	if (r != Z_OK)
	{
		if (r == Z_MEM_ERROR)
		{
			rdata->setError() << "not enough memery!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		else if (r == Z_BUF_ERROR)
		{
			rdata->setError() << "buff not enough!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		else
		{
			rdata->setError() << "failed to compress!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}
	return true;
}


bool
AosGZip::dataUncompress(
		char *destData,
		const u64 &destLen,
		const char *sourceData,
		const int sourceLen,
		const AosRundataPtr &rdata)
{
	aos_assert_r(sourceLen > 0, false);
	uLong len = destLen;
	int r = Z_BUF_ERROR;
	try
	{
		 r = uncompress ((unsigned char*)destData, &len, (unsigned char*)sourceData, sourceLen);
	}
	catch (...)
	{
		OmnAlarm << "failed to uncompress!" << enderr;
		return false;
	}

	if (r != Z_OK)
	{
		if (r == Z_MEM_ERROR)
		{
			rdata->setError() << "not enough memery!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		else if (r == Z_BUF_ERROR)
		{
			rdata->setError() << "buff not enough!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		else
		{
			rdata->setError() << "failed to uncompress!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}

	// Ketty 2013/01/26
	aos_assert_r(destLen == len, false);
	//destLen = len;
	//aos_assert_r(destLen > 0, false);
	return true;
}

