////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: x509_util.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef __x509_util__
#define __x509_util__

#include "CertVerify/common.h"

extern int time_to_UTC_time(UINT32 t, BYTE *buf);
extern int time_to_generalized_time(UINT32 t, BYTE *buf);
extern time_t asntime_to_time(BYTE * buf, const int len, const BOOLEAN is_UTC_time);
#endif

