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
// 07/08/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GICs_GicHtmlCheckboxTwo_h
#define Aos_GICs_GicHtmlCheckboxTwo_h
			
#include "GICs/GIC.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"



class AosGicHtmlCheckboxTwo : public AosGic
{

public:
	AosGicHtmlCheckboxTwo(const bool flag);
	~AosGicHtmlCheckboxTwo();

	virtual bool	
	generateCode(
	    const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code);
	bool
	getData(
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const AosHtmlReqProcPtr &htmlPtr);
	bool createHtmlCode(AosHtmlCode &code);
	bool	
	createJsonCode(
			AosXmlTagPtr &vpd,
			AosHtmlCode &code);
	bool createXml();
private:
	AosXmlTagPtr       mData;
	OmnString          mMain;
	OmnString          mNext;
	OmnString          mHeadTip;
};

#endif

