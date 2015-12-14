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
// 2013/01/013 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/QueryEngineObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/BitmapEngineObj.h"
#include "SEInterfaces/QueryProcCallback.h"

AosQueryEngineObjPtr AosQueryEngineObj::smObject;
static AosXmlTagPtr		sgJimoDoc;
static OmnMutex			sgLock;




bool
AosQueryEngineObj::runQueryStatic(
		const AosRundataPtr &rdata, 
		const AosQueryReqObjPtr &query, 
		const AosQueryProcCallbackPtr &callback)
{
	aos_assert_rr(smObject, rdata, false);
	return smObject->runQuery(rdata, query, callback);
}


bool
AosQueryEngineObj::hasSectionsOnCube(
		const AosRundataPtr &rdata, 
		const int cube_id, 
		const vector<u64> &queried_sections, 
		vector<u64> &expected_sections)
{
	// This function determines whether there are contents on Cube 'cube_id'. 
	// If 'queried_sections' is empty, it means we don't know what sections to
	// cover. It returns true (meaning there may be contents on the remote).
	// Otherwise, it filters all the sections in 'queried_sections' for the
	// ones that are hosted on 'cube_id'.
	if (queried_sections.size() == 0) return true;

	expected_sections.clear();
	for (u32 i=0; i<queried_sections.size(); i++)
	{
		if (AosBitmapEngineObj::getCubeId(queried_sections[i]) == cube_id)
		{
			expected_sections.push_back(queried_sections[i]);
		}
	}

	if (expected_sections.size() > 0) return true;
	return false;
}



AosQueryEngineObj::AosQueryEngineObj(const int version)
:
AosJimo(AosJimoType::eQueryEngine, version)
{
}


static bool AosCreateQueryEngineJimoDoc(const AosRundataPtr &rdata)
{
	if (sgJimoDoc) return true;
	OmnString jimostr = "<jimo ";
	jimostr << AOSTAG_CURRENT_VERSION << "=\"1\" "
		<< "zky_classname=\"AosQueryEngine\" "
		<< "zky_otype=\"" << AOSOTYPE_JIMO << "\" "
		<< "zky_objid=\"query_engine_jimo\">"
		<< "<versions>"
		<< 		"<ver_1>libQueryEngine.so</ver_1>"
		<< "</versions>"
		<< "</jimo>";

	sgJimoDoc = AosStr2Xml(rdata.getPtr(), jimostr AosMemoryCheckerArgs);
	aos_assert_r(sgJimoDoc, 0);
	return sgJimoDoc;
}


AosQueryEngineObjPtr 
AosQueryEngineObj::getQueryEngine()
{
	if (smObject) return smObject;

	sgLock.lock();
	if (smObject)
	{
		sgLock.unlock();
		return smObject;
	}

	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	if (!sgJimoDoc)
	{
		if (!AosCreateQueryEngineJimoDoc(rdata))
		{
			sgLock.unlock();
			OmnAlarm << "bitmapengineobj_internal_error" << enderr;
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

	if (jimo->getJimoType() != AosJimoType::eQueryEngine)
	{
		OmnAlarm << "bitmapengineobj_invalid_jimo" << enderr;
		sgLock.unlock();
		return 0;
	}

	smObject = dynamic_cast<AosQueryEngineObj*>(jimo.getPtr());
	sgLock.unlock();
	if (!smObject)
	{
		OmnAlarm << "internal_error" << enderr;
		return 0;
	}

	return smObject;
}


