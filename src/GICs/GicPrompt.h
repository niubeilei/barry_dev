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
// 07/16/2010: Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GICs_GicPrompt_h
#define Aos_GICs_GicPrompt_h

#include "GICs/GIC.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"



class AosGicPrompt : public AosGic
{

public:
	AosGicPrompt(const bool flag);
	~AosGicPrompt();

	virtual bool	
	generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj,
		const OmnString &parentId,
		AosHtmlCode &code);

private:
	bool	
	convertToJson(
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		OmnString &jsonStr,
		const OmnString &div_id,
		const OmnString &label_id,
		const OmnString &gic_value);
};


#endif

