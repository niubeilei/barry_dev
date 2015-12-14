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
// 2013/10/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JqlStmtSchedule_H
#define AOS_JQLStatement_JqlStmtSchedule_H

#include "JQLStatement/JqlStatement.h"
#include "Util/String.h"

class AosJqlStmtSchedule : public AosJqlStatement
{

public:
	enum ScheduleType
	{
		eInvalid,     

		eWeekday,
		eMonth,
		eEveryday,
		eDate,
		
		eMaxEntry     
	};
	
private:
	//data from JQLParser
	OmnString		mScheduleName;
	OmnString		mJobName;
	OmnString		mRunTime;
	OmnString		mDate;
	ScheduleType	mScheduleType;
	OmnString		mDateType;
	OmnString		mStopTime;
	OmnString		mPriority;
	OmnString 		mStrType;

public:
	AosJqlStmtSchedule();
	~AosJqlStmtSchedule();

	//getter/setters
	void setScheduleName(OmnString name);
	void setJobName(OmnString name);
	void setDateType(OmnString date_type);
	void setDate(OmnString date);
	void setRunTime(OmnString run_time);
	void setStopTime(OmnString stop_time);
	void setPriority(int priority);

	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

	bool runSchedule(const AosRundataPtr &rdata);
	bool stopSchedule(const AosRundataPtr &rdata);
	bool createSchedule(const AosRundataPtr &rdata);  
	bool showSchedules(const AosRundataPtr &rdata);   
	bool describeSchedule(const AosRundataPtr &rdata);
	bool dropSchedules(const AosRundataPtr &rdata);   

	AosXmlTagPtr convertToXml(const AosRundataPtr &rdata);

private:

	OmnString getScheduleTypeStr();
	AosJqlStmtSchedule::ScheduleType toEnum(OmnString type);
	
};

#endif
