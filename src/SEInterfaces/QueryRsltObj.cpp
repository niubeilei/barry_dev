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
// 11/22/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/QueryRsltObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"



AosQueryRsltObjPtr	AosQueryRsltObj::smObject;

AosQueryRsltObjPtr 
AosQueryRsltObj::getQueryRsltStatic()
{
	aos_assert_r(smObject, 0);
	return smObject->getQueryRslt();
}


void 
AosQueryRsltObj::setQueryRslt(const AosQueryRsltObjPtr &obj)
{
	smObject = obj;
}

	
bool
AosQueryRsltObj::exchangeContentStatic(
		const AosQueryRsltObjPtr &rslt1,
		const AosQueryRsltObjPtr &rslt2)
{
	aos_assert_r(smObject, 0);
	return smObject->exchangeContent(rslt1, rslt2);
}

