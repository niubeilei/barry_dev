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
// 		Base64 Character Mapping:
// 		0   A  17  R  34  i  51  z  
// 		1   B  18  S  35  j  52  0  
// 		2   C  19  T  36  k  53  1  
// 		3   D  20  U  37  l  54  2  
// 		4   E  21  V  38  m  55  3  
// 		5   F  22  W  39  n  56  4  
// 		6   G  23  X  40  o  57  5  
// 		7   H  24  Y  41  p  58  6  
// 		8   I  25  Z  42  q  59  7  
// 		9   J  26  a  43  r  60  8  
// 		10  K  27  b  44  s  61  9  
// 		11  L  28  c  45  t  62  +  
// 		12  M  29  d  46  u  63  /  
// 		13  N  30  e  47  v  
// 		14  O  31  f  48  w  (pad)  =  
// 		15  P  32  g  49  x  
// 		16  Q  33  h  50  y    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aosUtil/Base64.h"

#include "alarm_c/alarm.h"
#include "aosUtil/ReturnCode.h"


static const char *b64chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int aos_base64_decode(const char *data, u32 data_len, char *dest, u32 dest_len) 
{
	char c;
	int cur = 0, val = 0, i = 0;

	// clear line of such as "-----BEGIN CERTIFICATE-----"
	// if (strstr((char *)data, "BEGIN "))
	//	while(*data !='\n')
	//		data++;

	aos_assert_r(dest_len >= data_len * 3 / 4 + 1, -1);

	while (data_len) 
	{
		c = *data;
		data_len--;
		data++;

		if (c >= 'A' && c <= 'Z') val = c - 'A';
		else if (c >= 'a' && c <= 'z') val = c - 'a' + 26;
		else if (c >= '0' && c <= '9') val = c - '0' + 52;
		else if (c == '+') val = 62;
		else if (c == '/') val = 63;
		else if (c == '=') break;
		else {return -eAosRc_InvalidBase64Input;}

		switch (cur++) 
		{
		case 0:
			 dest[i] = (val << 2) & 0xfc;
			 break;

		case 1:
			 dest[i] |= (val >> 4) & 0x03;
			 i++;
			 dest[i] = (val << 4) & 0xf0;
			 break;

		case 2:
			 dest[i] |= (val >> 2) & 0x0f;
			 i++;
			 dest[i] = (val << 6) & 0xc0;
			 break;

		case 3:
			 dest[i] |= val & 0x3f;
			 i++;
			 cur = 0;
			 break;
		}
	}

	dest[i] = 0;
	return i;
}


// 
// It returns the length of the results
//
int aos_base64_encode(const char *data, u32 data_len, char *dest, u32 dest_len) 
{
	char *buf = dest;
	const unsigned char *orig = (const unsigned char *)data;

	aos_assert_r(dest_len >= data_len * 4 / 3 + 1, -1);

	// Encode 3 bytes at a time.
	while (data_len >= 3) 
	{
		buf[0] = b64chars[(orig[0] >> 2) & 0x3f];
		buf[1] = b64chars[((orig[0] << 4) & 0x30) | ((orig[1] >> 4) & 0xf)];
		buf[2] = b64chars[((orig[1] << 2) & 0x3c) | ((orig[2] >> 6) & 0x3)];
		buf[3] = b64chars[orig[2] & 0x3f];
		orig += 3;
		buf += 4;
		data_len -= 3;
	}

	// Padding
	if (data_len > 0) 
	{
		buf[0] = b64chars[(data[0] >> 2) & 0x3f];
		buf[1] = b64chars[(data[0] << 4) & 0x30];
		if (data_len > 1) 
		{
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

