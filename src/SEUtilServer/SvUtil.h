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
// 2011/03/01	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEUtilSv_SvUtil_h
#define Aos_SEUtilSv_SvUtil_h

#include "Util/String.h"
#include "Security/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "XmlUtil/Ptrs.h"

extern OmnString AosCalHashkey(const AosSessionPtr &session, const AosXmlTagPtr &doc);
#endif
