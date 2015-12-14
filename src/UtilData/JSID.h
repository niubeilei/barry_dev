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
#ifndef Aos_UtilData_JSID_h
#define Aos_UtilData_JSID_h

const u64 AOSJSID_SYSTEM = 1235;
const u64 AOSJSID_PUBLIC = 1236;

class AosJSID
{
public:
	static bool isValid(const u64 jsid) {return jsid > 0;}
};

#endif
