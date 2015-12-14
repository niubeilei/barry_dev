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
//
// Modification History:
// 06/07/2011	Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#if 0
#include "CounterTime/YearMonthDayGranularity.h"
#include "CounterTime/TimeGranularity.h"
#include "CounterTime/TimeUtil.h"
#include "CounterUtil/CounterName.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocFileMgr.h"
#include "XmlUtil/XmlTag.h"


AosYearMonthDayGranularity::AosYearMonthDayGranularity(
		const AosTimeGranularity::TimeFormat format,
		const bool regflag)
:
AosTimeGranularity(format, regflag)
{
}


bool
AosYearMonthDayGranularity::procCounter(
		u32 &seqno, 
		u64 &offset,
		u32 &size,
		u64 &nameIILID,
		u64 &valueIILID,
		const AosCounterTimeInfo &time,
		const OmnString &iilKey,
		const u64 &prevCounterId,
		const int64_t &value, 
		const AosDocFileMgrPtr &file, 
		const AosRundataPtr &rdata)
{
	// This function updates the counter identified by [seqno, offset]. 
	// In the current implementations, the data for [seqno, offset]
	// is:
	// 		size		(4 bytes)
	// 		year		(4 bytes)
	// 		value		(8 bytes)
	// 		year		(4 bytes)
	// 		value		(8 bytes)
	// 		year		(4 bytes)
	// 		value		(8 bytes)
	// 		...
	// 1. Find the corresponding year record in this block. 
	// 2. If the year record is not there yet, create it. 
	// 3. Update the counter
	// 4. Save to file
	aos_assert_rr(file, rdata, false);
	try
	{
		char *mem = OmnNew char[size+eYearMonthDayRecordSize+10];
		if (size == 0)
		{
			// This is to create a new block. 
			memset(mem, 0, size+eYearMonthDayRecordSize+10);
		}
		else
		{
			bool rslt = file->readDoc(seqno, offset, mem, size);
			if (!rslt)
			{
				rdata->setError() << "Read doc error";
				OmnAlarm << rdata->getErrmsg() << enderr;
				OmnDelete [] mem;
				return false;
			}
		}

		int num_records = (size / eYearMonthDayRecordSize);
		YearMonthDayRecord *records = (YearMonthDayRecord*)mem;

		// Find the record
		bool insertFlag = false;
		int idx = findOrCreateRecord(records, time, value, num_records, insertFlag);
		if (idx < 0)
		{
			rdata->setError() << "Internal error";
			OmnAlarm << rdata->getErrmsg() << enderr;
			OmnDelete [] mem;
			return false;
		}

		// Save the changes
		if (insertFlag) size += eYearMonthDayRecordSize;

		bool rslt = file->saveDoc(seqno, offset, size, mem);
		if (!rslt)
		{
			rdata->setError() << "Failed to save file"; 
			OmnAlarm << rdata->getErrmsg() << enderr;
			OmnDelete [] mem;
			return false;
		}

		OmnDelete [] mem;
		return true;
	}

	catch (...)
	{
		rdata->setError() << "Failed to allocate memory";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosYearMonthDayGranularity::retrieveCounter(
		const AosCounterRecord &record,
	    const AosCounterTimeInfo &time,
	    const AosDocFileMgrPtr &file,
	    const AosRundataPtr &rdata)
{
	// It retrieves a counter. The result are returned through "rdata".
	// The contents of the rdata should be in the form:
	// <contents value="xxxx" />
	aos_assert_r(rdata, false);
	aos_assert_r(file, false);
	u32 seqno = record.getCounterSeqno();
	u64 offset = record.getCounterOffset();
	u32 size = record.getCounterTimeSize();
	int month = time.getMonth() - 1;
    int day = time.getSpendDay() - 1;

    aos_assert_r(month >= 0 && month < 12, false);
    aos_assert_r(day >= 0 && day < 366, false);

	if (size == 0 || size >= eMaxYearMonthDayRecordsSize)
	{
		rdata->setError() << "Size Incorrect : " << size ;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	try
	{
		char *mem = OmnNew char[size + 100];

		bool readFlag = file->readDoc(seqno, offset, mem, size);
		if (!readFlag)
		{
			rdata->setError() << "Read doc error";
			OmnAlarm << rdata->getErrmsg() << enderr;
			OmnDelete [] mem;
			return false;
		}
		int recordsNum = size / eYearMonthDayRecordSize;
		YearMonthDayRecord *records = (YearMonthDayRecord*)mem;
		aos_assert_r(recordsNum > 0, false);
		if (records[0].year > time.getYear())
		{
			OmnDelete [] mem;
			setEmptyContents(rdata);
			return true;
		}
		if (records[recordsNum - 1].year < time.getYear())
		{
			OmnDelete [] mem;
			setEmptyContents(rdata);
			return true;
		}
		int start = 0, end = recordsNum - 1;
		int middle = 0;
		while(start <= end)
		{
			middle = (start + end)/2;
			u32 target = records[middle].year;
			if (target == time.getYear())
			{
				OmnString contents = "<contents total='1'>";
                contents << "<counter ";
                contents << AOSTAG_SUM <<"=\"" << records[middle].day[day] << "\" ";
                contents << AOSTAG_CNAME <<"=\"" << record.getCounterName() << "\" />";
                contents << "</contents>";
                 rdata->setContents(contents);
				break;
			}
			if (target > time.getYear())
			{
				end = middle - 1;
			}
			else
			{
				start = middle + 1;
			}
		}
		OmnDelete [] mem;
		return true;
	}
	catch(...)
	{
		rdata->setError() << "Failed to allocate memory";
        OmnAlarm << rdata->getErrmsg() << enderr;
        return false;
	}
	return true;
}


bool
AosYearMonthDayGranularity::retrieveCounterSet(
		const u64 &iilid,
		const OmnString &cname,
		OmnString &contents,
        const AosTimeCond &timeCond,
        const AosRundataPtr &rdata)
{
	// It retrieves the needed counters. Results are returned through
	// "respRecord" , which is in the form:
	// 	<records>
	// 		<record year="xxx" month="xxx" day="xxx" value="xxx"/>
	// 		<record year="xxx" month="xxx" day="xxx" value="xxx"/>
	// 		...
	// 	</records>
	//
	// Since this is 'year, month, day' query, it needs to calculate
	// the starting day and the ending day. The days for the same 
	// year is stored in an IIL:
	// 		[day, value]
	// 		[day, value]
	// 		...
	// Note that 'day' is calculated as an integer in the following format:
	// 		yyyymmdd
	// For instance, 1970.1.1 is converted into 19700101, 2012.03.22 is converted
	// to '20120322'. 
	
	aos_assert_r(rdata, false);
	aos_assert_r(respRecord, false);

	if (iilid == 0)
	{
		OmnAlarm << "IILID is empty" << enderr;
		setEmptyContents(rdata);
		return true;
	}

	u64 startday = timeCond.getStartUniDay();
	u64 endday = timeCond.getEndUniDay();
	if (endday > startday)
	{
		setEmptyContents(rdata);
        return true;
    }

	AosQueryRsltPtr query_rslt = AosGetQueryRslt(rdata);
	aos_assert_rr(query_rslt, rdata, false);
	bool rslt = AosDbQuery(query_rslt, 0, eAosOpr_ge, startday,
			eAosOpr_le, endday, rdata);
	if (!rslt)
	{
		OmnAlarm << rdata->getErrmsg() << enderr;
		setEmptyContents(rdata);
		return true;
	}

	query_rslt->reset();
	bool finished = false;
	u64 value, time;
	while (query_rslt->nextDocid(value, time, finished, rdata) && !finished)
	{
		contents << "<record " << AOSTAG_YEAR << "=\"" << AosUniTime::getYear(time)
			<< "\" " << AOSTAG_MONTH << "=\"" << AosUniTime::getMonth(time)
			<< "\" " << AOSTAG_DAY << "=\"" << AosUniTime::getDay(time)
			<< "\" " << AOSTAG_VALUE << "=\"" << value
			<< "/>";
	}
    return true;
}


bool
AosYearMonthDayGranularity::retrieveCountersByQry(
            const u32 seqno,
            const u64 offset,
            const u32 size,
            const OmnString &cname,
            const AosTimeCond &timeCond,
            const AosDocFileMgrPtr &file,
            const AosRundataPtr &rdata)
{
	// It retrieves the needed counters. Results are returned through 
	// 'rdata', which is in the form:
	// 	<contents total="xxx">
	// 			<record cname="xxxx" time="xxxx" value="" />
	// 			<record cname="xxxx" time="xxxx" value="" />
	// 			....
	// 	</contents>
	aos_assert_r(rdata, false);
	if (size == 0 || size >= eMaxYearMonthDayRecordsSize)
	{
		rdata->setError() << "Size incorrect: " << size;
		return false;
	}

	aos_assert_rr(file, rdata, false);
	try
	{
		char *mem = OmnNew char[size + 100];
		bool readFlag = file->readDoc(seqno, offset, mem, size);
		if (!readFlag)
		{	
			rdata->setError() << "Read doc error" ;
			OmnAlarm << rdata->getErrmsg() << enderr;
			OmnDelete [] mem;
			return false;
		}
		YearMonthDayRecord *records = (YearMonthDayRecord*)mem;
		int recordsNum = size / eYearMonthDayRecordSize;
		if (recordsNum <= 0)
		{
			setEmptyContents(rdata);
			OmnDelete [] mem;
			return true;
		}
		if (records[0].year > timeCond.getEndYear())
		{
			setEmptyContents(rdata);
			OmnDelete [] mem;
			return true;
		}
		if (records[recordsNum - 1].year < timeCond.getStartYear())
		{
			setEmptyContents(rdata);
			OmnDelete [] mem;
			return true;
		}
		int start = 0, end = recordsNum -1;
		int middle = 0;
		int fromYearPos;

		while(start <= end)
		{
			middle = (start + end)/2;
			u32 target = records[middle].year;
			if (target == timeCond.getStartYear())
			{
				fromYearPos = middle;
				break;
			}

			if (target > timeCond.getStartYear())
			{
				end = middle - 1;
			}
			else
			{
				start = middle + 1;
			}
		}

		if (start > end) fromYearPos = start;
		TimeFormat type = AosTimeGranularity::toEnum(timeCond.getType());
        switch(type)
        {
        case eYear:
            return assembleYearResp(fromYearPos, cname, records, timeCond, rdata);
            break;

        case eYearMonth:
            return assembleYearMonthResp(fromYearPos, cname, records, timeCond, rdata);
            break;

        case eYearMonthDay:
            return assembleYearMonthDayResp(fromYearPos, cname, records, timeCond, rdata);
            break;

        default:
            OmnAlarm << "Inrecognized Time type" << enderr;
            break;
        }
		return true;
	}
	catch(...)
	{
		rdata->setError() << "Failed to allocate memory";
        OmnAlarm << rdata->getErrmsg() << enderr;

        return false;
	}
	return true;
}


int 
AosYearMonthDayGranularity::findOrCreateRecord(
		YearMonthDayRecord *records,
		const AosCounterTimeInfo &time, 
		const int64_t value,
		const int size,
		bool &insertFlag)
{
	u32 year = time.getYear();
	int month = time.getMonth() - 1;
	int day = time.getSpendDay() - 1;

	aos_assert_r(month >= 0 && month < 12, false);
	aos_assert_r(day >= 0 && day < 366, false);
	if (size == 0)
	{
		records[0].year = year;
		records[0].value = value;
		records[0].month[month] = value;
		records[0].day[day] = value;
		insertFlag = true;
		return 0;
	}
	if (year < records[0].year)
    {
        memmove(&records[1], &records[0], eYearMonthDayRecordSize * size);
		memset(&records[0], 0, eYearMonthDayRecordSize);

        records[0].year = year;
        records[0].value = value;
        records[0].month[month] = value;
		records[0].day[day] = value;
        insertFlag = true;
        return 0;
    }

	if (year > records[size-1].year)
    {
		memset(&records[size], 0, eYearMonthDayRecordSize);
        records[size].year = year;
        records[size].value = value;
        records[size].month[month] = value;
		records[size].day[day] = value;
        insertFlag = true;
        return size;
    }

	int start = 0, end = size - 1;
	int middle = 0; 
	while (start <= end)
	{
		middle = start +((end - start)/2);
		u32 target = records[middle].year;
		if (target == year)
		{
			records[middle].value += value;
			records[middle].month[month] += value;
			records[middle].day[day] += value;
			return middle;
		}
		else if (target > year)
		{
			end = middle - 1;
		}
		else
		{
			start = middle + 1;
		}
	}
	// Not Found, and create this Record.
	//char mem[eYearMonthDayRecordSize+10];
	//AosBuff buff(mem, eYearMonthDayRecordSize+10, 0);
	if (start < size)     // insert this record in thsi Block.
	{
		memmove(&records[start+1], &records[start], eYearMonthDayRecordSize * (size - start));
		memset(&records[start], 0, eYearMonthDayRecordSize);

		records[start].year = year;
		records[start].value = value;
		records[start].month[month] = value;
		records[start].day[day] = value;
		insertFlag = true;
		return start;
	}

	OmnShouldNeverComeHere;
	return -1;
}


bool
AosYearMonthDayGranularity::assembleYearResp(
                          		const u32 &yearPos,
                            	const OmnString &cname,
                            	const YearMonthDayRecord *records,
                            	const AosTimeCond &timeCond,
                            	const AosRundataPtr &rdata)
{
	u32 fromYearPos = yearPos;
	//u32 memCrtYear = records[fromYearPos].year;
	u32 condStartYear = timeCond.getStartYear();
	u32 condEndYear = timeCond.getEndYear();

	OmnString contents = "<contents total=\"";
	OmnString recordsStr;
	int sum = 0;
	OmnString tempCname, suffix;
    bool rslt = AosCounterName::decomposeName(cname, tempCname, suffix);
    aos_assert_rr(rslt, rdata, false);
	for (u32 i = condStartYear; i<= condEndYear; i++)
	{
		sum++;
		OmnString recordStr = "<counter ";
		recordStr << AOSTAG_CNAME << "=\"" 
		  		  << tempCname << "\" " << AOSTAG_ALLCNAME << "=\""
		  		  << cname << "\" " << AOSTAG_TIME << "=\""
				  << i << "\" " << AOSTAG_SUM << "=\"";
		int vv = (records[fromYearPos].year == i)? records[fromYearPos].value : 0;
		recordStr << vv << "\" />";
		recordsStr << recordStr;
		if (records[fromYearPos].year == i) fromYearPos++;
		//memCrtYear = records[fromYearPos].year;
	}
	//Linda, 2011/12/22
	/*while (memCrtYear >= condStartYear && memCrtYear <= condEndYear)
	{
		sum++;
		OmnString recordStr = "<counter ";
		recordStr << AOSTAG_CNAME << "=\"" 
		  		  << tempCname << "\" " << AOSTAG_ALLCNAME << "=\""
		  		  << cname << "\" " << AOSTAG_TIME << "=\""
				  << memCrtYear << "\" " << AOSTAG_SUM << "=\""
				  << records[fromYearPos].value << "\" />";
				recordsStr << recordStr;
		fromYearPos++;
		memCrtYear = records[fromYearPos].year;
	}
	*/
	contents << sum << "\">" << recordsStr << "</contents>";
	rdata->setContents(contents);
	rdata->setOk();
	return true;
}


bool
AosYearMonthDayGranularity::assembleYearMonthResp(
                           	 	const u32 &yearPos,
                            	const OmnString &cname,
                            	const YearMonthDayRecord *records,
                            	const AosTimeCond &timeCond,
                            	const AosRundataPtr &rdata)
{		
	u32 fromYearPos = yearPos;
	//u32 memCrtYear = records[fromYearPos].year;
	u32 condStartYear = timeCond.getStartYear();
	u32 condEndYear = timeCond.getEndYear();

	int condStartMonth = timeCond.getStartMonth();
	int condEndMonth = timeCond.getEndMonth();

	OmnString contents = "<contents total=\"";
	OmnString recordsStr;
	int sum = 0;
	OmnString tempCname, suffix;
	bool rslt = AosCounterName::decomposeName(cname, tempCname, suffix);
	aos_assert_rr(rslt, rdata, false);

	for (u32 j = condStartYear; j <= condEndYear; j++)
	{
		int fromMonth = 1, toMonth = 12;
		if (j == condStartYear)
		{
			fromMonth = condStartMonth;
		}
		if (j == condEndYear)
		{
			toMonth = condEndMonth;
		}

		aos_assert_rr(fromMonth >= 1 && toMonth <= 12, rdata, false);
		for (int i = fromMonth-1; i <= toMonth-1; i++)
		{
			sum++;
			OmnString recordStr = "<counter ";
			recordStr << AOSTAG_CNAME << "=\"" 
		  			  << tempCname << "\" " << AOSTAG_ALLCNAME << "=\""
		  			  << cname << "\" " << AOSTAG_TIME << "=\""
				      << j << "-" << i + 1 << "\" " << AOSTAG_SUM << "=\"";
			int vv = (records[fromYearPos].year == j)?records[fromYearPos].month[i] : 0;
			recordStr << vv << "\" />";
			recordsStr << recordStr;
		}
		if (records[fromYearPos].year == j) fromYearPos++;
	}
	//Linda, 2011/12/22
	/*
	while (memCrtYear >= condStartYear && memCrtYear <= condEndYear)
	{
		int fromMonth = 1, toMonth = 12;
		if (memCrtYear == condStartYear)
		{
			fromMonth = condStartMonth;
		}
		if (memCrtYear == condEndYear)
		{
			toMonth = condEndMonth;
		}

		aos_assert_rr(fromMonth >= 1 && toMonth <= 12, rdata, false);
		for (int i = fromMonth-1; i <= toMonth-1; i++)
		{
			sum++;
			OmnString recordStr = "<counter ";
			recordStr << AOSTAG_CNAME << "=\"" 
		  			  << tempCname << "\" " << AOSTAG_ALLCNAME << "=\""
		  			  << cname << "\" " << AOSTAG_TIME << "=\""
				      << memCrtYear << "-" << i + 1 << "\" " << AOSTAG_SUM << "=\""
					  << records[fromYearPos].month[i] << "\" />";
			recordsStr << recordStr;
		}

		fromYearPos++;
		memCrtYear = records[fromYearPos].year;
	}
	*/
	contents << sum << "\">" << recordsStr << "</contents>";
	rdata->setContents(contents);
	rdata->setOk();
	return true;
}


bool
AosYearMonthDayGranularity::assembleYearMonthDayResp(
                            const u32 &yearPos,
                            const OmnString &cname,
                            const YearMonthDayRecord *records,
                            const AosTimeCond &timeCond,
                            const AosRundataPtr &rdata)
{	
	u32 fromYearPos = yearPos;
	//u32 memCrtYear = records[fromYearPos].year;
	u32 condStartYear = timeCond.getStartYear();
	u32 condEndYear = timeCond.getEndYear();

	int condStartMonth = timeCond.getStartMonth();
	int condEndMonth = timeCond.getEndMonth();
	int condStartDay = timeCond.getStartDay();
	int condEndDay = timeCond.getEndDay();

	OmnString contents = "<contents total=\"";
	OmnString recordsStr;
	int sum = 0;
	OmnString tempCname, suffix;
	bool rslt = AosCounterName::decomposeName(cname, tempCname, suffix);
	aos_assert_rr(rslt, rdata, false);
	for (u32 y = condStartYear; y<= condEndYear; y++)
	{
		int fromMonth = 1, toMonth = 12;
		if (y == condStartYear)
		{
			fromMonth = condStartMonth;
		}
		if (y == condEndYear)
		{
			toMonth = condEndMonth;
		}

		aos_assert_rr(fromMonth >= 1 && toMonth <= 12, rdata, false);
		for (int i = fromMonth-1; i <= toMonth-1; i++)
		{
			int fromDay = 1;
			int days = AosTimeUtil::getDaysByYearAndMonth(y, i+1);
			int toDay = days;

			if ((y == condStartYear) && (i+1 == condStartMonth))
			{
				fromDay = condStartDay;
			}
			if ((y == condEndYear) && (i+1 == condEndMonth))
			{
				toDay = condEndDay;
			}

			aos_assert_rr(fromDay >= 1 && toDay <= days, rdata, false);
			OmnString startDay, endDay;
			startDay << y << "-" << i + 1 << "-" << fromDay << " 00:00:00.000";
			endDay << y << "-" << i + 1 << "-" << toDay << " 00:00:00.000";
			AosCounterTimeInfo start(startDay);
			AosCounterTimeInfo end(endDay);

			for(int j = start.getSpendDay() - 1; j <= end.getSpendDay() - 1; j++)
			{
				sum++;
				AosCounterTimeInfo timeInfo((int)y, i + 1, j + 1);
				OmnString recordStr = "<counter ";
				recordStr << AOSTAG_CNAME << "=\"" 
		  			  << tempCname << "\" " << AOSTAG_ALLCNAME << "=\""
		  			  << cname << "\" " << AOSTAG_TIME << "=\""
				      << y << "-" << i + 1 << "-" << timeInfo.getDay() << "\" " << AOSTAG_SUM << "=\"";
				//	  << records[fromYearPos].day[j] << "\" />";
				int vv = (records[fromYearPos].year == y)? records[fromYearPos].day[j] : 0;
				recordStr << vv << "\" />";
				recordsStr << recordStr;
			}
		}

		if (records[fromYearPos].year == y) fromYearPos++;
	}
	//Linda, 2011/12/22
	/*
	while (memCrtYear >= condStartYear && memCrtYear <= condEndYear)
	{
		int fromMonth = 1, toMonth = 12;
		if (memCrtYear == condStartYear)
		{
			fromMonth = condStartMonth;
		}
		if (memCrtYear == condEndYear)
		{
			toMonth = condEndMonth;
		}

		aos_assert_rr(fromMonth >= 1 && toMonth <= 12, rdata, false);
		for (int i = fromMonth-1; i <= toMonth-1; i++)
		{
			int fromDay = 1;
			int days = AosTimeUtil::getDaysByYearAndMonth(memCrtYear, i+1);
			int toDay = days;

			if ((memCrtYear == condStartYear) && (i+1 == condStartMonth))
			{
				fromDay = condStartDay;
			}
			if ((memCrtYear == condEndYear) && (i+1 == condEndMonth))
			{
				toDay = condEndDay;
			}

			aos_assert_rr(fromDay >= 1 && toDay <= days, rdata, false);
			OmnString startDay, endDay;
			startDay << memCrtYear << "-" << i + 1 << "-" << fromDay << " 00:00:00.000";
			endDay << memCrtYear << "-" << i + 1 << "-" << toDay << " 00:00:00.000";
			AosCounterTimeInfo start(startDay);
			AosCounterTimeInfo end(endDay);

			for(int j = start.getSpendDay() - 1; j <= end.getSpendDay() - 1; j++)
			{
				sum++;
				AosCounterTimeInfo timeInfo((int)memCrtYear, i + 1, j + 1);
				OmnString recordStr = "<counter ";
				recordStr << AOSTAG_CNAME << "=\"" 
		  			  << tempCname << "\" " << AOSTAG_ALLCNAME << "=\""
		  			  << cname << "\" " << AOSTAG_TIME << "=\""
				      << memCrtYear << "-" << i + 1 << "-" << timeInfo.getDay() << "\" " << AOSTAG_SUM << "=\""
					  << records[fromYearPos].day[j] << "\" />";
				recordsStr << recordStr;
			}
		}

		fromYearPos++;
		memCrtYear = records[fromYearPos].year;
	}
	*/
	contents << sum << "\">" << recordsStr << "</contents>";
	rdata->setContents(contents);
	rdata->setOk();
	return true;
}

#endif
