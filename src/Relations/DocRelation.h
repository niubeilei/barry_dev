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
// 10/31/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Relations_DocRelation_h
#define AOS_Relations_DocRelation_h

#include "Relation/Relation.h"


class AosDocRelation : public AosRelation
{

private:

public:
	AosRelation(const bool flag);
	~AosRelation();

	virtual bool checkRelation(
					const AosEventPtr &event, 
					const AosXmlTagPtr &container,
					const AosXmlTagPtr &doc,
					const AosXmlTagPtr &relation,
					const AosRundataPtr &rdata);
};
#endif

