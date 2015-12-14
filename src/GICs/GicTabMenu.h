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
// 07/08/2010: Created by John Huang
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GICs_GicTabMenu_h
#define Aos_GICs_GicTabMenu_h

#include "GICs/GIC.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlServer/HtmlReqProc.h"


class AosGicTabMenu : public AosGic
{

public:
	AosGicTabMenu(const bool flag);
	~AosGicTabMenu();

	virtual bool	
	generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code);
	bool
	changToArray(
		OmnString &str,
		vector<OmnString> &temp);
};

#endif
