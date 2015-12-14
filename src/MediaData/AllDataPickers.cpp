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
// 12/24/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "MediaData/AllDataPickers.h"

#include "MediaData/Ptrs.h"
#include "MediaData/ElectroCardioGram.h"
#include "MediaData/ElectroCardioGram2.h"
#include "Util/HashUtil.h"


//AosStr2U32_t         sgNameMap;    
//AosU322Str_t         sgCodeMap;
AosDataPickerPtr     sgPickers[AosDataPickerId::eMax];
AosAllDataPickers	 gAosAllDataPickers;

AosAllDataPickers::AosAllDataPickers()
{
static	AosElectroCardioGram lsElectroCardioGram(true);
static	AosElectroCardioGram2 lsElectroCardioGram2(true);
}
