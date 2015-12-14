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
// 2013/05/03 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataCalcDyn_DataCalcWordCount_h
#define Aos_DataCalcDyn_DataCalcWordCount_h

#include "DataCalc/DataCalc.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/ValueRslt.h"

class AosDataCalcWordCount : public AosDataCalc
{
public:
	AosDataCalcWordCount(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &sdoc, 
			const OmnString &libname,
			const OmnString &method,
			const OmnString &version);
	~AosDataCalcWordCount();

	virtual bool run(const AosRundataPtr &rdata, 
					AosValueRslt &input,
					AosValueRslt &output);
	virtual bool run(const AosRundataPtr &rdata, 
					const char *field, 
					const int len,
					AosValueRslt &output);

private:
	bool config( 		const AosRundataPtr &rdata,
						const AosXmlTagPtr &def);
};
#endif
