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
#ifndef AOS_TimeSelector_TimeMonthWeek2Epoch_h
#define AOS_TimeSelector_TimeMonthWeek2Epoch_h

#include "TimeSelector/TimeSelector.h"


class AosTimeMonthWeek2Epoch : public AosTimeSelector 
{

public:
	AosTimeMonthWeek2Epoch(const bool reg);
	~AosTimeMonthWeek2Epoch();

	virtual bool  run(
			AosValueRslt &value,
			const AosXmlTagPtr &sdoc, 
			const AosRundataPtr &rdata);
private:
	bool  getValue(
			int &value,
			const AosXmlTagPtr &tag, 
			const AosRundataPtr &rdata);

	int64_t getDetailsTime(
			const int64_t &delt,
			const AosXmlTagPtr &sdoc, 
			const AosRundataPtr &rdata);



};
#endif

