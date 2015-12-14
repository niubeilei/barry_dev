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
// 12/24/2010	Created by Jackie
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_MediaData_ElectroCardioGram2_h
#define Aos_MediaData_ElectroCardioGram2_h

#include "MediaData/DataPicker.h"


class AosElectroCardioGram2 : public AosDataPicker
{
public:
	AosElectroCardioGram2(const bool regflag);
	~AosElectroCardioGram2()
	{
	}

	virtual bool pickData2(
				const OmnString &data,
				AosXmlTagPtr &rslts, 
				const AosXmlTagPtr &sdoc,
				const AosRundataPtr &rdata);
private:
	static int CompDouble(const void *p1, const void *p2);
};
#endif

