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
// 2013/03/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CappletBasic/CappletSort.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"


AosCappletSort::AosCappletSort(const bool flag)
:
AosCapplet(AosCappletId::eSort, flag)
{
}


AosCappletSort::~AosCappletSort()
{
}


AosCappletPtr 
AosCappletSort::clone()
{
	return OmnNew AosCappletSort();
}


bool 
AosCappletSort::run(
		const AosRundataPtr &rdata, 
		AosDatalet &parms,
		const AosDataletPtr &datalet)
{
	// This capplet sorts data. The data is stored in 'datalet' in 
	// form of BuffArray. 
	//
	// This capplet assumes the following named parms:
	// 	eSortedField		// The data to be sorted
	// 	eRecordLen			// The record length (must be fixed length)
	// 	eCompareFunc		// The compare function for the sorter
	// 	eIsStable			// Whether the sorting is stable (default: false)
	//	eWithDocid			// Whether data have docids (default: false)
	//	eDataMandatory		// Whether data is mandatory
	//
	aos_assert_rr(datalet, rdata, false);
	AosDict::E sorted_field = parms.getDictId(AosDict::eSortedField);
	int record_len = parms.getInt(AosDict::eRecordLen, -1);
	AosCompareFuncPtr comp_func = parms.getCompareFunc(AosDict::eCompareFunc);
	bool is_stable = parms.getBool(AosDict::eIsStable, false);
	bool with_docid = parms.getBool(AosDict::eWithDocid, false);

	bool data_mandatory = parms.getInt(AosDict::eDataMandatory, true);
	aos_assert_rm(sorted_field, "missing_sorted_field_id", parms, rdata, false);
	aos_assert_rm(comp_func, "missing_compare_function", parms, rdata, false);
	aos_assert_rm(record_len >, "invalid_record_len", parms, rdata, false);

	AosBuffPtr buff = datalet.getBuff(sorted_field);
	if (!buff)
	{
		if (data_mandatory)
		{
			aos_assert_rm(false, "missing_sorted_data", parms, rdata, false);
			return false;
		}
		parms[AosDict::eProcMsg] = AosLangTerm("data_is_empty");
		return true;
	}

	AosBuffArray buff_array(buff, comp_func, stable);
	buff_array.setWithDocid(with_docid);
	buff_array.sort();

	return true;
}


bool
AosCappletSort::example(const AosRundataPtr &rdata)
{
	const u32 size = 10000;
	u64 *numbers = OmnNew u64[size];
	u64 *rand_numbers = OmnNew u64[size];
	for (u32 i=0; i<size; i++) numbers[i] = i;

	// Shuffle the numbers
	u32 ss = size;
	int idx = 0;
	while (ss)
	{
		u32 id = rand() % ss;
		rand_numbers[idx] = numbers[id];
		if (id != ss-1)
		{
			numbers[id] = numbers[ss-1];
		}
		ss--;
	}

	AosDataletPtr datalet = AosDataletBuff();
	*datalet[AosDict::eName] = "Sort Capplet Example";
	*datalet[AosDict::eSize] = size;
	datalet->append(AosDict::eSortedData, (void *)rand_numbers, sizeof(u64)*size);
	*datalet[AosDict::ePosion] = 1234567890;

	AosParms parms;
	parms[AosDict::eSortedField] = AosDict::eSortedData;
	parms[AosDict::eRecordLen] = record_len;
	parms[AosDict::eCompareFunction] = OmnNew AosCompareU64();
	parms[AosDict::eIsStable] = false;
	parms[AosDict::eWithDocid] = true;

	bool rslt = AosRunCapplet(rdata, parms, AosCappletId::eSortData, datalet);
	aos_assert_rpm(rslt, "failed_sorting", parms, rdata, false);
	AosBuffPtr buff = datalet->getBuff(AosDict::eSortedData);

	return true;
}

