////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// Modification History:
// Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAgentUtil_SearchIp_h
#define Aos_JimoAgentUtil_SearchIp_h

#include "JimoAgentUtil/CountDownLatch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <deque>
#include <set>
#include <vector>
#include <string>


using namespace std;

class AosSearchIp
{
private:
	int 				mTimeConnect;
	int 				mPort;
	deque<string>		mIpList;
	set<string>			mOnlineList;
	pthread_mutex_t		mLock;
	sem_t				mSem;
	sem_t				mMutex;
	int					mNumThreads;
	AosCountDownLatch*	mLatch;
	vector<pthread_t>	mThreadIds;
public:
	AosSearchIp(
			const int num_thrds,
			const int port,
			const int time_connect);
	~AosSearchIp();
	bool 	start(const int start_ip, const int end_ip);
	bool	start(vector<string> &ip_list);
	void 	run();
	static void* threadFunc(void* args);
	set<string> getOnlineList() { return mOnlineList; }
	static bool isValidSubNetMask(const string &sub_net_mask);
	static bool isValidIp(const string &ip_addr);
private:
	void initThread();
	void destroyThread();
};
#endif

