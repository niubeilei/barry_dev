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
// 2013/05/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CondsDyn_Sample_h
#define Aos_CondsDyn_Sample_h

#include "Rundata/Rundata.h"
#include "SEInterfaces/DLLCondObj.h"
#include "Util/ValueRslt.h"

class AosDLLCondSample : public AosDLLCondObj
{
public:
	AosDLLCondSample(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &sdoc, 
			const OmnString &libname,
			const OmnString &method,
			const OmnString &version);
	~AosDLLCondSample();

	virtual bool evalCond(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual bool evalCond(const AosRundataPtr &rdata);
	virtual bool evalCond(const char *record, const int len, const AosRundataPtr &rdata);
	virtual bool evalCond(const AosDataRecordObjPtr &record, const AosRundataPtr &rdata);
	virtual bool evalCond(const AosValueRslt &value, const AosRundataPtr &rdata);

private:
	bool config( 		const AosRundataPtr &rdata,
						const AosXmlTagPtr &def);
};
#endif
