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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 06/16/2011	Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_INotify_INotifyMgr_h
#define AOS_INotify_INotifyMgr_h

#include "SingletonClass/SingletonImpl.cpp"
#include "Rundata/Rundata.h"

#include <dirent.h>
#include <sys/inotify.h>


OmnDefineSingletonClass(AosINotifyMgrSingleton,
                        AosINotifyMgr,
                        AosINotifyMgrSelf,
                        OmnSingletonObjId::eINotifyMgr,
                        "INotifyMgr");

class AosINotifyMgr : virtual public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	enum
	{
		eEventSize = sizeof(struct inotify_event),
		eBuffSize = (eEventSize + 16) << 10,
		eMask = IN_CREATE | IN_DELETE | IN_MOVED_TO,
		eMoveTo = IN_MOVED_TO,
		eNewDir = IN_CREATE | IN_ISDIR,
		eDelDir = IN_DELETE | IN_ISDIR
	};
	int				 			mFd;
	OmnString 					mRootDir;
	OmnString					mContainer;
	OmnString					mPublic;
	map<int, OmnString> 		mDirset;
	map<OmnString, OmnString> 	mAttrs;
	OmnThreadPtr				mThread;

public:
	AosINotifyMgr();
	~AosINotifyMgr();

	//
    // Singleton class interface
    //
    static AosINotifyMgr*    getSelf();
    virtual bool        start();
    virtual bool        stop();
	virtual bool        config(const AosXmlTagPtr &config);
	//ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;
private:
	bool initWatch(const int mask, const OmnString &root);
	bool addWatch(const int fd, const OmnString &dir, const int mask);
	bool removeWatch(const OmnString &dir);
	bool watchMonitor(const int fd);
	bool appendDir(const int fd, struct inotify_event *event, const int mask);
	bool doAction(const int fd, struct inotify_event *event);
	bool createMapDoc( const OmnString &filename, const int wd);
	bool testDate(char* strdate);
	bool removeDir(const OmnString &name, const int wd);
};
#endif

