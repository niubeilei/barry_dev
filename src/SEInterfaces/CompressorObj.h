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
// 06/04/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_CompressorObj_h
#define Aos_SEInterfaces_CompressorObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/CompressorType.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosCompressorObj : virtual public OmnRCObject
{
protected:
	OmnString				mName;
	AosCompressorType::E	mType;
	bool					mIsTemplate;

public:
	AosCompressorObj()
	:
	mType(AosCompressorType::eInvalid),
	mIsTemplate(false)
	{
	}

	AosCompressorObj(
				const OmnString &name, 
				const AosCompressorType::E type, 
				const bool reg);
	~AosCompressorObj();

	// AosCompressor Interface
	virtual bool compress(
				const AosBuffPtr &input, 
				const AosRundataPtr &rdata) = 0;

	virtual AosCompressorObjPtr clone(
				const AosXmlTagPtr &def, 
				const AosRundataPtr &rdata) = 0;

	static AosCompressorObjPtr getCompressorStatic(
				const AosCompressorType::E type, 
				const AosRundataPtr &rdata);

	static AosCompressorObjPtr getCompressorStatic(
				const AosXmlTagPtr &item, 
				const AosRundataPtr &rdata);

private:
	static bool registerCompressor(const AosCompressorObjPtr &partitioner);
};
#endif

