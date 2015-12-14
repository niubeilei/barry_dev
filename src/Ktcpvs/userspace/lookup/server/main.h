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
#ifndef RB_SERVER_HEAD_H
#define RB_SERVER_HEAD_H


#define SERVER_PORT 12005
#define BACKLOG 5

int read_requ(int sockfd, char *buf, int size);
int write_all(int fd, void *buf, int n);
void serv_respon(int sockfd);
int main(int argc, char *argv[]);





#endif
