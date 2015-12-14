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
//   
//
// Modification History:
// 12/24/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "MediaData/DataPickerIds.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"


#define AOSDATAPICKER_ELECTRO_CARDIIOGRAM	"elect_cardiogram"
#define AOSDATAPICKER_ELECTRO_CARDIIOGRAM2	"elect_cardiogram2"
#define AOSDATAPICKER_INVALID				"invalid"


OmnString 
AosDataPickerId::toStr(const E c)
{
	switch (c)
	{
	case eElectroCardioGram:
		 return AOSDATAPICKER_ELECTRO_CARDIIOGRAM;

	case eElectroCardioGram2:
		 return AOSDATAPICKER_ELECTRO_CARDIIOGRAM2;

	default:
		 break;
	}

	OmnAlarm << "Unrecognized data picker algorithm: " << (int)c << enderr;
	return AOSDATAPICKER_INVALID;
}


AosDataPickerId::E
AosDataPickerId::toEnum(const OmnString &name)
{
	if (name == AOSDATAPICKER_ELECTRO_CARDIIOGRAM) return eElectroCardioGram;
	if (name == AOSDATAPICKER_ELECTRO_CARDIIOGRAM2) return eElectroCardioGram2;
	return eInvalid;
}

