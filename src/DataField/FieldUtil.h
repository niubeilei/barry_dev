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
// 07/08/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataField_FieldUtil_h
#define Aos_DataField_FieldUtil_h

#include "Rundata/Ptrs.h"


class AosStrValueInfo;

class AosFieldUtil
{
public:
	static bool setFieldValue(
					char *record, 
					const int record_len, 
					char *value, 
					const int value_len, 
					const AosStrValueInfo &valinfo,
					AosRundata* rdata);
};
#endif

