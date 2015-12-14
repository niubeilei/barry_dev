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
#ifndef RB_NETGATE_HEAD_H
#define RB_NETGATE_HEAD_H

#define SERVER_PORT 12005


int main(int argc, char *argv[]);
void cli_requ(int sockfd, char *buf);
int read_all(int fd, void *buf, int n);
int write_all(int fd, void *buf, int n);
void cli_rec(int sockfd, char *buf);

#endif
