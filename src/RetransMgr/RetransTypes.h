////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RetransTypes.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Retrans_RetransEnum_h
#define Omn_Retrans_RetransEnum_h

#include "Util/Array.h"

class OmnRetransEnum
{
public:
	enum 
	{
		eMaxTriggerPoints = 20,
		eMaxSchedules = 20
	};
};

typedef OmnArray<int, OmnRetransEnum::eMaxTriggerPoints>			OmnRetransSchedule;
typedef OmnArray<OmnRetransSchedule, OmnRetransEnum::eMaxSchedules>	OmnRetransSchedules;

#endif
