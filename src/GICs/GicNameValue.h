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
// 08/07/2010: Created by ketty
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GICs_GicNameValue_h
#define Aos_GICs_GicNameValue_h

#include "GICs/GIC.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"



class AosGicNameValue : public AosGic
{

public:
	AosGicNameValue(const bool flag);
	~AosGicNameValue();

	virtual bool	
	generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj,
		//const AosGicType &gictype,
		const OmnString &parentid,
		AosHtmlCode &code);

private:
};

#endif

