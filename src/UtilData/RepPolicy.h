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
// 2015/03/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_UtilData_RepPolicy_h
#define Aos_UtilData_RepPolicy_h

class AosRepPolicy
{
public:
	inline static u32 getRepPolicy(const u8 sync, const u8 async, const u8 remote)
	{
		return sync + (async << 8) + (remote << 16);
	}
	inline static u8 getNumSync(const u32 nn) {return nn ;}
	inline static u8 getNumAsync(const u32 nn) {return (nn >> 8);}
	inline static u8 getNumRemotes(const u32 nn) {return (nn >> 16);}
	inline static u8 getLocalCopies(const u32 nn) {return (u8)(nn) + (u8)(nn >> 8);}
};
#endif

