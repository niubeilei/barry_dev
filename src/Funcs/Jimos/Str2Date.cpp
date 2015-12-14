////////////////////////////////////////////////////////////////////////////     
////                                                                               
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
#include "Funcs/Jimos/Str2Date.h" 
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"
#include "UtilTime/TimeUtil.h"

extern "C"
{	 
	AosJimoPtr AosCreateJimoFunc_AosStr2Date_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosStr2Date(version);
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

AosStr2Date::AosStr2Date(const int version)
:
AosGenFunc("Str2Date", version)
{
}

AosStr2Date::AosStr2Date()
:
AosGenFunc("Str2Date", 1)
{
}

AosStr2Date::AosStr2Date(const AosStr2Date &rhs)
:
AosGenFunc(rhs)
{
}

AosStr2Date::~AosStr2Date()
{
}


bool
AosStr2Date::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosStr2Date::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}


bool
AosStr2Date::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	if (mValue.isNull())                                                    
	{                                                                   
		value.setNull();                                           
		return true;                                                    
	}                                                                   
	OmnString timeStr = mValue.getStr();                                 
	if (timeStr == "")                                                     
	{                                                                   
		value.setNull();                                           
		return true;                                                    
	}                                                                   

	if (!AosGenFunc::getValue(rdata, 1, record)) return false;
	if (mValue.isNull())                                                    
	{                                                                   
		value.setNull();                                           
		return true;                                                    
	}                                                                   
	OmnString format = mValue.getStr();                                   
	if (format == "")                                                       
	{                                                                   
		value.setNull();                                           
		return true;                                                    
	}

	timeStr.toLower();
	bool error = false,isAmPmTime = false;
	int isAm = 0;
	u32 year = 0, month = 0, day = 0;
	u32 hour = 0, minute = 0, second = 0;
	u32 week = 10, weekNumber = 100;
	u32	dayNumber = 1000;
	u32 hasV = 0, hasX = 0, xYear = 0;
	u32 hasU = 0, hasR = 0;
	char lastUOrV;
	OmnString *second_part = new OmnString();
	format.replaceStrWithSpace();
	timeStr.replaceStrWithSpace();
	int formatLength = format.length();
	int timeStrLength = timeStr.length();
	int idx = 0;
	OmnString tmp;
	bool isBreak = false, hasTime = false, hasDay = false;
	OmnString *time = new OmnString();

	for(int i=0; i<formatLength; i += 2)
	{
		if (error)	break;
		if (isBreak) break;
		if (format.getChar(i) == ' ')                                         
		{                                                                     
			    i ++;
		}                                                                     
		if (timeStr.getChar(idx) == ' ')                                      
		{                                                                     
			    idx ++;                                                           
		}            
		if (timeStrLength < idx+1)	break;
		tmp = "";
		if (format.getChar(i) == '%' && (i+1) < formatLength)
		{
			u32 length;
			switch (format.getChar(i+1))
			{
				case '%':
					length = 1;                                      
					if (!isEnoughLength(timeStrLength, length, idx))
					{
						isBreak = true;
					}      
					idx += length;    
					if (length == 1)
					{
						if (timeStr.subString(idx, length) != "%")
						{
							error = true;
						}
					}
					break;     

				/* year */
				case 'X':
					hasX = 1;
				case 'x':
					if (format.getChar(i+1) == 'x')	hasX = 2;
				case 'Y':
					length = 4;
				case 'y':
					hasDay = true;
					if (format.getChar(i+1) == 'y')	length = 2;
					if (!isEnoughLength(timeStrLength, length, idx)) isBreak = true;
					tmp = timeStr.subString(idx, length);
					if (isU32(tmp, length, year))                    
					{                                          
						idx += length;                         
						if (length <= 2)                       
						{                                      
							year = year_2000_handling(year);   
						}                    
						if (format.getChar(i+1) == 'X' || format.getChar(i+1) == 'x')     
						{                                                                 
							xYear = year;                                                 
						}                                                                 
					}  
					else 
					{
						error = true;
					}
					break;           

					/* month */
				case 'm':
				case 'c':
					hasDay = true;
					length = 2;
					if (!isEnoughLength(timeStrLength, length, idx)) isBreak = true;
					tmp = timeStr.subString(idx, length);
					if (isU32(tmp, length, month))  
					{
						idx += length;
						if (month < 0 || month >= 13)
						{
							error = true;
						}
					}
					else 
					{
						error = true;
					}
					break;
				case 'M':
					hasDay = true;
					//January...December
					if (checkWord(timeStr, idx, tmp))
					{
						if (judgeMonth(tmp, month, 'M'))     
						{                                            
							if (month < 0 || month >= 13)            
							{                                        
								error = true;                        
							}                                        
						}                                            
						else                                         
						{                                            
							error = true;                            
						}                                            
						                                             
					}
					else 
					{
						error = true;
					}
					break;                                      
				case 'b':
					//Jan...Dec
					hasDay = true;
					if (checkWord(timeStr, idx, tmp))    
					{
						if (judgeMonth(tmp, month, 'b'))             
						{                                                    
							if (month < 0 || month >= 13)                    
							{                                                
								error = true;                                
							}                                                
						}                                                    
						else                                                 
						{                                                    
							error = true;
							break;
						}                                                    
					}
					else 
					{
						error = true;
					}
					break;

					/* day */
				case 'd':
				case 'e':
					hasDay = true;
					length = 2;
					if (!isEnoughLength(timeStrLength, length, idx)) isBreak = true;
					tmp = timeStr.subString(idx, length);    
					if (isU32(tmp, length, day))                  
					{                                        
						idx += length;                       
						if (day < 0 || day >32)
						{
							error = true;
						}
					}                
					else 
					{
						error = true;
					}
					break;
				case 'D':
					//1st, 2nd, 3r
					hasDay = true;
					length = 2;
					if (!isEnoughLength(timeStrLength, length, idx)) isBreak = true;
					tmp = timeStr.subString(idx, length);   
					if (isU32(tmp, length, day))
					{
						idx += (length+2);
						if (day < 0 || day >32)      
						{                            
							error = true;
						}                            
					} 
					else 
					{
						error = true;
					}
					break;

					/* hour */
				case 'h':
				case 'I':
				case 'l':
					isAmPmTime = true;
				case 'k':
				case 'H':
					hasTime = true;
					length = 2;
					if (!isEnoughLength(timeStrLength, length, idx)) isBreak = true;
					tmp = timeStr.subString(idx, length);        
					if (isU32(tmp, length, hour))                      
					{                                            
						idx += length; 
						if ((hour <= 0 || hour > 12) && isAmPmTime)                  
						{                                        
							error = true;                        
						}                                        
						else if ((hour < 0 || hour >= 24) && !isAmPmTime) 
						{
							error = true;
						}
					}                                            
					else                                         
					{                                            
						error = true;                            
					}                                            
					break;                                       

					/* minute */
				case 'i':
					hasTime = true;
					length = 2;
					if (!isEnoughLength(timeStrLength, length, idx)) isBreak = true;
					tmp = timeStr.subString(idx, length);                      
					if (isU32(tmp, length, minute))                                    
					{                                                          
						idx += length;                                         
						if ((minute < 0 || minute >= 60))           
						{                                                      
							error = true;                                      
						}                                                      
					}                                                          
					else                                                       
					{                                                          
						error = true;                                          
					}                                                          
					break;              

					/* second */
				case 's':
				case 'S':
					hasTime = true;
					length = 2;                                
					if (!isEnoughLength(timeStrLength, length, idx)) isBreak = true;
					tmp = timeStr.subString(idx, length);       
					if (isU32(tmp, length, second))                     
					{                                           
						idx += length;                          
						if ((second < 0 || second >= 60))         
						{                                       
							error = true;                       
						}                                       
					}                                           
					else                                        
					{                                           
						error = true;                           
					}                                           
					break;                                      
					
					/* second part */
				case 'f':
					hasTime = true;
					length = 6;                                
					if (!isEnoughLength(timeStrLength, length, idx)) isBreak = true;
					tmp = timeStr.subString(idx, length);         
					if (tmp.isDigitStr())                       
					{                                             
						idx += length;  
						*second_part = tmp;
						second_part->removeWhiteSpaces();
						if (second_part->getChar(0) == '-')         
						{                                         
							error = true;                         
						}                                         
					}                                             
					else                                          
					{                                             
						error = true;                             
					}                                             
					break;

					/* pm or am */
				case 'p':
					length = 2;
					if (!isEnoughLength(timeStrLength, length, idx)) isBreak = true;
					tmp = timeStr.subString(idx, length);
					if (isLetter(tmp, length)) 
					{
						idx += length; 
						if (tmp == "am")
						{
							isAm = 1;
						}
						else if (tmp == "pm")   
						{
							isAm = 2;
						}
						else 
						{
							error = true;
						}
					}
					else 
					{
						error = true;
					}
					break;

					/* exotic things */
				case 'W':
					if (checkWord(timeStr, idx, tmp))
					{
						if (!judgeWeek(tmp, week, 'W'))
						{
							error = true;
						}
					}
					else 
					{
						error = true;
					}
					break;
				case 'a':
					tmp = "";
					if (checkWord(timeStr, idx, tmp))
					{                                                                    
						if (!judgeWeek(tmp, week, 'a'))                             
						{                                                            
							error = true;                                            
						}                                                            
					}                                                                    
					else                                                                 
					{                                                                    
					    error = true;                                                    
					}                                                                    
					break;                                                               
				case 'w':
					length = 1;
					if (!isEnoughLength(timeStrLength, length, idx)) isBreak = true;
					tmp = timeStr.subString(idx, length);          
					if (isU32(tmp, length, week))           
					{                                            
						idx += length;                           
						if (week > 6 && week != 10)
						{   
							error = true;                          
						}                                          
					}   
					else             
					{                
						error = true;
					}     
					break;
				case 'j':
					length = 3;
					if (!isEnoughLength(timeStrLength, length, idx)) isBreak = true;
					tmp = timeStr.subString(idx, length);          
					if (isU32(tmp, length, dayNumber))           
					{                                            
						idx += length;                           
						if (dayNumber != 1000 && dayNumber < 0)
						{   
							error = true;                          
						}                                          
					}   
					else             
					{                
						error = true;
					}   
					break;

					/* week numbers */
				case 'V':
					hasV = 1;
				case 'v':
					if (format.getChar(i+1) == 'v')	hasV = 2;
				case 'U':
					if (format.getChar(i+1) == 'U')	hasU = 1;
				case 'u':
					if (format.getChar(i+1) == 'u')	hasU = 2;
					length = 2;
					if (!isEnoughLength(timeStrLength, length, idx)) isBreak = true;
					tmp = timeStr.subString(idx, length);   
					if (isU32(tmp, length, weekNumber))                  
					{                                              
						idx += length;                             
						if ((weekNumber != 100 && weekNumber > 53) || weekNumber < 0)
						{
							error = true;
						}
					}                                              
					else                                           
					{                                              
						error = true;                              
					}           
					lastUOrV = format.getChar(i+1);
					break;

				/* time in am/pm notation */
				case 'r':
					hasR = 1;
					hasTime = true;
					if (!judgeTime(*time, timeStr, idx, timeStrLength))
					{
						error = true;
					}
					if (timeStr.getChar(idx) == ' ')    idx++; 
					char timeChar;
					if (idx < timeStrLength)
					{
						timeChar = timeStr.getChar(idx);
						switch (timeChar)
						{
							case 'a':
							case 'p':
								if (idx+1 <= timeStrLength)
								{
									if (timeStr.getChar(idx+1) == 'm')
									{
										*time << timeStr.subString(idx, 2);
										idx += 2;
									}
									else 
									{
										error = true;
									}
								}
								else 
								{
									error = true;
								}
								break;
							default:
								error = true;
								break;
						}
					}
					break;

					/* time in 24-hour notation */
				case 'T':
					hasTime = true;
					if (!judgeTime(*time, timeStr, idx, timeStrLength))
					{
						error = true;
					}
					break;

					/* convesion s[ecofoers that match classes of characters*/
				case '.':
					timeChar = timeStr.getChar(idx);
					while (timeChar == '.' && (idx+1) < timeStrLength)
					{
						idx ++;                           
						timeChar = timeStr.getChar(idx);  
						if (idx == timeStrLength)	break;
					}
					break;
				case '@':
					timeChar = timeStr.getChar(idx);
					while ((timeChar >= 'a' && timeChar <= 'z') || (timeChar >= 'A' && timeChar <= 'Z'))
					{
						idx ++;                                
						timeChar = timeStr.getChar(idx);       
						if (idx == timeStrLength)	break;
					}
					break;
				case '#':
					timeChar = timeStr.getChar(idx);                                                       
					while (timeChar >= '0' && timeChar <= '9')        
					{                                                                                           
						idx ++;                                                                                 
						timeChar = timeStr.getChar(idx);                                                        
						if (idx == timeStrLength)   break;                                                      
					}                                                                                           
					break;
				default:
					error = true;
					break;
			}
		}
		else if (idx < timeStrLength && format.getChar(i) != timeStr.getChar(idx))
		{
			error = true;
		}
		else 
		{
			idx ++;
			i --;
		}
	}

	OmnString timeFormat;
	if (hasX)
	{
		if (lastUOrV != 'v' && lastUOrV != 'V')
		{
			error = true;
		}
	}
	if (lastUOrV == 'V' || lastUOrV == 'v')
	{
		if (!hasX)
		{
			error = true;
		}
	}
	if (error)                 
	{                   
		timeFormat = "NULL";
		value.setStr(timeFormat);
		delete second_part;
		delete time;
		return true;           
	}                         
	if (hour != 0 && isAm == 2)
	{
		hour += 12;
	}
	if (hasX == hasV && hasX)
	{
		year = xYear;
	}
	if (week != 10 && weekNumber != 100 && year)                        
	{                                                                   
		//caculate month and day                                        
		if (!hasU && week == 0) week = 7;                               
		calcDayNumberByWeek(year, hasU, week, weekNumber, dayNumber);   
		calcDay(year, month, day, dayNumber);                           
	}
	else 
	{
		if (hasX == hasV && hasX)	year = 0;
		if (dayNumber != 1000 && year)               
		{                                            
			//caculate month and day                 
			calcDay(year, month, day, dayNumber);    
		}                                            
	}
	if (*time != "")
	{
		char type = 'T';
		if (hasR) type = 'r';
		if (!calcTime(*time, hour, minute, second, type))
		{
			timeFormat = "NULL";
			value.setStr(timeFormat); 
			delete second_part;
			delete time;
			return true;
		}
	}

	int paramLen;
	if (hasDay)
	{
		paramLen = 4;
		appendStr(timeFormat, year, paramLen);
		timeFormat << "-";
		paramLen = 2;
		appendStr(timeFormat, month, paramLen);
		timeFormat << "-";
		paramLen = 2;      
		appendStr(timeFormat, day, paramLen);
		if (hasTime || *second_part != "") 
		{
			timeFormat << " ";
			paramLen = 2;
			appendStr(timeFormat, hour, paramLen);        
			timeFormat << ":";
			paramLen = 2;      
			appendStr(timeFormat, minute, paramLen);      
			timeFormat << ":"; 
			paramLen = 2;      
			appendStr(timeFormat, second, paramLen);
			if (*second_part != "")
			{
				while (second_part->length() < 6)            
				{                                           
					*second_part <<  "0";                    
				}                                           
				timeFormat << "." << *second_part;           
			}
		}
		value.setStr((string)timeFormat);
	}
	else if (hasTime || *second_part != "")
	{
		paramLen = 2;
		appendStr(timeFormat, hour, paramLen);
		timeFormat << ":";
		paramLen = 2;      
		appendStr(timeFormat, minute, paramLen); 
		timeFormat << ":";
		paramLen = 2;      
		appendStr(timeFormat, second, paramLen); 
		if (*second_part != "") 
		{                                        
			while (second_part->length() < 6)     
			{                                    
				*second_part <<  "0";             
			}                                    
			timeFormat << "." << *second_part;    
		}                                        
		value.setStr((string)timeFormat);
	}
 	else 
	{
		timeFormat = "0000-00-00";
		value.setStr(timeFormat);
	}
	delete second_part;
	delete time;
	return true;
}

bool 
AosStr2Date::checkWord(OmnString str, int &idx, OmnString &word)
{
	int timeStrLength = str.length();
	char charWord;
	while (idx < timeStrLength)
	{
		charWord = str.getChar(idx);
		if ((charWord >= 'a' && charWord <= 'z') || (charWord >= 'A' && charWord <= 'Z'))
		{
			word << charWord;
			idx ++;
		}
		else 
		{
			if (word.length() == 0)
			{
				return false;
			}
			break;
		}
	}
	return true;
}

bool 
AosStr2Date::calcTime(OmnString time, u32 &hour, u32 &minute, u32 &second, char type)
{
	int flag = true;
	OmnString tmp;	
	vector<OmnString> substrs;
	OmnString substr = ":";
	AosStrSplit::splitStrByChar(time, substr.data(), substrs, 4);
	for (u32 i=0; i<substrs.size(); i++)
	{
		substrs[i].replaceStrWithSpace();
	}
	int num = substrs.size();
	u32 len;
	u32 temp;
	switch (num)
	{
		case 3:
			if (isU32(substrs[2], len, second))
			{
				if (len > 2 || len <= 0) 
				{
					flag = false;
				}
			}
			else 
			{
				flag = false;
			}
			if (len < (u32)substrs[2].length())
			{
				OmnString amOrPm = substrs[2].subString(substrs[2].length()-2, 2);
				if (amOrPm == "pm")
				{
					temp = 12;   
				}
				else if (amOrPm != "am" || type == 'T')
				{
					flag = false;   
				}
			}
		case 2:
			if (isU32(substrs[1], len, minute))
			{
				if (len > 2 || len <= 0)
				{
					flag = false;  
				}
			}
			else 
			{
				flag = false;
			}
		case 1:
			if (isU32(substrs[0], len, hour))    
			{                                    
				if (len > 2 || len <= 0)         
				{                                
					flag = false;                
				}   
				if (hour < 0 || hour >= 24)
				{
					flag = false;
				}
				if (type == 'r' && (hour > 12 || hour == 0))
				{
					if (hour > 12 || hour == 0)
					{
						flag = false;
					}
				}
				if (type == 'r' && hour == 12)
				{
					hour = 0;
				}
				if (type == 'r' && temp == 12)
				{
					hour += temp;
				}
			}
			else 
			{
				flag = false;
				break;
			}
			break;
		default:
			flag = false;
			break;
	}
	return flag;
}

void
AosStr2Date::calcDayNumberByWeek(u32 &year, u32 hasU, u32 week, u32 weekNumber, u32 &dayNumber)
{
	int weekStart = calcWeekDay(year);
	if (hasU == 2)
	{
		int temp = weekNumber;
		if (weekStart > 4)
		{
			temp ++;
		}
		if (temp <= 1)
		{
			year --;                                                   
			if ((year%400==0) || ((year%4==0) && !(year%100==0)))      
			{                                                          
				    dayNumber = 360 - weekStart + week + temp * 7;                    
			}                                                          
			else                                                       
			{                                                          
				    dayNumber = 359 - weekStart + week + temp * 7;                    
			}                                                          
		}
		else 
		{
			dayNumber = 7 * temp + week - weekStart - 6; 
		}
		if (week == 0)	dayNumber += 7;
	}
	else 
	{
		if (weekNumber == 0)
		{
			year --;
			if ((year%400==0) || ((year%4==0) && !(year%100==0)))
			{
				dayNumber = 367 - weekStart + week; 
			}
			else 
			{
				dayNumber = 366 - weekStart + week;
			}
		}
		else
		{
			dayNumber = 7 * weekNumber + week - weekStart + 1;	
		}
	}
}

int 
AosStr2Date::calcWeekDay(u32 iYear)  
{  
	int iWeek = 0;  
	u32 y = 0, c = 0, m = 0, d = 0;  
	int iMonth = 1;
	c = ( iYear - 1 ) / 100;  
	y = ( iYear - 1 ) % 100;  
	m = iMonth + 12;  
	d = 1;  
	iWeek = y + y / 4 + c / 4 - 2 * c + 26 * ( m + 1 ) / 10 + d - 1;      
	iWeek = iWeek >= 0 ? ( iWeek % 7 ) : ( iWeek % 7 + 7 );    
	if (iWeek == 0)
	{
		iWeek = 7;
	}
	return iWeek;  
}  

void 
AosStr2Date::calcDay(u32 &year, u32 &month, u32 &day, u32 dayNumber)
{
	bool isLeapYear = false;
	month = 0;
	if (((year%4==0) && (year%100!=0)) || (year%400==0))
	{
		isLeapYear = true;	
	}
	int temp = dayNumber;
	while (temp > 0)
	{
		int tmp = temp;
		month ++;
		if (month == 13)
		{
			year ++;
			if (((year%4==0) && (year%100!=0)) || (year%400==0))          
			{                                                             
				isLeapYear = true;                                        
			}                                                             
			month = 1;
		}
		
		switch (month)
		{
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
				temp -= 31;
				break;
			case 2:
				if (isLeapYear) 
				{
					temp -= 29;
				}
				else 
				{
					temp -= 28;
				}
				break;
			default:
				temp -= 30;
				break;
		}
		if (temp <= 0)
		{
			day = tmp;
		}
	}
	return;
}

bool 
AosStr2Date::judgeTime(OmnString &time, OmnString timeStr, int &idx, int timeStrLength)
{
	if (idx+1 <= timeStrLength)
	{
		int        len;
		regex_t       re;      /* 存储编译好的正则表达式，正则表达式在使用之前要经过编译 */
		regmatch_t    subs [SUBSLEN]; /* 存储匹配到的字符串位置 */
		char          matched [BUFLEN];     /* 存储匹配到的字符串 */
		int           err;
		char          src[100];
		char          pattern [] = "([01][0-9]|2[0-3]|[0-9])\\s*(:\\s*[0-5]{0,1}[0-9]\\s*){0,2}"; /* pattern字符串 */
		for (u32 i=(u32)idx; (i<(u32)timeStrLength && i-idx<100); i++)
		{
			src[i-idx] = timeStr.getChar(i);
		}
		err = regcomp(&re, pattern, REG_EXTENDED);
		err = regexec(&re, src, (size_t) SUBSLEN, subs, 0);
		if (err) 	return false;
		len = subs[0].rm_eo - subs[0].rm_so;
		memcpy (matched, src + subs[0].rm_so, len);
		matched[len] = '\0';
		regfree(&re); /* 用完了别忘了释放 */
		idx += len;
		string temp = matched;
		time = temp;
		if (idx+1 <= timeStrLength)
		{
			if (timeStr.getChar(idx) == ' ')
			{
				idx ++;
			}
			int num = 0;
			for (int i=0; i<time.length(); i++)
			{
				if (time.getChar(i) == ':')
				{
					num ++;
				}
			}
			if (idx+1 == timeStrLength && timeStr.getChar(idx) == ':' && num < 2)
			{
				idx ++;
			}
		}
	}
	return true;        
}

bool 
AosStr2Date::judgeWeek(OmnString &str, u32 &value, char type)
{
	bool flag = true;
	int length = str.length();
	if (type == 'a' && length > 3)
	{
		flag = false;
	}
	if (length > 0)
	{
		switch (str.getChar(0))
		{   
			case 'm':
				if (length > 6 && type == 'W')	flag = false;
				break;
			case 't':
				if (length > 2)
				{
					switch (str.getChar(1))
					{
						case 'u':
							if (length > 7 && type == 'W')
							{
								flag = false;
							}
							break;
						case 'h':
							if (length > 8 && type == 'W')
							{
								flag = false;
							}
							break;
						default:
							flag = false;
							break;
					}
				}
				else 
				{
					flag = false;
				}
				break;
			case 'w':
				break;
			case 'f':
				if (length > 6 && type == 'W')
				{
					flag = false;
				}
				break;
			case 's':
				if (length > 2)
				{
					switch (str.getChar(1))
					{
						case 'a':
							if (length > 8 && type == 'W')
							{
								flag = false;
							}
							break;
						case 'u':
							if (length > 6 && type == 'W')
							{
								flag = false;
							}
							break;
						default:
							flag = false;
							break;
					}
				}
				else 
				{
					flag = false;
				}
				break;
			default:
				flag = false;
				break;
		}
		if (!flag)	return flag;
		bool isMatch = false;
		u32 i = 0;
		for (; i<sizeof(str2date::weeks); i++)
		{
			if (str2date::weeks[i].subString(0, length) == str.subString(0, length))
			{
				isMatch = true;
				break;
			}
		}
		isMatch 
			? value = i
			: flag = false;
	}
	else 
	{
		flag = false;
	}
	return flag;
}

bool
AosStr2Date::isEnoughLength(u32 length, u32 &subLength, u32 idx)
{
	while (length < (subLength+idx))
	{
		subLength --;
		if (subLength <= 0)
		{
			return false;
		}
	}
	return true;
}

bool 
AosStr2Date::isLetter(OmnString str, u32 &length)
{
	for (int i=0; i<str.length(); i++)
	{
		char temp = str.getChar(i);
		if (temp < 'A' || (temp > 'Z' && temp < 'a') || temp > 'z')
		{
			if (i != 0)
			{
				length = i+1;
				return true;
			}
			else 
			{
				return false;
			}
		}
	}
	return true;
}

bool
AosStr2Date::judgeMonth(OmnString &str, u32 &value, char type)
{
	bool flag = true;
	u32 length = str.length();
	if (type == 'b')
	{
		if (str.getChar(0) == 's')
		{
			if (length > 4)
			{
				flag = false;
			}
		}
		else 
		{
			if (length > 3)
			{
				flag = false;
			}
		}
	}
	if (length > 0)
	{
		switch (str.getChar(0))
		{   
			case 'j':
				if (length > 1) 
				{
					case 'a':
						if (type == 'M' && length > 7) flag = false;
						break;
					case 'u':
						if (length > 2)
						{
							if (type == 'M' && length > 4) flag = false;	
							switch (str.getChar(2))
							{
								case 'n':
								case 'l':
									break;
								default:
									flag = false;
									break;
							}
						}
						else 
						{
							flag = false;	
						}
						break;
				}
				else 
				{
					flag = false;
				}
			case 'f':
				if (type == 'M' && length > 8) flag = false;
				break;
			case 'm':
				if (length > 3)
				{
					if (str.getChar(2) == 'y')
					{
						if (type == 'M' && length > 3) flag = false;
					}
					else if (str.getChar(2) == 'r')
					{
						if (type == 'M' && length > 5) flag = false;
					}
					else 
					{
						flag = false;
					}
				}
				break;
			case 's':
				break;
			case 'o':
				if (type == 'M' && length > 7)	flag = false;
				break;
			case 'n':
				if (type == 'M' && length > 8)	flag = false;    
				break;
			case 'd':
				if (type == 'M' && length > 8)  flag = false;
				break;
			default:
				flag = false;
				break;
		}
		if (!flag)	return flag;
		bool isMatch = false;
		u32 i = 0;
		for (; i<sizeof(str2date::months); i++)
		{
			if (str2date::months[i].subString(0, length) == str.subString(0, length))
			{
				isMatch = true;
				break;
			}
		}
		isMatch 
			? value = i + 1
			: flag = false;
	}
	else 
	{
		flag = false;
	}
	return flag;
}

bool
AosStr2Date::isU32(OmnString &str, u32 &length, u32 &value)
{

	if(!str.parseU32(0, length, value))
	{
		length --;
		if (length > 0)
		{
			str = str.subString(0, length);
			return isU32(str, length, value);	
		}
	}
	else
	{
		return true;
	}
	
	return false;
}

int
AosStr2Date::year_2000_handling(int year)
{
	if ((year=year+1900) < 1900+YY_PART_YEAR)
		year+=100;
	return year;
}

void                                                        
AosStr2Date::appendStr(OmnString &str,u32 value,int &len)    
{                                                           
	if (len > 0)
	{
		if (value/(int)pow(10,len-1) > 0)           
		{                                    
			str << value;                    
			return;                          
		}                                    
		else                                 
		{                                    
			str << "0";      
			len --;
			appendStr(str, value, len);    
		}                                    
	} 
}                                                           
                                                            
bool 
AosStr2Date::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	if (mSize != 2)
	{
		errmsg << "Str2Date needs  2 parameter.";
	    return false;
	}
			   			   
	return true;
}

AosJimoPtr
AosStr2Date::cloneJimo()  const
{
	try
	{
		return OmnNew AosStr2Date(*this);
	}
		  
	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}
			   
	OmnShouldNeverComeHere;
	return 0;
}


