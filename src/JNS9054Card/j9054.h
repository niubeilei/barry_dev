////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: j9054.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef J9054_HEAD_H
#define J9054_HEAD_H

int jmk_init(void); 
int jmk_free(void);

int jmk_kernel_ioctl(unsigned int cmd, void* arg);
#endif
