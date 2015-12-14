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
// Modification History:
// 2015/06/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAPI_JimoData_h
#define Aos_JimoAPI_JimoData_h

#include "Rundata/Rundata.h"
#include "SearchEngine/DocReq.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/DocTypes.h"
#include "Util/String.h"


namespace Jimo
{
inline bool jimoIsValidJPID(const u64 jpid)
{
	return jpid > 0;
}

inline bool jimoIsValidJSID(const u64 jsid)
{
	return jsid > 0;
}

};
#endif


