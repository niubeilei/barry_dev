////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 08/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CompressUtil/Compress.h"

#include "CompressUtil/GZip.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"

static AosCompressPtr 	sgColComp[AosCompressType::eMax];
static OmnString		sgNames[AosCompressType::eMax];
static OmnMutex			sgLock;
static OmnMutex			sgInitLock;
static bool				sgInited = false;

AosCompress::AosCompress(
		const OmnString &name, 
		const AosCompressType::E type,
		const bool flag)
:
mType(type)
{
	if (flag)
	{
		 AosCompressPtr thisptr(this, false);   // bool false: true :  
		if (!registerColComp(name, thisptr))
		{
			OmnThrowException("Failed registering");
			return;
		}
	}
}


AosCompress::~AosCompress()
{
}


bool
AosCompress::registerColComp(const OmnString &name, const AosCompressPtr  &b)
{
	sgLock.lock();
	if (!AosCompressType::isValid(b->mType))
	{
		sgLock.unlock();
		OmnString errmsg = "Incorrect data shuffler type: ";
		errmsg << b->mType;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (sgColComp[b->mType])
	{
		sgLock.unlock();
		OmnString errmsg = "Proc already registered: ";
		errmsg << b->mType;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgColComp[b->mType] = b;
	AosCompressType::addName(name, b->mType);
	sgLock.unlock();
	return true;
}


bool
AosCompress::init(const AosRundataPtr &rdata)
{
	static AosGZip 		lsGZip(true);

	sgInitLock.lock();
	sgInited = true;
	AosCompressType::check();
	sgInitLock.unlock();
	return true;
}

AosCompressPtr
AosCompress::getCompress(const OmnString &typestr, const AosRundataPtr &rdata)
{
	if (!sgInited) init(rdata);
	aos_assert_r(typestr != "", 0);
	AosCompressType::E type = AosCompressType::toEnum(typestr);
	aos_assert_rr(AosCompressType::isValid(type), rdata, 0);
	AosCompressPtr obj = sgColComp[type];
	return obj;
}


bool
AosCompress::compressStatic(
		const OmnString &type, 
		AosBuffPtr &compressedBuff,
		u64 &compressedLen,
		const AosBuffPtr &sourceBuff, 
		const int sourceLen, 
		const AosRundataPtr &rdata AosMemoryCheckDecl)
{
	AosCompressPtr obj = getCompress(type, rdata);
	aos_assert_r(obj, false);
	return obj->dataCompress(
		compressedBuff, compressedLen, sourceBuff->data(),
		sourceLen, rdata AosMemoryCheckerFileLine);
}

bool
AosCompress::uncompressStatic(
		const OmnString &type,
		AosBuffPtr &buff,
		const u64 &len,
		const AosBuffPtr &compressedBuff,
		const int compressedLen, 
		const AosRundataPtr &rdata AosMemoryCheckDecl)
{
	AosCompressPtr obj = getCompress(type, rdata);
	aos_assert_r(obj, false);
	buff = OmnNew AosBuff(len AosMemoryCheckerFileLine);
	bool rslt =  obj->dataUncompress(
		buff->data(), len, compressedBuff->data(), compressedLen, rdata);
	aos_assert_r(rslt, false);
	buff->setDataLen(len);
	return true;
}

//bool
//AosCompress::compressStatic(
//		const OmnString &type, 
//		AosBuffPtr &destBuff,         
//		u64 &destLen,          
//		const char *sourceData, 
//		const int sourceLen, 
//		const AosRundataPtr &rdata)
//{
//	AosCompressPtr obj = getCompress(type, rdata);
//	aos_assert_r(obj, false);
//	return obj->dataCompress(destBuff, destLen, sourceData, 
//			sourceLen, rdata);
//}

//bool
//AosCompress::uncompressStatic(
//		const OmnString &type,
//		char *destData,
//		u64 &destLen,        
//		const char *sourceData,
//		const int sourceLen, 
//		const AosRundataPtr &rdata)
//{
//	AosCompressPtr obj = getCompress(type, rdata);
//	aos_assert_r(obj, false);
//	return obj->dataUncompress(destData, destLen, sourceData, 
//			sourceLen, rdata);
//}
