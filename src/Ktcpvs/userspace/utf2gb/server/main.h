////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
 #ifndef PARSE_URL_HEAD_H
 #define PARSE_URL_HEAD_H
 
 #define SERVER_PORT 12004
 #define BACKLOG 5
 
 #define MAX_LENGTH 1024
 
 int main(int argc, char *argv[]);
 void serv_respon(int sockfd);
 int write_all(int fd, void *buf, int n);
 int read_requ(int sockfd, char *buf, int size);
 int charset_convertion(const char *from, const char *to, char **inbuf, int length, char **outbuf, int *outlen);
 int parse_url(char *in_buf, int in_length, char *out_buf, int *out_length);
 
 #endif

