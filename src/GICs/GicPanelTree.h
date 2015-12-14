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
// 10/18/2010: Created by Max 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GICs_GicPanelTree_h
#define Aos_GICs_GicPanelTree_h

#include "GICs/GIC.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlUtil/HtmlUtil.h"



class AosGicPanelTree : public AosGic
{

public:
	AosGicPanelTree(const bool flag);
	~AosGicPanelTree();

	virtual bool	
	generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj, 
		const OmnString &parentid, 
		AosHtmlCode &code);

private:
	bool
	convertToJson(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj, 
		const OmnString &parentid, 
		AosHtmlCode &code	
			);

};

#endif

