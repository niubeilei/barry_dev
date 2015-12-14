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
// 08/07/2010: Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GICs_GicGrid_h
#define Aos_GICs_GicGrid_h

#include "GICs/GIC.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"



class AosGicGrid : public AosGic
{
private:
	enum 
	{
		eMaxColumns = 100
	};

public:
	AosGicGrid(const bool flag);
	~AosGicGrid();

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
			AosXmlTagPtr &vpd,
			const AosXmlTagPtr &obj,
			AosHtmlCode &code,
			const AosHtmlReqProcPtr &htmlPtr);
};
#endif

