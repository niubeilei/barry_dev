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

#ifndef AOS_Rsync_MD5_H
#define AOS_Rsync_MD5_H

typedef unsigned int UINT32;
class AOS_MD5
{
    public:
        typedef struct
        {
            UINT32 state[4];   /* state (ABCD) */
            UINT32 count[2];   /* number of bits, modulo 2^64 (lsb first) */
            unsigned char buffer[64];   /* input buffer */
        } MD5_CTX;

        static unsigned char PADDING[64];
    public:
        AOS_MD5();
        virtual ~AOS_MD5();
        static void MD5Transform (UINT32 a[4], unsigned char b[64]);
        static void MD5Init(MD5_CTX *context);
        static void MD5Update(MD5_CTX *context, unsigned char *input, unsigned int inputLen);
        static void MD5Final(unsigned char digest[16], MD5_CTX * context);
        static void Encode(unsigned char *output, UINT32 *input, unsigned int len);
        static void Decode(UINT32 *output, unsigned char *input, unsigned int len);
        static void TransTo32ByteMd5(char *pTextStr, char *pOutStr);
    protected:
    private:
};

#endif // MD5_H
