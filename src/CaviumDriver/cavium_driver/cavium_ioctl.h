////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: cavium_ioctl.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#if !defined (__CAVIUM_IOCTL_H__)
#define __CAVIUM_IOCTL_H__

#if defined (_WIN32)
#include "windows_ioctl.h"
#elif defined (linux)
#include "linux_ioctl.h"
#elif defined (__FreeBSD__)
#include "freebsd_ioctl.h"
#elif defined (__NetBSD__)
#include "netbsd_ioctl.h"
#else
#include "custom_ioctl.h"
#endif

#endif
