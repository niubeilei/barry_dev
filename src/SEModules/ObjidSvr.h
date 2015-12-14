////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 10/22/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEModules_ObjidSvr_h
#define AOS_SEModules_ObjidSvr_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEUtil/Objid.h"
#include "SEUtil/Cloudid.h"
#include "Util/String.h"
#include "XmlInterface/XmlRc.h"
#include "Rundata/Rundata.h"


class AosObjidSvr
{
private:
	AosObjidSvr();
	~AosObjidSvr();

	enum
	{
		eMaxObjidTries = 100,
		eSecLength = 3,
		eNotPossible = 2
	};

public:
	static bool
	ensureObjidUnique(
			OmnString &objid, 
			const u64 &docid, 
			const bool checkFirst,
			const AosRundataPtr &rdata)
	{
		OmnString prefix, cid;
		bool rslt = AosObjid::decomposeObjid(objid, prefix, cid);
		aos_assert_r(rslt, false);
		aos_assert_r(docid, false);
	
		return ensureObjidUnique(objid, prefix, cid, docid, checkFirst, rdata);
	}

	static bool
	ensureObjidUnique(
		OmnString &objid, 
		const OmnString &prefix, 
		const OmnString &cid, 
		const u64 &docid,
		const bool checkFirst,
		const AosRundataPtr &rdata);
};
#endif

