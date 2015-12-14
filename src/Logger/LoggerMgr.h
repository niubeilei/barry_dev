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
//
// Modification History:
// 2013/03/17 Commented out by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Omn_Logger_LoggerMgr_h
#define Omn_Logger_LoggerMgr_h

#include "Logger/Ptrs.h"
#include "Logger/LogType.h"
#include "Message/Ptrs.h"
#include "NMS/HouseKpObj.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/ValList.h"
#include "XmlParser/Ptrs.h"

OmnDefineSingletonClass(OmnLoggerMgrSingleton,
						OmnLoggerMgr,
						OmnLoggerMgrSelf,
						OmnSingletonObjId::eLoggerMgr,
						"LoggerMgr");

class OmnLoggerMgr : public OmnHouseKpObj,
				     public OmnThreadedObj

{
	OmnDefineRCObject;

private:
	enum 
	{
		eMaxLoggers = 50
	};

	OmnMutexPtr		mLock;
	OmnCondVarPtr	mCondVar;
	OmnThreadPtr	mThread;
	OmnLoggerPtr	mLoggers[eMaxLoggers];
	OmnVList<OmnLogEntryPtr>	mQueue;

public:
	OmnLoggerMgr();
	virtual ~OmnLoggerMgr();

	static OmnLoggerMgr *	getSelf();
	bool		start();
	bool		stop();
	bool		config(const OmnXmlParserPtr &parser);

    //
    // OmnThreadedObj interface
    //
    virtual bool threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
    virtual bool signal(const int threadLogicId);
    virtual bool checkThread(OmnString &errmsg, const int tid) const;

	bool		addLogger(const OmnLoggerPtr &logger);
	bool		removeLogger(const OmnLoggerPtr &logger);
	//bool		procMsg(const OmnMsgPtr &msg);

	OmnRslt		addEntry(const OmnLogEntryPtr &entry);
	void		log(const OmnLogType::E id, 
					const OmnString &str, 
					const bool flushflag);

	void		log(const OmnLogType::E id, const OmnLogEntryPtr &str);

	virtual void		procHouseKeeping(const int64_t &tick);
	virtual OmnString	getName() const {return "LoggerMgr";}

	virtual OmnString   getSysObjName() const {return "OmnLoggerMgr";}
	virtual OmnSingletonObjId::E  getSysObjId() const 
				{return OmnSingletonObjId::eLoggerMgr;}

	OmnLoggerPtr	getLogger(const OmnString &name) const;
	OmnLoggerPtr	createLogger(const OmnLogType::E type, 
								 const OmnString &name, 
								 const OmnXmlItemPtr &conf);
};
#endif
#endif
