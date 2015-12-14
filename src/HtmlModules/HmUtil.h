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
// 07/19/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_HmtlModules_HmUtil_h
#define AOS_HmtlModules_HmUtil_h

#include "XmlUtil/Ptrs.h"
#include "Util/String.h"



extern OmnString AosCreateQuery(const AosXmlTagPtr &query);
extern bool AosEvalQueryParm(const AosXmlTagPtr &querydp);

#endif

