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
#include "CounterTime/YearMonthGranularity.h"
#include "CounterTime/TimeGranularity.h"
#include "CounterUtil/CounterName.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocFileMgr.h"
#include "XmlUtil/XmlTag.h"


AosYearMonthGranularity::AosYearMonthGranularity(
		const AosTimeGranularity::TimeFormat format,
		const bool regflag)
:
AosTimeGranularity(format, regflag)
{
}


bool
AosYearMonthGranularity::procCounter(
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
	// 		value1		(8 bytes)
	// 		value2		(8 bytes)
	// 		value3		(8 bytes)
	// 		value4		(8 bytes)
	// 		value5		(8 bytes)
	// 		value6		(8 bytes)
	// 		value7		(8 bytes)
	// 		value8		(8 bytes)
	// 		value9		(8 bytes)
	// 		value10		(8 bytes)
	// 		value11		(8 bytes)
	// 		value12		(8 bytes)
	// 		...
	// 1. Find the corresponding year record in this block. 
	// 2. If the year record is not there yet, create it. 
	// 3. Update the counter
	// 4. Save to file
	aos_assert_rr(file, rdata, false);
	try
	{
		char *mem = OmnNew char[size+eYearMonthRecordSize+10];
		if (size == 0)
		{
			// This is to create a new block. 
			memset(mem, 0, size+eYearMonthRecordSize+10);
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

		int num_records = (size / eYearMonthRecordSize);
		YearMonthRecord *records = (YearMonthRecord*)mem;

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
		if (insertFlag) size += eYearMonthRecordSize;
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
AosYearMonthGranularity::retrieveCounter(
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

	aos_assert_r(month >= 0 && month < 12, false);
	if (size == 0 || size >= eMaxYearMonthRecordsSize)
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
		int recordsNum = size / eYearMonthRecordSize;
		YearMonthRecord *records = (YearMonthRecord*)mem;
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
				contents << AOSTAG_SUM <<"=\"" << records[middle].month[month] << "\" ";
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
AosYearMonthGranularity::retrieveCounterSet(
        const u32 seqno,
        const u64 offset,
        const u32 size,
        AosXmlTagPtr &respRecord,
        const AosTimeCond &timeCond,
        const AosDocFileMgrPtr &file,
        const AosRundataPtr &rdata)
{
	// It retrieves the needed counters. Results are returned through 
	// 'respRecord', which is in the form:
	// 	<record 
	// 		month1="xxx"
	// 		month2="xxx"
	// 		...
	// 		monthn="xxx"/>
	// where attribute names are determined as:
	// 		zkytm_yyyymm for months, or 
	// 		zkytm_yyyy   for years
	//
	aos_assert_r(rdata, false);
	if (size == 0 || size >= eMaxYearMonthRecordsSize)
	{
		rdata->setError() << "Size incorrect: " << size;
		return false;
	}

	aos_assert_r(file, false);
	aos_assert_r(respRecord, false);
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
		YearMonthRecord *records = (YearMonthRecord*)mem;
		int recordsNum = size / eYearMonthRecordSize;
		if (recordsNum > 0)
		{
			if (records[0].year > timeCond.getEndYear())
			{
				OmnDelete [] mem;
				setEmptyContents(rdata);
				return true;
			}
			if (records[recordsNum - 1].year < timeCond.getStartYear())
			{
				OmnDelete [] mem;
				setEmptyContents(rdata);
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

			fromYearPos = start;

			u32 memCrtYear = records[fromYearPos].year;
			u32 condStartYear = timeCond.getStartYear();
			u32 condEndYear = timeCond.getEndYear();

			int condStartMonth = timeCond.getStartMonth();
			int condEndMonth = timeCond.getEndMonth();

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
				for(int i = fromMonth-1; i <= toMonth-1; i++)
				{
					rsltAttrName << i + 1 ;
					respRecord->setAttr(rsltAttrName, records[fromYearPos].month[i]);
				}

				fromYearPos++;
				memCrtYear = records[fromYearPos].year;
			}
			OmnDelete [] mem;
			return true;
		}
		OmnDelete [] mem;
		return false;
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
AosYearMonthGranularity::retrieveCountersByQry(
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
	if (size == 0 || size >= eMaxYearMonthRecordsSize)
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
		YearMonthRecord *records = (YearMonthRecord*)mem;
		int recordsNum = size / eYearMonthRecordSize;
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
AosYearMonthGranularity::findOrCreateRecord(
		YearMonthRecord *records,
		const AosCounterTimeInfo &time, 
		const int64_t value,
		const int num_records,
		bool &insertFlag)
{
	u32 year = time.getYear();
	int month = time.getMonth()-1;

	aos_assert_r(month >= 0 && month < 12, false);
	if (num_records == 0)
	{
		records[0].year = year;
		records[0].value = value;
		records[0].month[month] = value;
		insertFlag = true;
		return 0;
	}
	
	if (year < records[0].year)
	{
		memmove(&records[1], &records[0], eYearMonthRecordSize * num_records);
		memset(&records[0], 0, eYearMonthRecordSize);
		records[0].year = year;
		records[0].value = value;
		records[0].month[month] = value;
		insertFlag = true;
		return 0;

	}

	if (year > records[num_records-1].year)
	{
		memset(&records[num_records], 0, eYearMonthRecordSize);
		records[num_records].year = year;
		records[num_records].value = value;
		records[num_records].month[month] = value;
		insertFlag = true;
		return num_records;
	}

	int start = 0, end = num_records- 1;
	int middle = 0; 
	while (start <= end)
	{
		middle = (start +end)/2;
		u32 target = records[middle].year;
		if (target == year)
		{
			records[middle].value += value;
			records[middle].month[month] += value;
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
	//char mem[eYearMonthRecordSize+10];
	//AosBuff buff(mem, eYearMonthRecordSize+10, 0);
	if (start < num_records)     // insert this record in thsi Block.
	{
		memmove(&records[start+1], &records[start], eYearMonthRecordSize * (num_records - start));
		memset(&records[start], 0, eYearMonthRecordSize);

		records[start].year = year;
		records[start].value = value;
		records[start].month[month] = value;
		insertFlag = true;
		return start;
	}

	OmnShouldNeverComeHere;
	return -1;
}


bool
AosYearMonthGranularity::assembleYearResp(
                              const u32 &yearPos,
							  const OmnString &cname,
							  const YearMonthRecord *records,
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
	OmnString tempCname, suffix;
	bool rslt = AosCounterName::decomposeName(cname, tempCname, suffix);
    aos_assert_rr(rslt, rdata, false);
	while (memCrtYear >= condStartYear && memCrtYear <= condEndYear)
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
	contents << sum << "\">" << recordsStr << "</contents>";
	rdata->setContents(contents);
	rdata->setOk();
	return true;
}


bool
AosYearMonthGranularity::assembleYearMonthResp(
                              const u32 &yearPos,
							  const OmnString &cname,
							  const YearMonthRecord *records,
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
	OmnString tempCname, suffix;
	bool rslt = AosCounterName::decomposeName(cname, tempCname, suffix);
    aos_assert_rr(rslt, rdata, false);
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
	contents << sum << "\">" << recordsStr << "</contents>";
	rdata->setContents(contents);
	rdata->setOk();
	return true;
}
#endif
