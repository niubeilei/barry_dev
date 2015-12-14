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
// 2013/08/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/TransBitmapQueryAgentObj.h"

#include "API/AosApiS.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"


static AosXmlTagPtr		sgJimoDoc;
static OmnMutex			sgLock;


AosTransBitmapQueryAgentObj::AosTransBitmapQueryAgentObj(const int version)
:
AosJimo(AosJimoType::eTransBitmapQueryAgent, version)
{
}


AosTransBitmapQueryAgentObj::~AosTransBitmapQueryAgentObj()
{
}

AosTransBitmapQueryAgentObjPtr	
AosTransBitmapQueryAgentObj::getAgent(
			const AosRundataPtr &rdata, 
			const u64 term_id,
			const vector<OmnString> &iilnames,
			const vector<AosQueryRsltObjPtr> &node_list,
			const vector<AosBitmapObjPtr> &partial_bitmaps, 
			const vector<u32> &expected_sections)
{
	// create jimo
	sgLock.lock();
	if (!sgJimoDoc)
	{
		if (!AosCreateTransBitmapQueryAgentJimoDoc(rdata))
		{
			OmnAlarm << "bitmapengineobj_internal_error" << enderr;
			sgLock.unlock();
			return 0;
		}
	
		if (!sgJimoDoc)
		{
			OmnAlarm << "bitmapengineobj_internal_error" << enderr;
			sgLock.unlock();
			return 0;
		}
	}
	
	AosJimoPtr jimo = AosCreateJimo(rdata.getPtr(), sgJimoDoc);
	if (!jimo)
	{
		OmnAlarm << "bitmapengineobj_internal_error" << enderr;
		sgLock.unlock();
		return 0;
	}
	
	if (jimo->getJimoType() != AosJimoType::eTransBitmapQueryAgent)
	{
		OmnAlarm << "bitmapengineobj_invalid_jimo" << enderr;
		sgLock.unlock();
		return 0;
	}
	sgLock.unlock();
			
	AosTransBitmapQueryAgentObjPtr agent = dynamic_cast<AosTransBitmapQueryAgentObj*>(jimo.getPtr());
	agent->config(
			rdata, 
			term_id,
			iilnames,
			node_list,
			partial_bitmaps, 
			expected_sections);

	return agent;
}

bool 
AosTransBitmapQueryAgentObj::AosCreateTransBitmapQueryAgentJimoDoc(const AosRundataPtr &rdata)
{
	if (sgJimoDoc) return true;
	OmnString jimostr = "<jimo ";
	jimostr << AOSTAG_CURRENT_VERSION << "=\"1\" "
		<< "zky_classname=\"AosTransBitmapQueryAgent\" "
		<< "zky_otype=\"" << AOSOTYPE_JIMO << "\" "
		<< "zky_objid=\"_jimo_trans_bitmap_query_agent\">"
		<< "<versions>"
		<< 		"<version_1>libQueryEngine.so</version_1>"
		<< "</versions>"
		<< "</jimo>";

	sgJimoDoc = AosStr2Xml(rdata.getPtr(), jimostr AosMemoryCheckerArgs);
	aos_assert_r(sgJimoDoc, 0);
	return sgJimoDoc;
}

