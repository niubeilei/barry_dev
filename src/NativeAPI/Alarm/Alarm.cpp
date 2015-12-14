////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Alarm.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Alarm/Alarm.h"

#include "Alarm/AlarmMgr.h"
#include "API/AosApi.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "Porting/ThreadDef.h"
#include "Porting/Sleep.h"
#include "Util1/Time.h"

OmnAlarmEntry &
OmnAlarmEntryBegin::operator << (const OmnErrId::E errId)
{
	//return OmnAlarmMgr::getEntry(mFile, mLine, errId);
}

OmnAlarmEntry &
OmnAlarmEntry::operator << (const OmnEndError)
{
	/*
	mThreadId = OmnGetCurrentThreadId();
	OmnAlarmMgr::closeEntry(*this);

	if (!OmnAlarmMgr::ignoreAlarm(mFile, mLine))
	{
		// Chen Ding, 2013/05/30
		int pause_on_alarm = OmnAlarmMgr::pauseOnAlarm();
		AosAlarmBreakPoint(pause_on_alarm);	 // Ketty 2013/04/28 
	}
*/
	return *this;
}


OmnAlarmEntry & 
OmnAlarmEntry::operator << (const OmnString &errMsg)
{
	if (strlen(mErrMsg) + errMsg.length() < eErrMsgMaxLength)
	{
		strcat(mErrMsg, errMsg.getBuffer());
	}
	return *this;
}


OmnAlarmEntry & 
OmnAlarmEntry::operator << (const std::string &errMsg)
{
	if (strlen(mErrMsg) + errMsg.length() < eErrMsgMaxLength)
	{
		strcat(mErrMsg, errMsg.data());
	}
	return *this;
}

OmnAlarmEntryBegin::OmnAlarmEntryBegin(const char *file, const int line)
{


	strncpy(mFile, file, OmnAlarmEntry::eMaxFileNameLength);
	mLine = line;
}


bool
OmnAlarmEntry::toString(
		const int num_alarms, 
		char *data, 
		const int length) const
{
	if(mErrorId == OmnErrId::eSynErr)
	{
		return toString_SynErr(num_alarms, data, length);
	}
	return toString_Alarm(num_alarms, data, length);
}

bool
OmnAlarmEntry::toString_Alarm(
		const int num_alarms, 
		char *data, 
		const int length) const
{
	// 
	// Write the alarm in the following form:
	//
	//	"\n\n<"
	//	<< OmnTime::getSecTick()
	//	<< "> ******* Alarm Entry **********"
	//	"\nFile:              " << mFile << 
	//	"\nLine:              " << mLine << 
	//  "\nAlarm ID:          " << mErrorId << 
	//	"\nSeqno:             " << mAlarmSeqno << 
	//	"\nTrigger Thread ID: " << mThreadId <<
	//	"\nTrigger Time:      " << mTime <<
	//	"\nError Message:     " << mErrMsg << 
	//	"\n*************************\n";
	// 
	// Note that we did not use OmnString in order not to generate any alarms
	// while converting this entry into a string. 
	// 
	int index = 0;
	if (!addChar(data, length, index, "\n\n<")) return false;
	if (!addInt(data, length, index, OmnTime::getSecTick())) return false;

	OmnString aa = "> ***** Alarm Entry ";
	aa << mFile << ":" << mLine << ":" << num_alarms << " ******";
	// if (!addChar(data, length, index, "> ***** Alarm Entry *****")) return false;
	if (!addChar(data, length, index, aa.data())) return false;
	// if (!addChar(data, length, index, "\nFile:                ")) return false;
	// if (!addChar(data, length, index, mFile)) return false;
	// if (!addChar(data, length, index, "\nLine:                ")) return false;
	// if (!addInt(data, length, index, mLine)) return false;
	if (!addChar(data, length, index, "\nError Id:            ")) return false;
	if (!addInt(data, length, index, mErrorId)) return false;
	if (!addChar(data, length, index, "\nSeqno:               ")) return false;
	if (!addInt(data, length, index, mAlarmSeqno)) return false;
	if (!addChar(data, length, index, "\nThread ID:           ")) return false;
	if (!addIntHex(data, length, index, mThreadId)) return false;
	if (!addChar(data, length, index, "\nTrigger Time:        ")) return false;
	if (!addChar(data, length, index, mTime)) return false;
	if (!addChar(data, length, index, "\nError Message:       ")) return false;
	if (!addChar(data, length, index, mErrMsg)) return false;
	if (!addChar(data, length, index, "\n*************************\n")) return false;
	return true;
}	

bool
OmnAlarmEntry::toString_SynErr(
		const int num_alarms, 
		char *data, 
		const int length) const
{
	// 
	// Write the alarm in the following form:
	//
	//	"\n\n<"
	//	<< OmnTime::getSecTick()
	//	<< "> [Syntax Error][File:Line]"<< 
	//	"\nTrigger Thread ID: " << mThreadId <<
	//	"\nTrigger Time:      " << mTime <<
	//	"\nError Message:     " << mErrMsg << 
	//	"\n\n";
	// 
	// Note that we did not use OmnString in order not to generate any alarms
	// while converting this entry into a string. 
	// 
	int index = 0;
	if (!addChar(data, length, index, "\n\n<")) return false;
	if (!addInt(data, length, index, OmnTime::getSecTick())) return false;

	OmnString aa = "> [Syntax Error][";
	aa << mFile << ":" << mLine << ":" << num_alarms << "]";
	if (!addChar(data, length, index, aa.data())) return false;
	//if (!addChar(data, length, index, "\nError Id:            ")) return false;
	//if (!addInt(data, length, index, mErrorId)) return false;
	//if (!addChar(data, length, index, "\nSeqno:               ")) return false;
	//if (!addInt(data, length, index, mAlarmSeqno)) return false;
	if (!addChar(data, length, index, "\nThread ID:           ")) return false;
	if (!addIntHex(data, length, index, mThreadId)) return false;
	if (!addChar(data, length, index, "\nTrigger Time:        ")) return false;
	if (!addChar(data, length, index, mTime)) return false;
	if (!addChar(data, length, index, "\nError Message:       ")) return false;
	if (!addChar(data, length, index, mErrMsg)) return false;
	if (!addChar(data, length, index, "\n\n")) return false;
	return true;
}

OmnString
OmnAlarmEntry::toXml(const int num_alarms) const
{
	// Write the alarm in the following form:
	//
	//	<alarm_doc zky_filename=\"mFile\" 
	//			   zky_linenum=\"mLine\" 
	//			   zky_num_alarms=\"num_alarms\" 
	//			   zky_alarm_id=\"mErrorId\" 
	//			   zky_alarm_seqno=\"mAlarmSeqno\" 
	//			   zky_thread_id=\"mThreadId\" 
	//			   zky_trigger_time=\"mTime\">
	//		<error_msg><![CDATA[mErrMsg]]></error_msg>
	//	</alarm_doc>
	
	OmnString docstr = "<alarm_doc ";
	docstr << "zky_filename=\"" << mFile << "\" "
		   << "zky_linenum=\"" << mLine << "\" "
		   << "zky_num_alarms=\"" << num_alarms << "\" "
		   << "zky_alarm_id=\"" << mErrorId << "\" "
		   << "zky_alarm_seqno=\"" << mAlarmSeqno << "\" "
		   << "zky_thread_id=\"" << mThreadId << "\" "
		   << "zky_module_name=\"" << mModuleName << "\" "
		   << "zky_pctrs=\"" << AOSZTG_ALARM << "\" "
		   << "zky_public_ctnr=\"true\" "
		   << "zky_public_doc=\"true\" "
		   << "zky_trigger_time=\"" << mTime << "\">"
		   << "<error_msg><![CDATA[" << mErrMsg << "]]></error_msg>"
		<< "</alarm_doc>\n";
	return docstr;
}

