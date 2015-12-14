////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 08/20/2013	Created by Young Pan
////////////////////////////////////////////////////////////////////////////
#include "SystemMonitor/RunCmd.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApiS.h"                 
#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/Ptrs.h"
#include "UtilComm/ConnBuff.h"
#include "XmlUtil/XmlTag.h"              

#include "SingletonClass/SingletonImpl.cpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#define AE_READABLE 1   //AE::a event
#define AE_WRITABLE 2

const char *echo = "echo 12345678900123456789\n";
const char *echostr = "12345678900123456789\n";
int flag_len = strlen(echostr);

OmnSingletonImpl(AosRunCmdSingleton,
				 AosRunCmd,
				 AosRunCmdSelf,
				 "RunCmd");

AosRunCmd::AosRunCmd()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar)
{
}


AosRunCmd::~AosRunCmd()
{
}


bool
AosRunCmd::start()
{
	// init data
	initInfo();
	if (pipe(mStdinFd) < 0 || pipe(mStdoutFd) || pipe(mStderrFd) < 0)
	{
		OmnAlarm << "pipe error:" << strerror(errno) << enderr;
	}
	setFdNonblocking(mStdoutFd[0]);
	setFdNonblocking(mStderrFd[0]);

	if ((mPid = fork()) < 0)
	{
		OmnAlarm << "fork error" << enderr;
	}
	else if (mPid > 0)
	{
		// parent pocess do that:
		// 		write command into the stdinfd pipe.
		// 		read results of command execute from stdout and stderr pipe
		
		// Create by Young 2013/09/10
		close(mStdinFd[0]);
		close(mStdoutFd[1]);	
		close(mStderrFd[1]);
		sleep(1);

		// create epoll
		if ((mEpollFd = createEvent()) == -1)
		{
			OmnAlarm << "create event error: " << strerror(errno) << enderr;
		}
		
		if (!(addEvent(mStdoutFd[0], AE_READABLE) && addEvent(mStderrFd[0], AE_READABLE)))
		{
			OmnAlarm << "addEvent error" << enderr;
			return false;
		}

		// one thread read stdout and stderr
		OmnThreadedObjPtr thisptr(this, false); 
		mThread = OmnNew OmnThread(thisptr, "runCmd", eReadPipeThrdId, false, true, __FILE__, __LINE__);
		mThread->start(); 
	}
	else
	{
		// child process do that:
		// run command, then write stdout and stderr into pipe
		close(mStdinFd[1]); 	// close write end
		close(mStdoutFd[0]);	// close read end
		close(mStderrFd[0]);	// close read end
		if (mStdinFd[0] != STDIN_FILENO)
		{
			if (dup2(mStdinFd[0], STDIN_FILENO) != STDIN_FILENO)
			{
				OmnAlarm << "dup2 error:" << strerror(errno) << enderr;
			}

			close(mStdinFd[0]);	// don't need this after dup2
		}	
		if (mStdoutFd[1] != STDOUT_FILENO)
		{
			if (dup2(mStdoutFd[1], STDOUT_FILENO) != STDOUT_FILENO)
			{
				OmnAlarm << "dup2 error:" << strerror(errno) << enderr;
			}

			close(mStdoutFd[1]);
		}

		if (mStderrFd[1] != STDERR_FILENO)
		{
			if (dup2(mStderrFd[1], STDERR_FILENO) != STDERR_FILENO)
			{
				OmnAlarm << "dup2 error:" << strerror(errno) << enderr;
			}

			close(mStderrFd[1]);
		}
		
		if (execl("/bin/tcsh", "", (char *)0) == -1)
		{
			OmnAlarm << "execl error: " << strerror(errno) << enderr;
		}
	}

	return true;
}


bool 
AosRunCmd::stop()
{
	// kill subprocess
	int status;
	// Created by Young 2013-10-15
	if (delEvent(mStdoutFd[0], AE_READABLE) && delEvent(mStderrFd[0], AE_READABLE))
	{
		OmnAlarm << "delete event error: " << enderr;
	}
	if (kill(mPid, 9) == -1)
	{
		OmnAlarm << "kill error: " << strerror(errno) << enderr;
	}
	if (waitpid(mPid, &status, 0) == -1)
	{
		OmnAlarm << "waitpid error: " << strerror(errno) << enderr;
	}
	mThread->stop();

	return true;
}


bool
AosRunCmd::config(const AosXmlTagPtr &config)
{
	return true;
}


bool 
AosRunCmd::initInfo()
{
	mCrtIdx = 0;
	mCmdExeInfo = "";
	mIsSuccess = true;
	mIsWaiting = true;
	mIsLongTimeCmd = false;

	return true;
}


OmnString 
AosRunCmd::run(const OmnString &cmd)
{
	if (mIsLongTimeCmd) return "a command is running";
	mLock->lock();
	initInfo();
	if (cmd == "") return mCmdExeInfo;
	OmnString tmpCmd = cmd;
	tmpCmd << '\n';
	char *cmdStr = tmpCmd.getBuffer();

	if (write(mStdinFd[1], cmdStr, strlen(cmdStr)) == -1)
	{
		bool rslt = stop();
		aos_assert_r(rslt, "");
		rslt = start();
		aos_assert_r(rslt, "");
		write(mStdinFd[1], cmdStr, strlen(cmdStr));
	}
	usleep(100);
	if (write(mStdinFd[1], echo, strlen(echo)) == -1)	
	{
		bool rslt = stop();
		aos_assert_r(rslt, "");
		rslt = start();
		aos_assert_r(rslt, "");
		write(mStdinFd[1], cmdStr, strlen(cmdStr));
		usleep(100);
		write(mStdinFd[1], echo, strlen(echo));
	}

	bool timeout = false;
	int i = 0;
	while (mIsWaiting)
	{
		if (++i > 10) 
		{
			mIsWaiting = false;
			mIsLongTimeCmd = true;
		}
		mCondVar->timedWait(mLock, timeout, 1);
	}
	mLock->unlock();

	// get the data
	OmnString info = mCmdExeInfo;
	initInfo();
	return info;
}


bool
AosRunCmd::cancel()
{
	int status;
	// Created by Young 2013-10-15
	if (delEvent(mStdoutFd[0], AE_READABLE) && delEvent(mStderrFd[0], AE_READABLE))
	{
		OmnAlarm << "delete event error: " << enderr;
	}
	if (kill(mPid, 9) == -1)
	{
		OmnAlarm << "kill error: " << strerror(errno) << enderr;
		return false;
	}
	if (waitpid(mPid, &status, 0) == -1)
	{
		OmnAlarm << "waitpid error: " << strerror(errno) << enderr;
	}
	mThread->stop();

	initInfo();
	bool rslt = start();
	aos_assert_r(rslt, false);

	return true;
}

bool
AosRunCmd::runBackgroud(const OmnString &cmd)
{
	return true;
}


bool
AosRunCmd::threadFunc( 
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	struct epoll_event e[2];
	while(1)
	{
		readPipe(e);
		if (checkIsFinished())
		{
			return true;
		}
	}
}


bool
AosRunCmd::checkIsFinished()
{
	// check mCmdExeInfo whether include echostr 
	// if has, notice another thread unlock
	// if doesn't have, read again.
	if (mCrtIdx >= flag_len && 
			strncmp(&mCmdExeInfo.getBuffer()[mCrtIdx-flag_len], echostr, flag_len) == 0)
	{
		// It has read in all the responses. 
		mCmdExeInfo.setLength(mCrtIdx-flag_len);
		OmnString tmpInfo = mCmdExeInfo;
		if (mIsSuccess)
		{
			mCmdExeInfo = "T";
		}
		else
		{
			mCmdExeInfo = "F";
		}
		mCmdExeInfo << tmpInfo;
		mIsLongTimeCmd = false;
		mIsWaiting = false;
		mCondVar->signal();

		return true;
	}

	return false;
}


bool
AosRunCmd::signal(const int threadLogicId)
{
	return true;
}


bool
AosRunCmd::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosRunCmd::setFdNonblocking(int fd)
{
	int flags;

	flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
	{
		OmnAlarm << "fcntl error: " << strerror(errno) << enderr;
		return false;
	}

	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) == -1)
	{
		OmnAlarm << "fcntl error: " << strerror(errno) << enderr;
		return false;
	}

	return true;
}


int 
AosRunCmd::createEvent()
{
	return epoll_create(2);
}


bool  
AosRunCmd::addEvent(int fd, int mask)
{
	struct epoll_event ee;

	if(mask & AE_READABLE) ee.events |= EPOLLIN | EPOLLET;;
	//if(mask & AE_WRITABLE) ee.events |= EPOLLOUT;

	ee.data.fd = fd;
	if (epoll_ctl(mEpollFd, EPOLL_CTL_ADD, fd, &ee) == -1) 
	{
		OmnAlarm << "epoll_ctl error: " << strerror(errno) << enderr;
		return false;
	}

	return true;
}


int 
AosRunCmd::waitEvent(struct epoll_event *e)
{
	return epoll_wait(mEpollFd, e, 64, 100);
}


bool
AosRunCmd::delEvent(int fd, int mask)
{
	struct epoll_event ee;
	memset(&ee, 0x0, sizeof ee);

	if(mask & AE_READABLE) ee.events |= EPOLLIN;
	if(mask & AE_WRITABLE) ee.events |= EPOLLOUT;

	ee.data.fd = fd;
	if (epoll_ctl(mEpollFd, EPOLL_CTL_DEL, fd, &ee) == -1)
	{
		OmnAlarm << "epoll_ctl delete error: " << strerror(errno) << enderr;
		return false;
	}

	return true;
}


int 
AosRunCmd::readPipe(struct epoll_event *e)
{
	int ret, i, n;
	char buf[100];
	memset(buf, 0x0, 100);
	if((ret = waitEvent(e)) == -1)
	{
		OmnString err = strerror(errno);
		if (errno != EINTR && errno != EAGAIN)
		{
			int err = errno;
			OmnAlarm << "epoll_wait error!"
				<< "; errno:" << err << enderr;
		}
	}

	//if (ret == 0)
	//{
		// it means timeout.
	//}

	for (i=0; i<ret; i++)
	{
		int pipefd = e[i].data.fd;
		if (pipefd == mStderrFd[0])
		{
			mIsSuccess = false;
		}

		while ((n = read(pipefd, buf, 100)) > 0)
		{
			mCrtIdx += n;
			buf[n] = '\0';
			mCmdExeInfo << buf;
			if (mCmdExeInfo.length() > 1000000)
			{
				// mCmdExeInfo is too long
				u32 len = mCmdExeInfo.length();
				OmnString cmdExeValue = mCmdExeInfo.substr((len/2), (len-1));
				mCrtIdx = len - (len / 2);
				mCmdExeInfo = cmdExeValue;
			}
		}
	}

	return ret;
}



