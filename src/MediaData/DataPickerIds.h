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
#ifndef Aos_MediaData_DataPickerIds_h
#define Aos_MediaData_DataPickerIds_h

#include "Util/String.h"

#define AOSDATAPICKER_ELECTRO_CARDIIOGRAM	"elect_cardiogram"
#define AOSDATAPICKER_ELECTRO_CARDIIOGRAM2	"elect_cardiogram2"

class AosDataPickerId
{
public:
	enum E
	{
		eInvalid, 

		eElectroCardioGram, 
		eElectroCardioGram2, 

		eMax
	};

	static bool isValid(const E c)
	{
		return c > eInvalid && c < eMax;
	}

	static OmnString toStr(const E c);
	static E toEnum(const OmnString &name);
};
#endif

