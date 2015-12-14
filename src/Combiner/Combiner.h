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
#ifndef Aos_Combiner_Combiner_h
#define Aos_Combiner_Combiner_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/CombinerObj.h"
#include "SEInterfaces/CombinerType.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosCombiner : virtual public AosCombinerObj
{
private:

public:
	AosCombiner(
				const OmnString &name, 
				const AosCombinerType::E type,
				const bool regflag);
	~AosCombiner();

	static bool init();
};

#endif

