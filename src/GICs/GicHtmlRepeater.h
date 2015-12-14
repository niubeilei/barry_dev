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
// 10/01/2011: Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GICs_GicHtmlRepeater_h
#define Aos_GICs_GicHtmlRepeater_h

#include "GICs/GIC.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlUtil/HtmlUtil.h"


class AosGicHtmlRepeater : public AosGic
{
private:
	int			mItemHeight;
	OmnString	mColor0;
	OmnString	mColor1;
	OmnString	mTplName;
public:
	AosGicHtmlRepeater(const bool flag);
	~AosGicHtmlRepeater();

	virtual bool	
	generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj, 
		const OmnString &parentid, 
		AosHtmlCode &code);
};

#endif

