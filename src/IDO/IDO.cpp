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
// 2015/03/12 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IDO/IDO.h"

#include "Rundata/Rundata.h"
#include "Util/Buff.h"

AosIDO::AosIDO()
:
mVersion(-1),
mMayUseLocalCopy(true),
mDocid(0)
{
}


AosIDO::AosIDO(const u64 docid)
:
mVersion(-1),
mMayUseLocalCopy(true),
mDocid(docid)
{
}


AosIDO::AosIDO(
		const OmnString &name,
		const OmnString &classname, 
		const OmnString &jimo_name,
		const int version)
:
mIDOName(name),
mClassname(classname),
mJimoName(jimo_name),
mVersion(version),
mMayUseLocalCopy(true),
mDocid(0)
{
}


AosIDO::~AosIDO()
{
}


AosIDOPtr
AosIDO::createIDO(AosRundata *rdata, AosBuff *buff)
{
	// 'buff' is encoded as:
	// 		IDO Name	OmnString
	// 		classname	OmnString
	// 		jimo_name	OmnString
	// 		version		int
	// 		...
	//
	OmnString name = buff->getOmnStr("");
	aos_assert_rr(name != "", rdata, 0);

	OmnString classname = buff->getOmnStr("");
	OmnString jimo_name = buff->getOmnStr("");
	int version = buff->getInt(-1);
	aos_assert_rr(version >= 0, rdata, 0);

	AosJimoPtr jimo;
	if (jimo_name != "")
	{
		jimo = AosCreateJimoByName(rdata, jimo_name, version);
	}
	else if (classname != "")
	{
		jimo = AosCreateJimoByClassname(rdata, classname, version);
	}

	aos_assert_rr(jimo, rdata, 0);

	aos_assert_rr(jimo->getJimoType() == AosJimoType::eIDO, rdata, 0);

	AosIDOPtr ido = dynamic_cast<AosIDO *>(jimo.getPtr());
	aos_assert_rr(ido, rdata, 0);

	buff->reset();
	bool rslt = ido->serializeFrom(rdata, buff);
	aos_assert_rr(rslt, rdata, 0);
	return ido;
}


bool 
AosIDO::serializeFrom(AosRundata *rdata, AosBuff *buff)
{
	mIDOName = buff->getOmnStr("");
	mClassname = buff->getOmnStr("");
	mJimoName = buff->getOmnStr("");
	mVersion = buff->getInt(-1);
	return true;
}


bool 
AosIDO::serializeTo(AosRundata *rdata, AosBuff *buff) const
{
	buff->setOmnStr(mIDOName);
	buff->setOmnStr(mClassname);
	buff->setOmnStr(mJimoName);
	buff->setInt(mVersion);
	return true;
}

