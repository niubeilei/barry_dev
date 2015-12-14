////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: cavium_endian.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#if !defined (__CAVIUM_ENDIAN_H__)
#define __CAVIUM_ENDIAN_H__

#if defined (_X86_) || defined (i386) || defined (i686) 
#include "cavium_le.h"
#elif defined (mips) || defined (ppc)
#include "cavium_be.h"
#elif defined(__amd64__)
#include "cavium_le.h"
#elif defined(__i386__)
#include "cavium_le.h"
#else
#error "Unknown architecture"
#endif

#endif
