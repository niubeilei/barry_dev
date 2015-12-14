////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: OcspTestDriver.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_CertVerify_Tester_OcspTestDriver_h
#define Aos_CertVerify_Tester_OcspTestDriver_h

#include "aosUtil/Types.h"


// 
// This function tests OCSP implementation. It will use 'serial_num'
// as the initial serial number. During the testing, each OCSP query
// is constructed by increment 'serial_num'. The total OCSP queries
// is 'num_tries'. 
// 
// Parameters:
//	'req_id':IN
//		The request ID. This is set by the caller. 
//	'const char *serial_num': IN
//		The initial serial number. 
//	'serial_len': IN
//		The length of the serial number.
//	'num_tries': IN
//		The number of queries for the test.
//	'max_conns': IN
//		The maximum concurrent requests during the testing. 
//
// Return Values:
//	eAosRc_Success:	the call is accepted.
//
extern int AosOcspTester_start(
			u32 req_id,
			const char *serial_num, 
			u32 serial_len,
			u32 num_tries,
			u32 max_conns);
			
// 
// This function retrieves the test results. 
//
// Parameters:
//	'req_id':IN
//		This is the request ID that is used to identify the request.
//	'status': OUT
//		0: Test not finished yet
//		1: Test finished.
//	'num_success': OUT
//		The number of successful queries
//	'num_failed': OUT
//		The number of failed queries
//
extern int AosOcspTester_retrieveRslt(
			u32 req_id,
			u8  *status,
			u32 *num_valid,
			u32 *num_invalid, 
			u32 *num_failed);

#endif

