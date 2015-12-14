////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MemPoolMul.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_MemPoolMul_h
#define Omn_Util_MemPoolMul_h

#include "Util/BasicTypes.h"
#include "Util1/Ptrs.h"


class OmnMemBlock;
class OmnString;

class OmnMemPoolMul
{
private:
	enum 
	{
		eSize1 = 100,
		eSize2 = 500,
		eSize3 = 2000,
		eSize4 = 5000,
		eSize5 = 10000,

		eMaxBlocks1 = 1000,
		eMaxBlocks2 = 500,
		eMaxBlocks3 = 300,
		eMaxBlocks4 = 200,
		eMaxBlocks5 = 100
	};

	static OmnMemPoolPtr	mSize1Pool;
	static OmnMemPoolPtr	mSize2Pool;
	static OmnMemPoolPtr	mSize3Pool;
	static OmnMemPoolPtr	mSize4Pool;
	static OmnMemPoolPtr	mSize5Pool;

	static int 				mTotalOversizeObj;

public:
	static bool		allocate(const char *file, 
							 const int line, 
							 OmnMemBlock &mem);
	static bool		allocate(const char *file, 
							 const int line, 
							 OmnMemBlock &mem, 
							 const uint &size);
	static void		release(const uint size, char *ptr);
	static uint		purge();
	static void		checkMemory(OmnString &result);

    static int determineSize(const int n)
    {
        if (n <= eSize1)
        {
            return eSize1;
        }

        if (n <= eSize2)
        {
            return eSize2;
        }

        if (n <= eSize3)
        {
            return eSize3;
        }

        if (n <= eSize4)
        {
            return eSize4;
        }

        if (n <= eSize5)
        {
            return eSize5;
        }

        return n;
    }

	static OmnString 	getObjCnt();

};
#endif
