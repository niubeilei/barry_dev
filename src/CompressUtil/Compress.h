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
#ifndef Aos_CompressUtil_Compress_h
#define Aos_CompressUtil_Compress_h

#include "CompressUtil/Ptrs.h"
#include "CompressUtil/CompressType.h"
#include "Rundata/Ptrs.h"
#include "Util/ValueRslt.h"

class AosCompress : virtual public OmnRCObject
{
	OmnDefineRCObject;        

private:
	AosCompressType::E		mType;

public:
	AosCompress(
			const OmnString &name, 
			const AosCompressType::E type,
			const bool flag);
	~AosCompress();

	virtual bool dataCompress(
			AosBuffPtr &destBuff, 
			u64 &destLen, 
			const char *sourceData,
			const int sourceLen,
			const AosRundataPtr &rdata AosMemoryCheckDecl) = 0;

	virtual bool dataUncompress(
			char *destData, 
			const u64 &destLen,
			const char *sourceData,
			const int sourceLen,
			const AosRundataPtr &rdata) = 0;

	static AosCompressPtr getCompress(const OmnString  &typestr, const AosRundataPtr &rdata);

	static bool compressStatic(
			const OmnString  &typestr, 
			AosBuffPtr &compressedBuff,
			u64 &compressedLen,
			const AosBuffPtr &sourceBuff,
			const int sourceLen,
			const AosRundataPtr &rdata AosMemoryCheckDecl);

	static bool uncompressStatic(
			const OmnString &type,
			AosBuffPtr &buff,
			const u64 &len,
			const AosBuffPtr &compressedBuff,
			const int compressedLen,
			const AosRundataPtr &rdata AosMemoryCheckDecl);

//	static bool compressStatic(
//			const OmnString  &typestr, 
//			AosBuffPtr &compressedBuff,
//			u64 &compressedLen,
//			const char *sourceData,
//			const int sourceLen,
//			const AosRundataPtr &rdata);
//
//	static bool uncompressStatic(
//			const OmnString &type,
//			char *destData,
//			u64 &destLen,
//			const char *compressedData,
//			const int compressedLen,
//			const AosRundataPtr &rdata);
protected:
	bool config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	bool registerColComp(const OmnString &name, const AosCompressPtr  &blob);
	static bool init(const AosRundataPtr &rdata);
};

#endif

