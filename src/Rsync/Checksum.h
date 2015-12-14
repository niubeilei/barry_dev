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
// 2015/02/03 Created by Koala Ren
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Rsync_CHECKSUM_H
#define AOS_Rsync_CHECKSUM_H

#include "MD5.h"
#include "aosUtil/Types.h"

class AOSChecksum
{
public:
    AOSChecksum();
    virtual ~AOSChecksum();
    static u32 getChecksum(char *buf, int len);
    static void getMD5(char *buf, int len, char *MD5_result);
};

#endif // CHECKSUM_H
