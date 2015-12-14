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
// 2014/04/08 Created by King 
////////////////////////////////////////////////////////////////////////////
#include "Funcs/Jimos/DateFormat.h"
//#include "boost/date_time/posix_time/posix_time.hpp"
//#include "boost/date_time.hpp"
#include "API/AosApi.h"
#include "time.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"
//using namespace boost::gregorian;
//using namespace boost::posix_time;

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDateFormat_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDateFormat(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosDateFormat::AosDateFormat(const int version)
:
AosGenFunc("DateFormat", version)
{
}

AosDateFormat::AosDateFormat()
:
AosGenFunc("DateFormat", 1)
{
}



AosDateFormat::AosDateFormat(const AosDateFormat &rhs)
:
AosGenFunc(rhs)
{
}


AosDateFormat::~AosDateFormat()
{
}


bool
AosDateFormat::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosDateFormat::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}


bool
AosDateFormat::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	// Recode year month day hour minuter second
	int date_items[6] = {0,0,0,0,0,0};	
	int year, month, day, hour, minute, second;
	int item_begin = 0;
	int item_num = 0;
	int item_cnt = 0;
	int len = 0;
	int i = 0;
	char temp[20];
	char format_symbol;
	struct tm dateinfo;	
	OmnString val; 
	string format_str, date_str;

	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	if (mValue.isNull())
	{
		goto setnullvalue;
	}
	date_str = mValue.getStr();
	if (date_str == "")
	{
		goto setnullvalue;
	}

	if (!AosGenFunc::getValue(rdata, 1, record)) return false;
	if (mValue.isNull())
	{
		goto setnullvalue;
	}
	format_str = mValue.getStr();
	if (format_str == "")
	{
		goto setnullvalue;
	}

	// Parse and check date string
	len = date_str.size();
	try
	{
		while(i < len)
		{
			if(isdigit(date_str[i]))
			{	
				item_begin = i;
				item_cnt = 1;
				while(isdigit(date_str[i]) && i < len)
				{
					i++;
					item_cnt++;
				}
				if(item_num > 6)
				{
					break;
				}
				// Truncate string and convert to int
				date_items[item_num++] = atoi((date_str.substr(item_begin, item_cnt)).c_str());
			}
			else if(isalpha(date_str[i]))
			{
				goto setnullvalue;
			}
			else
			{
				i++;
			}
		}
		// Must have year month day
		if(item_num < 3)
		{
			goto setnullvalue;
		}
		// Check year
		year = date_items[0];
		if(year < 0 || year > 10000)
		{	
			goto setnullvalue;
		}
		else if(year > 9 && year < YY_PART_YEAR)
		{
			year += 2000;
		}
		else if(year >= YY_PART_YEAR && year < 100)
		{
			year += 1900;
		}
		date_items[0] = year;
		// Check month
		month = date_items[1];
		if(month < 0 || month > 13)
		{
			goto setnullvalue;
		}
		// Check day
		day = date_items[2];
		if(!isValidDay(year, month, day))
		{
			goto setnullvalue;
		}
		// Check hour 
		hour = date_items[3];
		if(hour < 0 || hour > 23)
		{
			goto setnullvalue;
		}
		// Check minute 
		minute = date_items[4];
		if(minute < 0 || minute > 59)
		{
			goto setnullvalue;
		}
		// Check seconds 
		second = date_items[5];
		if(second < 0 || second > 59)
		{
			goto setnullvalue;
		}
	}
	catch (...)
	{
		OmnAlarm<<"Failed parse and check data string"<<enderr;
		goto setnullvalue;
	}

	// Convert date string to specified format 
	len = format_str.size();
	convertDate(&dateinfo, date_items);	

	try
	{
		for(int j = 0; j < len; j++)
		{
			memset(temp, 0x0, sizeof(temp));
			if(format_str[j] == '%' && j < len - 1)
			{
				format_symbol = format_str[++j];
				switch(format_symbol)
				{
					// Year
					case 'y': // Year, numeric (two digits)
						year %= 100;	
						sprintf(temp, "%02u", year);
						break;
					case 'Y': // Year, numeric, four digits
						sprintf(temp, "%04u", year);
						break;
					
					// Month
					case 'c': // Month, numeric (0..12)
						sprintf(temp, "%u", month);
						break;
					case 'm': // Month, numeric (00..12)
						sprintf(temp, "%02u", month);
						break;
					case 'M': // Month name (January..December)
						if(month == 0)
							goto setnullvalue;
						sprintf(temp, "%s", sMonth[month].c_str());
						break;
					case 'b': // Abbreviated month name (Jan..Dec)
						if(month == 0)
							goto setnullvalue;
						sprintf(temp, "%s", sMonth[month].substr(0, 3).c_str());
						break;

					// Day
					case 'd': // Day of the month, numeric (00..31)
						sprintf(temp, "%02u", day);
						break;
					case 'e': // Day of the month, numeric (0..31)
						sprintf(temp, "%u", day);
						break;
					case 'D': // Day of the month with English suffix (0th, 1st, 2nd, 3rd, …)
						if(day <= 3)
						{
							sprintf(temp, "%s", sDay[day].c_str());
						}
						else
						{	
							sprintf(temp, "%uth", day);
						}
						break;
						
					// Hour
					case 'h': // Hour (01..12)
					case 'I': // Hour (01..12)
						//strftime(temp, 20, "%I", &dateinfo);
						sprintf(temp, "%02u", hour==0 ? 12 : hour%12);
						break;
					case 'l': // Hour (1..12)
						sprintf(temp, "%u", hour==0 ? 12 : hour%12);
						break;	
					case 'k': //Hour (0..23) 
						sprintf(temp, "%u", hour);
						break;
					case 'H': //Hour (00..23)
						sprintf(temp, "%02u", hour);
						//strftime(temp, 20, "%H", &dateinfo);
						break;	
						
					// Minutes
					case 'i': //Minutes, numeric (00..59) 
						sprintf(temp, "%02u", minute);
						break;
					
					// Seconds
					case 'S': // Seconds (00..59)
					case 's': // Seconds (00..59)
						sprintf(temp, "%02u", second);
						break;
							
					// Second part
					// Always '000000', the same with mysql
					case 'f': // Microseconds (000000..999999)
						sprintf(temp, "%s", "000000");
						break;
						
					// AM | PM
					case 'p': // AM or PM 
						strftime(temp, 20, "%p", &dateinfo);
						break;
					
					// Exotic things					
					// Calculate weekday use zeller formula
					case 'W': // Weekday name (Sunday..Saturday)
						strftime(temp, 20, "%A", &dateinfo);
						break;
					case 'a': // Abbreviated weekday name (Sun..Sat)
						strftime(temp, 20, "%a", &dateinfo);
						break;
					case 'w': // Day of the week (0=Sunday..6=Saturday)
						strftime(temp, 20, "%w", &dateinfo);
						break;
					case 'j': //Day of year (001..366)
						strftime(temp, 20, "%j", &dateinfo);
						break;
					
					case 'r': // Time, 12-hour (hh:mm:ss followed by AM or PM)
						strftime(temp, 20, "%r", &dateinfo);
						break;
					case 'T': // Time, 24-hour (hh:mm:ss)
						strftime(temp, 20, "%T", &dateinfo);
						break;

					// Week numbers
					case 'U': // Week (00..53), where Sunday is the first day of the week; WEEK() mode 0 
						strftime(temp, 20, "%U", &dateinfo);
						break;
					case 'u': // Week (00..53), where Monday is the first day of the week; WEEK() mode 1 

						strftime(temp, 20, "%V", &dateinfo);
/*						
						strftime(temp, 20, "%w", &dateinfo);
						week = atoi(temp);
						memset(temp, 0x0, 20);
						strftime(temp, 20, "%U", &dateinfo);
						week_day = atoi(temp);
						if(week == 0)
						{
							week_day += 1;	
						}
						memset(temp, 0x0, 20);
						sprintf(temp, "%u", week_day);
*/						
						break;
					
					// Not support
					case 'V': // Week (01..53), where Sunday is the first day of the week; WEEK() mode 2; used with %X
						//strftime (temp,20,"%V",timeinfo);
					case 'v': // Week (01..53), where Monday is the first day of the week; WEEK() mode 3; used with %x
						//strftime (temp,20,"%u",timeinfo);
					case 'X': // Year for the week where Sunday is the first day of the week, numeric, four digits; used with %V 
					case 'x': // Year for the week, where Monday is the first day of the week, numeric, four digits; used with %v
						goto setnullvalue;	
						break;
						
					default:
						sprintf(temp, "%c", format_symbol);
				}
				val += temp;
			}
			else 
			{
				sprintf(temp, "%c", format_str[j]);
				val += temp;
			}
		}
		value.setStr(val);
	}
	catch (...)
	{
		OmnAlarm<<"Convert data string error"<<enderr;
		goto setnullvalue;
	}	
	
	return true;

setnullvalue:
	OmnString timeFormat;
	timeFormat = "NULL";                
	value.setStr(timeFormat);         
	return true;
}


bool 
AosDateFormat::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments == 2
	if (mSize!=2)
	{
		errmsg << "DateFormat only needs  2 parameters.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosDateFormat::cloneJimo()  const
{
	try
	{
		return OmnNew AosDateFormat(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

// Vonvert user date to tm
bool 
AosDateFormat::convertDate(struct tm *timeinfo, int date_items[])
{
	//time_t rawtime;
	
	//time ( &rawtime );
	//timeinfo = localtime ( &rawtime );
	timeinfo->tm_year = date_items[0] - 1900;
	timeinfo->tm_mon  = date_items[1] - 1;
	timeinfo->tm_mday = date_items[2];
	timeinfo->tm_hour = date_items[3];
	timeinfo->tm_min  = date_items[4];
	timeinfo->tm_sec  = date_items[5];

	mktime(timeinfo);

	return true;

}

bool 
AosDateFormat::isLeapYear(int year)
{
	if((year % 4 == 0 && year % 100 != 0 ) || year % 400 == 0)
		return true;
	
	return false;
}

bool 
AosDateFormat::isValidDay(int year, int month, int day)
{
	int max_day = 0;
	if (month <= 12 && day <= 31) 
	{ 
				
		if((month == 0 || month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12))
			max_day = 31;
		else if(month == 4 || month == 6 || month == 9 || month == 11)
			max_day = 30;
		else if(month == 2 && isLeapYear(year))
			max_day = 29;
		else
			max_day = 28;
		
		if(day >= 0 && day <= max_day)
			return true;
	} 

	return false; 
} 


