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
// 06/28/2012 Created by Brian
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataBlob_DataBlobFullHandler_h
#define Aos_DataBlob_DataBlobFullHandler_h


#include "Rundata/Rundata.h"


class AosDataBlobFullHandler : virtual public OmnRCObject
{
	OmnDefineRCObject;
public:
	virtual bool dataBlobFull(const AosDataBlobObjPtr &blob, const AosRundataPtr &rdata) = 0;
};
#endif
