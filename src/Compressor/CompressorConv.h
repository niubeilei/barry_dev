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
#ifndef Aos_Compressor_CompressorConv_h
#define Aos_Compressor_CompressorConv_h

#include "Compressor/Compressor.h"


class AosCompressorConv : virtual public AosCompressor
{
	OmnDefineRCObject;

private:

public:
	AosCompressorConv(const bool regflag);
	AosCompressorConv(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosCompressorConv();

	// AosCompressor Interface
	virtual bool compress(
				const AosBuffPtr &input, 
				const AosRundataPtr &rdata);

	virtual AosCompressorObjPtr clone(
				const AosXmlTagPtr &def, 
				const AosRundataPtr &rdata);

private:
	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
};

#endif

