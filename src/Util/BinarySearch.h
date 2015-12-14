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
// 04/12/2012: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Util_BinarySearch_h
#define Aos_Util_BinarySearch_h

#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "Util/String.h"
#include "Util/Orders.h"
#include <iterator>
#include <vector>
#include <algorithm>
using namespace std;


inline bool AosOmnStrLessAlpha(const OmnString &left, const OmnString &right)
{
	if (left < right) return true;
	return false;
}

inline bool AosOmnStrLessNumAlpha(const OmnString &left, const OmnString &right)
{
	int len1 = left.length();
	int len2 = right.length();
	if (len1 < len2) return true;
	if (len1 > len2) return false;
	if (left < right) return true;
	return false;
}


inline bool AosBinaryInsert(
		vector<OmnString> &values, 
		const OmnString &value, 
		const AosOrder::E order)
{
	vector<OmnString>::iterator pos;
	switch (order)
	{
	case AosOrder::eAlphabetic:
		 pos = lower_bound(values.begin(), values.end(), value, AosOmnStrLessAlpha);
		 break;

	case AosOrder::eNumAlphabetic:
		 pos = lower_bound(values.begin(), values.end(), value, AosOmnStrLessNumAlpha);
		 break;

	default:
		 OmnAlarm << "unrecognized order: " << order << enderr;
		 return false;
	}

	if (pos != values.end()) 
	{
		cout << "To add: " << value.data() << ":" << (*pos).data() << endl;
	}
	else
	{
		cout << "To append: " << value.data() << endl;
	}
	values.insert(pos, value);
	return true;
}


inline u32 AosBinaryFindPos(
		vector<OmnString> &values, 
		const OmnString &value, 
		const AosOrder::E order)
{
	vector<OmnString>::iterator pos = lower_bound(
			values.begin(), values.end(), value, AosOmnStrLessAlpha);
	if (pos != values.end()) return pos - values.begin();
	return values.size();
}

u32 AosBinaryFindPos(
		OmnString *values, 
		u64 *docids,
		const int num_entries,
		const OmnString &value, 
		const u64 &docid,
		const AosOrder::E order);

// If found, 'found' is set to true and 'index' (returned) holds:
// 		values[index] opr value && docid <= docids[index]
// This is used for deleting and modifying (since it needs to find the
// entry to delete or modify). 
//
// For adding an entry, the function will return 'index' that holds:
// 		values[index] == value && docid <= docids[index], or
// 		value < values[index] 
int AosFindFirstIndexForAdding(
		const char **values, 
		const u64 *docids,
		const u32 num_values, 
		const OmnString &value, 
		const u64 &docid,
		const bool isNumAlpha);

int AosFindFirstIndexForDeleting(
		const char **values, 
		const u64 *docids,
		const u32 num_values, 
		const OmnString &value, 
		const u64 &docid,
		const bool isNumAlpha);

// This function finds the first entry:
// 		values opr value
// If found, it returns the first index (either normal or reverse). If not
// found, it returns -5.
int AosFindFirstIndexForQuery(
		const char **values, 
		const u32 num_values, 
		const int sidx,
		const AosOpr opr, 
		const OmnString &value, 
		const bool isNumAlpha, 
		const bool reverse);

#endif
