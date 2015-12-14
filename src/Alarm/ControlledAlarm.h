////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ControlledAlarm.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Alarm_ControlledAlarm_h
#define Omn_Alarm_ControlledAlarm_h


class OmnControlledAlarm 
{
private:

public:
	OmnControlledAlarm() {}
	~OmnControlledAlarm() {}

	OmnControlledAlarm & operator << (const char *errmsg)
	{
		mAlarm.appendErrMsg(OmnString(errmsg));
		return *this;
	}

	OmnControlledAlarm & operator << (const OmnString &errMsg)
	{
		mAlarm.appendErrMsg(errMsg);
		return *this;
	}

	OmnControlledAlarm & operator << (const int value)
	{
		mAlarm.appendErrMsg(value);
		return *this;
	}

	OmnRslt operator << (const OmnEndError er);

	OmnErrId::E		getAlarmId() const {return mAlarm.getAlarmId();}
	int				getAlarmSeqno() const {return mAlarm.getAlarmSeqno();}
	OmnString		getErrMsg() const {return mAlarm.getErrMsg();}

	OmnSoAlarm &	getSo() {return mAlarm;}

	void	setEntryId(const int id) {mAlarm.setEntryId(id);}
	void	setLevel(const OmnAlarmLevel::E l) {mAlarm.setLevel(l);}
	void	setAlarmCat(const OmnAlarmCat::E cat) {mAlarm.setAlarmCat(cat);}
	void	setAlarmId(const OmnErrId::E i) {mAlarm.setAlarmId(i);}
	void	setSeqno(const int sno) {mAlarm.setSeqno(sno);}
	void	setThreadId(const int tid) {mAlarm.setThreadId(tid);}
	void	setEntityId(const OmnString &tid) {mAlarm.setEntityId(tid);}
	void	setFile(const OmnString &f) {mAlarm.setFile(f);}
	void	setLine(const int line) {mAlarm.setLine(line);}
	void	setTime(const OmnString &s) {mAlarm.setTime(s);}
	void	setErrMsg(const OmnString &errMsg) {mAlarm.setErrMsg(errMsg);}

	OmnRslt		serializeTo(OmnSerialTo &s) const;
	OmnRslt		serializeToDb() const;
	OmnRslt		serializeFrom(OmnSerialFrom &s);
	OmnString	toString() const;
	OmnString	toLongForm() const;
};

#endif

