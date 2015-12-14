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
// This is a simple inner join, or an inner join of two tables
//
// Modification History:
// 2015/01/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryProcJimos/SimpleJoinInner.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosSimpleJoinInner_1(
			const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosSimpleJoinInner(version);
			aos_assert_r(jimo, 0);
			return jimo;
		}

		catch (...)
		{
			AosSetErrorU(rdata, "Failed creating jimo") << enderr;
			return 0;
		}

		OmnShouldNeverComeHere;
		return 0;
	}
}


AosSimpleJoinInner::AosSimpleJoinInner(const int version)
:
AosQueryProcJimo(version, 1)
{
}


AosSimpleJoinInner::~AosSimpleJoinInner()
{
}


AosJimoPtr
AosSimpleJoinInner::cloneJimo() const
{
	try
	{
		return OmnNew AosSimpleJoinInner(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}


bool
AosSimpleJoinInner::procJoin(
		AosRundata *rdata, 
		AosQueryReqStmt *query, 
		AosJoinStmt *join)
{
	// There are two tables: mMasterTable and mJoinedTable. 
	// No virtual field has been created yet. This function 
	// creates the map. 
	vector<OmnString> key_fields = getKeyFields(rdata);
	if (key_fields.size() <= 0)
	{
		AosLogUserError(rdata, "missing_key_fields") << enderr;
		return false;
	}

	OmnString iilname = AosIILName::composeContainerListIILName(mJoinedTable->getTablename());
	aos_assert_rr(iilname != "", rdata, false);

	OmnString jimo_name = join->getCreateJoinMapJimoName(rdata)
	AosStreamJobObjPtr job;
	if (jimo_name != "")
	{
		AosJimoPtr jimo = AosCreateJimoByName(rdata, jimo_name);
		if (!jimo)
		{
			AosLogUserError(rdata, "failed_creating_jimo") << enderr;
			return false;
		}

		if (jimo->getJimoType() != AosJimoType::eStreamJob)
		{
			AosLogUserError(rdata, "invalid_jimo") << enderr;
			return false;
		}

		job = dynamic_cast<AosStreamJobObj*>(jimo.getPtr());
		aos_assert_rr(job, rdata, false);
	}
	else
	{
		job = createJob(rdata);
		aos_assert_rr(job, rdata, false);
	}

	return Jimo::runStreamJob(rdata, job);
}


bool
AosSimpleJoinInner::createMapFinished(rdata)
{
	if (!mCreateMapSuccess) return rdata->popCaller();

	// The map has been created successfully. It is the time to
	// create the join virtual field.
}


