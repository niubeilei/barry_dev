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
// 06/04/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Sorter_Sorter_h
#define Aos_Sorter_Sorter_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/SorterObj.h"
#include "SEInterfaces/SorterType.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosSorter : virtual public AosSorterObj
{
private:

public:
	AosSorter(
				const OmnString &name, 
				const AosSorterType::E type,
				const bool regflag);
	~AosSorter();

	static bool init();
};

#endif

