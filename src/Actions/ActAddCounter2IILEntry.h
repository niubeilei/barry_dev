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
// 12/26/2012	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActAddCounter2IILEntry_h
#define Aos_SdocAction_ActAddCounter2IILEntry_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "Rundata/Ptrs.h"


class AosActAddCounter2IILEntry : virtual public AosSdocAction
{
public:
	AosActAddCounter2IILEntry(const bool flag);
	~AosActAddCounter2IILEntry();

	u64 getIILID(const OmnString &counter_id, const AosRundataPtr &rdata);

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;
	bool createEntry(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
};
#endif

