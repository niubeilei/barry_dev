////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Base64.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Base64/Base64.h"

int EncodeBase64(
		unsigned char* pSrc, 
		unsigned char* pDst, 
		int nSrcLen, 
		int nMaxLineLen)
{
	unsigned char* src = pSrc;
	unsigned char* dst = pDst;
	
	unsigned char c1, c2, c3;    // 输入缓冲区读出3个字节
	int nDstLen = 0;             // 输出的字符计数
	int nLineLen = 0;            // 输出的行长度计数
	int nDiv = nSrcLen / 3;      // 输入数据长度除以3得到的倍数
	int nMod = nSrcLen % 3;      // 输入数据长度除以3得到的余数
	for (int i = 0; i < nDiv; i ++)
	{
		c1 = *src++;
		c2 = *src++;
		c3 = *src++;

		*dst++ = EnBase64Tab[c1 >> 2];
		*dst++ = EnBase64Tab[((c1 << 4) | (c2 >> 4)) & 0x3f];
		*dst++ = EnBase64Tab[((c2 << 2) | (c3 >> 6)) & 0x3f];
		*dst++ = EnBase64Tab[c3 & 0x3f];
		nLineLen += 4;
	    nDstLen += 4;
		                                             
        if (nLineLen > nMaxLineLen - 4)
		{
			*dst++ = '\r';
			*dst++ = '\n';
			nLineLen = 0;
			nDstLen += 2;
		}
    }

	if (nMod == 1)
	{
		c1 = *src++;
		*dst++ = EnBase64Tab[(c1 & 0xfc) >> 2];
		*dst++ = EnBase64Tab[((c1 & 0x03) << 4)];
		*dst++ = '=';
		*dst++ = '=';
	    nDstLen += 4;
	}
	else if (nMod == 2)
	{
		c1 = *src++;
		c2 = *src++;
		*dst++ = EnBase64Tab[(c1 & 0xfc) >> 2];
		*dst++ = EnBase64Tab[((c1 & 0x03) << 4) | ((c2 & 0xf0) >> 4)];
		*dst++ = EnBase64Tab[((c2 & 0x0f) << 2)];
		*dst++ = '=';
		nDstLen += 4;
	}
	*dst = '\0';
	return nDstLen;
}


int DecodeBase64(unsigned char* pSrc,unsigned char* pDst, int nSrcLen)
{
	int nDstLen;
	int nValue;             // 解码用到的长整数
	int i;
		
	i = 0;
	nDstLen = 0;
	unsigned char* src = pSrc;
	unsigned char* dst = pDst;

	while (i < nSrcLen)
	{
		if (*src != '\r' && *src !='\n')
		{
			nValue = DeBase64Tab[(unsigned char)*src++] << 18;
			nValue += DeBase64Tab[(unsigned char)*src++] << 12;
			*dst++ = (nValue & 0x00ff0000) >> 16;
			nDstLen++;
			if (*src == '=')
			{
				break;
			}
			nValue += DeBase64Tab[(unsigned char)*src++] << 6;
			*dst++ = (nValue & 0x0000ff00) >> 8;
			nDstLen++;
			if (*src == '=')
			{
				break;
			}
			nValue += DeBase64Tab[(unsigned char)*src++];
			*dst++ =nValue & 0x000000ff;
			nDstLen++;
			i += 4;
		}
		else        // 回车换行，跳过
		{
			src++;
			i++;
		}
    }
	*dst = '\0';
	return nDstLen;
}

