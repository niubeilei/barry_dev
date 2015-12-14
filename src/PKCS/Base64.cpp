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
//  Copied from the original file 
////////////////////////////////////////////////////////////////////////////

#include "PKCS/Base64.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
#include "KernelSimu/string.h"



int AosBase64_decode1(const char *input, 
					u32 input_len, 
					char *output)
{
	char c;
	int cur = 0, val = 0, i = 0;
    
    // clear the line, if any: "-----BEGIN CERTIFICATE-----"
    if (strstr((char *)input, "-----BEGIN "))
	{
        while(*input !='\n' && input_len) {input++; input_len--;}
	}

	// 
	// Skip to the first valid character
	//
	while (input_len)
	{
		c = *input;
		if ((c >= 'A' && c <= 'Z') ||
			(c >= 'a' && c <= 'z') ||
			(c >= '0' && c <= '9') ||
			c == '+' || c == '/')
		{
			break;
		}

		input++;
		input_len--;
	}

	while (input_len) 
	{
		c = *input;
		input_len--;
		input++;

		if (c >= 'A' && c <= 'Z') val = c - 'A';
		else if (c >= 'a' && c <= 'z') val = c - 'a' + 26;
		else if (c >= '0' && c <= '9') val = c - '0' + 52;
		else if (c == '+') val = 62;
		else if (c == '/') val = 63;
		else if (c == '\n' || c == '\r') continue;
		else break;

		switch (cur++) 
		{
		case 0:
			 output[i] = (val << 2) & 0xfc;
			 break;

		case 1:
			 output[i] |= (val >> 4) & 0x03;
			 i++;
			 output[i] = (val << 4) & 0xf0;
			 break;

		case 2:
			 output[i] |= (val >> 2) & 0x0f;
			 i++;
			 output[i] = (val << 6) & 0xc0;
			 break;

		case 3:
			 output[i] |= val & 0x3f;
			 i++;
			 cur = 0;
			 break;
		}
	}

	output[i] = 0;
	return i;
}

#if 0
static char *b64chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int base64_encode(const unsigned char *data, int len, char *dest) {
	char *buf = dest;

	/* Encode 3 bytes at a time. */
	while (len >= 3) {
		buf[0] = b64chars[(data[0] >> 2) & 0x3f];
		buf[1] = b64chars[((data[0] << 4) & 0x30) | ((data[1] >> 4) & 0xf)];
		buf[2] = b64chars[((data[1] << 2) & 0x3c) | ((data[2] >> 6) & 0x3)];
		buf[3] = b64chars[data[2] & 0x3f];
		data += 3;
		buf += 4;
		len -= 3;
	}

	if (len > 0) {
		buf[0] = b64chars[(data[0] >> 2) & 0x3f];
		buf[1] = b64chars[(data[0] << 4) & 0x30];
		if (len > 1) {
			buf[1] += (data[1] >> 4) & 0xf;
			buf[2] = b64chars[(data[1] << 2) & 0x3c];
		}
		else buf[2] = '=';
		buf[3] = '=';
		buf += 4;
	}

	*buf = '\0';
	return(buf - dest);
}
#endif


int AosBase64_decode(const char *input, 
					 u32 input_len, 
					 char **output)
{
    u32 output_len = input_len * 3 / 4 + 10;
	*output = (char *)aos_malloc(output_len+10);
	aos_assert1(input);

	return AosBase64_decode1(input, input_len, *output);
}

