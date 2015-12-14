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
// This class serves as the super class for all buff based records.
//
// Modification History:
// 2013/12/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "LoggerJimos/LogAccessPassed.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DataField/DataField.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/SchemaID.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/XmlTag.h"


u64 AosLogAccessPassed::smSchemaID = 0;
bool AosLogAccessPassed::smLogEnabled = true;

static bool sgInited = false;
static OmnMutex sgLock;

extern "C"
{
AosJimoPtr AosCreateJimoFunc_LogAccessPassed_0(const AosRundataPtr &rdata, const u32 version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosLogAccessPassed(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetError(rdata, "internal_error") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosLogAccessPassed::AosLogAccessPassed(const u32 version)
:
AosBuffLogger("access_passed_log", version)
{
	if (!sgInited) 
	{
		smSchemaID = AosGetSchemaDocid(AOS_SCHEMA_ID_ACCESS_PASSED_LOG);
		sgInited = true;
	}
}


AosLogAccessPassed::~AosLogAccessPassed()
{
}


bool
AosLogAccessPassed::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	mFields.clear();
	mFieldIdxs.clear();
	mRecordDoc = def->clone(AosMemoryCheckerArgsBegin); 
	mName = mRecordDoc->getAttrStr(AOSTAG_NAME, "access_passed_log");

	int idx = 0;
	mFieldIdxs["schema_id"] 		= idx++;
	mFieldIdxs["client_id"] 		= idx++;
	mFieldIdxs["entity_id"] 		= idx++;
	mFieldIdxs["entity_name"] 		= idx++;
	mFieldIdxs["operation"] 		= idx++;
	mFieldIdxs["username"] 			= idx++;
	mFieldIdxs["accessed_objid"] 	= idx++;
	mFieldIdxs["creation_time"] 	= idx++;

	if (mFieldIdxs.size() != eNumFields)
	{
		AosSetEntityError(rdata, "internal_error", "Access Passed Log", mName) << enderr;
		return false;
	}

	mValues.resize(mFieldIdxs.size());

	return true;
}


bool
AosLogAccessPassed::createLog(
		const AosRundataPtr &rdata,
		AosSecReq &sec_req,
		const AosXmlTagPtr &access_tag)
{
	// The log is in the format:
	//  SchemaID        (u64)
	//  ClientID		(int)
	//  EntityID        (String)
	//  EntityName      (String)
	//  Operation       (String)
	//  UserName        (String)
	//  AccessedObjid   (String)
	//  CreationTime    (u32)
	if (!smLogEnabled) return true;
	if (smSchemaID == 0) return true;
	
	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	
	int idx = 0;
	buff->setU64(smSchemaID); idx++;        	// Schema ID
	buff->setInt(AosGetSelfClientId()); idx++;             	// Client ID
	buff->setOmnStr("access_passed"); idx++;               	// Entry ID
	buff->setOmnStr("Access Passed");  idx++;              	// Entry Name
	buff->setOmnStr(sec_req.getOprStr()); idx++;            // Operation
	buff->setOmnStr(sec_req.getRequesterUsername()); idx++; // Username
	buff->setOmnStr(sec_req.getAccessedObjid()); idx++;     // Accessed Objid
	buff->setU32(OmnTime::getCrtSec()); idx++;             	// The creation time

	aos_assert_rr(idx == eNumFields, rdata, false);
	return true;
}


bool	
AosLogAccessPassed::parseData(const AosRundataPtr &rdata)
{
	aos_assert_rr(mFields.size() == eNumFields, rdata, false);
	
	mBuff->reset();
	int idx = 0;

	mBuff->getU64(0);									// Schema ID (pop off)
	mValues[idx++].setInt(mBuff->getInt(-1));			// ClientID
	mValues[idx++].setStrValue(mBuff->getOmnStr(""));		// Entry ID	
	mValues[idx++].setStrValue(mBuff->getOmnStr(""));		// Entry Name
	mValues[idx++].setStrValue(mBuff->getOmnStr(""));		// Entry ID	
	mValues[idx++].setStrValue(mBuff->getOmnStr(""));		// Operation 
	mValues[idx++].setStrValue(mBuff->getOmnStr(""));		// Username
	mValues[idx++].setStrValue(mBuff->getOmnStr(""));		// Accessed Objid 
	mValues[idx++].setU32(mBuff->getU32(0));				// Creation Time

	aos_assert_rr(idx == eNumFields, rdata, false);
	return true;
}

