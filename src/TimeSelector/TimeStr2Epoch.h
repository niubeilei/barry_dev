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
#ifndef AOS_TimeSelector_TimeStr2Epoch_h
#define AOS_TimeSelector_TimeStr2Epoch_h

#include "TimeSelector/TimeSelector.h"


class AosTimeStr2Epoch : public AosTimeSelector 
{

public:
	AosTimeStr2Epoch(const bool reg);
	~AosTimeStr2Epoch();

	virtual bool  run(
			AosValueRslt &value,
			const AosXmlTagPtr &sdoc, 
			const AosRundataPtr &rdata);

};
#endif

