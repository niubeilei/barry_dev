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
// 02/04/2011: Created by Cody
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GICs_GicHtmlButton_h
#define Aos_GICs_GicHtmlButton_h

#include "GICs/GIC.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosGicHtmlButton : public AosGic
{

public:
	AosGicHtmlButton(const bool flag);
	~AosGicHtmlButton();

	virtual bool	
	generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj, 
		const OmnString &parentid,
		AosHtmlCode &code);
};

#endif

