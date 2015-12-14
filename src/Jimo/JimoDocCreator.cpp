////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2014/04/12 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Jimo/JimoDocCreator.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DllMgrObj.h"
#include "XmlUtil/XmlTag.h"

static OmnMutex sgLock;
static bool     sgInited = false;
AosJimoDocCreator::map_t AosJimoDocCreator::smMap;

bool
AosJimoDocCreator::init()
{
	sgLock.lock();
	if (sgInited)
	{
		sgLock.unlock();
		return true;
	}

	smMap["concat"] = "libQueryFuncJimos.so";
	
	sgLock.unlock();
	return true;
}


AosXmlTagPtr 
AosJimoDocCreator::createJimoDoc(
		const AosRundataPtr &rdata, 
		const OmnString &objid)
{
	if (!sgInited) init();

	itr_t itr = smMap.find(objid);
	if (itr == smMap.end()) return 0;

	OmnString libname = itr->second;

	OmnString jimostr = "<jimo ";
	jimostr << AOSTAG_CURRENT_VERSION << "=\"1\" "
		<< "zky_otype=\"" << AOSOTYPE_JIMO << "\" "
		<< "zky_objid=\"" << objid << "\">"
		<< "<versions>"
		<< 		"<version_1>" << libname << "</version_1>"
		<< "</versions>"
		<< "</jimo>";

	AosXmlTagPtr jimo_doc = AosStr2Xml(rdata.getPtr(), jimostr AosMemoryCheckerArgs);
	aos_assert_rr(jimo_doc, rdata, 0);
	return jimo_doc;
}


