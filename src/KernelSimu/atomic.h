////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: atomic.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_atomic_h
#define Omn_aos_KernelSimu_atomic_h

#ifdef __KERNEL__
#include <asm/atomic.h>

#else
// Userland Portion
#ifndef atomic_t
#define atomic_t	int
#endif

#define atomic_inc(x) (*(x))++
#define atomic_dec(x) (*(x))--
#define atomic_read(x) (*(x))
#define atomic_set(x, y) (*(x)) = (y)
#define atomic_dec_and_test(x) (--(*(x)) == 0)

#endif

#endif

