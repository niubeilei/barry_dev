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
// Modification History:
// 2015/09/12 Created by Barry
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/QueryFormatObj.h"

AosQueryFormatObjPtr
AosQueryFormatObj::createQueryFormatStatic(
		const AosXmlTagPtr &def,
		AosRundata *rdata)
{
	aos_assert_r(def, NULL);
	OmnString name = def->getAttrStr("name");
	AosJimoPtr jimo = AosCreateJimoByName(rdata, "query_format", name, 1);
	aos_assert_r(jimo, NULL);
	AosQueryFormatObj* format = dynamic_cast<AosQueryFormatObj*>(jimo.getPtr());
	return format;
}




