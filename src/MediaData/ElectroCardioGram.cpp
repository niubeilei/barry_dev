////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This algorithm assumes the input is a sequence of segments. The segment
// size is defined by the smart doc. For each segment with size M, the 
// algorithm picks N data out of M. 
//
// 1. Calculate the Segment Average
// 2. Calculate the Segment average of the Segment Max and Segment Average, 
//    called Average Max;
// 3. Calculate the Segment average of the Segment Min and Segment Average, 
//    called Average Min;
// 4. For each group M/N, it pick one data based on:
// 		a. If there is a value that is larger than the average max, pick the
// 		   largest one.
// 		b. If there is a value that is smaller than the average min, pick the
// 		   smallest one.
// 		c. Otherwise, pick the last one in the group.
//
// Modification History:
// 12/24/2010	Created by Jackie
////////////////////////////////////////////////////////////////////////////
#include "MediaData/ElectroCardioGram.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "MultiLang/LangTermIds.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


const int sgDefaultMaxSegmentSize = 100000;

AosElectroCardioGram::AosElectroCardioGram(const bool flag)
:
AosDataPicker(AOSDATAPICKER_ELECTRO_CARDIIOGRAM, AosDataPickerId::eElectroCardioGram, flag)
{
}


bool 
AosElectroCardioGram::pickData(
		const OmnString &data,
		OmnString &rslts, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	// This function picks the data for electro cardiogram. The algorithm is
	// based on the one from Garea. 
	rslts = "";
	if (data == "") return true;

	WorkingData_t working_data;
	memset(&working_data, 0, sizeof(WorkingData_t));

	OmnString sep = sdoc->getAttrStr(AOSTAG_DATA_SEP, ",");
	if (sep == "")
	{
		AosSetError(rdata, AOSLT_MISSING_DATA_SEPARATOR);
		OmnAlarm << rdata->getErrmsg() << ": " << sdoc->toString() << enderr;
		return false;
	}

	OmnString rslt_sep = sdoc->getAttrStr(AOSTAG_RESULT_SEP, ",");
	if (rslt_sep == "") rslt_sep = sep;

	int max_segment_size = sdoc->getAttrInt(AOSTAG_MAX_SIZE, -1);
	if (max_segment_size <= 0 || max_segment_size > sgDefaultMaxSegmentSize)
	{
		max_segment_size = sgDefaultMaxSegmentSize;
	}

	int segment_size = sdoc->getAttrInt(AOSTAG_SEGMENT_SIZE, -1);
	if (segment_size <= 0)
	{
		AosSetError(rdata, AOSLT_INVALID_SEGMENT_SIZE);
		OmnAlarm << rdata->getErrmsg() << ": " << sdoc->toString() << enderr;
		return false;
	}

	if (segment_size > max_segment_size)
	{
		AosSetError(rdata, AOSLT_SEGMENT_SIZE_TOO_BIG);
		OmnAlarm << rdata->getErrmsg() << ": " << sdoc->toString() << enderr;
		return false;
	}

	int sampling_size = sdoc->getAttrInt(AOSTAG_SAMPLING_SIZE, -1);
	if (sampling_size <= 0)
	{
		AosSetError(rdata, AOSLT_INVALID_SAMPLING_SIZE);
		OmnAlarm << rdata->getErrmsg() << ": " << sdoc->toString() << enderr;
		return false;
	}

	if (sampling_size > segment_size)
	{
		// Sampling size can't be larger than the data ize
		AosSetError(rdata, AOSLT_INVALID_SAMPLING_SIZE);
		OmnAlarm << rdata->getErrmsg() << ": " << sdoc->toString() << enderr;
		return false;
	}

	bool is_integral = sdoc->getAttrBool(AOSTAG_IS_INTEGRAL, true);
	float fstep = segment_size * 1.0 / sampling_size;

	int max_segments = sdoc->getAttrInt(AOSTAG_MAX_SEGMENTS, -1);
	if (max_segments <= 0)
	{
		max_segments = sgDefaultMaxSegmentSize;
	}

	rslts = "";
	int guard = max_segments;
	OmnStrParser1 parser(data, sep);
	OmnString word;
	double values[segment_size];
	while (guard-- && parser.hasMore())
	{
		// Read the segment into an array
		char *ptr;
		int i=0;
		for (i=0; i<segment_size && parser.hasMore(); i++)
		{
			word = parser.nextWord();
			values[i] = strtod(word.data(), &ptr);
		}

		if (i < segment_size) break;

		initWorkingData(values, segment_size, &working_data);

		int start = 0;
		float accum_step = 0;
		for (i=0; i<sampling_size; i++)
		// for (i=0; i<10; i++)
		{
			double value = 0;

			accum_step += fstep;
			int step = (int)accum_step;
			accum_step -= step;

			int max_loc = working_data.max_thd;
			int max_pos = 0;
			int min_loc = working_data.min_thd;
			int min_pos = 0;

			if (start + step - 1 >= segment_size) 
			{
				value = values[segment_size-1];
			}
			else
			{
				bool value_found = false;
				// Find the min and max 
				for (int k = 0; k < step; k++)
				{
					if (start + k >= segment_size) break;
					
					if (values[start + k] > max_loc)		// calculate the max pos 
					{
						max_pos = k;
						max_loc = values[start + k];
	  				}

					if (values[start + k] < min_loc)	// calculate the min pos
					{
						min_pos = k;
						min_loc = values[start + k];
					}
				}

				if ((max_loc > working_data.max_thd) && (max_loc > working_data.max0))
				{
					working_data.max0 = max_loc;
			
					if (start + max_pos > segment_size - 1)
					{
						value = values[segment_size - 1];
					}
					else
					{
						value = values[start + max_pos];
					}
					value_found = true;
				}
				else
				{
					working_data.max0 = working_data.baseline;
				}

				if ((min_loc < working_data.min_thd) && (min_loc < working_data.min0))
				{
					working_data.min0 = min_loc;

					if (start + min_pos > segment_size - 1)
					{
		    			value = values[segment_size - 1];
					}
					else
					{
		    			value = values[start + min_pos];
					}
					value_found = true;
				}
				else
				{
					working_data.min0 = working_data.baseline;
				}

				if (!value_found)
				{
					if (step + start > segment_size - 1) 
					{
						value = values[segment_size - 1];
					}
					else
					{
						value = values[step + start - 1];
					}
				}
				start += step;

				if (rslts.length() != 0) rslts << rslt_sep;

				if (is_integral)
				{
					rslts << (int64_t)value;
				}
				else
				{
					rslts << value;
				}
			}
		}
OmnMark;
	}

OmnScreen << "Data results: " << rslts << endl;
	return true;
}


bool
AosElectroCardioGram::initWorkingData(
		const double *data, 
		const int segment_size,
		AosElectroCardioGram::WorkingData_t *working_data)
{
	aos_assert_r(data && working_data, false);

	double max = 0;
	double min = 32000;
	int i;

	// Calculate the average, max, min
	double average = 0;
	for (i = 0; i < segment_size; i++)
	{
		average += data[i]; 

if (data[i] == 676)
	OmnMark;
		if (data[i] > max) max = data[i];
		if (data[i] < min) min = data[i];
	}

	average = average / segment_size;
	working_data->baseline = average;
	working_data->max_thd  = (average + max) / 2;
	working_data->min_thd  = (average + min) / 2;
OmnScreen << "max +++++++: " << working_data->max_thd << endl;
OmnScreen << "min +++++++: " << working_data->min_thd << endl;
OmnScreen << "baseline +++++++: " << working_data->baseline << endl;
	return true;
}

