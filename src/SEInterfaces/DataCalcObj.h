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
// 2013/05/05 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataCalcObj_h
#define Aos_SEInterfaces_DataCalcObj_h

#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DLLObj.h"
#include "Rundata/Ptrs.h"
#include "Util/String.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/Ptrs.h"

class AosDataCalcObj : public AosDLLObj
{
protected:
	OmnString		mCalcName;

public:
	AosDataCalcObj(const OmnString &name, 
					const OmnString &libname, 
					const OmnString &method,
					const OmnString &version);
	virtual ~AosDataCalcObj();

	virtual bool run(const AosRundataPtr &rdata, 
					AosValueRslt &input,
					AosValueRslt &output) = 0;
	virtual bool run(const AosRundataPtr &rdata, 
					const char *field, 
					const int len,
					AosValueRslt &output) = 0;
};
#endif

