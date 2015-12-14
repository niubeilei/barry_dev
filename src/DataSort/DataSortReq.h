////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 2014/09/24 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataSort_DataSortReq_h
#define AOS_DataSort_DataSortReq_h

#include "DataSort/Ptrs.h"


class AosDataSortReq : public OmnRCObject 
{
public:
	virtual bool procReq() = 0;
};

#endif
