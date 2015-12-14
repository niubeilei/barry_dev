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
// 2014/04/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Funcs_Jimos_DateFormat_h
#define Aos_Funcs_Jimos_DateFormat_h

#include "Funcs/GenFunc.h"

// two-digit year < this add 20; >= this add 19
#define YY_PART_YEAR 70
const string sMonth[] = {"x", "January", "February", "March", "April", "May", "June", \
 		   				 "July", "August", "September", "October", "November", "December"};
const string sDay[]   = {"0th", "1st", "2nd", "3rd"};


class AosDateFormat : public AosGenFunc
{
	OmnDefineRCObject;

private:
	virtual bool isLeapYear(int year);
	virtual bool isValidDay(int year, int month, int day);
	virtual bool convertDate(struct tm *timeinfo, int date_items[]);

public:
	AosDateFormat(const int version);
	AosDateFormat();
	AosDateFormat(const AosDateFormat &rhs);
	~AosDateFormat();
	
	virtual AosDataType::E getDataType(
					AosRundata *rdata, 
					AosDataRecordObj *record);

	virtual bool getValue(
					AosRundata *rdata, 
					AosValueRslt &value, 
					AosDataRecordObj *record);

	virtual AosJimoPtr cloneJimo() const;

	virtual bool config(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &worker_doc,
					const AosXmlTagPtr &jimo_doc);

	virtual bool syntaxCheck(
				const AosRundataPtr &rdata, 
				OmnString &errmsg);

};
#endif

