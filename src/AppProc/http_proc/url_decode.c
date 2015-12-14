////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: url_decode.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include <linux/string.h>
#include "aosUtil/Memory.h"

//////////////////////////////////////////////////////////////////////////////
//
// This funciton wiil decore the url string in the buffer.
// Change "%xx" to a hex byte 
//
//////////////////////////////////////////////////////////////////////////////

void url_decode(char* buffer)
{
    unsigned int buffer_len;

    char* front;

    buffer_len = strlen(buffer);

    front = strchr(buffer, '%');
    if (front == NULL)
    {
        return;
    }

    while (front <= buffer+buffer_len)
    {
        if((*front)=='%')
        {
            if((*(front+1)>=0x30)&&(*(front+1)<=0x39))
            {
                if ((*(front+2)>=0x30)&&(*(front+2)<0x39))
                {
                    *front = (*(front+1) - 0x30)*16 + (*(front+2)- 0x30);
                    memcpy(front+1, front+3, buffer_len - (front - buffer) - 3);
                    memset(front + 1 + (buffer_len - (front - buffer) -3), 0x00, 2);

                }
            }
        }
        front++;
    }

}

