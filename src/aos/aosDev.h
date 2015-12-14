////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosDev.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_aos_core_dev_h
#define aos_aos_core_dev_h

struct net_device;

extern struct net_device *aosGetDevByName(const char *name);
extern struct net_device *aosGetDevByIndex(int index);
extern int aosSetDevPromiscuityByName(const char *name);
extern int aosSetDevPromiscuity(struct net_device *dev);
extern int aosResetDevPromiscuityByName(const char *name);
extern int aosResetDevPromiscuity(struct net_device *dev);
extern int aosIsDevPromiscuityByName(const char *name, int *rslt);
extern int aosIsDevPromiscuity(struct net_device *dev);

#endif
