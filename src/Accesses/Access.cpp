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
// 10/24/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Accesses/Access.h"

#include "alarm_c/alarm.h"
#include "Accesses/AccessAdmin.h"
#include "Accesses/AccessForbidden.h"
#include "Accesses/AccessGroup.h"
#include "Accesses/AccessLoginPublic.h"
#include "Accesses/AccessPrivate.h"
#include "Accesses/AccessPublic.h"
#include "Accesses/AccessRole.h"
#include "Accesses/AccessRoot.h"
#include "Accesses/AccessTimedPublic.h"
#include "Accesses/AccessUserEnum.h"
#include "Accesses/AccessByManager.h"
#include "Accesses/AccessRelation.h"
#include "Accesses/AccessCond.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/SchemaMgrObj.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


AosAccess::Hash_t AosAccess::smAccessMap;
u64			AosAccess::smSchemaIDAccessPassedLog = 0;
u64			AosAccess::smSchemaIDAccessDeniedLog = 0;
static OmnMutex			sgLock;
static bool				sgInited = false;


// Static member data
bool AosAccess::smLogPassed = false;
bool AosAccess::smLogDenied = true;

AosAccess::AosAccess(const OmnString &name)
:
AosJimo(AosJimoType::eAccessControl, 0),
mName(name)
{
	//init();
}


AosAccess::AosAccess(
		const OmnString &name,
		const int version)
:
AosJimo(AosJimoType::eAccessControl, version),
mName(name)
{
}


bool
AosAccess::init(const AosRundataPtr &rdata)
{
	aos_assert_r(!sgInited, false);
	AosAccessPtr access;

	smAccessMap[AOSACCESSTYPE_ADMIN] 		= OmnNew AosAccessAdmin();
	smAccessMap[AOSACCESSTYPE_FORBIDDEN] 	= OmnNew AosAccessForbidden();
	smAccessMap[AOSACCESSTYPE_GROUP] 		= OmnNew AosAccessGroup();
	smAccessMap[AOSACCESSTYPE_LOGIN_PUBLIC] = OmnNew AosAccessLoginPublic();
	smAccessMap[AOSACCESSTYPE_PRIVATE] 		= OmnNew AosAccessPrivate();
	smAccessMap[AOSACCESSTYPE_PUBLIC] 		= OmnNew AosAccessPublic();
	smAccessMap[AOSACCESSTYPE_ROLE] 		= OmnNew AosAccessRole();
	smAccessMap[AOSACCESSTYPE_ROOT] 		= OmnNew AosAccessRoot();
	smAccessMap[AOSACCESSTYPE_TIMED_PUBLIC] = OmnNew AosAccessTimedPublic();
	smAccessMap[AOSACCESSTYPE_USER_ENUM] 	= OmnNew AosAccessUserEnum();
	smAccessMap[AOSACCESSTYPE_BYMANAGER] 	= OmnNew AosAccessByManager();
	smAccessMap[AOSACCESSTYPE_RELATION] 	= OmnNew AosAccessRelation();
	smAccessMap[AOSACCESSTYPE_CONDITION] 	= OmnNew AosAccessCond();

	// smSchemaIDAccessPassedLog = AosGetSchemaID(rdata, AOS_SCHEMA_TYPE_ACCESS_PASSED_LOG);
	// smSchemaIDAccessDeniedLog = AosGetSchemaID(rdata, AOS_SCHEMA_TYPE_ACCESS_DENIED_LOG);
	return true;
}


bool
AosAccess::config(const AosXmlTagPtr &conf)
{
	aos_assert_r(conf, false);
	smLogPassed = conf->getAttrBool("log_granted_accesses", false);
	smLogDenied = conf->getAttrBool("log_denied_accesses", true);

	return true;
}


bool 
AosAccess::checkAccessStatic(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &tag,
		AosSecReq &sec_req,
		bool &granted,
		bool &denied)
{
	aos_assert_rr(tag, rdata, false);
	HashItr_t itr = smAccessMap.find(tag->getAttrStr(AOSTAG_TYPE));
	AosAccessPtr access;
	if (itr == smAccessMap.end())
	{
		// Did not find the access. Check whether it is a jimo.
		AosJimoPtr jimo = AosCreateJimo(rdata.getPtr(), tag);
		if (!jimo)
		{
			granted = false;
			AosSetEntityError(rdata, "access_access_not_found", 
					"Access", "access_not_found") << tag << enderr;
			return false;
		}

		if (jimo->getJimoType() != AosJimoType::eAccessControl)
		{
			AosSetEntityError(rdata, "access_not_access_jimo", 
					"Access", "access_not_access_jimo") << tag << enderr;
			return false;
		}

		access = dynamic_cast<AosAccess*>(jimo.getPtr());
		if (!access)
		{
			AosSetError(rdata, "internal_error") << enderr;
			return false;
		}
	}
	else
	{
		access = itr->second;
	}

	aos_assert_rr(access, rdata, false);
	return access->checkAccess(rdata, sec_req, tag, granted, denied);
}


/*
bool
AosAccess::checkRegistrations()
{
	if (sgInited) return true;
	sgLock.lock();
	for (int i=AosAccessType::ePrivate; i<AosAccessType::eMax; i++)
	{
		if (!sgAccesses[i])
		{
			OmnAlarm << "Missing Security Access Registration: " << i << enderr;
		}
	}
	sgInited = true;
	sgLock.unlock();
	return true;
}
*/


bool 
AosAccess::logPassed(
		const AosRundataPtr &rdata,
		AosSecReq &sec_req,
		const AosXmlTagPtr &access_tag)
{
	// The log is in the format:
	// 	SchemaID		(u64)
	// 	EntityID		(String)
	// 	EntityName		(String)
	// 	Operation		(String)
	// 	UserName		(String)
	// 	AccessedObjid	(String)
	// 	CreationTime	(u32)
	if (!smLogPassed) return true;
	
	if (smSchemaIDAccessPassedLog == 0) init(rdata);
	if (smSchemaIDAccessPassedLog == 0) return true;

	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);

	buff->setU64(smSchemaIDAccessPassedLog);		// Schema ID
	buff->setInt(AosGetSelfClientId());				// Client ID
	buff->setOmnStr("access_passed");				// Entry ID
	buff->setOmnStr("Access Passed");				// Entry Name
	buff->setOmnStr(sec_req.getOprStr());				// Operation
	buff->setOmnStr(sec_req.getRequesterUsername());	// Username
	buff->setOmnStr(sec_req.getAccessedObjid());		// Accessed Objid
	buff->setU32(OmnTime::getCrtSec());				// The creation time

	return true;
}


bool 
AosAccess::logDenied(
		const AosRundataPtr &rdata,
		AosSecReq &req,
		const AosXmlTagPtr &access_tag, 
		const OmnString &deny_reason)
{
	// The log is in the format:
	// 	SchemaID		(u64)
	// 	EntityID		(String)
	// 	EntityName		(String)
	// 	DenyReason		(String)
	// 	Operation		(String)
	// 	UserName		(String)
	// 	AccessedObjid	(String)
	// 	CreationTime	(u32)
	// AosSetEntityError(rdata, deny_reason, "DenyReason", deny_reason) << enderr;
	if (!smLogPassed) return true;

	if (smSchemaIDAccessDeniedLog == 0) init(rdata);
	if (smSchemaIDAccessDeniedLog == 0) return true;

	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);

	buff->setU64(smSchemaIDAccessDeniedLog);		// Schema ID
	buff->setInt(AosGetSelfClientId());				// Client ID
	buff->setOmnStr("access_denied");				// Entry ID
	buff->setOmnStr("Access Denied");				// Entry Name
	buff->setOmnStr(deny_reason);					// Deny Reason
	buff->setOmnStr(req.getOprStr());				// Operation
	buff->setOmnStr(req.getRequesterUsername());	// Username
	buff->setOmnStr(req.getAccessedObjid());		// Accessed Objid
	buff->setU32(OmnTime::getCrtSec());				// The creation time

	return true;
}


bool 
AosAccess::logError(
		const AosSecReq &req,
		const AosXmlTagPtr &tag, 
		const OmnString &errmsg,
		const AosRundataPtr &rdata)
{
	// The log is in the format:
	// 	<accesslog zky_oprid="xxx" 
	// 		zky_requester="requester_docid"
	// 		zky_accessed="accessed_docid"
	// 		AOSTAG_PARENT_C=smLoContainer
	// 		zky_tags="tags from 'tag'"
	// 		zky_time="xxx"
	// 		zky_deny_reason=deny_reason>
	// 		errmsg
	// 	</accesslog>
	
	// Not implemented yet
	return true;
}

