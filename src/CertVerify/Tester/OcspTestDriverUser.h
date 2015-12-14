////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: OcspTestDriverUser.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_CertVerify_Tester_OcspTestDriverUser_h
#define Aos_CertVerify_Tester_OcspTestDriverUser_h

#include "aosUtil/Types.h"

bool
AosOcspTester_Start_User(u32 req_id,
                const char *serial_num,
                u32 serial_len,
                u32 num_tries,
                u32 max_conns);

int
AosOcspTester_RetrieveRslt_User(
            u32 req_id,
			u8	*status,
            u32 *valid,
            u32 *invalid,
            u32 *failed);

#endif
