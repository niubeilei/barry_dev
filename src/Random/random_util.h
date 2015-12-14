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
// 12/25/2011	Copied from Ramdom/RandomUtil.h by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Random_random_util_h
#define Omn_Random_random_util_h

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "Debug/Debug.h"
#include "aosUtil/Types.h"
#include "Random/RandomTypes.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util/MacAddr.h"
#include "Util/StrType.h"


extern u32 aos_next_u32(const u32 min, const u32 max);
extern int aos_next_int(const int min, const int max);
extern int64_t aos_next_int64(const int64_t min, const int64_t max);
extern OmnString aos_next_incorrect_ip();
extern bool aos_next_pct(const u32 pct);
extern OmnString aos_next_ip_str(
			const OmnIpAddr &netaddr, 
			const OmnIpAddr &mask);
extern OmnIpAddr aos_next_ip(
			const OmnIpAddr &netaddr, 
			const OmnIpAddr &mask);
extern OmnIpAddr aos_next_mask();
extern OmnIpAddr aos_next_mask(const u32 min, const u32 max);
extern OmnString aos_next_mask_str(const u32 min, const u32 max);
extern OmnIpAddr aos_next_bad_mask(
			const OmnIpAddr &min, 
			const OmnIpAddr &max);
extern OmnString aos_next_incorrect_ip(
			const OmnIpAddr &addr, 
			const OmnIpAddr &mask);
extern OmnString aos_next_time_str(AosTimeStrFormat_e);
extern OmnString aos_next_invalid_time_str(const u32 min, const u32 max);
extern char aos_next_hex_char();
extern OmnString aos_next_hex_byte();
extern OmnMacAddr aos_next_mac();
extern OmnString  aos_next_incorrect_mac();

// #define PERCENTAGE(X) (OmnRandom::nextInt(1,100) <= (X))
// #define RAND_INT(X,Y) (OmnRandom::nextInt((X),(Y)))
// #define RAND_INT2(X,Y,M,N) (OmnRandom::nextInt((X),(Y),(M),(N)))
// #define RAND_STR(X,Y) (OmnRandom::nextOmnStr((X),(Y)))
// #define RAND_BOOL()   (OmnRandom::nextBool())
#endif
