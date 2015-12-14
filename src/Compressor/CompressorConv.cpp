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
#include "Compressor/CompressorConv.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"

AosCompressorConv::AosCompressorConv(const bool regflag)
:
AosCompressor(AOSCOMPRESSOR_CONVENTIONAL, AosCompressorType::eConv, regflag)
{
}


AosCompressorConv::AosCompressorConv(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosCompressor(AOSCOMPRESSOR_CONVENTIONAL, AosCompressorType::eConv, false)
{
	if (!config(def, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosCompressorConv::~AosCompressorConv()
{
}


bool
AosCompressorConv::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosCompressorConv::compress(
		const AosBuffPtr &input, 
		const AosRundataPtr &rdata)
{
	// This sorter sorts records by some fields in the records.
	OmnNotImplementedYet;
	return false;
}


AosCompressorObjPtr 
AosCompressorConv::clone(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosCompressorConv(def, rdata);
	}

	catch (...)
	{
		return 0;
	}
}

