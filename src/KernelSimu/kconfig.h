////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: kconfig.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_kconfig_h
#define Omn_aos_KernelSimu_kconfig_h

#ifdef AOS_KERNEL_SIMULATE


struct file {
	struct file *next;
	struct file *parent;
	char *name;
	int lineno;
	int f_flags;
};

#else
// #include <scripts/kconfig.h>
#endif

#endif

