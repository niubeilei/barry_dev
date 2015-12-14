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
// 2011/02/19: Created by Ken
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GICs_GicHtmlTab_h
#define Aos_GICs_GicHtmlTab_h

#include "GICs/GIC.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include <vector>


class AosGicHtmlTab : public AosGic
{

public:
	AosGicHtmlTab(const bool flag);
	~AosGicHtmlTab();

	virtual bool	
	generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code);
			
private:
	
	OmnString
	generateTemplate(OmnString &cont, const AosHtmlReqProcPtr &htmlPtr);


	OmnString
	getRecordValue(const AosXmlTagPtr &record, const OmnString &path, OmnString &dft);
	
	OmnString
	createHtmlCode(const OmnString &temp, const AosXmlTagPtr &record);

	int 
	getActiveNum(const int &divwidth,const AosXmlTagPtr &vpd);
};

#endif

