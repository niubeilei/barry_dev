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
#include "Compressor/Compressor.h"

#include "Compressor/CompressorConv.h"
#include "Debug/ExitHandler.h"
#include "Debug/Debug.h"

AosCompressor::AosCompressor(
		const OmnString &name, 
		const AosCompressorType::E type,
		const bool regflag)
:
AosCompressorObj(name, type, regflag)
{
}


AosCompressor::~AosCompressor()
{
}


bool
AosCompressor::init()
{
	static AosCompressorConv			lsCompressorConv(true);

	if (!AosCompressorType::check())
	{
		OmnExitApp("Some compressors were not initialized");
		return false;
	}

	OmnScreen << "All compressors are initialized" << endl;
	return true;
}
