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
// calculate 32bit checksum and MD5 value
//
// Modification History:
// 2015/02/03 Created by Koala Ren
////////////////////////////////////////////////////////////////////////////

#include "AOS_Checksum.h"
#include <string.h>
#include <iostream>

using namespace std;
unsigned int AOS_Checksum::get_checksum1(char *buf,int len)
{
    int i;
    uint32 s1, s2;

    s1 = s2 = 0;
    for (i = 0; i < len; i++)
    {
        s1 += buf[i];
        s2 += s1;
    }
    return (s1 & 0xffff) + (s2 << 16);
}
void AOS_Checksum::get_checksum2(char *buf,int len,char *MD5_result)//get MD5
{
    char tem_buf[len+1];
    bzero(tem_buf,len+1);
    bcopy(buf,tem_buf,len);
    AOS_MD5::TransTo32ByteMd5(tem_buf,MD5_result);
}
