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
//  This file contains a group of random functions and a random class. 
//  The OmnRandom class is obselete. You are encouraged to use the 
//  random functions.  
//
// Modification History:
// 01/25/2007 Created by Chen Ding
// 05/18/2007 Moved from Util by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Random/RandomUtil.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Porting/TimeOfDay.h"
#include "Random/RandomRule.h"
#include "Random/RandomStr.h"
#include "Random/random_util.h"
#include "Util/ValueRslt.h"
#include "Util/IpAddr.h"
#include "XmlUtil/XmlTag.h"
#include <arpa/inet.h>
#include <vector> 
// #include <algorithms> 


char	OmnRandom::mAlphanum[65] = "abcedfghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";
int OmnRandom::mAlphanumLen = 64;


const int sgNumCellPhonePrefix = 30;
static OmnString sgCellPhonePrefix[sgNumCellPhonePrefix];
static OmnMutex sgCellPhoneLock;
static bool sgCellPhoneInited = false;

static void initCellPhonePrefix()
{
	sgCellPhoneLock.lock();
	if (sgCellPhoneInited) 
	{
		sgCellPhoneLock.unlock();
		return;
	}

	sgCellPhonePrefix[0] = "86130";
	sgCellPhonePrefix[1] = "86131";
	sgCellPhonePrefix[2] = "86132";
	sgCellPhonePrefix[3] = "86133";
	sgCellPhonePrefix[4] = "86134";
	sgCellPhonePrefix[5] = "86135";
	sgCellPhonePrefix[6] = "86136";
	sgCellPhonePrefix[7] = "86137";
	sgCellPhonePrefix[8] = "86138";
	sgCellPhonePrefix[9] = "86139";
	sgCellPhonePrefix[10] = "86150";
	sgCellPhonePrefix[11] = "86151";
	sgCellPhonePrefix[12] = "86152";
	sgCellPhonePrefix[13] = "86153";
	sgCellPhonePrefix[14] = "86154";
	sgCellPhonePrefix[15] = "86155";
	sgCellPhonePrefix[16] = "86156";
	sgCellPhonePrefix[17] = "86157";
	sgCellPhonePrefix[18] = "86158";
	sgCellPhonePrefix[19] = "86159";
	sgCellPhonePrefix[20] = "86180";
	sgCellPhonePrefix[21] = "86181";
	sgCellPhonePrefix[22] = "86182";
	sgCellPhonePrefix[23] = "86183";
	sgCellPhonePrefix[24] = "86184";
	sgCellPhonePrefix[25] = "86185";
	sgCellPhonePrefix[26] = "86186";
	sgCellPhonePrefix[27] = "86187";
	sgCellPhonePrefix[28] = "86188";
	sgCellPhonePrefix[29] = "86189";
	sgCellPhoneInited = true;
	sgCellPhoneLock.unlock();
}


void
OmnRandom::init()
{
	srand(AosGetUsec());
}


void
OmnRandom::init(u32 seed)
{
	if (0 == seed)
	{
		srandom(time(NULL));
	}
	else
	{
		srandom(seed);
	}
}


OmnString 
OmnRandom::strIP()
{
	int		intPart, i;
	char	ipAddr[16];
	char	charPart[5];
	
	memset(charPart, sizeof(charPart), 0);
	memset(ipAddr, sizeof(ipAddr), 0);

	intPart = nextU32(1, 255);
	sprintf(ipAddr, "%d.", intPart);
		
	for (i = 0; i < 2; i++)
	{
		intPart = nextU32(0, 255);
		sprintf(charPart, "%d.", intPart);
		strcat(ipAddr, charPart); 
	}

	intPart = nextU32(1, 254);
	sprintf(charPart, "%d", intPart);
	strcat(ipAddr, charPart); 
    
	OmnString ret;
	ret = ipAddr;

	return ret;
}


OmnString
OmnRandom::netMac()
{
	//
	//the mac form of xx:xx:xx:xx:xx:xx
	//
	
	OmnString mac;
	char Mac[18];
	char str[]="0123456789ABCDEF";
	for (int i=0; i<17; i++)
	{
		int index = nextU32(0, 15);
		Mac[i] = str[index];

	}
	Mac[17] = '\0';
	// set U/L , I/G flags
	switch(Mac[1])
	{
		case '1':
		case '2':
		case '3':
			Mac[1] = '0';
			break;
		case '5':
		case '6':
		case '7':
			Mac[1] = '4';
			break;
		case '9':
		case 'A':
		case 'B':
			Mac[1] = '8';
			break;
		case 'D':
		case 'E':
		case 'F':
			Mac[1] = 'C';
			break;
		default:			
			break;
	}
	Mac[2]=Mac[5]=Mac[8]=Mac[11]=Mac[14]=':';
	
	mac=Mac;

	return mac;
}


OmnString
OmnRandom::printableStr(int min, int max, bool with_single_quote)
{
	int len = getLength(min, max);
	char buf[len+1];
	for (int k=0; k<len; k++)
	{
		char c = (char)nextU32(32, 126);
		if (c == '"' || c == '\\')
		{
			k--;
			continue;
		}

		if (c == '\'' && !with_single_quote)
		{
			k--;
			continue;
		}

		buf[k] = c;
	}

	buf[len] = 0;
	return buf;
}

OmnString
OmnRandom::nextLetterDigitStr(const u32 min, const u32 max, const bool f)
{
	OmnString str(max+1, 'c', true);
	nextLetterDigitStr(str.getBuffer(), min, max, f);
	str.setLength(strlen(str.getBuffer()));
	return str;
}


OmnString
OmnRandom::digitStr(int minlen, int maxlen)
{
	int len = getLength(minlen, maxlen);
	char str[len+1];
	for (int k=0; k<len; k++)
	{
		str[k] = nextU32(0, 9) + '0';
	}

	str[len] = 0;
	return str;
}


/*
int
OmnRandom::getUniqueInt()
{
	// This function maintains 3 million unique integers that
	// are arranged randomly. 
	const int lsNumValues = 3000000;
	static int lsValues[lsNumValues];
	static bool lsInited = false;
	static int lsIndex = 0;

	if (!lsInited)
	{
		int values = OmnNew int[lsNumValues];
		for (int i=0; i<lsNumValues; i++)
		{
			values[i] = i;
		}

		int nn = lsNumValues;
		for (int i=0; i<lsNumValues; i++)
		{
			if (nn == 0) 
			{
				lsValues[i] = values[0];
				break;
			}

			int idx = rand() % nn;
			lsValues[i] = values[idx];
			values[idx] = values[nn-1];
			nn--;
		}
	}

	if (lsIndex >= lsNumValues) lsIndex = 0;
	return lsValues[lsIndex++];
}
*/


OmnString
OmnRandom::digitStr(
		int total_len, 
		bool left_align, 
		char padding, 
		const bool unique, 
		const bool reset_unique)
{
	static int lsUniqueValue = 1000;

	if (unique)
	{
		int vv = lsUniqueValue++;
		OmnString ss;
		ss << vv;
		if (ss.length() > total_len)
		{
			ss.setLength(total_len);
			return ss;
		}

		OmnString rr(total_len - ss.length(), padding, true);
		if (left_align)
		{
			ss << rr;
			return ss;
		}
	
		rr << ss;
		return rr;
	}

	int len = getLength(1, total_len);
	char str[total_len+1];

	if (left_align)
	{
		for (int k=0; k<len; k++)
		{
			str[k] = nextU32(0, 9) + '0';
		}
		for (int k=len; k<total_len; k++)
		{
			str[k] = padding;
		}
	}
	else
	{
		for (int k=1; k<=len; k++)
		{
			str[total_len-k] = nextU32(0, 9) + '0';
		}
		for (int k=len+1; k<=total_len; k++)
		{
			str[total_len-k] = padding;
		}
	}

	str[total_len] = 0;
	return str;
}


// int 
// OmnRandom::nextInt(const int mask)
// {
// 	return (rand() & mask);
// }

int 
OmnRandom::nextInt(int min1, int max1, int min2, int max2)
{
	if (nextBool()) return nextInt1(min1, max1);

	return nextInt1(min2, max2);
}


int 
OmnRandom::nextInt1(int min, int max)
{
	// Chen Ding, 2014/12/28
	// return getLength(min, max);
	aos_assert_r(min <= max, -1);
	if (min == max) return min;

	i64 len = max-min;
	int vv = rand();
	if (vv >= len) vv = vv%len;
	return min + len;
}


i64 
OmnRandom::nextInt64(i64 min, i64 max)
{
	// Rain, 2014/12/28
	// return getLength(min, max);
	aos_assert_r(min <= max, -1);
	if (min == max) return min;

	i64 len = max-min;
	i64 vv = rand();
	if (vv >= len) vv = vv%len;
	return min + len;
}

	
OmnString
OmnRandom::asciiStr(int minlen, int maxlen)    
{                                                                        
	int len = getLength(minlen, maxlen);                                   
	char str[len+1];
	for (int i=0; i<len; i++)                                            
	{                                                                    
		str[i] = nextU32(1, 127);                                        
	}                                                                    
	str[len] = 0;                                                        
	return str;                                                         
}      


OmnString
OmnRandom::noSpaceStr(int minlen, int maxlen)
{
	int strLen = getLength(minlen, maxlen);
	char buff[strLen+1];
	
	for (int k=0; k<strLen; k++)
	{
		 buff[k] = (char)nextU32(33, 126);
		 if (buff[k] == '"' || buff[k] == '\\' || buff[k] == '\t')
		 {
			 k--;
			 continue;
		 }
	}
	buff[strLen] = 0;
	return buff;
}              


OmnString
OmnRandom::binaryStr(int minlen, int maxlen)
{
	int len = getLength(minlen, maxlen);
	char str[len+1];
	for (int k=0; k<len; k++)
	{
		 str[k] = (char)nextU32(0, 255);
	}
	OmnString dd(str, len);
	return dd;
}


OmnString
OmnRandom::lowcaseStr(int minlen, int maxlen)
{
	int len = getLength(minlen, maxlen);
	char str[len+1];
	for (int k=0; k<len; k++)
	{
		str[k] = nextU32(0, 25) + 'a';
	}

	str[len] = 0;
	return str;
}


OmnString
OmnRandom::capitalStr(int minlen, int maxlen)
{
	int len = getLength(minlen, maxlen);
	char str[len+1];
	for (int k=0; k<len; k++)
	{
		str[k] = nextU32(0, 25) + 'A';
	}

	str[len] = 0;
	return str;
}


OmnString
OmnRandom::letterStr(int minlen, int maxlen)
{
	static char letters[53] = "abcdefghijklmnopqrstuvwxyz"
							  "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int len = getLength(minlen, maxlen);
	char str[len+1];
	for (int k=0; k<len; k++)
	{
		str[k] = letters[nextU32(0, 51)];
	}

	str[len] = 0;
	return str;
}


OmnString
OmnRandom::letterDigitStr(
		int minlen, 
		int maxlen, 
		const bool firstDigitAllowed)
{
	// A Letter Digit String consists of [a-z], [A-Z], [0, 9]. 
	// If 'firstDigitAllowed' is true, the first letter can be 
	// a digit. Otherwise, the first character must be a letter.
	static char letters[63] = "abcdefghijklmnopqrstuvwxyz"
							  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
							  "0123456789";
	int len = getLength(minlen, maxlen);
	char str[len+1];
	if (firstDigitAllowed)
	{
		str[0] = letters[nextU32(0, 61)];
	}
	else
	{
		str[0] = letters[nextU32(0, 51)];
	}

	for (int k=1; k<len; k++)
	{
		str[k] = letters[nextU32(0, 61)];
	}

	str[len] = 0;
	return str;
}

bool 
OmnRandom::nextLetterDigitStr(char *str, 
					const int minlen, 
					const int maxlen, 
					const bool firstDigitAllowed = false)
{
	// 
	// A Letter Digit String consists of [a-z], [A-Z], [0, 9]. 
	// If 'firstDigitAllowed' is true, the first letter can be 
	// a digit. Otherwise, the first character must be a letter.
	//
	static char letters[63] = "abcdefghijklmnopqrstuvwxyz"
							  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
							  "0123456789";
	int len = nextInt1(minlen, maxlen);
	if (firstDigitAllowed)
	{
		str[0] = letters[nextU32(0, 61)];
	}
	else
	{
		str[0] = letters[nextU32(0, 51)];
	}

	for (int k=1; k<len; k++)
	{
		str[k] = letters[nextU32(0, 61)];
	}

	str[len] = 0;
	return true;
}

OmnString
OmnRandom::varName(
	const int minlen, 
	const int maxlen, 
	const bool firstUnderscoreAllowed) 
{
	// A Variable Name consists of [a-z], [A-Z], [0, 9], '_'. 
	// If 'firstDigitAllowed' is true, the first letter can be 
	// a digit. Otherwise, the first character must be a letter.
	static char letters[64] = "abcdefghijklmnopqrstuvwxyz"
							  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
							  "0123456789_";
	int len = getLength(minlen, maxlen);
	char str[len+1];
	if (firstUnderscoreAllowed)
	{
		str[0] = letters[nextU32(0, 62)];
	}
	else
	{
		str[0] = letters[nextU32(0, 61)];
	}

	for (int k=1; k<len; k++)
	{
		str[k] = letters[nextU32(0, 62)];
	}

	str[len] = 0;
	return str;
}


OmnString 
OmnRandom::mixedString(int minLen, int maxLen)
{
	int len = getLength(minLen, maxLen);
    OmnString ret(len,' ',true);

    for(int i = 0;i < len;i++)
    {
    	if(i < len -1 && nextBool())
    	{
            *(ret.getBuffer() + i) = (unsigned char)(nextU32(176, 247));
    		i++;
            *(ret.getBuffer() + i) = (unsigned char)(nextU32(161, 254));
    		
    	}else
    	{
            *(ret.getBuffer() + i) = (unsigned char)(nextU32(33, 126));
        }
    }

    return ret;
}


bool 
OmnRandom::nextBool()
{
	return rand()%2;
}


int 
OmnRandom::intByRange(
		int min1, int max1, int weight1, 
		int min2, int max2, int weight2)
{
	// This function randomly generates an integer based on the weights
	int ww = weight1 + weight2;
	aos_assert_r(ww > 0, min1);
	int idx = rand() % ww;
	if (idx < weight1)
	{
		if (min1 >= max1) return min1;
		return (rand() % (max1-min1+1)) + min1;
	}

	if (min2 >= max2) return min2;
	return (rand() % (max2-min2+1)) + min2;
}


int 
OmnRandom::intByRange(
		int min1, int max1, int weight1, 
		int min2, int max2, int weight2,
		int min3, int max3, int weight3)
{
	// This function randomly generates an integer based on the weights
	int ww = weight1 + weight2 + weight3;
	aos_assert_r(ww > 0, min1);
	int idx = rand() % ww;
	if (idx < weight1)
	{
		if (min1 >= max1) return min1;
		return (rand() % (max1-min1)) + min1;
	}

	if (idx < weight1 + weight2)
	{
		if (min2 >= max2) return min2;
		return (rand() % (max2-min2)) + min2;
	}

	if (min3 >= max3) return min3;
	return (rand() % (max3-min3)) + min3;
}


int 
OmnRandom::intByRange(
		int min1, int max1, int weight1, 
		int min2, int max2, int weight2,
		int min3, int max3, int weight3,
		int min4, int max4, int weight4)
{
	// This function randomly generates an integer based on the weights
	int ww = weight1 + weight2 + weight3 + weight4;
	aos_assert_r(ww > 0, min1);
	int idx = rand() % ww;
	if (idx < weight1)
	{
		if (min1 >= max1) return min1;
		return (rand() % (max1-min1)) + min1;
	}

	if (idx < weight1 + weight2)
	{
		if (min2 >= max2) return min2;
		return (rand() % (max2-min2)) + min2;
	}

	if (idx < weight1 + weight2 + weight3)
	{
		if (min3 >= max3) return min3;
		return (rand() % (max3-min3)) + min3;
	}

	if (min4 >= max4) return min4;
	return (rand() % (max4-min4)) + min4;
}


int 
OmnRandom::intByRange(int *min, int *max, int *weights, const int len)
{
	// This function randomly generates an integer based on the weights
	int ww = 0;
	for (int i=0; i<len; i++) ww += weights[i];

	int idx = rand() % ww;
	ww = weights[0];
	for (int i=0; i<len-1; i++)
	{
		if (idx < ww)
		{
			if (min[i] == max[i]) return min[i];
			return (rand() % (max[i]-min[i])) + min[i];
		}

		ww += weights[i+1];
	}
	if (min[len-1] == max[len-1]) return min[len-1];
	return (rand() % (max[len-1]-min[len-1])) + min[len-1];
}


int 
OmnRandom::intByRange(
		int min1, int max1, int weight1, 
		int min2, int max2, int weight2,
		int min3, int max3, int weight3,
		int min4, int max4, int weight4,
		int min5, int max5, int weight5)
{
	// This function randomly generates an integer based on the weights
	int ww = weight1 + weight2 + weight3 + weight4 + weight5;
	aos_assert_r(ww > 0, min1);
	int idx = rand() % ww;
	if (idx < weight1)
	{
		if (min1 >= max1) return min1;
		return (rand() % (max1-min1)) + min1;
	}

	if (idx < weight1 + weight2)
	{
		if (min2 >= max2) return min2;
		return (rand() % (max2-min2)) + min2;
	}

	if (idx < weight1 + weight2 + weight3)
	{
		if (min3 >= max3) return min3;
		return (rand() % (max3-min3)) + min3;
	}

	if (idx < weight1 + weight2 + weight3 + weight4)
	{
		if (min4 >= max4) return min4;
		return (rand() % (max4-min4)) + min4;
	}

	if (min5 >= max5) return min5;
	return (rand() % (max5-min5)) + min5;
}


int 
OmnRandom::intByRange(
		int min1, int max1, int weight1, 
		int min2, int max2, int weight2,
		int min3, int max3, int weight3,
		int min4, int max4, int weight4,
		int min5, int max5, int weight5,
		int min6, int max6, int weight6)
{
	// This function randomly generates an integer based on the weights
	int ww = weight1 + weight2 + weight3 + weight4 + weight5 + weight6;
	aos_assert_r(ww > 0, min1);
	int idx = rand() % ww;
	if (idx < weight1)
	{
		if (min1 >= max1) return min1;
		return (rand() % (max1-min1)) + min1;
	}

	if (idx < weight1 + weight2)
	{
		if (min2 >= max2) return min2;
		return (rand() % (max2-min2)) + min2;
	}

	if (idx < weight1 + weight2 + weight3)
	{
		if (min3 >= max3) return min3;
		return (rand() % (max3-min3)) + min3;
	}

	if (idx < weight1 + weight2 + weight3 + weight4)
	{
		if (min4 >= max4) return min4;
		return (rand() % (max4-min4)) + min4;
	}

	if (idx < weight1 + weight2 + weight3 + weight4 + weight5)
	{
		if (min5 >= max5) return min5;
		return (rand() % (max5-min5)) + min5;
	}

	if (min6 >= max6) return min6;
	return (rand() % (max6-min6)) + min6;
}


OmnString
OmnRandom::getRandomStr(
		const OmnString &origStr,
		const int lenremained,
		const int lennew)
{
	OmnString newstr(lenremained+lennew,'\0',false);
	const char* origstr = origStr.data();
	char* newstrptr = newstr.getBuffer();
	strncpy(newstrptr,origstr,lenremained);
	char* curpos = newstrptr + lenremained;
	for(int i = 0;i < lennew;i++)
	{
//		char	OmnRandom::mAlphanum[65] = "abcedfghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";
//		int OmnRandom::mAlphanumLen = 64;
		*curpos = mAlphanum[nextU32(0,mAlphanumLen-1)];
		curpos ++;
	}
	*curpos = '\0';
	return newstr;
}


bool 
OmnRandom::percent(int p1)
{
	if (p1 <= 0) return 0;
	int r = rand() % 100;
	return (r < p1);
}


int 
OmnRandom::percent(int p1, int p2)
{
	if (p1 <= 0) p1 = 0;
	if (p2 <= 0) p2 = 0;
	int ww = p1 + p2;
	if (ww <= 0) return 0;
	int r = rand() % ww;
	if (r < p1) return 0;
	return 1;
}


int 
OmnRandom::percent(int p1, int p2, int p3)
{
	if (p1 <= 0) p1 = 0;
	if (p2 <= 0) p2 = 0;
	if (p3 <= 0) p3 = 0;
	int ww = p1 + p2 + p3;
	if (ww <= 0) return 0;
	int r = rand() % ww;
	if (r < p1) return 0;
	if (r < p1 + p2) return 1;
	return 2;
}


int 
OmnRandom::percent(int p1, int p2, int p3, int p4)
{
	if (p1 <= 0) p1 = 0;
	if (p2 <= 0) p2 = 0;
	if (p3 <= 0) p3 = 0;
	if (p4 <= 0) p4 = 0;
	int ww = p1 + p2 + p3 + p4;
	if (ww <= 0) return 0;
	int r = rand() % ww;
	if (r < p1) return 0;
	if (r < p1 + p2) return 1;
	if (r < p1 + p2 + p3) return 2;
	return 3;
}


int 
OmnRandom::percent(int p1, int p2, int p3, int p4, int p5)
{
	if (p1 <= 0) p1 = 0;
	if (p2 <= 0) p2 = 0;
	if (p3 <= 0) p3 = 0;
	if (p4 <= 0) p4 = 0;
	if (p5 <= 0) p5 = 0;
	int ww = p1 + p2 + p3 + p4 + p5;
	if (ww <= 0) return 0;
	int r = rand() % ww;
	if (r < p1) return 0;
	if (r < p1 + p2) return 1;
	if (r < p1 + p2 + p3) return 2;
	if (r < p1 + p2 + p3 + p4) return 3;
	return 4;
}


int 
OmnRandom::percent(int p1, int p2, int p3, int p4, int p5, int p6)
{
	if (p1 <= 0) p1 = 0;
	if (p2 <= 0) p2 = 0;
	if (p3 <= 0) p3 = 0;
	if (p4 <= 0) p4 = 0;
	if (p5 <= 0) p5 = 0;
	if (p6 <= 0) p6 = 0;
	int ww = p1 + p2 + p3 + p4 + p5 + p6;
	if (ww <= 0) return 0;
	int r = rand() % ww;
	if (r < p1) return 0;
	if (r < p1 + p2) return 1;
	if (r < p1 + p2 + p3) return 2;
	if (r < p1 + p2 + p3 + p4) return 3;
	if (r < p1 + p2 + p3 + p4 + p5) return 4;
	return 5;
}


// static u32 totally_incorrect_ip_pct = 10;
// static u32 add_trailing_dot_pct = 5;
// static u32 create_correct_ip_byte_pct = 30;


OmnString 
OmnRandom::strByType(const AosStrType type, int min, int max)
{
	switch (type)
	{
	case eAosStrType_AsciiStr:
		 return OmnRandom::asciiStr(min, max);

	case eAosStrType_Printable:
		 return OmnRandom::printableStr(min, max, true);

	case eAosStrType_NoSpaceStr:
		 return OmnRandom::noSpaceStr(min, max);

	case eAosStrType_BinStr:
		 return OmnRandom::binaryStr(min, max);

	case eAosStrType_LowcaseStr:
		 return OmnRandom::lowcaseStr(min, max);

	case eAosStrType_CapitalStr:
		 return OmnRandom::capitalStr(min, max);

	case eAosStrType_DigitStr:
		 return OmnRandom::digitStr(min, max);

	case eAosStrType_LetterStr:
		 return OmnRandom::letterStr(min, max);

	case eAosStrType_LetterDigitStr:
		 return OmnRandom::letterDigitStr(min, max);

	case eAosStrType_VarName:
		 return OmnRandom::varName(min, max, false);

	case eAosStrType_Symbol:
	case eAosStrType_WhiteSpace:
	case eAosStrType_WhiteSpaceWithNewLines:
	case eAosStrType_NonPrintable:
	case eAosStrType_ControlChar:
	     {
		 	//AosRandomStr gen("", min, max, type);
			//return gen.nextStr();
			return "";
		 }

	case eAosStrType_HHMMSS:
		 return aos_next_time_str(eAosTimeStrFormat_HHMMSS);

	case eAosStrType_InvalidHHMMSS:
		 return aos_next_invalid_time_str(min, max);

	default:
		 OmnAlarm << "Unrecognized string type: " << type << enderr;
		 return "";
	}

	OmnAlarm << "Program error" << enderr;
	return "";
}


int 
OmnRandom::ipAddr(uint32_t *addr, uint32_t netAddr, uint32_t mask)
{
	if (NULL == addr)
		return -1;
	
	netAddr  = netAddr & mask;
    uint32_t randip = htonl(nextU32(0, ntohl(~mask)));

	*addr = ((((netAddr >> 24) & 0xff) | ((randip >> 24) & 0xff)) << 24) |
		    ((((netAddr >> 16) & 0xff) | ((randip >> 16) & 0xff)) << 16) |
			((((netAddr >> 8) & 0xff) | ((randip >> 8) & 0xff)) << 8) |
			((netAddr & 0xff) | (randip & 0xff));

	return 0;
}


OmnString
OmnRandom::letterDigitDashUnderscore(int minlen, int maxlen)
{
	static char lsAllwords[] = 
		"0123456789-_"
		"abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int len = getLength(minlen, maxlen);
	char str[len+1];
	int i = 0;
	int c;
	for(i = 0; i < len; i ++)
	{
		c = nextU32(0, (sizeof(lsAllwords) - 2));
		str[i] = lsAllwords[c];
	}
	str[len] = 0;
	return str;
}


OmnString
OmnRandom::pickDomainName()
{
	int weights = nextU32(1, 100);
	int dots_num = 1;

	OmnString str;
	if (weights <= 20)
		dots_num = 1;
	else if (weights <= 80)
		dots_num = 2;
	else if (weights <= 90)
		dots_num = 3;
	else if (weights <= 95)
		dots_num = 4;
	else if (weights <= 98)
		dots_num = 5;
	else if (weights == 99)
		dots_num = 6;
	else if (weights == 100)
		dots_num = nextU32(7, 10);

	int i; 
	OmnString word;
	for (i = 0; i <= dots_num; i++)
	{
		weights = nextU32(0, 100);

		if (weights < 20) 
			word = letterDigitDashUnderscore(1, 3);
		else if (weights < 70)
			word = letterDigitDashUnderscore(4, 8);
		else if (weights < 90)
			word = letterDigitDashUnderscore(9, 20);
		else if (weights < 98)
			word = letterDigitDashUnderscore(21, 40);
		else if (weights < 100)
			word = letterDigitDashUnderscore(41, 60);
		
		if (dots_num == i) 
		{
			str << word;
		} 
		else 
		{
			str << word << ".";
		}
	}

	return str;
}


bool 
OmnRandom::percent(
		const AosRandomRulePtr &rule,
		const OmnString &name, 
		const int pct)
{
	if (!rule) return percent(50);
	return rule->percent(name, 50);
}


OmnString
OmnRandom::pickWordSeparator()
{
	return ",";
}


OmnString 
OmnRandom::pickEntrySeparator()
{
	return ",";
}

u64 
OmnRandom::nextU64()
{
	return ((u64)(rand()) << 32) | ((u64)rand());
}

OmnString 
OmnRandom::pickValueDataType()
{
	int start = AosDataType::getFirstEntry();	
	int end = AosDataType::getLastEntry();	
	int idx = nextInt1(start, end);
	return AosDataType::getTypeStr((AosDataType::E)idx);
}

AosOpr
OmnRandom::nextOpr()
{
	return (AosOpr)(rand()%(int(eAosOpr_max)));
}


OmnString 
OmnRandom::cellPhoneNumber(
		const int total_length, 
		const int phone_num_len, 
		const bool with_country_code,
		const bool left_align, 
		const char padding)
{
	char buff[total_length+1];

	aos_assert_r(phone_num_len <= total_length, "");

	if (!sgCellPhoneInited) initCellPhonePrefix();
	int idx = rand() % sgNumCellPhonePrefix;
	const char *data = sgCellPhonePrefix[idx].data();

	idx = 0;
	if (left_align)
	{
		if (with_country_code)
		{
			buff[0] = data[0];
			buff[1] = data[1];
			buff[2] = data[2];
			buff[3] = data[3];
			buff[4] = data[4];
			idx = 5;
		}
		else
		{
			buff[0] = data[2];
			buff[1] = data[3];
			buff[2] = data[4];
			idx = 3;
		}

		for (int i=idx; i<phone_num_len; i++)
		{
			buff[i] = rand() % 10 + '0';
		}

		for (int i=phone_num_len; i<total_length; i++) buff[i] = padding;
		buff[total_length] = 0;
		return buff;
	}

	// right alignment
	for (int i=0; i<total_length-phone_num_len; i++)
	{
		buff[i] = padding;
	}

	if (with_country_code)
	{
		buff[total_length-phone_num_len] 	 = data[0];
		buff[total_length-phone_num_len+1] = data[1];
		buff[total_length-phone_num_len+2] = data[2];
		buff[total_length-phone_num_len+3] = data[3];
		buff[total_length-phone_num_len+4] = data[4];
		idx = 5;
	}
	else
	{
		buff[total_length-phone_num_len] = data[2];
		buff[total_length-phone_num_len+1] = data[3];
		buff[total_length-phone_num_len+2] = data[4];
		idx = 3;
	}

	for (int i=phone_num_len-idx; i>0; i--)
	{
		buff[total_length - i] = rand() % 10 + '0';
	}
	buff[total_length] = 0;
	return buff;
}


OmnString 
OmnRandom::letterDigitStr(
		const u32 total_length, 
		const u32 min_length, 
		const bool left_align,
		const char padding)
{
	if (total_length == min_length) 
	{
		return nextLetterDigitStr(total_length, total_length, true);
	}

	aos_assert_r(total_length > min_length, "");

	int delta = total_length - min_length;
	int len = min_length + rand() % delta;

	int padlen = total_length - len;
	if (padlen == 0) 
	{
		return nextLetterDigitStr(total_length, total_length, true);
	}

	char buff[padlen+1];
	for (int i=0; i<padlen; i++) buff[i] = padding;
	buff[padlen] = 0;

	OmnString ss = nextLetterDigitStr(len, len, true);
	if (left_align)
	{
		ss << buff;
		return ss;
	}

	OmnString s(buff);
	s << ss;
	return s;
}


OmnString
OmnRandom::dateStr(const int min_year, const int max_year)
{
	// YYYYMMDDHHMISS
	aos_assert_r(max_year >= min_year, "");
	int years = max_year - min_year + 1;
	int year = rand() % years + min_year;
	OmnString ss;
	ss << year;

	int month = (rand() % 12) + 1;
	if (month < 10) ss << "0";
	ss << month;

	int day = (rand() % 31) + 1;
	switch (month)
	{
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		 break;

	case 2:
		 if (day > 28) day = 28;
		 break;

	case 4:
	case 6:
	case 9:
	case 11:
		 if (day == 31) day--;
		 break;
	}

	if (day < 10) ss << "0";
	ss << day;

	int hour = rand() % 24;
	if (hour < 10) ss << "0";
	ss << hour;

	int minute = rand() % 60;
	if (minute < 10) ss << "0";
	ss << minute;

	int second = rand() % 60;
	if (second < 10) ss << "0";
	ss << second;
	return ss;
}


OmnString
OmnRandom::generateValue(const OmnString &min_value, const OmnString &max_value)
{
	// This function generates a new string that is bigger than 'min_value'
	// and smaller than 'max_value'. If 'max_value' is empty, it is equivalent
	// to infinite.
	if (max_value != "")
	{
		aos_assert_r(strcmp(min_value.data(), max_value.data()) < 0, "");
	}
	OmnString ss = min_value;

	int nn = intByRange(
				1, 3, 100, 
				4, 20, 50,
				21, 100, 20,
				101, 1000, 5);
	OmnString prev_value;
	for (int i=0; i<nn; i++)
	{
		// Determine whether to increment the string or add a char. 
		if ((rand() % 50) == 0 && (ss.length() < max_value.length() || max_value.length() ==0))
		{
			int v = rand() % 10 + '0';
			ss << v;
		}
		else
		{
			ss.increment();
			if (max_value.length() > 0 && strcmp(ss.data(), max_value.data()) >= 0)
			{
				return prev_value;
			}

			if (prev_value.length() <= 0) prev_value = ss;
		}
	}

	return ss;
}


OmnString
OmnRandom::generateNextKey(
		u64 &crt_value, 
		const OmnString &prefix,
		const OmnString &seed,
		const OmnString &separator,
		const int prefix_len,
		const char leading_char,
		const u32 bitmask)
{
	// This function generates a new string that is composed of 
	// 		prefix + <new_value> + separator + seed
	// where <new_value> is 'crt_value' + AosGetHashKey(crt_value), filtered
	// with a bitmask. For instance, if bitmask is 0x3ff (10 bits), the new
	// value is the hash of 'crt_value' divided by 1000 + crt_value.
	// The <new_value> is converted into string. If 'prefix_len' > 0, 
	// <new_value> is converted into a fixed length string, front padded
	// with 'leading_char'. 
	crt_value += AosGetHashKey((char *)&crt_value, sizeof(u64)) & bitmask;
	if (prefix_len <= 0)
	{
		OmnString ss = prefix;
		ss << crt_value << separator << seed;
		return ss;
	}

	OmnString ss(prefix_len, leading_char, true);
	OmnString rr = prefix;
	rr << crt_value;
	aos_assert_r(rr.length() <= prefix_len, "");
	ss.setLength(prefix_len - rr.length());
	ss << crt_value << separator << seed;
	return ss;
}


// Chen Ding, 2013/02/15
int 
OmnRandom::letterDigitStr(
		char *str,
		int minlen, 
		int maxlen, 
		const bool firstDigitAllowed)
{
	// A Letter Digit String consists of [a-z], [A-Z], [0, 9]. 
	// If 'firstDigitAllowed' is true, the first letter can be 
	// a digit. Otherwise, the first character must be a letter.
	static char letters[63] = "abcdefghijklmnopqrstuvwxyz"
							  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
							  "0123456789";
	int len = getLength(minlen, maxlen);
	if (firstDigitAllowed)
	{
		str[0] = letters[nextU32(0, 61)];
	}
	else
	{
		str[0] = letters[nextU32(0, 51)];
	}

	for (int k=1; k<len; k++)
	{
		str[k] = letters[nextU32(0, 61)];
	}

	str[len] = 0;
	return true;
}


/*
bool
OmnRandom::generateUniqueNumbers(
		const int min, 
		const int max, 
		vector<int> &numbers)
{
	// This function generates max-min number of unique random numbers
	numbers.clear(); 
	int size = max - min + 1;
	aos_assert_r(size > 0, false);
	for (int i=min; i<=max; i++) 
	{
    	numbers.push_back(i); 
	}
	random_shuffle(numbers.begin(), numbers.end());
	return true;
}
*/


