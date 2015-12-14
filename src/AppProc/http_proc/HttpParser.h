////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HttpParser.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

char *FindStr(char* buff, unsigned int buff_len, char* str);
int check_valid(char* buff, unsigned int buff_len);
unsigned int find_header(char* buff, unsigned int buff_len);
unsigned int find_chunked_end(char* buff, unsigned int buff_len);
unsigned int find_multipart_end(char* buff, unsigned int buff_len);
char *FindStr(char* buff, unsigned int buff_len, char* str);
unsigned int GetTransferTypeFromHeader(char* buff, unsigned int buff_len, char* transfer_type);
unsigned long GetContentLength(char* head, int HeadLen);
int CheckAction(char* InBuff, int InBuffLen);
int GetMsgType(char* head_buff, unsigned int head_buff_len);
int HttpRequestParse2HostName(char *InBuff, unsigned int InBuffLen, char* OutBuff, unsigned int OutBuffLen, unsigned int* ReturnLength);
int HttpRequestParseAction(char* InBuff, int InBuffLen);
int HttpRequestParse2Port(char* InBuff, int InBuffLen, unsigned short *nPort);
int HttpRequestParse2Resource(char *InBuff, unsigned int InBuffLen, char* OutBuff, unsigned int OutBuffLen, unsigned int* ReturnLength);
int HttpRquestParseGetEnCodingType(char* head, unsigned int head_len);

