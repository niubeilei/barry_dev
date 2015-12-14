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
// 2013/08/14 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/DataConverterObj.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


AosDataConverterObj::AosDataConverterObj(const u32 version)
:
AosJimo(AosJimoType::eDataConverter, version)
{
}

