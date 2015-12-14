////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// This is a utility to select docs.
//
// Modification History:
// 04/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataGenUtil_DataGenSimpleRand_h
#define AOS_DataGenUtil_DataGenSimpleRand_h

#include "DataGenUtil/DataGenUtil.h"
#include "SEUtil/Ptrs.h"


class AosDataGenSimpleRand : public AosDataGenUtil 
{
private:
	int mMin;
	int mMax;

public:
	AosDataGenSimpleRand(const bool reg);
	AosDataGenSimpleRand(const AosXmlTagPtr &config, const AosRundataPtr &rdata);
	AosDataGenSimpleRand();
	~AosDataGenSimpleRand();

	virtual bool nextValue(
			AosValueRslt &value,
			const AosXmlTagPtr &sdoc, 
			const AosRundataPtr &rdata);

	virtual bool nextValue(AosValueRslt &value, const AosRundataPtr &rdata);

	OmnString	RandomInteger(int min, int max);

	virtual AosDataGenUtilPtr clone(const AosXmlTagPtr &config, const AosRundataPtr &rdata);
	
	bool parse(const AosXmlTagPtr &config, const AosRundataPtr &rdata);

};
#endif

