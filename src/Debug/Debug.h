////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Debug.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Debug_Debug_h
#define Omn_Debug_Debug_h

#include "Debug/ErrorMsg.h"
#include "MultiLang/DictUtil.h"
#include "Tracer/TraceEntry.h"
#include "Tracer/Tracer.h"
#include "Util/String.h"
#include "Util/LogBag.h"
#include "XmlParser/Ptrs.h"

#ifndef AOSMONITORLOG_DEFINE
#define AOSMONITORLOG_DEFINE OmnString _monitor_local_log; _monitor_local_log << __FILE__ << ":" << __LINE__
#endif

#ifndef AOSMONITORLOG
#define AOSMONITORLOG _monitor_local_log << ":" << __LINE__ << ":"
#endif

#ifndef AOS_APPEND_DEBUG_LOG
#define AOS_APPEND_DEBUG_LOG(msg) OmnDebug::appendDebugContents(__FILE__, __LINE__, msg)
#endif

class OmnDebug
{
private:
	// 
	// No one should generate instances of this class.
	//
	OmnDebug();

public:
	~OmnDebug();

	static int		mDebugLevel;
	static bool		mTraceComm;
	static bool		mTraceThread;
	static bool		mTraceMgcp;
	static bool		mTraceCP;
	static bool		mTraceDb;
	static bool		mTraceTimer;
	static bool		mTraceWrite;
	static bool		mTraceRead;
	static bool		mTraceHK;		// House Keeping
	static bool		mTraceMR;		// Trace Memory Management 
	static bool		mTraceNM;		// Trace NMS 
	static bool		mTraceRT;		// Trace Retransmission 
	static bool		mTraceMO;		// Trace Managed Objects 

	OmnDebug &operator << (char *);

	//
	// Linux does not accept this
	// Chen Ding, 6/18/2002
	//
	//OmnDebug &operator << (ostream &(__cdecl *)(ostream &));

	static void		setDebugLevel(const int level);
	static OmnRslt	config(const OmnXmlParserPtr &def);

	// Chen Ding, 2013/09/30
	static bool appendDebugContents(
					const char *fname, 
					const int line, 
					const OmnString &contents);
};

#ifndef OmnRaiseRslt
#define OmnRaiseRslt(x) OmnRslt x; x
#endif

#ifndef OmnTimestamp
#define OmnTimestamp { \
			timeval t; \
			gettimeofday(&t, 0); \
			OmnTrace << "Timestamp: " << t.tv_sec << ":" << t.tv_usec << endl; \
		}
#endif

#ifndef OmnCout
#define OmnCout cout << hex << "<0x" << pthread_self() << dec << ":" << __FILE__ << ":" << __LINE__  << "> " 
#endif

#ifndef OmnScreen
#define OmnScreen OmnTrace
#endif

#ifndef OmnMark
#define OmnMark cout << "<" << __FILE__ << ":" << __LINE__ << ">" << endl
#endif

#ifndef OmnLineInfo
#define OmnLineInfo	OmnString("[") + OmnString(__FILE__) + OmnString(":") + OmnItoa(__LINE__)
#endif

#ifndef OmnFuncInfo
#define OmnFuncInfo cout << "[" << __FILE__ \
			<< ":" << __LINE__ \
			<< ":" << __FUNCTION__ \
			<< "]"
#endif

#ifndef OmnTagFuncInfo
#define OmnTagFuncInfo if (false) \
			cout << "[FFF: " << __FILE__ \
				<< ":" << __LINE__ \
				<< ":" << __FUNCTION__ \
				<< "]"
#endif

#ifndef OmnFileLine
#define OmnFileLine __FILE__, __LINE__
#endif

#ifndef AosStrLog
#define AosStrLog(log) (log)->getEntry(__FILE__, __LINE__)
#endif

// extern AosLogBag gAosLogBag1;
// #ifndef AosStrLog1
// #define AosStrLog1 gAosLogBag1.getEntry(__FILE__, __LINE__)
// #endif

// 
// This is an enhanced assert. It checks the condition "cond". 
// If the condition is true, do nothing. Otherwise, it raises
// an alarm with the error contents "errMsg", and then return
// with the value "rslt"
//
// #ifndef OmnAssert
//#define OmnAssert(cond, errMsg, rslt) 
//		if (!(cond)) 
//		{
//			OmnAlarm << "Assert fail: " << (errMsg) << enderr; 
//			return (rslt); 
//		}
//#endif

// 
// This assert will throw exception if the condition fails.
//
#ifndef OmnExceptionAssert
#define OmnExceptionAssert(cond, exceptType, errMsg) \
		if (!(cond)) \
		{\
			OmnEvent << OmnErrId::eException \
				<< (exceptType) << (errMsg) << enderr; \
			OmnExcept e(OmnFileLine, exceptType, errMsg); \
			throw e;\
		}
#endif


// 
// Check will not stop function execution, as Assert does. 
// If it used to check conditions. If conditions fail, it 
// will return the control to the caller (i.e., stop the 
// normal processing).
// 
// Following checks are defined:
//	OmnCheck1(cond)				If the condition fails, return.
//  OmnCheck2(cond, msg)		If the condition fails, log 'msg' and return.
//  OmnCheck3(cond, rslt)		If the condition fails, return 'rslt'.
//  OmnCheck4(cond, msg, rslt)	If the condition fails, log 'msg' and return 'rslt'.
//
#ifndef OmnCheck
#define OmnCheck(cond, errMsg) \
		if (!(cond)) \
		{\
			OmnAlarmEntryBegin(__FILE__, __LINE__) << OmnErrId::eCheckFail << (errMsg) << enderr; \
			return; \
		}
#endif

#ifndef OmnCheckAReturn
#define OmnCheckAReturn(cond, errMsg, rslt) \
		if (!(cond)) \
		{\
			OmnAlarmEntryBegin(__FILE__, __LINE__) << OmnErrId::eCheckFail << (errMsg) << enderr; \
			return (rslt); \
		}
#endif


// 
// This is an enhanced assert. It checks the condition "cond". 
// If the condition is true, do nothing. Otherwise, it raises
// an alarm with the error contents "errMsg", and then return.
//
#ifndef OmnAssert
#define OmnAssert(cond, errmsg)													\
		if (!(cond))															\
		{																		\
			OmnAlarm << OmnErrId::eAlarmAssertFailed << (errmsg) << enderr;		\
			exit(0);															\
		}																		
#endif

#ifndef OmnAssert1
#define OmnAssert1(cond, errMsg) \
		if (!(cond)) \
		{\
			OmnAlarm << OmnErrId::eAlarmAssertFail << (errMsg) << enderr; \
			return; \
		}
#endif

#ifndef OmnFuncEntry
#define OmnFuncEntry(name)
#endif

#ifndef OmnFuncExit
#define OmnFuncExit(x)
#endif

#define OmnStreaming true

#endif
