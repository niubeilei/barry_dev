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
#include "CounterTime/YearMonthDayHourGranularity.h"
#include "CounterTime/TimeGranularity.h"
#include "CounterTime/TimeUtil.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocFileMgr.h"
#include "XmlUtil/XmlTag.h"


AosYearMonthDayHourGranularity::AosYearMonthDayHourGranularity(
		const AosTimeGranularity::TimeFormat format,
		const bool regflag)
:
AosTimeGranularity(format, regflag)
{
}


bool
AosYearMonthDayHourGranularity::procCounter(
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
		char *mem = OmnNew char[size+eYearMonthDayHourRecordSize+10];
		if (size == 0)
		{
			// This is to create a new block. 
			memset(mem, 0, size+eYearMonthDayHourRecordSize+10);
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

		int num_records = (size / eYearMonthDayHourRecordSize);
		YearMonthDayHourRecord *records = (YearMonthDayHourRecord*)mem;

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
		if (insertFlag) size += eYearMonthDayHourRecordSize;

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
AosYearMonthDayHourGranularity::retrieveCounter(
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
	int hour = time.getSpendHour() - 1;

    aos_assert_r(month >= 0 && month < 12, false);
    aos_assert_r(day >= 0 && day < 366, false);
    aos_assert_r(hour >= 0 && day < 366 * 24, false);


	if (size == 0 || size >= eMaxYearMonthDayHourRecordsSize)
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
		int recordsNum = size / eYearMonthDayHourRecordSize;
		YearMonthDayHourRecord *records = (YearMonthDayHourRecord*)mem;
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
                contents << AOSTAG_SUM <<"=\"" << records[middle].hour[hour] << "\" ";
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
AosYearMonthDayHourGranularity::retrieveCounterSet(
        const u32 seqno,
        const u64 offset,
        const u32 size,
        AosXmlTagPtr &respRecord,
        const AosTimeCond &timeCond,
        const AosDocFileMgrPtr &file,
        const AosRundataPtr &rdata)
{
	// It retrieves the needed counters. Results are returned through
	// "respRecord" , which is in the form:
	// <record
	// 		zkytm_2001="xxx"
	// 		zkytm_2002="xxx"
	// 		zkytm_2003="xxx" />
	
	aos_assert_r(rdata, false);
	aos_assert_r(file, false);
	aos_assert_r(respRecord, false);

	if (size == 0 || size >= eMaxYearMonthDayHourRecordsSize)
    {
        rdata->setError() << "Size incorrect: " << size;
        return false;
    }
	try
	{
		char *mem = OmnNew char[size + 100];
		bool readFlag = file->readDoc(seqno, offset, mem, size);
		if(!readFlag)
        {
            rdata->setError() << "Read doc error" ;
            OmnAlarm << rdata->getErrmsg() << enderr;
            OmnDelete [] mem;
            return false;
        }
        YearMonthDayHourRecord *records = (YearMonthDayHourRecord*)mem;
        int recordsNum = size / eYearMonthDayHourRecordSize;
		aos_assert_r(recordsNum > 0, false);
		if (records[0].year > timeCond.getEndYear())
        {
			OmnDelete [] mem;
            return true;
        }
        if (records[recordsNum - 1].year < timeCond.getStartYear())
        {
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
		u32 memCrtYear = records[fromYearPos].year;
        u32 condStartYear = timeCond.getStartYear();
        u32 condEndYear = timeCond.getEndYear();
		
        int condStartMonth = timeCond.getStartMonth();
        int condEndMonth = timeCond.getEndMonth();
		int condStartDay = timeCond.getStartDay();
		int condEndDay = timeCond.getEndDay();

        while(memCrtYear >= condStartYear && memCrtYear <= condEndYear)
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

            aos_assert_r(fromMonth >= 1 && toMonth <= 12, false);
            OmnString rsltAttrName = AOSTAG_COUNTER_RSLTPREFIX;
            rsltAttrName << "_" << memCrtYear << "_" ;

            for(int i = fromMonth-1; i < toMonth-1; i++)
            {
                rsltAttrName << i + 1 ;
				int fromDay = 1, toDay = 366;
				if ((memCrtYear == condStartYear) && (i+1 == condStartMonth))
				{
					fromDay = condStartDay;
				}
				if ((memCrtYear == condEndYear) && (i+1 == condEndMonth))
				{
					toDay = condEndDay;
				}
				aos_assert_r(fromDay >=1 && toDay <= 366, false);
				for(int j = fromDay - 1; j < toDay -1; j++)
				{
					AosCounterTimeInfo timeInfo(memCrtYear, i + 1, j + 1);
					rsltAttrName << "_" << timeInfo.getDay();
                	respRecord->setAttr(rsltAttrName, records[fromYearPos].day[j]);
				}
            }

            fromYearPos++;
            memCrtYear = records[fromYearPos].year;
        }
		OmnDelete [] mem;

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
AosYearMonthDayHourGranularity::retrieveCountersByQry(
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
	if (size == 0 || size >= eMaxYearMonthDayHourRecordsSize)
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
		YearMonthDayHourRecord *records = (YearMonthDayHourRecord*)mem;
		int recordsNum = size / eYearMonthDayHourRecordSize;
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

        case eYearMonthDayHour:
            return assembleYearMonthDayHourResp(fromYearPos, cname, records, timeCond, rdata);
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
AosYearMonthDayHourGranularity::findOrCreateRecord(
		YearMonthDayHourRecord *records,
		const AosCounterTimeInfo &time, 
		const int64_t value,
		const int size,
		bool &insertFlag)
{
	u32 year = time.getYear();
	int month = time.getMonth() - 1;
	int day = time.getSpendDay() - 1;
	int hour = time.getSpendHour() - 1;

	aos_assert_r(month >= 0 && month < 12, false);
	aos_assert_r(day >= 0 && day < 366, false);
	aos_assert_r(hour >= 0 && hour < 366 * 24, false);
	if (size == 0)
	{
		records[0].year = year;
		records[0].value = value;
		records[0].month[month] = value;
		records[0].day[day] = value;
		records[0].hour[hour] = value;
		insertFlag = true;
		return 0;
	}
	if (year < records[0].year)
    {
        memmove(&records[1], &records[0], eYearMonthDayHourRecordSize * size);
		memset(&records[0], 0, eYearMonthDayHourRecordSize);

        records[0].year = year;
        records[0].value = value;
        records[0].month[month] = value;
		records[0].day[day] = value;
		records[0].hour[hour] = value;
        insertFlag = true;
        return 0;
    }

	if (year > records[size-1].year)
    {
		memset(&records[size], 0, eYearMonthDayHourRecordSize);
        records[size].year = year;
        records[size].value = value;
        records[size].month[month] = value;
		records[size].day[day] = value;
		records[size].hour[hour] = value;
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
			records[middle].hour[hour] += value;
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
	//char mem[eYearMonthDayHourRecordSize+10];
	//AosBuff buff(mem, eYearMonthDayHourRecordSize+10, 0);
	if (start < size)     // insert this record in thsi Block.
	{
		memmove(&records[start+1], &records[start], eYearMonthDayHourRecordSize * (size - start));
		memset(&records[start], 0, eYearMonthDayHourRecordSize);

		records[start].year = year;
		records[start].value = value;
		records[start].month[month] = value;
		records[start].day[day] = value;
		records[start].hour[hour] = value;
		insertFlag = true;
		return start;
	}

	OmnShouldNeverComeHere;
	return -1;
}


bool
AosYearMonthDayHourGranularity::assembleYearResp(
                          		const u32 &yearPos,
                            	const OmnString &cname,
                            	const YearMonthDayHourRecord *records,
                            	const AosTimeCond &timeCond,
                            	const AosRundataPtr &rdata)
{
	u32 fromYearPos = yearPos;
	u32 memCrtYear = records[fromYearPos].year;
	u32 condStartYear = timeCond.getStartYear();
	u32 condEndYear = timeCond.getEndYear();

	OmnString contents = "<contents total=\"";
	OmnString recordsStr; 
	int sum = 0;
	while (memCrtYear >= condStartYear && memCrtYear <= condEndYear)
	{
		sum++;
		OmnString recordStr = "<counter ";
		recordStr << AOSTAG_CNAME << "=\"" 
		  		  << cname << "\" " << AOSTAG_TIME << "=\""
				  << memCrtYear << "\" " << AOSTAG_SUM << "=\""
				  << records[fromYearPos].value << "\" />";
				recordsStr << recordStr;
		fromYearPos++;
		memCrtYear = records[fromYearPos].year;
	}
	contents << sum << "\">" << recordsStr << "</contents>";
	rdata->setContents(contents);
	rdata->setOk();
	return true;
}


bool
AosYearMonthDayHourGranularity::assembleYearMonthResp(
                           	 	const u32 &yearPos,
                            	const OmnString &cname,
                            	const YearMonthDayHourRecord *records,
                            	const AosTimeCond &timeCond,
                            	const AosRundataPtr &rdata)
{		
	u32 fromYearPos = yearPos;
	u32 memCrtYear = records[fromYearPos].year;
	u32 condStartYear = timeCond.getStartYear();
	u32 condEndYear = timeCond.getEndYear();

	int condStartMonth = timeCond.getStartMonth();
	int condEndMonth = timeCond.getEndMonth();

	OmnString contents = "<contents total=\"";
	OmnString recordsStr; 
	int sum = 0;
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
		for (int i = fromMonth-1; i < toMonth-1; i++)
		{
			sum++;
			OmnString recordStr = "<counter ";
			recordStr << AOSTAG_CNAME << "=\"" 
		  			  << cname << "\" " << AOSTAG_TIME << "=\""
				      << memCrtYear << "-" << i + 1 << "\" " << AOSTAG_SUM << "=\""
					  << records[fromYearPos].month[i] << "\" />";
			recordsStr << recordStr;
		}

		fromYearPos++;
		memCrtYear = records[fromYearPos].year;
	}
	contents << sum << "\">" << recordsStr << "</contents>";
	rdata->setContents(contents);
	rdata->setOk();
	return true;
}


bool
AosYearMonthDayHourGranularity::assembleYearMonthDayResp(
                            const u32 &yearPos,
                            const OmnString &cname,
                            const YearMonthDayHourRecord *records,
                            const AosTimeCond &timeCond,
                            const AosRundataPtr &rdata)
{	
	u32 fromYearPos = yearPos;
	u32 memCrtYear = records[fromYearPos].year;
	u32 condStartYear = timeCond.getStartYear();
	u32 condEndYear = timeCond.getEndYear();

	int condStartMonth = timeCond.getStartMonth();
	int condEndMonth = timeCond.getEndMonth();
	int condStartDay = timeCond.getStartDay();
	int condEndDay = timeCond.getEndDay();

	OmnString contents = "<contents total=\"";
	OmnString recordsStr; 
	int sum = 0;
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
			aos_assert_r(fromDay >=1 && toDay <= days, false);
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
		  			  << cname << "\" " << AOSTAG_TIME << "=\""
				      << memCrtYear << "-" << i + 1 << "-" << timeInfo.getDay() << "\" " << AOSTAG_SUM << "=\""
					  << records[fromYearPos].day[j] << "\" />";
				recordsStr << recordStr;
			}
		}

		fromYearPos++;
		memCrtYear = records[fromYearPos].year;
	}
	contents << sum << "\">" << recordsStr << "</contents>";
	rdata->setContents(contents);
	rdata->setOk();
	return true;
}


bool
AosYearMonthDayHourGranularity::assembleYearMonthDayHourResp(
                            const u32 &yearPos,
                            const OmnString &cname,
                            const YearMonthDayHourRecord *records,
                            const AosTimeCond &timeCond,
                            const AosRundataPtr &rdata)
{	
	u32 fromYearPos = yearPos;
	u32 memCrtYear = records[fromYearPos].year;
	u32 condStartYear = timeCond.getStartYear();
	u32 condEndYear = timeCond.getEndYear();

	int condStartMonth = timeCond.getStartMonth();
	int condEndMonth = timeCond.getEndMonth();
	int condStartDay = timeCond.getStartDay();
	int condEndDay = timeCond.getEndDay();
	int condStartHour = timeCond.getStartHour();
	int condEndHour = timeCond.getEndHour();

	OmnString contents = "<contents total=\"";
	OmnString recordsStr = "<records>";
	int sum = 0;
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
				int fromHour = 0, toHour = 23;
				
				if ((memCrtYear == condStartYear) && (i+1 == condStartMonth))
				{
					fromHour = condStartHour;
				}
				if ((memCrtYear == condEndYear) && (i+1 == condEndMonth))
				{
					toHour = condEndHour;
				}
				aos_assert_r(fromHour >= 0 && toHour <= 23, false);
				OmnString startHour, endHour;
				AosCounterTimeInfo allDay((int)memCrtYear, i + 1, j + 1);
				aos_assert_r(allDay.getDay() >= 1 && allDay.getDay() <= days, false);
				startHour << memCrtYear << "-" << i + 1 << "-" << allDay.getDay() << " "<< fromHour << ":00:00.000";
				endHour << memCrtYear << "-" << i + 1 << "-" << allDay.getDay() << " "<< toHour << ":00:00.000";
            	AosCounterTimeInfo start(startHour);
            	AosCounterTimeInfo end(endHour);

				for(int m = start.getSpendHour() - 1; m <= end.getSpendHour() - 1; m++)
				{
					sum++;
					AosCounterTimeInfo timeInfo((int)memCrtYear, i + 1, j + 1, m + 1);
					OmnString recordStr = "<counter ";
					recordStr << AOSTAG_CNAME << "=\"" 
		  			  		  << cname << "\" " << AOSTAG_TIME << "=\""
				      		  << memCrtYear << "-" << i + 1 << "-" << timeInfo.getDay() << ":" << timeInfo.getHour() << "\" " << AOSTAG_SUM << "=\""
					  		  << records[fromYearPos].hour[m] << "\" />";
					recordsStr << recordStr;
				}
			}
		}

		fromYearPos++;
		memCrtYear = records[fromYearPos].year;
	}
	recordsStr << "</records>";
	contents << sum << "\">" << recordsStr << "</contents>";
	rdata->setContents(contents);
	rdata->setOk();
	return true;
}
#endif
