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
#ifndef Aos_MediaData_ElectroCardioGram_h
#define Aos_MediaData_ElectroCardioGram_h

#include "MediaData/DataPicker.h"


class AosElectroCardioGram : public AosDataPicker
{
private:
	typedef struct 
	{
		int baseline;
		int max_thd;
		int min_thd;

		int max0;
		int min0;
	} WorkingData_t;

public:
	AosElectroCardioGram(const bool regflag);
	~AosElectroCardioGram()
	{
	}

	virtual bool pickData(
				const OmnString &data,
				OmnString &rslts, 
				const AosXmlTagPtr &sdoc,
				const AosRundataPtr &rdata);

private:
	bool initWorkingData(
				const double *data, 
				const int segment_size,
				AosElectroCardioGram::WorkingData_t *working_data);
};
#endif

