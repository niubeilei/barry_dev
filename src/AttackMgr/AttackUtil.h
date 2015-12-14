////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AttackMgr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_AttackMgr_AttackUtil_h
#define Aos_AttackMgr_AttackUtil_h


#include <fcntl.h>

class AosAttackUtil
{
public:	
	static unsigned short ip_sum (u_short *addr, int len);

};
#endif
