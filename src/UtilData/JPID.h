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
// 2015/06/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_UtilData_JPID_h
#define Aos_UtilData_JPID_h

const u64 AOSJPID_SYSTEM = 1234;
const u64 AOSJPID_PUBLIC = 2345;

class AosJPID
{
public:
	static bool isValid(const u64 jpid) {return jpid > 0;}
};

#endif
