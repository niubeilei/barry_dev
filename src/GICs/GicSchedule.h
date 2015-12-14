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
// 07/15/2010: Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GICs_GicSchedule_h
#define Aos_GICs_GicSchedule_h

#include "GICs/GIC.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"



class AosGicSchedule : public AosGic
{

public:
	AosGicSchedule(const bool flag);
	~AosGicSchedule();

	virtual bool	
	generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj,
		const OmnString & parentId,
		AosHtmlCode &code
	);
private:
};
#endif

