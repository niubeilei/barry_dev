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
// 08/06/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryCond_QueryCond_h
#define Aos_QueryCond_QueryCond_h

#include "Alarm/Alarm.h"
#include "QueryCond/Ptrs.h"
#include "QueryCond/CondIds.h"
#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Opr.h"
#include "XmlUtil/Ptrs.h"


class AosQueryCond : virtual public OmnRCObject
{
	OmnDefineRCObject;

protected:
	AosCondId::E		mId;
	OmnString			mCondName;

public:
	AosQueryCond(const OmnString &name, const AosCondId::E id, const bool regflag);
	virtual ~AosQueryCond();

	// Query Condition Interface
	virtual bool			parse(const AosXmlTagPtr &def, const AosRundataPtr &rdata) = 0;
	virtual AosQueryCondPtr	clone() const = 0;
	virtual bool			toString(OmnString &str) = 0;

	static AosQueryCondPtr parseCondStatic(
				const AosXmlTagPtr &term, 
				const AosRundataPtr &rdata);

	static AosQueryCondPtr getCond(const OmnString &idstr);

	// Below are conversion functions. These should be removed after data conversion
	static bool convertCond();

	static AosQueryCondPtr createCond(const AosOpr opr, const u64 &value);

private:
	bool registerCond(const AosQueryCondPtr &cond, OmnString &errmsg);

	static AosQueryCondPtr getCondCloneStatic(const OmnString &idstr);
};
#endif

