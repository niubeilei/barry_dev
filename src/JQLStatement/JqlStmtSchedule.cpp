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
#include "JQLStatement/JqlStmtSchedule.h"

#include "Debug/Debug.h"
#include "JQLExpr/Expr.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApi.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/BitmapObj.h"

AosJqlStmtSchedule::AosJqlStmtSchedule()
{
	mScheduleName = "";
	mOp = JQLTypes::eOpInvalid;
}

AosJqlStmtSchedule::~AosJqlStmtSchedule()
{
}

bool
AosJqlStmtSchedule::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	// 2015/7/28 JIMODB-77
	bool rslt = checkDatabase(rdata);
	if (!rslt) return false;

	if (mOp == JQLTypes::eCreate) return createSchedule(rdata);
	if (mOp == JQLTypes::eShow) return showSchedules(rdata);
	if (mOp == JQLTypes::eDrop) return dropSchedules(rdata);
	if (mOp == JQLTypes::eDescribe) return describeSchedule(rdata);

	AosSetEntityError(rdata, "JQL_schema_run_err", "JQL Schedule", "")
		<< "opr is undefined!" << enderr;                               
	return false;                                                           
}


bool
AosJqlStmtSchedule::createSchedule(const AosRundataPtr &rdata)
{
	//check if the dataset already exists
	OmnString objid = getObjid(rdata, JQLTypes::eJobDoc, mScheduleName);
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	if (doc)
	{
		OmnString error;
		error << "Schedule already exists: " << mScheduleName;	
		rdata->setJqlMsg(error);

		//already exists
		OmnCout << "Schedule already exists: " << mScheduleName << endl;	
		return true;
	}

	//generate the xml data firstly
	doc = convertToXml(rdata);
	if (!doc) return false;

	if ( !createDocByObjid(rdata, doc, objid) )
	{
		OmnString error;
		error << "Failed to create Schedule doc.";
		rdata->setJqlMsg(error);
	}

	return true;
}


AosXmlTagPtr
AosJqlStmtSchedule::convertToXml(const AosRundataPtr &rdata)
{
	OmnString doc;
	OmnString taskname;
	AosValueRslt vv;

	mScheduleType = toEnum(mDateType);
	doc << "<schedule zky_schedule_name=\"" << mScheduleName << "\""
		<< "zky_pctrs=\"" << AOSTAG_JQL_SCHEDULES_DEF_CTNR << "\" zky_otype=\"schedule\">";

	if (mScheduleType == eWeekday || mScheduleType == eMonth)
	{
		doc << "<schedule type=\""<< mDateType <<"\""
			<< "weekday=\"" << mDate << "\" time=\"" << mRunTime << "\""
			<< "runtimes=\"" << mStopTime << "\"  priority=\"" << mPriority << "\"/>";
	}
	else if (mScheduleType == eEveryday)
	{
		doc << "<schedule type=\""<< mDateType <<"\""
			<< "time=\"" << mRunTime << "\" stoptime=\"" << mStopTime <<"\""
			<< "priority=\"" << mPriority << "\"/>";
	}
	else if (mScheduleType == eDate)
	{
		doc << "<schedule type=\""<< mDateType <<"\""
			<< "time=\"" << mRunTime << "\""
			<< "priority=\"" << mPriority << "\"/>";
	}
	else
	{
		// set error
	}
		doc << "<job name=\"" << mJobName <<"\"/>"
			<< "</schedule>";
	//translate it to xml
	return AosXmlParser::parse(doc AosMemoryCheckerArgs); 
}


bool
AosJqlStmtSchedule::runSchedule(const AosRundataPtr &rdata)
{
	return true;
}


bool
AosJqlStmtSchedule::stopSchedule(const AosRundataPtr &rdata)
{
	return true;
}

bool
AosJqlStmtSchedule::showSchedules(const AosRundataPtr &rdata)
{
	return true;
}

bool 
AosJqlStmtSchedule::dropSchedules(const AosRundataPtr &rdata)
{
	return true;
}


bool
AosJqlStmtSchedule::describeSchedule(const AosRundataPtr &rdata)
{
	return true;
}


AosJqlStatement *
AosJqlStmtSchedule::clone()
{
	return OmnNew AosJqlStmtSchedule(*this);
}


void 
AosJqlStmtSchedule::dump()
{
}


/***************************
 * Getter/setter
 * **************************/
void 
AosJqlStmtSchedule::setScheduleName(OmnString name)
{
	mScheduleName = name;
}


void 
AosJqlStmtSchedule::setJobName(OmnString name)
{
	mJobName = name;
}


void 
AosJqlStmtSchedule::setDateType(OmnString date_type)
{
	mDateType = date_type; 
}


void
AosJqlStmtSchedule::setDate(OmnString date)
{
	mDate = date;
}


void
AosJqlStmtSchedule::setRunTime(OmnString run_time)
{
	mRunTime = run_time;
}


void
AosJqlStmtSchedule::setStopTime(OmnString stop_time)
{
	mStopTime = stop_time;
}


void
AosJqlStmtSchedule::setPriority(int priority)
{
	mPriority = "";
	mPriority << priority;
}

OmnString
AosJqlStmtSchedule::getScheduleTypeStr()
{
	switch(mScheduleType)
	{
	case eMonth:
			return "month";
	case eWeekday:
			return "weekday";
	case eEveryday: 
			return "everyday";
	case eDate:     
			return "date";
	default:
		return "";
	}
	return "";
}

AosJqlStmtSchedule::ScheduleType 
AosJqlStmtSchedule::toEnum(OmnString type)
{
	if (type == "month") return eMonth;
	if (type == "weekday") return eWeekday;
	if (type == "everyday") return eEveryday;
	if (type == "date") return eDate;
	return eInvalid;
}


