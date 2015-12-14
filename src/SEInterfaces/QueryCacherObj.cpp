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
// 2013/12/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/QueryCacherObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"

AosQueryCacherObjPtr AosQueryCacherObj::smCacher;

static AosXmlTagPtr sgQueryCacherJimoDoc;
static OmnMutex sgLock;


AosQueryCacherObj::AosQueryCacherObj()
{
}


AosQueryCacherObjPtr
AosQueryCacherObj::createQueryCacherStatic(const AosRundataPtr &rdata)
{
	if (smCacher) return smCacher->cloneCacher();

	sgLock.lock();
	OmnString jimostr = "<jimo ";
	jimostr << "current_version=\"1\" "
		<< "zky_classname=\"AosQueryCacher\" "
		<< AOSTAG_OTYPE << "=\"" << AOSOTYPE_JIMO << "\" "
		<< "zky_objid=\"query_cacher_jimo\">"
		<< "<versions>"
		<< 		"<version_1>libQueryCacher.so</version_1>"
		<< "</versions>"
		<< "</jimo>";

	AosXmlTagPtr jimo_doc = AosStr2Xml(rdata.getPtr(), jimostr AosMemoryCheckerArgs);
	sgLock.unlock();
	aos_assert_r(jimo_doc, 0);
	AosJimoPtr jimo = AosCreateJimo(rdata.getPtr(), jimo_doc);
	aos_assert_rr(jimo, rdata, 0);
	aos_assert_rr(jimo->getJimoType() == AosJimoType::eQueryCacher, rdata, 0);
	smCacher = dynamic_cast<AosQueryCacherObj*>(jimo.getPtr());
	aos_assert_rr(smCacher, rdata, 0);
	return smCacher->cloneCacher();
}


