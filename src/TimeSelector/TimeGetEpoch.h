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
#ifndef AOS_TimeSelector_TimeGetEpoch_h
#define AOS_TimeSelector_TimeGetEpoch_h

#include "TimeSelector/TimeSelector.h"


class AosTimeGetEpoch : public AosTimeSelector 
{

public:
	AosTimeGetEpoch(const bool reg);
	~AosTimeGetEpoch();

	virtual bool  run(
			AosValueRslt &value,
			const AosXmlTagPtr &sdoc, 
			const AosRundataPtr &rdata);

};
#endif

