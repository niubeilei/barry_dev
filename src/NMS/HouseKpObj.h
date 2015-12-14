////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HouseKpObj.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_NMS_HouseKpObj_h
#define Omn_NMS_HouseKpObj_h

#include "aosUtil/Types.h"
#include "Porting/LongTypes.h"
#include "Util/String.h"

class OmnHouseKp;


class OmnHouseKpObj 
{
public:
	virtual ~OmnHouseKpObj() {}
	virtual void		procHouseKeeping(const int64_t &tick) = 0;
	virtual OmnString	getName() const = 0;
};
#endif
