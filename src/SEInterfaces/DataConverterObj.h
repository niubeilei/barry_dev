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
// 2013/08/14 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataConverterObj_h
#define Aos_SEInterfaces_DataConverterObj_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "XmlUtil/Ptrs.h"

class AosDataConverterObj : virtual public AosJimo
{
public:
	AosDataConverterObj(const u32 version);

	virtual bool run(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &data) = 0; 

	virtual bool run(
					const AosRundataPtr &rdata,
					const OmnString &data) = 0; 
};
#endif
