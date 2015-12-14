////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Alarm.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Alarm_Alarm_h
#define Omn_Alarm_Alarm_h

#include "Alarm/Ptrs.h"
#include "Alarm/HtmlError.h"
#include "aosUtil/Types.h"
#include "Debug/ErrId.h"
#include "Util/Object.h"
#include "Util/RCObjImp.h"
#include <stdio.h>
#include <string>


#ifndef AosHtmlWarn
#define AosHtmlWarn(errmsg) AosGenerateHtmlErrorCode(__FILE__, __LINE__, OmnErrId::eWarning, errmsg)
#endif

#ifndef AosHtmlAlarm
#define AosHtmlAlarm(errmsg) AosGenerateHtmlErrorCode(__FILE__, __LINE__, OmnErrId::eAlarm, errmsg)
#endif

#ifndef enderr
#define enderr OmnEndError()
#endif

class OmnEndError
{
};

enum AlarmLevel
{
	eAlarmLevel1,
	eAlarmLevel2,
	eAlarmLevel3,
	eAlarmLevel4,
	eAlarmLevel5
};

#ifndef OmnNotImplementedYet
#define OmnNotImplementedYet OmnAlarm << "Not implemented yet" << enderr;
#endif

#ifndef OmnShouldNeverComeHere
#define OmnShouldNeverComeHere OmnAlarm << "Should never come to this point" << enderr;
#endif

#ifndef OmnShouldNeverComeToThisPoint
#define OmnShouldNeverComeToThisPoint OmnAlarm << "Should never come to this point" << enderr;
#endif

#ifndef OmnAlert 
#define OmnAlert OmnTracer::getTraceEntry(__FILE__, __LINE__, true) << "+++++ "
#endif

#ifndef OmnProgAlarm 
#define OmnProgAlarm OmnAlarmEntryBegin(__FILE__, __LINE__) << OmnErrId::eProgError
#endif

#ifndef OmnWarn 
#define OmnWarn OmnAlarmEntryBegin(__FILE__, __LINE__) << OmnErrId::eWarning 
#endif

#ifndef OmnWarnProgError
#define OmnWarnProgError OmnAlarmEntryBegin(__FILE__, __LINE__) << OmnErrId::eWarning 
#endif

#ifndef OmnAlarm
#define OmnAlarm OmnAlarmEntryBegin(__FILE__, __LINE__) << OmnErrId::eAlarm 
#endif

#ifndef OmnSynErr
#define OmnSynErr OmnAlarmEntryBegin(__FILE__, __LINE__) << OmnErrId::eSynErr 
#endif

#ifndef OmnAlarmProgError
#define OmnAlarmProgError OmnAlarmEntryBegin(__FILE__, __LINE__) << OmnErrId::eAlarm 
#endif





class OmnSerialTo;
class OmnSerialFrom;
class OmnString;

class OmnAlarmEntry : virtual public OmnObject
{
	OmnDefineRCObject;

public:
	enum
	{

		eFile,
		eLine,
		eAlarmCat,
		eAlarmId,
		eAlarmSeqno,
		eEntityId,
		eThreadId,
		eTime,
		eErrMsg,

		eMaxFileNameLength = 200,
		eTimeStrLength = 101,
		eErrMsgMaxLength = 1010
	};

private:
	char				mFile[eMaxFileNameLength+1];
	int					mLine;
	OmnErrId::E			mErrorId;
	int					mAlarmSeqno;
	unsigned int		mThreadId;
	char				mTime[eTimeStrLength+1];
	char				mErrMsg[eErrMsgMaxLength];
	bool				mNeedToRaise;
	OmnString			mModuleName;	// 2014/10/28 Felicia, Query-460

public:
	OmnAlarmEntry() 
		:
	OmnObject(OmnClassId::eOmnAlarmEntry),
	mLine(0),
	mErrorId(OmnErrId::eUnknown),
	mThreadId(0),
	mNeedToRaise(true)
	{
		mFile[0] = 0;
		mTime[0] = 0;
		mErrMsg[0] = 0;
	}

	~OmnAlarmEntry() {}

	//
	// OmnObject interface
	//
	virtual int				objSize() const {return sizeof(*this);}

	bool toString(const int nn, char *data, const int length) const;
	OmnString	toXml(const int num_alarms) const;
	OmnAlarmEntry & operator << (const OmnString &errMsg);
	OmnAlarmEntry & operator << (const std::string &errMsg);
	OmnAlarmEntry & operator << (const OmnEndError er);

	OmnAlarmEntry & operator << (const char *errmsg)
	{
		if (errmsg)
		{
			if (strlen(mErrMsg) + strlen(errmsg) < eErrMsgMaxLength)
			{
				strcat(mErrMsg, errmsg);
			}
			else
			{
				strncat(mErrMsg, errmsg, eErrMsgMaxLength - strlen(mErrMsg) - 1);
			}
		}

		return *this;
	}


	OmnAlarmEntry & operator << (const u8 *msg)
	{
		return *this << (const char *)msg;
	}


	OmnAlarmEntry & operator << (const int value)
	{
		if (strlen(mErrMsg) + 10 < eErrMsgMaxLength)
		{
			sprintf(&mErrMsg[strlen(mErrMsg)], "%d", value);
		}
		return *this;
	}

	OmnAlarmEntry & operator << (const u8 value)
	{
		if (strlen(mErrMsg) + 3 < eErrMsgMaxLength)
		{
			sprintf(&mErrMsg[strlen(mErrMsg)], "%u", (unsigned int)value);
		}
		return *this;
	}

	// 
	// Chen Ding, 02/06/2007
	//
	OmnAlarmEntry & operator << (const u32 value)
	{
		if (strlen(mErrMsg) + 10 < eErrMsgMaxLength)
		{
			sprintf(&mErrMsg[strlen(mErrMsg)], "%u", (unsigned int)value);
		}
		return *this;
	}

	// 
	// Chen Ding, 12/10/2007
	//
	OmnAlarmEntry & operator << (void *ptr)
	{
		if (strlen(mErrMsg) + 10 < eErrMsgMaxLength)
		{
			sprintf(&mErrMsg[strlen(mErrMsg)], "%lx", (unsigned long)ptr);
		}
		return *this;
	}

#ifndef __amd64__
	OmnAlarmEntry & operator << (const unsigned long value)
	{
		char buff[101];
		sprintf(buff, "%lu", value);
		if (strlen(mErrMsg) + strlen(buff) + 1 < eErrMsgMaxLength)
		{
			sprintf(&mErrMsg[strlen(mErrMsg)], "%s", buff);
		}
		return *this;
	}
#endif

	OmnAlarmEntry & operator << (const u64 value)
	{
		char buff[30];
		sprintf(buff, "%llu", (unsigned long long int)value);
		if (strlen(mErrMsg) + strlen(buff) + 1 < eErrMsgMaxLength)
		{
			sprintf(&mErrMsg[strlen(mErrMsg)], "%s", buff);
		}
		return *this;
	}

	OmnAlarmEntry & operator << (const int64_t value)
	{
		char buff[30];
		sprintf(buff, "%lld", (long long)value);
		if (strlen(mErrMsg) + strlen(buff) + 1 < eErrMsgMaxLength)
		{
			sprintf(&mErrMsg[strlen(mErrMsg)], "%s", buff);
		}
		return *this;
	}

	OmnAlarmEntry & operator << (const double value)
	{
		char buff[30];
		sprintf(buff, "%lf", value);
		if (strlen(mErrMsg) + strlen(buff) + 1 < eErrMsgMaxLength)
		{
			sprintf(&mErrMsg[strlen(mErrMsg)], "%s", buff);
		}
		return *this;
	}

	OmnErrId::E		getErrId() const {return mErrorId;} 
	int				getAlarmSeqno() const {return mAlarmSeqno;}
	bool			needToRaise() const {return mNeedToRaise;}
	const char * const	getErrMsg() const 
	{
		return mErrMsg;
	}

	//
	// Sets
	//
	void	setErrId(const OmnErrId::E id) {mErrorId = id;}
	void	setModuleName(const OmnString &name) {mModuleName = name;}
	void	setThreadId(const unsigned int i) {mThreadId=i;}
	void	setFile(const char *file)
	{
		strncpy(mFile, file, eMaxFileNameLength);
	}

	void	setLine(const int line) {mLine = line;}
	void	setSeqno(const int sno) {mAlarmSeqno = sno;}
	void	setTime(const char *t)
	{
		if (!t || strlen(t) >= eTimeStrLength)
		{
			return;
		}

		strcpy(mTime, t);
	}

	void	setErrMsg(const char *msg)
	{
		if (!msg || strlen(msg) >= eErrMsgMaxLength)
		{
			strncpy(mErrMsg, msg, eErrMsgMaxLength-1);
			return;
		}
		strcpy(mErrMsg, msg);
	}

	void	appendErrMsg(const int value)
	{
		if (strlen(mErrMsg) + 10 >= eErrMsgMaxLength)
		{
			return;
		}

		sprintf(&mErrMsg[strlen(mErrMsg)], "%d", value);
	}

	static bool	addChar(char *to, 
					const int length, 
					int &pos,
					const char * const from)
	{
		int fLen = strlen(from);
		if (pos + fLen >= length)
		{
			return false;
		}

		strncpy(&to[pos], from, fLen);
		pos += fLen;
		to[pos] = 0;
		return true;
	}

	static bool addInt(char *to, 
				const int length,
				int &pos, 
				const int value)
	{
		char buff[30];
		sprintf(buff, "%d", value);
		int len = strlen(buff);
		if (pos + len >= length)
		{
			return false;
		}
		strncpy(&to[pos], buff, len);
		pos += len;
		to[pos] = 0;
		return true;
	}

	static bool addIntHex(char *to, 
				const int length,
				int &pos, 
				const int value)
	{
		char buff[30];
		sprintf(buff, "0x%x", value);
		int len = strlen(buff);
		if (pos + len >= length)
		{
			return false;
		}
		strncpy(&to[pos], buff, len);
		pos += len;
		to[pos] = 0;
		return true;
	}

	const char *getFilename() const {return mFile;}
	const int getLine() const {return mLine;}
	bool toXml(OmnString &doc);

private:
	bool toString_Alarm(const int nn, char *data, const int length) const;
	bool toString_SynErr(const int nn, char *data, const int length) const;

};


struct OmnAlarmEntryBegin
{
private:
	char			mFile[OmnAlarmEntry::eMaxFileNameLength+1];
	int				mLine;

public:
    OmnAlarmEntryBegin(const char *file, const int line);
	OmnAlarmEntry & operator << (const OmnErrId::E errId);
};
#endif

