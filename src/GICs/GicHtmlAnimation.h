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
// 2011/02/18: Created by Michael Yang 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_GICs_GicHtmlAnimation_h
#define Aos_GICs_GicHtmlAnimation_h

#include "GICs/GIC.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"



class AosGicHtmlAnimation : public AosGic
{

public:
	AosGicHtmlAnimation(const bool flag);
	~AosGicHtmlAnimation();

	virtual bool	
	generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code);
		
private:
};

#endif

