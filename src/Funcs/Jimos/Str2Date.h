//////////////////////////////////////////////////////////////////////////////
//// Copyright (C) 2005
//// Packet Engineering, Inc. All rights reserved. 
//// 
//// Redistribution and use in source and binary forms, with or without 
//// modification is not permitted unless authorized in writing by a duly
//// appointed officer of Packet Engineering, Inc. or its derivatives
//// 
//// Description: 
////    
//// Modification History:
//// 2015/03/04 Created by Crystal Cao 
//////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Funcs_Jimos_Str2Date_h
#define Aos_Funcs_Jimos_Str2Date_h
#define YY_PART_YEAR       70
#define SUBSLEN 1              /* 匹配子串的数量 */
#define BUFLEN 20          /* 匹配到的字符串buffer长度 */
#include <regex.h>
#include <stdio.h>
#include <string.h>
#include "Funcs/GenFunc.h"

namespace str2date
{
	OmnString months[] = {"january", "february", "march",
		"april", "may", "june", "july", "august", 
		"september", "october", "november", "december"};
	OmnString weeks[] = { "sunday", "monday", "tuesday", 
		"wednesday", "thursday", "friday", "saturday"};
}
class AosStr2Date : public AosGenFunc
{
	OmnDefineRCObject;
private:
	bool isU32(OmnString &str, u32 &length, u32 &value);
	int  year_2000_handling(int year);
	void appendStr(OmnString &str, u32 value, int &len);
	bool judgeWeek(OmnString &str, u32 &value, char type);
	bool judgeMonth(OmnString &str, u32 &value, char type);
	bool judgeTime(OmnString &time, OmnString timeStr, int &idx, int timeStrLength); 
	bool isEnoughLength(u32 length, u32 &subLength, u32 idx);
	void calcDay( u32 &year, u32 &month, u32 &day, u32 dayNumber);
	bool calcTime( OmnString time, u32 &hour, u32 &minute, u32 &second, char type);
	bool isLetter(OmnString str, u32 &length);
	bool checkWord( OmnString str, int &idx, OmnString &word);
	void calcDayNumberByWeek( u32 &year, u32 hasU, u32 week, u32 weekNumber, u32 &dayNumber);
	int  calcWeekDay(u32 iYear);

public:
	AosStr2Date();
	AosStr2Date(const int version);
	AosStr2Date(const AosStr2Date &rhs);
	~AosStr2Date();

	virtual AosDataType::E getDataType(
							AosRundata *rdata,
							AosDataRecordObj *record);

	virtual bool 		getValue(
							AosRundata *rdata, 
							AosValueRslt &value, 
							AosDataRecordObj *record);

	virtual AosJimoPtr 	cloneJimo() const;

	virtual bool 		config(
							const AosRundataPtr &rdata,
							const AosXmlTagPtr &worker_doc,
							const AosXmlTagPtr &jimo_doc);

	virtual bool 		syntaxCheck(
							const AosRundataPtr &rdata, 
							OmnString &errmsg);
};
#endif
