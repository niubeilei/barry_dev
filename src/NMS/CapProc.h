////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CapProc.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_NMS_CapProc_h
#define Omn_NMS_CapProc_h

#include "Util/String.h"
#include "Util/RCObject.h"

class OmnCapProc : public virtual OmnRCObject
{

public:
	virtual OmnString	getName() const = 0;
//	virtual void		startCapture(const OmnSoWebActionPtr &act) = 0;
//	virtual void		stopCapture(const OmnSoWebActionPtr &act) = 0;
};

#endif
