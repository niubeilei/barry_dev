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
// 2013/05/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataCalc_DataCalc_h
#define Aos_DataCalc_DataCalc_h

#include "SEInterfaces/DataCalcObj.h"


class AosDataCalc : public AosDataCalcObj
{
	OmnDefineRCObject;

protected:
	OmnString 	mName;

public:
	AosDataCalc(const OmnString &name, 
			const OmnString &libname, 
			const OmnString &method, 
			const OmnString &version);
	~AosDataCalc();

	virtual bool run(const AosRundataPtr &rdata, 
					AosValueRslt &input,
					AosValueRslt &output);
	virtual bool run(const AosRundataPtr &rdata, 
					const char *field, 
					const int len,
					AosValueRslt &output);
};

#endif
