////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HttpParser.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#define FALSE 0
#define TRUE 1

#include <linux/kernel.h>
#include <linux/slab.h>

#include "KernelSimu/string.h"
#include "AppProc/ReturnCode.h"
#include "AppProc/http_proc/HttpParser.h"
#include "AppProc/http_proc/HTTP.h"

#include "aosUtil/Tracer.h"




char *FindStr(char* buff, unsigned int buff_len, char* str);
//////////////////////////////////////////////////////////////////////////////
//	check the beginning of the message
//////////////////////////////////////////////////////////////////////////////
int check_valid(char* buff, unsigned int buff_len)
{
	char* cmd_type[] = {"GET", "POST"};
	int i = 0;
	unsigned int cmd_len = 0;
	for (i = (sizeof(cmd_type)/sizeof(char*)) -1; i >= 0; i--)
	{
		cmd_len = strlen(*(cmd_type+i));
		if (buff_len < cmd_len)
			continue;
		if (memcmp(buff, *(cmd_type+i),cmd_len) == 0)
		{
			break;
		}
	}
	if (i < 0)
		return -1;
	return 0;
}


//////////////////////////////////////////////////////////////////////////
//
// find "\r\n\r\n" to find out a header's ending
//
//////////////////////////////////////////////////////////////////////////
unsigned int find_header(char* buff, unsigned int buff_len)
{
	
	char* p = buff;

	if (buff_len < 4)
	{
		return 0;
	}
	while (p!=(buff + buff_len - 3))
	{
		if (memcmp(p, "\r\n\r\n", 4) == 0)
			break;
		p++;
	}
	if (p == (buff + buff_len - 3))
	{
		return 0; //can't find out ending of header
		
	}
	
	p += 4;

	return (p - buff); //find out ending of header and return header's length
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// 
// find "0\r\n" to find out ending for chunked mode
//
//////////////////////////////////////////////////////////////////////////
unsigned int find_chunked_end(char* buff, unsigned int buff_len)
{
	char* p = buff;
	while ( p!= (buff + buff_len -2))
	{
		if (memcmp(p, "0\r\n", 4) == 0)
			break;
		p++;
	}
	if (p == (buff + buff_len -2))
	{
		return 0;	//can't find out ending
		
	}
	p += 3;
	return (p - buff);	//find out and return the length from beggining to ending 
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// find the last boundary
//
//////////////////////////////////////////////////////////////////////////
unsigned int find_multipart_end(char* buff, unsigned int buff_len)
{
	
	char *p = 0;
	p = (char*)FindStr(buff, buff_len, "-----------------------------");
	if (p == 0)			//can't find out
	{
		
		return 0;
	}
	p += 29;
	
	p = (char*)FindStr(p, (buff_len - (p - buff)), "--\r\n");
	if (p == 0)
	{
		
		return 0;
	}
	p += 4;
	return (p - buff);
	
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//
//	find the appointed string from buffer
//
//////////////////////////////////////////////////////////////////////////
char *FindStr(char* buff, unsigned int buff_len, char* str)
{
	int i;
	unsigned int str_len = strlen(str);
	char* p = buff;
	while (p <= (buff + buff_len - str_len))
	{
		if (*p == *str)
			i = 0;
		if (memcmp(p, str, str_len) == 0)
			break;
		p++;
	}
	if (p > (buff + buff_len - str_len))
	{
		return 0;
	}
	return p;
}

//////////////////////////////////////////////////////////////////////////
//
// Get transfer-type from buffer
//
//////////////////////////////////////////////////////////////////////////
unsigned int GetTransferTypeFromHeader(char* buff, unsigned int buff_len, char* transfer_type)
{
	
	unsigned int content_length = 0;
	char *p = buff;
	
	
	p = (char*)FindStr(buff, buff_len, "Content-Length: ");
	if (p != 0)
	{
		sscanf (p , "Content-Length: %u\r\n", &content_length);
		if (content_length > 0)
		{
			*transfer_type = eAosHttpmsgFlag_LengthKnown;
			return content_length;							// found length
		}
		
	}
	
	
	p = (char*)FindStr(buff, buff_len, "Content-Type: multipart/");	
	if (p != 0)
	{
		*transfer_type = eAosHttpmsgFlag_LengthIndicator_Multipart;
		return 0;
	}
	
	p = (char*)FindStr(buff, buff_len, "Transfer-Encoding: chunked");
	if (p != 0)
	{
		*transfer_type = eAosHttpmsgFlag_LengthIndicator_Chunked;
		return 0;
	}
	
	
	
	*transfer_type = 0;
	
	return 0;
	
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//
// if there is a "Content-Length" field, get the Content Length from it
//
//////////////////////////////////////////////////////////////////////////
unsigned long GetContentLength(char* head, int HeadLen)
{
	unsigned int ret = 0;
	char *p=0, *q=0;
	while (p != (head + HeadLen - 15))
	{
		if (memcpy(p, "Content-Length:", 15) == 0)
			break;
		p++;
	}
	if (p == (head + HeadLen -15))
		return 0;
	
	q = p;
	while  (q != (head + HeadLen))
	{
		if (memcpy(q,  "\r\n\r\n", 4) == 0)
			break;
	       q++;
	}
	if  (q == (head+HeadLen))
		return 0;
	if  (sscanf (p , "Content-Length: %u\r\n", &ret) == 0)
		return 0;
	return ret;
	
	
	
}

//////////////////////////////////////////////////////////////////////////
//
// Judge the Message Type. "GET" or "POST"
//
//////////////////////////////////////////////////////////////////////////
int CheckAction(char* InBuff, int InBuffLen)
{
	char* p;
	p = InBuff;
	while ((*p!=0x20)&&(p < (InBuff + InBuffLen))) p++; //move p to first black
	if (p - InBuff > 4 )
	{
		return FALSE; // action name length is too long
	}
	
	if ((memcmp(InBuff, "GET", p - InBuff)!=0)&&(memcmp(InBuff, "POST", p - InBuff)!=0))	//if action name is not GET or POST
	{
		return FALSE;
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
//	Judge the Message Type. "GET" or "POST"
//
//////////////////////////////////////////////////////////////////////////
int GetMsgType(char* head_buff, unsigned int head_buff_len)
{
	char* p;
	p = head_buff;
	if (memcmp(p, "GET ", 4) == 0)
		return HTTP_METHOD_GET;
	else if (memcmp(p, "POST ", 5) == 0)
		return HTTP_METHOD_POST;
	else 
		return HTTP_METHOD_UNKNOWN;
}


//////////////////////////////////////////////////////////////////////////
//	This function can pick up HostName from a http head buffer
//////////////////////////////////////////////////////////////////////////
int HttpRequestParse2HostName(char *InBuff, unsigned int InBuffLen, char* OutBuff, unsigned int OutBuffLen, unsigned int* ReturnLength)
{
	char  *p =0;
	char *q =0;
	char *m = 0;
	
	if (CheckAction(InBuff, InBuffLen) == FALSE)
	{
		return FALSE;
	}
	p = InBuff;
	while (p <= (InBuff + InBuffLen))
	{
		if ((memcmp(p, "Host: ", 6) == 0) || (memcmp(p, "host: ", 6)==0))
			break;
		p++;
	}
	if (p > (InBuff + InBuffLen))
		return FALSE;
	
	p += 6;
	q = p;
	while (q <= (InBuff + InBuffLen))
	{
		if (memcmp(q, "\r\n", 2)==0)
			break;
		q++;
	}
	
	if (q >= (InBuff + InBuffLen))
		return FALSE;
	
	m = q;
	while (m != p)
	{
		if (*m == ':')
			break;
		m--;
	}
	
	if (m == p)
	{
		*ReturnLength = (int)(q - p);
		if (*ReturnLength > OutBuffLen)
			return FALSE;
		memcpy(OutBuff, p, *ReturnLength);
		return TRUE;
	}else
	{
		*ReturnLength = (int)(m - p);
		if (*ReturnLength > OutBuffLen)
			return FALSE;
		memcpy(OutBuff, p, *ReturnLength);
		return TRUE;
	}
	
	
}
int HttpRequestParseAction(char* InBuff, int InBuffLen)
{
	if (InBuffLen < 4)
		return HTTP_METHOD_UNKNOWN;
	
	if (memcmp(InBuff, "GET ", 4) == 0)	//if action name is not GET or POST
	{
		return HTTP_METHOD_GET;
	}
	if (memcmp(InBuff, "POST ", 5)==0)
	{
		return HTTP_METHOD_POST;
	}
	return HTTP_METHOD_UNKNOWN;
}


//////////////////////////////////////////////////////////////////////////
//
//	This function can pick up "port" from a http head buffer if it exists.
//
//////////////////////////////////////////////////////////////////////////
int HttpRequestParse2Port(char* InBuff, int InBuffLen, unsigned short *nPort)
{
	unsigned int port = 0;
	char  *p,*q, *m;
	char PortStr[6];
	memset(PortStr, 0x00, 6);
	
	*nPort = 80;	
	if (CheckAction(InBuff, InBuffLen) == FALSE)
	{
		return FALSE;
	}
	p = InBuff;
	
	while (p <= (InBuff + InBuffLen))
	{
		if (memcmp(p, "Host:", 5) == 0)
			break;
		p++;
	}
	
	if (p == (InBuff + InBuffLen))
		return FALSE;
	
	p += 5;
	
	q = p;
	while (q <= (InBuff + InBuffLen))
	{
		if (memcmp(q, "\r\n", 2) == 0)
			break;
		q++;
	}
	
	if (q == (InBuff + InBuffLen))
		return FALSE;
	
	m = p;
	while (m != q)
	{
		if (*m == ':')
			break;
		m++;
	}
	
	if (m == q)
	{
		return TRUE;	
	}
	else
	{	
		
		m++;
		memcpy(PortStr, m, (unsigned int)(q - m));
		sscanf(PortStr, "%u", &port);
		*nPort = (unsigned short)port;
	//	*nPort = atoi(PortStr);
		return TRUE;
	}
	
}

//////////////////////////////////////////////////////////////////////////
//
//	This function can pick up the resource string from a http head buffer
//
//////////////////////////////////////////////////////////////////////////

int HttpRequestParse2Resource(
	char *InBuff, 
	unsigned int InBuffLen, 
	char* OutBuff, 
	unsigned int OutBuffLen, 
	unsigned int* ReturnLength)
{
	char *p, *q, *m;

	memset (OutBuff, 0, OutBuffLen);
	*ReturnLength = 0;
		
	if (CheckAction(InBuff, InBuffLen) == FALSE)
	{
		return FALSE;
	}
	
	p = InBuff;
	while (p < (InBuff + InBuffLen))
	{
		if (memcmp(p, " /", 2)== 0)
			break;
		p++;
	}
	
	if (p >= (InBuff + InBuffLen))
		return FALSE;
	p++;

	q = p;
	while (q < (InBuff + InBuffLen))
	{
		if (memcmp(q, " HTTP", 5) == 0)
			break;
		q++;
	}
	if (q >= (InBuff + InBuffLen))
		return FALSE;
	
	m = q;
	while (m > p)
	{
		if (*m == '?')
			break;
		m--;
	}

	if (m == p)
	{
		*ReturnLength = (int)(q -p);
		if (*ReturnLength > OutBuffLen)
			return FALSE;
		memcpy(OutBuff, p, *ReturnLength);
		return TRUE;
		
	}else
	{
		*ReturnLength = (int)(m - p);
		if (*ReturnLength > OutBuffLen)
			return FALSE;
		memcpy(OutBuff, p, *ReturnLength);
		return TRUE;
		
	}
		
}


//////////////////////////////////////////////////////////////////////////
//
// find out transfer-type
//
//////////////////////////////////////////////////////////////////////////
int HttpRquestParseGetEnCodingType(char* head, unsigned int head_len)
{
	char* p;
	p = head;
	while (p != head + head_len)
	{
		if (*p=='T')
		{
			if (memcpy(p, "Transfer-Encoding: chunked", 26)==0)
				return EnCoding_Chunked;
			if (memcpy(p, "Transfer-Encoding: multipart", 28) == 0)
				return EnCoding_Mime;
			
		}
		p++;
	}
	
	p = head;
	while (p != head + head_len)
	{
		if (*p=='T')
		{
			if (memcpy(p, "Content-Length:", 15) == 0)
				return EnCoding_Length;
		}
		p++;
	}
	return 0;
	
	
}

