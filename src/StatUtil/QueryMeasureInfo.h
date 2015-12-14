////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2014/07/25 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StatUtil_QueryMeasureInfo_h
#define AOS_StatUtil_QueryMeasureInfo_h

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Rundata/Ptrs.h"
#include "Util/String.h"
#include "Util/DataTypes.h"

struct AosQueryMeasureInfo 
{
	OmnString 				mName;
	OmnString				mAggrFuncStr;
	OmnString				mRawFieldName;
};


#endif
