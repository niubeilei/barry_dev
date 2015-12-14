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
// 08/24/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////

#include "StorageMgrUtil/SignatureAlgo.h" 

#include "alarm_c/alarm.h"

/*
bool AosSignatureAlgo::smInited = false;	
u32	 AosSignatureAlgo::smAlgoLen[] = {0};

void
AosSignatureAlgo::init()
{
	smAlgoLen[eMD5] = 32;
	smInited = true;
}

u32
AosSignatureAlgo::getSignLen(const E algo)
{
	if(!smInited)	init();
	aos_assert_r(isValid(algo), 0);
	aos_assert_r(smAlgoLen[algo], 0);
	
	return smAlgoLen[algo];
}
*/

