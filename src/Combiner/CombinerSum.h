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
#ifndef Aos_Combiner_CombinerSum_h
#define Aos_Combiner_CombinerSum_h

#include "Combiner/Combiner.h"


class AosCombinerSum : virtual public AosCombiner
{
	OmnDefineRCObject;

private:

public:
	AosCombinerSum(const bool regflag);
	AosCombinerSum(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosCombinerSum();

	// AosCombiner Interface
	virtual bool combine(
				const AosDataTablePtr &input, 
				AosDataTablePtr &output, 
				const AosRundataPtr &rdata);

	virtual AosCombinerObjPtr clone(
				const AosXmlTagPtr &def, 
				const AosRundataPtr &rdata);

private:
	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
};

#endif

