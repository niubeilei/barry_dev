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
// This is a utility to select docs.
//
// Modification History:
// 10/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Relations/Relation.h"

#include "Relation/RelationTypes.h"
#include "Thread/Mutex.h"

static AosRelationPtr sgRelations[AosRelationType::eMax];
static OmnMutex sgLock;

AosRelation::AosRelation()
{
}


AosRelation::~AosRelation()
{
}


bool 
AosRelation::checkRelationStatic(
		const AosEventPtr &event,
		const AosXmlTagPtr &container,
		const AosXmlTagPtr &doc,
		const AosXmlTagPtr &relation, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(relation, rdata, false);
	AosRelationPtr relation = getRelation(relation, rdata);
	aos_assert_rr(relation, rdata, false);
	return relation->checkRelation(event, container, doc, relation, rdata);
}


bool
AosRelation::registerRelation(const AosRelationPtr &relation)
{
	sgLock.lock();
	if (!AosRelationType::isValid(relation->mType))
	{
		sgLock.unlock();
		OmnAlarm << "Incorrect relation: " << enderr;
		return false;
	}

	if (sgRelations[relation->mType])
	{
		sgLock.unlock();
		OmnAlarm << "Relation already registered: " << relation->mType << enderr;
		return false;
	}

	sgRelations[relation->mType] = relation;
	sgLock.unlock();
	return true;
}

