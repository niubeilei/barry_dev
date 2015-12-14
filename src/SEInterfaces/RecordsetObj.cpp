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
// 2013/11/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/RecordsetObj.h"

AosRecordsetObjPtr AosRecordsetObj::smObject;

AosRecordsetObjPtr 
AosRecordsetObj::createRecordsetStatic(
		AosRundata *rdata,
		const AosXmlTagPtr &def)
{
	aos_assert_r(smObject, 0);
	return smObject->createRecordset(rdata, def);
}
	
AosRecordsetObjPtr 
AosRecordsetObj::createRecordsetStatic(
		AosRundata *rdata,
		const AosDataRecordObjPtr &record)
{
	aos_assert_r(smObject, 0);
	return smObject->createRecordset(rdata, record);
}

AosRecordsetObjPtr 
AosRecordsetObj::createStreamRecordsetStatic(
		AosRundata *rdata,
		const AosDataRecordObjPtr &record)
{
	aos_assert_r(smObject, 0);
	return smObject->createStreamRecordset(rdata, record);
}

bool
AosRecordsetObj::checkEmpty(const AosRecordsetObjPtr &recordset)
{
	if (!recordset || recordset->size() <= 0) return true;
	return false;
}

