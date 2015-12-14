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
#ifndef OMN_Accesses_Access_h
#define OMN_Accesses_Access_h

#include "Rundata/Ptrs.h"
#include "Accesses/DenyReasons.h"
#include "Accesses/AccessType.h"
#include "Accesses/Ptrs.h"
#include "Jimo/Jimo.h"
#include "SEBase/SecOpr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/HashUtil.h"
#include "Util/HashMap.h"
#include "XmlUtil/Ptrs.h"

class AosSecReq;

class AosAccess : public AosJimo
{
private:
	static bool			smLogPassed;
	static bool			smLogDenied;
	static u64			smSchemaIDAccessPassedLog;
	static u64			smSchemaIDAccessDeniedLog;

	typedef hash_map<const OmnString, AosAccessPtr, Omn_Str_hash, compare_str> Hash_t;
	typedef hash_map<const OmnString, AosAccessPtr, Omn_Str_hash, compare_str>::iterator HashItr_t;

	static Hash_t 	smAccessMap;

protected:
	OmnString	mName;

public:
	AosAccess(const OmnString &name);
	AosAccess(const OmnString &name, const int version);

	virtual bool checkAccess(
					const AosRundataPtr &rdata, 
					AosSecReq &sec_req, 
					const AosXmlTagPtr &tag, 
					bool &granted,
					bool &denied) = 0;

	static bool config(const AosXmlTagPtr &conf);
	static bool init(const AosRundataPtr &rdata);

	static bool checkAccessStatic(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &tag,
					AosSecReq &sec_req,
					bool &granted,
					bool &denied);

protected:
	bool logPassed( const AosRundataPtr &rdata,
					AosSecReq &req,
					const AosXmlTagPtr &tag);

	bool logDenied( const AosRundataPtr &rdata,
					AosSecReq &req,
					const AosXmlTagPtr &access_tag, 
					const OmnString &deny_reason);

	bool logError(
					const AosSecReq &req,
					const AosXmlTagPtr &tag, 
					const OmnString &errmsg,
					const AosRundataPtr &rdata);

private:
	bool registerAccess(const AosAccessPtr &access, const OmnString &name);
};
#endif

