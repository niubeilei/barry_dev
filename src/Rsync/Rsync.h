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

#ifndef AOS_Rsync_RSYNC_H
#define AOS_Rsync_RSYNC_H

#include "Checksum.h"
#include <sys/types.h>
#include <algorithm>
#include <vector>
#include "aosUtil/Types.h"

typedef struct
{
    off_t 		offset;			/* offset in file of this chunk */
    int 		len;			    /* length of chunk of file */
    int 		id;			        /* index of this chunk */
    u32 		checksum;	        /* simple checksum */
    char 		md5[33];	        /* md5 checksum  */
}Sum_Buf;


typedef struct
{
    off_t 		flength;		/* total file length */
    int 		count;			/* how many chunks */
    int 		n;			    /* block_length */
    Sum_Buf 	*sums;		/* points to info for each chunk */
}Sum_Struct;


typedef struct //save report result
{
    int 	flag;// if flag == 1 save blockid, flag == 0 save different string blcok;
    int 	blockid1;
    int 	blockid2;
    off_t 	pos1;
    off_t 	len1;
    off_t 	pos2;
    off_t 	len2;
}Result;


class AOSMatch;

class AOSRsync
{
private:
    int                 mBlocksize;
    AOSMatch            *mMatch;
    std::vector<Result>      mResult;
    Sum_Struct*         mS;
    char*               mReference_buf;
    char*               mTarget_buf;
    int                 mBlockid;
    bool                mIsEqual;

public:
    AOSRsync(u32 BlockSize);
    virtual ~AOSRsync();
    Sum_Struct* generateSums(char *buf, off_t filelen);//divide file2 and calclulate the block checksum
    bool compare(char* filename1, char* filename2);
    bool addDifStr(off_t pos1, off_t len1, off_t pos2, off_t len2);
    bool readFile(char *filename , char * &buf, off_t &len);
    bool printResult();//print the final result
};

#endif // RSYNC_H
