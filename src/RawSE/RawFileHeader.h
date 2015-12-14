////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Zykie Networks, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 2014-11-26 created by White
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_RawSE_RawFileHeader_H_
#define Aos_RawSE_RawFileHeader_H_

#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

class AosRawFileHeader : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
    AosRawFileHeader();
    virtual ~AosRawFileHeader();

    bool updateHeader(
    		AosRundata	*rdata,
    		const u64	ullRawFileID);
};


#endif /* Aos_RawSE_RawFileHeader_H_ */
