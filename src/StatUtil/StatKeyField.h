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
#ifndef AOS_StatUtil_StatKeyField_h
#define AOS_StatUtil_StatKeyField_h

#include "StatUtil/StatField.h"


class AosStatKeyField: public AosStatField
{

public:
	AosStatKeyField();
	AosStatKeyField(const OmnString &fname);
	AosStatKeyField(const OmnString &fname, const AosDataType::E &fieldType);
	
	virtual bool config(const AosXmlTagPtr &conf);
	
	virtual OmnString toXmlString();
	
};

#endif
