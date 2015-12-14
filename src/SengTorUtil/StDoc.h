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
// 01/08/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSengTorUtil_StDoc_h
#define AosSengTorUtil_StDoc_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosStDoc : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	u64		 		locdid;
	u64		 		svrdid;
	OmnString 		objid;
	u64				parent_locdid;
	OmnString		creator;
};

#endif

