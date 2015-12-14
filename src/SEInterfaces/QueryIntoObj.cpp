////////////////////////////////////////////////////////////////////////////
//
// copyright (c) 2005
// packet engineering, inc. all rights reserved.
//
// redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of packet engineering, inc. or its derivatives
//
// description:
//
// modification history:
// 09/12/2015 created by barry niu
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/QueryIntoObj.h"

#include "Jimo/Jimo.h"

AosQueryIntoObjPtr
AosQueryIntoObj::createQueryIntoStatic(
		const AosXmlTagPtr &def,
		AosRundata *rdata)
{
	aos_assert_r(def, NULL);

	OmnString name = def->getAttrStr("name");
	AosJimoPtr jimo = AosCreateJimoByName(rdata, "query_into", name, 1);
	aos_assert_r(jimo, NULL);
	AosQueryIntoObj* queryInto = dynamic_cast<AosQueryIntoObj*>(jimo.getPtr());
	return queryInto;
}
