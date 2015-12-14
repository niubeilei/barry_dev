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
//
// Modification History:
// 01/27/2007 Moved from KernelUtil to aosUtil by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_aosUtil_AosPlatform_h
#define Omn_aosUtil_AosPlatform_h

#ifdef __KERNEL__

#define aos_printk printk

#else

#define aos_printk printf

#endif // end of __KERNEL__

#endif

