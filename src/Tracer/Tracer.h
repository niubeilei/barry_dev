////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Tracer.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Tracer_Tracer_h
#define Omn_Tracer_Tracer_h

#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Util/MemoryChecker.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/BasicTypes.h"
#include "Util1/TimerObj.h"
#include "XmlParser/Ptrs.h"
#include "XmlUtil/Ptrs.h"

OmnDefineSingletonClass(OmnTracerSingleton,
						OmnTracer,
						OmnTracerSelf,
                		OmnSingletonObjId::eTracer, 
						"Tracer");

class OmnTraceEntry;

class OmnTracer : virtual public OmnTimerObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eNumEntries = 1000,
		eMaxFileSize = 1000000000, //1G
		eMaxFileNum = 2
	};

private:
	OmnFilePtr			mTracerFile;
	OmnFilePtr			mAlarmFile;
	OmnString			mTracerFileName;
	static OmnString	mBaseName;
	OmnString			mAlarmFileName;
	uint				mTracerFileSize;
	uint				mAlarmFileSize;
	OmnString			mProductName;
	OmnString			mVersion;
	OmnString			mPatch;
	int					mBuildNo;

	static OmnFilePtr	mCallLog;
	static OmnFilePtr	mSysAlarmLog;
	static OmnFilePtr	mUserLog;

	OmnString			mBuildTime;
	int					mFlushFileTimeTick;
	int					mCrtFileIdx;
	OmnString			mTimeStr;
	//barry 2015/11/11
	//JIMODB-1063
	OmnMutexPtr			mLock;
	uint				mMaxFileSize;
	int					mMaxFileNum;
	OmnString			mDeleteFileName;

public:
	OmnTracer();
	virtual ~OmnTracer();

	static OmnTracer *	getSelf();
	bool			openTracer(const OmnString &productName, 
								   const OmnString &version,
								   const OmnString &patch,
								   const OmnString &buildTime,
								   const int buildno);

	static bool		mCreateCallLog;
	static int		mNumFiles;
	static int		mDebugLevel;
	static bool		mTraceComm;
	static bool		mTraceThread;
	static bool		mTraceCP;
	static bool		mTraceDb;
	static bool		mTraceTimer;
	static bool		mTraceWrite;
	static bool		mTraceRead;
	static bool		mTraceHB;		// Heartbeat 
	static bool		mTraceHK;		// House Keeping
	static bool		mTraceNM;		// Trace NMS 
	static bool		mTraceRT;		// Trace Retransmission 
	static bool		mTraceMO;		// Trace Managed Objects 
	static bool		mTraceUser;		// Trace User 
	static bool		mTraceKernelApi;
	static bool		mTraceSysAlarm;
	static bool		mStdio;
	static bool		mStdioAndFile;	//Barry 2015/12/02
	static bool		mTraceDoc;		// Chen Ding, 2015/02/01
	static void		setNumLogFiles(const int n) {mNumFiles = n;}

	static void		retrieveFileName(const OmnXmlParserPtr &parser);
	bool			start();
	bool			stop();
	OmnRslt			config(const AosXmlTagPtr &conf);

	void			configTracer();
	static OmnTraceEntry &	getTraceEntry(const char *file,
								  const int line,
								  const bool flag);
	bool			writeTraceEntry(const char *str, const int length);
	bool			writeAlarmEntry(const char *str, const int length);
	bool			writeEventEntry(const char *str, const int length);

private:
	bool			determineFileNames();
	bool			writeTraceEntryPrivate(const char *str, const int length);
	bool			createFiles();
	OmnFilePtr		createFile(const OmnString &filename AosMemoryCheckDecl);
	bool			closeTracer();

	void			procHouseKeeping();

	static void		createSysAlarmLog();
	static void		writeSysAlarmLog(const OmnString &log);
	void 			renameFileNames();

public:
	const char * getTimeStr() const {return mTimeStr.data();}
	void timeout(	const int timerId,
			        const OmnString &timerName,
					void *parm);

};


#ifndef OmnTrace 
#define OmnTrace OmnTracer::getTraceEntry(__FILE__, __LINE__, true)
#endif

#ifndef OmnTraceComm 
#define OmnTraceComm OmnTracer::getTraceEntry(__FILE__, __LINE__, OmnTracer::mTraceComm)
#endif

#ifndef OmnTraceMgcp
#define OmnTraceMgcp OmnTracer::getTraceEntry(__FILE__, __LINE__, OmnTracer::mTraceMgcp)
#endif

#ifndef OmnTraceThread
#define OmnTraceThread OmnTracer::getTraceEntry(__FILE__, __LINE__, OmnTracer::mTraceThread)
#endif

#ifndef OmnTraceDb
#define OmnTraceDb OmnTracer::getTraceEntry(__FILE__, __LINE__, OmnTracer::mTraceDb)
#endif

#ifndef OmnTraceMO
#define OmnTraceMO OmnTracer::getTraceEntry(__FILE__, __LINE__, OmnTracer::mTraceMO)
#endif

#ifndef OmnTraceHB
#define OmnTraceHB OmnTracer::getTraceEntry(__FILE__, __LINE__, OmnTracer::mTraceHB)
#endif

#ifndef OmnTraceHK
#define OmnTraceHK OmnTracer::getTraceEntry(__FILE__, __LINE__, OmnTracer::mTraceHK)
#endif

#ifndef OmnTraceWrite
#define OmnTraceWrite OmnTracer::getTraceEntry(__FILE__, __LINE__, OmnTracer::mTraceWrite)
#endif

#ifndef OmnTraceRead
#define OmnTraceRead OmnTracer::getTraceEntry(__FILE__, __LINE__, OmnTracer::mTraceRead)
#endif

#ifndef OmnTraceKernelApi
#define OmnTraceKernelApi OmnTracer::getTraceEntry(__FILE__, __LINE__, OmnTracer::mTraceKernelApi)
#endif

#ifndef OmnTraceNM
#define OmnTraceNM OmnTracer::getTraceEntry(__FILE__, __LINE__, OmnTracer::mTraceNM)
#endif

#ifndef OmnTraceCP
#define OmnTraceCP OmnTracer::getTraceEntry(__FILE__, __LINE__, OmnTracer::mTraceCP)
#endif

#ifndef OmnTraceTimer
#define OmnTraceTimer OmnTracer::getTraceEntry(__FILE__, __LINE__, OmnTracer::mTraceTimer)
#endif

#ifndef OmnTraceRT
#define OmnTraceRT OmnTracer::getTraceEntry(__FILE__, __LINE__, OmnTracer::mTraceRT)
#endif



#endif
