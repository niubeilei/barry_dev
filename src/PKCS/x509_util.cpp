////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: x509_util.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef __KERNEL__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#endif

#include "PKCS/asn1.h"
#include "PKCS/x509.h"

struct asn_tm {
	int     sec;		/* seconds */
	int     min;		/* minutes */
	int     hour;		/* hours */
	int     day;		/* day of the month */
	int     mon;		/* month */
	int     year;		/* year */
};


#define SECONDS_OF_HOUR		(60 * 60)
#define SECONDS_PER_DAY		(SECONDS_OF_HOUR * 24)
#define ISLEAP(year)		((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))

/* Compute the `struct tm' representation of *tm,
   Return nonzero if not successful.  */
static int
xxx_get_tm(const unsigned int t, long offset, struct asn_tm *tm) {
	long int days, rem, year;
	/* How many days come before each month (0-12).  */
	static const unsigned short int MON_DAY_OF_YEAR[2][13] =  {
		/* Normal years.  */
		{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
		/* Leap years.  */
		{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
	};
	const unsigned short int *ip;
	int i;


	days = t / SECONDS_PER_DAY;
	rem =  t % SECONDS_PER_DAY;

	// add timezone offset
	rem += offset;

	while (rem < 0) {
		rem += SECONDS_PER_DAY;
		--days;
	}
	while (rem >= SECONDS_PER_DAY) {
		rem -= SECONDS_PER_DAY;
		++days;
	}

	tm->hour = rem / SECONDS_OF_HOUR;
	rem %= SECONDS_OF_HOUR;
	tm->min = rem / 60;
	tm->sec = rem % 60;

	/* January 1, 1970 was a Thursday.  */
	year = 1970;

#define DIV(a, b)		((a) / (b) - ((a) % (b) < 0))
#define LEAPS_THRU_END_OF(year) (DIV (year, 4) - DIV (year, 100) + DIV (year, 400))

  	while (days < 0 || days >= (ISLEAP(year) ? 366 : 365)) {
		/* Guess a corrected year, assuming 365 days per year.  */
		long int year_of_guess = year + days / 365 - (days % 365 < 0);

		/* Adjust DAYS and Y to match the guessed year.  */
		days -= ((year_of_guess - year) * 365
			+ LEAPS_THRU_END_OF(year_of_guess - 1)
			- LEAPS_THRU_END_OF(year - 1));
		year = year_of_guess;
    	}

	tm->year = year;

	ip = MON_DAY_OF_YEAR[ISLEAP(year)];
	for (i = 11; days < (long int)ip[i]; i--)
		continue;
	days -= ip[i];
	tm->mon = i + 1;
	tm->day = days + 1;

	return 0;
}

int
time_to_UTC_time(UINT32 t, BYTE *buf) {
	struct asn_tm tm;
	int ret;

	ret = xxx_get_tm(t, 0, &tm);
	if (ret == 0 && buf)
		ret = sprintf((char *)buf, "%02d%02d%02d%02d%02d%02dZ",
			tm.year % 100, tm.mon, tm.day, tm.hour, tm.min, tm.sec);

	return ret;
}

int
time_to_generalized_time(UINT32 t, BYTE *buf) {
	struct asn_tm tm;
	int ret;

	ret = xxx_get_tm(t, 0, &tm);
	if (ret == 0 && buf)
		ret = sprintf((char *)buf, "%04d%02d%02d%02d%02d%02dZ",
			tm.year, tm.mon, tm.day, tm.hour, tm.min, tm.sec);

	return ret;
}



/* borrowed from linux time.h, we want it to use in userland */
static time_t
xxx_mktime(struct asn_tm *tm) {
	//printf("%d-%d-%d-%d-%d:%d\n",tm->year,tm->mon,tm->day,tm->hour,tm->min,tm->sec);
	if (0 >= (int) (tm->mon -= 2)) {	/* 1..12 -> 11,12,1..10 */
		tm->mon += 12;		/* Puts Feb last since it has leap day */
		tm->year -= 1;
	}

	return (((
		(UINT32) (tm->year/4 - tm->year/100 + tm->year/400 + 367*tm->mon/12 + tm->day) +
			tm->year*365 - 719499
	    	 )*24 + tm->hour 	/* now have hours   */
	        )*60 + tm->min 		/* now have minutes */
	       )*60 + tm->sec; 		/* finally seconds  */
}


/* Read a UTCTime and GeneralizedTime value */
static unsigned int
get_digits(const BYTE *buf_ptr) {
	return (((buf_ptr[0] - '0') * 10) + (buf_ptr[1] - '0'));
}

time_t asntime_to_time(BYTE * buf_ptr, const int len, const BOOLEAN is_UTC_time) {
	struct asn_tm  asntime;
	int value, length = len;

	memset(&asntime, 0, sizeof(struct asn_tm));
	if(!is_UTC_time) {
		value = (get_digits(buf_ptr) - 19) * 100;	/* Read the century */
		buf_ptr += 2;
		length -= 2;
	} else
		value = 0;
	asntime.year = get_digits(buf_ptr) + value + 1900;
	asntime.mon  = get_digits(buf_ptr + 2);
	asntime.day  = get_digits(buf_ptr + 4);
	asntime.hour = get_digits(buf_ptr + 6);
	asntime.min  = get_digits(buf_ptr + 8);
	if(length > 11)
		asntime.sec = get_digits(buf_ptr + 10);


	/* 		RFC3280
		Where YY is greater than or equal to 50, the year SHALL be
		interpreted as 19YY; and

		Where YY is less than 50, the year SHALL be interpreted as 20YY.
	 */

	if(is_UTC_time && asntime.year < 1950)
		asntime.year += 100;

	return xxx_mktime(&asntime);
}
