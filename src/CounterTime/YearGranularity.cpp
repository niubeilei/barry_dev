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
#include "CounterTime/YearGranularity.h"
#include "CounterTime/TimeGranularity.h"
#include "CounterUtil/CounterName.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocFileMgr.h"
#include "XmlUtil/XmlTag.h"


AosYearGranularity::AosYearGranularity(
		const AosTimeGranularity::TimeFormat format,
		const bool regflag)
:
AosTimeGranularity(format, regflag)
{
}


bool
AosYearGranularity::procCounter(
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
		char *mem = OmnNew char[size+eYearRecordSize+10];
		if (size == 0)
		{
			// This is to create a new block. 
			memset(mem, 0, size+eYearRecordSize+10);
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

		int num_records = (size / eYearRecordSize);
		YearRecord *records = (YearRecord*)mem;

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
		if (insertFlag) size += eYearRecordSize;
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
AosYearGranularity::retrieveCounter(
		const AosCounterRecord &record,
	    const AosCounterTimeInfo &time,
	    const AosDocFileMgrPtr &file,
	    const AosRundataPtr &rdata)
{
	// It retireves a counter. The result are returned through "rdata".
	// The contents of the rdata should be in the form:
	// <contents value="xxxx" />
	aos_assert_r(rdata, false);
	aos_assert_r(file, false);
	u32 seqno = record.getCounterSeqno();
	u64 offset = record.getCounterOffset();
	u32 size = record.getCounterTimeSize();
	if (size == 0 || size >= eMaxYearRecordsSize)
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
		int recordsNum = size / eYearRecordSize;
		YearRecord *records = (YearRecord*)mem;
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
				contents << AOSTAG_SUM <<"=\"" << records[middle].value << "\" ";
				contents << AOSTAG_CNAME <<"=\"" << record.getCounterName() << "\"/> ";
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
AosYearGranularity::retrieveCounterSet(
        const u32 seqno,
        const u64 offset,
        const u32 size,
        AosXmlTagPtr &respRecord,
        const AosTimeCond &timeCond,
        const AosDocFileMgrPtr &file,
        const AosRundataPtr &rdata)
{
	// It retrieves the needed counters. Results are returned through 
	// "respRecord", which is in the form:
	// <record 
	// 	zkytm_2001='xxx'
	// 	zkytm_2002='xxx'
	// 	zkytm_2003='xxx'>
	// </record>
	aos_assert_r(rdata, false);
	aos_assert_r(file, false);
	aos_assert_r(respRecord, false);
	if (size == 0 || size >= eMaxYearRecordsSize)
	{
		rdata->setError() << "Size incorrect : " << size;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	try
	{
		char *mem = OmnNew char[size +100];
		bool readFlag = file->readDoc(seqno, offset, mem, size);
		if (!readFlag)
		{
			rdata->setError() << "Read doc error" ;
			OmnAlarm << rdata->getErrmsg() << enderr;
			OmnDelete [] mem;
			return false;
		}
		YearRecord *records = (YearRecord*)mem;
		int recordsNum = size / eYearRecordSize;
		aos_assert_r(recordsNum > 0, false);
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
		while(memCrtYear >= condStartYear && memCrtYear <= condEndYear)
		{
		    OmnString rsltAttrName = AOSTAG_COUNTER_RSLTPREFIX;
		    rsltAttrName << "_" << memCrtYear;
	        respRecord->setAttr(rsltAttrName, records[fromYearPos].value);
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
AosYearGranularity::retrieveCountersByQry(
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
	// 			<record zky_cname="xxxx" time="xxxx" value="" />
	// 			<record zky_cname="xxxx" time="xxxx" value="" />
	// 			....
	// 	</contents>
	aos_assert_r(rdata, false);
	if (size == 0 || size >= eMaxYearRecordsSize)
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
		YearRecord *records = (YearRecord*)mem;
		int recordsNum = size / eYearRecordSize;
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

//		u32 memCrtYear = records[fromYearPos].year;
		u32 condStartYear = timeCond.getStartYear();
		u32 condEndYear = timeCond.getEndYear();

		OmnString contents = "<contents total=\"";
		OmnString recordsStr;
		int sum = 0;
		OmnString tempCname, suffix;
		bool rslt = AosCounterName::decomposeName(cname, tempCname, suffix);
		aos_assert_rr(rslt, rdata, false);
		for (u32 i = condStartYear; i <= condEndYear ; i++)
		{
			sum++;
			OmnString recordStr = "<counter ";
		    recordStr << AOSTAG_CNAME << "=\"" 
		  			  << tempCname << "\" " << AOSTAG_ALLCNAME << "=\""
		  			  << cname << "\" " << AOSTAG_TIME << "=\""
				      << i << "\" "<< AOSTAG_SUM<< "=\"";
			int vv = (records[fromYearPos].year == i)? records[fromYearPos].value : 0;
			recordStr << vv << "\" />";
			recordsStr << recordStr;

			if (records[fromYearPos].year == i) fromYearPos++;
			//memCrtYear = records[fromYearPos].year;
		}
		/*
		while (memCrtYear >= condStartYear && memCrtYear <= condEndYear)
		{
			sum++;
			OmnString recordStr = "<counter ";
		    recordStr << AOSTAG_CNAME << "=\"" 
		  			  << tempCname << "\" " << AOSTAG_ALLCNAME << "=\""
		  			  << cname << "\" " << AOSTAG_TIME << "=\""
				      << memCrtYear << "\" "<< AOSTAG_SUM<< "=\""
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
	catch(...)
	{
		rdata->setError() << "Failed to allocate memory";
        OmnAlarm << rdata->getErrmsg() << enderr;

        return false;
	}
	return true;
}


int 
AosYearGranularity::findOrCreateRecord(
		YearRecord *records,
		const AosCounterTimeInfo &time, 
		const int64_t value,
		const int size,
		bool &insertFlag)
{
	u32 year = time.getYear();
	if (size == 0)
	{
		records[0].value = value;
		records[0].year = year;
		insertFlag = true;
		return 0;
	}

	if (year < records[0].year)
	{
        memmove(&records[1], &records[0], eYearRecordSize * size);
		memset(&records[0], 0, eYearRecordSize);

		records[0].value = value;
        records[0].year = year;
        insertFlag = true;
        return 0;
	}

	if (year > records[size-1].year)
	{
		memset(&records[size], 0, eYearRecordSize);
		records[size].value = value;
        records[size].year = year;
        insertFlag = true;
        return 0;
	}

	int start = 0, end = size - 1;
	int middle = 0; 
	while (start <= end)
	{
		middle = (start +end)/2;
		u32 target = records[middle].year;
		if (target == year)
		{
			records[middle].value += value;
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
	//char mem[eYearRecordSize+10];
	//AosBuff buff(mem, eYearRecordSize+10, 0);
	if (start < size)     // insert this record in thsi Block.
	{
		memmove(&records[start+1], &records[start], eYearRecordSize * (size - start));
		memset(&records[start], 0, eYearRecordSize);

		records[start].year = year;
		records[start].value = value;
		insertFlag = true;
		return start;
	}

	OmnShouldNeverComeHere;
	return -1;
}

#endif
