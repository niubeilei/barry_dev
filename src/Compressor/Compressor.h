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
#ifndef Aos_Compressor_Compressor_h
#define Aos_Compressor_Compressor_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/CompressorObj.h"
#include "SEInterfaces/CompressorType.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosCompressor : virtual public AosCompressorObj
{
private:

public:
	AosCompressor(
				const OmnString &name, 
				const AosCompressorType::E type,
				const bool regflag);
	~AosCompressor();

	static bool init();
};

#endif

