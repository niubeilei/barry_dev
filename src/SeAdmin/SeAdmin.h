////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 	Created: 06/23/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SeAdmin_SeAdmin_h
#define AOS_SeAdmin_SeAdmin_h

#include "Util/Opr.h"
#include "SearchEngine/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/SeTypes.h"
#include "SEUtil/DocTags.h"
#include "SEClient/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/DynArray.h"
#include "UtilComm/Ptrs.h"
#include "XmlInterface/XmlRc.h"


OmnDefineSingletonClass(AosSeAdminSingleton,
						AosSeAdmin,
						AosSeAdminSelf,
						OmnSingletonObjId::eSengAdmin,
						"SengAdmin");


class AosSeAdmin : virtual public OmnRCObject, 
				  virtual public OmnThreadedObj
{
	OmnDefineRCObject;

	enum 
	{
		eDftBackupFreq = 3600*4,		// Every four hours
		eDftThreadFreq = 1,				// Every 1 second
		eDftFullBackupHour = 3,
		eDftFullBackupMinute = 0
	};

private:
	OmnMutexPtr		mLock;
	OmnThreadPtr	mThread;
	
	OmnString		mBackupDir;
	OmnString		mDatapath;
	int				mBackupFreq;
	int				mThreadFreq;
	int				mLastBackup;
	int				mFullBkHour;
	int				mFullBkMinute;
	bool			mLastFullBackup;

public:
	AosSeAdmin();
	~AosSeAdmin();

    //
    // Singleton class interface
    //
    static AosSeAdmin*  	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual OmnString   getSysObjName() const {return "AosSeAdmin";}
    virtual OmnRslt     config(const OmnXmlParserPtr &def);
    virtual OmnSingletonObjId::E  
						getSysObjId() const 
						{
							return OmnSingletonObjId::eSeAdmin;
						}

	bool		start(const AosXmlTagPtr &config);

	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool	shutdownServer(AosXmlRc &errcode, OmnString &errmsg);

private:
	bool 	procAdmin();
	bool	checkBackup();
	bool	doDataBackup();
	bool	doFullBackup();
};
#endif

