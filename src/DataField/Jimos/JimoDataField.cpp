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
// 2013/07/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataField/Jimos/JimoDataField.h"


AosJimoDataField::AosJimoDataField(const int version)
:
AosJimo(AosJimoType::eDataField, version),
AosDataField(AosDataFieldType::eJimoField, AOSDATAFIELDTYPE_JIMO_FIELD, false)
{
}


AosJimoDataField::~AosJimoDataField()
{
}


bool 
AosJimoDataField::serializeTo(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosJimoDataField::serializeFrom(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	OmnNotImplementedYet;
	return false;
}

#endif
