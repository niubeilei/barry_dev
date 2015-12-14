//////////////////////////////////////////////////////////////////////////// //
// Copyright (C) 2010
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
#include "JimoAgentUtil/SearchIp.h"

AosSearchIp::AosSearchIp(
			const int num_thrds,
			const int port,
			const int time_connect)
:
mTimeConnect(time_connect),
mPort(port),
mNumThreads(num_thrds)
{
	pthread_mutex_init(&mLock, NULL);
	sem_init(&mSem, 0, 0);
	sem_init(&mMutex, 0, 1);
	mLatch = new AosCountDownLatch();
	initThread();
}

AosSearchIp::~AosSearchIp()
{
	delete mLatch;
}

void
AosSearchIp::initThread()
{
	if (mThreadIds.size() != 0)
	{
		return;
	}
	for (int i=0; i<mNumThreads; i++)
	{
		pthread_t tid;
		pthread_create(&tid,  0, threadFunc, (void*)this);
		mThreadIds.push_back(tid);
	}
//printf("number of threads: %d\n", (unsigned int)mThreadIds.size());
}

bool
AosSearchIp::start(vector<string> &ip_list)
{
	mLatch->setCount(ip_list.size());
	pthread_mutex_lock(&mLock);
	mOnlineList.clear();
	mIpList.clear();
	pthread_mutex_unlock(&mLock);
	for (size_t i=0; i<ip_list.size(); i++)
	{
		pthread_mutex_lock(&mLock);
		mIpList.push_back(ip_list[i]);
		pthread_mutex_unlock(&mLock);
		sem_post(&mSem);
	}
	mLatch->wait();
	return true;
}

bool
AosSearchIp::start(const int start_ip, const int end_ip)
{
	mLatch->setCount(end_ip- start_ip + 1);
	pthread_mutex_lock(&mLock);
//printf("start online list: %d, ip list: %d\n", (unsigned int)mOnlineList.size(), (unsigned int)mIpList.size());
	mOnlineList.clear();
	mIpList.clear();
	pthread_mutex_unlock(&mLock);
	char straddr[100];
	for(int i=start_ip; i<=end_ip; i++) 
	{
		pthread_mutex_lock(&mLock);
		sprintf(straddr, "192.168.99.%d\0", i);
		mIpList.push_back(straddr);
		pthread_mutex_unlock(&mLock);
		sem_post(&mSem);
	}
	mLatch->wait();
//printf("end online list: %d, ip list: %d\n", (unsigned int)mOnlineList.size(), (unsigned int)mIpList.size());
	return true;
}

void*
AosSearchIp::threadFunc(void* args)
{
	AosSearchIp* search_ip = (AosSearchIp*)args;
	search_ip->run();
	return 0;
}

void 
AosSearchIp::run()
{
	struct timeval timeout;
	fd_set wset;
	fd_set rset;
	pthread_detach(pthread_self());
	while(true)
	{
		sem_wait(&mSem);
		pthread_mutex_lock(&mLock);
		if (mIpList.empty())
		{
			pthread_mutex_unlock(&mLock);
			continue;
		}
		string ip = mIpList.front();
		mIpList.pop_front();
		pthread_mutex_unlock(&mLock);
		int sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if(sockfd == -1)
		{
			perror("socket()");
		}
		struct sockaddr_in server_addr;
		server_addr.sin_family = AF_INET;
		if(inet_pton(AF_INET, ip.data(), &(server_addr.sin_addr)) != 1)
		{
			perror("inet_pton()");
		}
		server_addr.sin_port = htons(mPort);
		int flags;
		if((flags = fcntl(sockfd, F_GETFL, 0)) < 0)
		{
			perror("fcntl()");
		}
		if(fcntl(sockfd, F_SETFL, flags | O_NONBLOCK)< 0)
		{
			perror("fcntl()");
		}
		int result = connect(sockfd, (struct sockaddr*)(&server_addr), sizeof(server_addr));
		if(result == -1) 
		{
			if(errno == EINPROGRESS) 
			{
				timeout.tv_sec  = 10;
				timeout.tv_usec = 0;
				if(mTimeConnect >= 100) 
				{
					timeout.tv_sec  = 0;
					timeout.tv_usec = mTimeConnect;
				} 
				else if(mTimeConnect > 0) 
				{
					timeout.tv_sec  = mTimeConnect;
					timeout.tv_usec = 0;
				}
				FD_ZERO(&wset);
				FD_SET(sockfd, &wset);
				rset = wset;
				int n = select(sockfd+1, &rset, &wset, 0, &timeout);
				if(n == -1 && errno!= EINTR)
				{
					perror("select() error");
				} 
				else if(n > 0) 
				{
					int optval;
					int optlen = 4;
					if(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (void*)&optval, (socklen_t*)&optlen) < 0)
					{
						perror("getsockopt()");
					}
					if(optval == 0) 
					{
						pthread_mutex_lock(&mLock);
						mOnlineList.insert(ip);
						pthread_mutex_unlock(&mLock);
					} 
					else 
					{
						if (optval == 111)
						{
							pthread_mutex_lock(&mLock);
							mOnlineList.insert(ip);
							pthread_mutex_unlock(&mLock);
						}
						else if (optval == 113)
						{
						}
					}
				} 
				else if(n == 0) 
				{
					pthread_mutex_lock(&mLock);
					mOnlineList.insert(ip);
					pthread_mutex_unlock(&mLock);
				}
			} 
			else
			{
				close(sockfd);
				perror("connect()");
			}
		}
		else 
		{
			pthread_mutex_lock(&mLock);
			mOnlineList.insert(ip);
			pthread_mutex_unlock(&mLock);
			close(sockfd);
		}
		close(sockfd);
		mLatch->countDown();
	}
}

bool
AosSearchIp::isValidIp(const string &ip_addr)
{
	struct in_addr addr;
	int ret;
	ret = inet_pton(AF_INET, ip_addr.data(), &addr);
	if (ret > 0)
	{
	}
	else if (ret < 0)
	{
	}
	else
	{
	}
	return (bool)ret;
}

bool
AosSearchIp::isValidSubNetMask(const string &sub_net_mask)
{
	unsigned int b = 0, i, n[4];
	sscanf(sub_net_mask.data(), "%u.%u.%u.%u", &n[3], &n[2], &n[1], &n[0]);
	for (i=0; i<4; i++)
	{
		b += n[i] << (i * 8);
	}
	b = ~b + 1;
	if((b & (b - 1)) == 0)
	{
		return true;
	}
	return false;
}


