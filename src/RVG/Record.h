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
//
// Modification History:
// Tuesday, December 18, 2007: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_RVG_Record_H
#define Aos_RVG_Record_H

#include "RVG/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include <vector>


class AosRecord : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	std::vector<AosValuePtr>	mRecord;

public:
	AosRecord();
	~AosRecord();

	bool	push_back(const AosValuePtr &value);
	bool	getValue(
				const AosTablePtr &table, 
				const OmnString &name, 
				AosValuePtr &value);
	bool	isEmpty() const;
	bool	clear();
};

#endif

