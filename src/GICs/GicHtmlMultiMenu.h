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
// 2011/12/10: Created by Ken
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_GICs_GicHtmlMultiMenu_h
#define Aos_GICs_GicHtmlMultiMenu_h

#include "GICs/GIC.h"


class AosGicHtmlMultiMenu : public AosGic
{

public:
	AosGicHtmlMultiMenu(const bool flag);
	~AosGicHtmlMultiMenu();

	virtual bool	
	generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code);

};

#endif

