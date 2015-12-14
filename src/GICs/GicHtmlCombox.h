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
#ifndef Aos_GICs_GicHtmlCombox_h
#define Aos_GICs_GicHtmlCombox_h

#include "GICs/GIC.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

using namespace std;


class AosGicHtmlCombox : public AosGic
{

public:
	AosGicHtmlCombox(const bool flag);
	~AosGicHtmlCombox();

	virtual bool	
	generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code);
		
private:
	bool
	generateCmbData(
		const OmnString &cmb_entries,
		const OmnString &cmb_entry_type,
		OmnString &sOptions);
	
	bool
	generateDclData(
		const AosHtmlReqProcPtr &htmlPtr,
		const AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &cmb_text_bind,
		const OmnString &cmb_value_query_dcl,
		OmnString &sOptions);
};

#endif

