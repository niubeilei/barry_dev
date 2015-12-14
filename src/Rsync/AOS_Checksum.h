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

#include "AOS_MD5.h"

typedef unsigned int uint32;
class AOS_Checksum
{
    public:
        AOS_Checksum();
        virtual ~AOS_Checksum();
        static unsigned int get_checksum1(char *buf,int len);
        static void get_checksum2(char *buf,int len,char *MD5_result);
    protected:
    private:
};

#endif // CHECKSUM_H
