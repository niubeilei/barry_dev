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
#ifndef AOS_Relations_Relation_h
#define AOS_Relations_Relation_h

#include "Relation/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosRelation : public OmnRCObject
{
	OmnDefineRCObject;

private:

public:
	AosRelation(const AosRelationType::E type, const bool flag);
	~AosRelation();

	virtual bool checkRelation(
					const AosEventPtr &event, 
					const AosXmlTagPtr &container,
					const AosXmlTagPtr &doc,
					const AosXmlTagPtr &relation,
					const AosRundataPtr &rdata) = 0;

	static bool checkRelationStatic(
					const AosEventPtr &event,
					const AosXmlTagPtr &container,
					const AosXmlTagPtr &doc,
					const AosXmlTagPtr &relation, 
					const AosRundataPtr &rdata);
					
private:
	bool registerRelation(const AosRelationPtr &relation);
};
#endif

