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
#ifndef Aos_GICs_GicHtmlList_h
#define Aos_GICs_GicHtmlList_h

#include "GICs/GIC.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include <vector>
#include "HtmlModules/Ptrs.h"
#include "HtmlModules/HandleImage.h"
#include "HtmlModules/DclDb.h"
#include "HtmlModules/DataCol.h"

class AosGicHtmlList : public AosGic
{

public:
	AosGicHtmlList(const bool flag);
	~AosGicHtmlList();

	virtual bool	
	generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code);
			
private:
	
	OmnString generateTemplate(OmnString &cont, const AosHtmlReqProcPtr &htmlPtr);
	OmnString getNewValue(OmnString &val,const OmnString &str_len);

	OmnString createHtmlCode(
					const OmnString &temp,
					const OmnString &str_len, 
					const AosXmlTagPtr &record, 
					const AosHtmlReqProcPtr &htmlPtr);

	bool retrieveContainers(
					const AosHtmlReqProcPtr &htmlPtr,
					AosXmlTagPtr &vpd,
					const AosXmlTagPtr &obj,
					const OmnString &parentid,
					AosHtmlCode &code);
};

#endif

