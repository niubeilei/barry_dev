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
#include "MediaData/ElectroCardioGram2.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "MultiLang/LangTermIds.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


const int sgDefaultMaxSegmentSize = 100000;

AosElectroCardioGram2::AosElectroCardioGram2(const bool flag)
:
AosDataPicker(AOSDATAPICKER_ELECTRO_CARDIIOGRAM2, AosDataPickerId::eElectroCardioGram2, flag)
{
}


bool 
AosElectroCardioGram2::pickData2(
		const OmnString &data,
		AosXmlTagPtr &rslts, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	// This function picks the data for electro cardiogram. The algorithm is
	// based on the one from shanghai. 
	if (data == "") return true;


	OmnString sep = sdoc->getAttrStr(AOSTAG_DATA_SEP, ",");
	if (sep == "")
	{
		AosSetError(rdata, AOSLT_MISSING_DATA_SEPARATOR);
		OmnAlarm << rdata->getErrmsg() << ": " << sdoc->toString() << enderr;
		return false;
	}

	OmnString rslt_sep = sdoc->getAttrStr(AOSTAG_RESULT_SEP, ",");
	if (rslt_sep == "") rslt_sep = sep;

	int workarrsize = sdoc->getAttrInt("workarrsize", 3);
	int b_num = sdoc->getAttrInt("b_num", 150);

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

	int guard = max_segments;
	OmnStrParser1 parser(data, sep);
	OmnString word;
	int value = 0;
	//A original
	//B calculus
	vector<int> A;
	vector<int> B;
	while (guard-- && parser.hasMore())
	{
		char *ptr;
		word = parser.nextWord();
		value = strtod(word.data(), &ptr);
		A.push_back(value);

	}
	int len = A.size();
	while(true)
	{
		int N = 0;
		B.clear();
		for(int i=0; i<len; i++)
		{
			if(i != len-1)
			{
				float tmp = (A[i] + A[i+1])/2.0 + 0.5;
				A[i] = tmp;
			}
		}
		for(int k=0; k<len; k++)
		{
			if(k == len -1)
			{
				B.push_back(A[k]);
				if(B[k] < 0)
				{
					N++;
				}
			}
			else
			{
				B.push_back(A[k+1] - A[k]);	
				if(B[k] < 0)
				{
					N++;
				}
			}
		}
cout << "N : ";
for(int f=0; f<1; f++)
{
cout << N << ",";
}
cout << endl;
cout << "len : " << len << endl;
		if(N < len/500*b_num)
		{
			break;
		}
	}
	

	vector<int> workarr;
	//C curve which is no QRS\T\P wave
	//D curve which is smooth wave
	vector<int> C;
	vector<int> D;
	for(int p=0; p<len; p++)
	{
		int index = p;
		for(int j=index-workarrsize; j<index+workarrsize+1; j++)
		{
			if(j<0 || j>len)
			{
				workarr.push_back(0);
			}
			else
			{
				workarr.push_back(A[j]);
			}
		}
		//sort
		qsort(&workarr[0], workarrsize*2+1, sizeof(int), CompDouble);
		C.push_back(workarr[workarrsize]);
		int L=A[p]-workarr[workarrsize];
		D.push_back(L);
	}

	int step;
	float accum_step = 0;
	int num = 0;
	vector<int> temarray;
	int tmp = 0;
	OmnString datastr;
	for(int m=0; m<len; m++)	
	{
		if(num == 0)
		{
			accum_step += fstep;
			step = (int)accum_step;
			accum_step -= step;
		}
		num++;
		value = A[m];
		temarray.push_back(value);
		if(step == num)
		{
			int sum = 0;
			for(int k=0; k<step; k++)
			{
				sum += temarray[k];
			}
			tmp = sum/step;
			num = 0;
			temarray.clear();
			if (datastr.length() != 0) datastr << rslt_sep;
			if (is_integral)
			{
				datastr << (int64_t)tmp;
			}
			else
			{
				datastr << tmp;
			}
		}
	}
OmnMark;
OmnScreen << "Data results: " << datastr << endl;
	OmnString nodestr;
	nodestr << "<SelData>" << datastr << "</SelData>";
	AosXmlParser parser2;
	rslts = parser2.parse(nodestr, "" AosMemoryCheckerArgs);
	return true;
}

int 
AosElectroCardioGram2::CompDouble(const void *p1, const void *p2)
{
	int num1 = *(int *)p1;
	int num2 = *(int *)p2;
	return num1>num2?1:-1;
}
