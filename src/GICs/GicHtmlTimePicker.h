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
// 2011/02/18: Created by Ken
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GICs_GicHtmlTimePicker_h
#define Aos_GICs_GicHtmlTimePicker_h

#include "GICs/GIC.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"



class AosGicHtmlTimePicker : public AosGic
{

public:
	AosGicHtmlTimePicker(const bool flag);
	~AosGicHtmlTimePicker();

	virtual bool	
	generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code);

	 bool  changeWeek(
			 const int &week,
			 OmnString &gic_week,
			 OmnString &gic_weekindx,
			 const OmnString &gic_language);
		
private:
};

#endif

