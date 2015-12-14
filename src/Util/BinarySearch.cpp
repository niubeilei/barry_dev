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
#if 0
#include "Util/BinarySearch.h"

int AosFindFirstIndexForAdding(
		const char **values, 
		const u64 *docids,
		const u32 num_values, 
		const OmnString &value, 
		const u64 &docid,
		const bool isNumAlpha)
{
	// This function finds the first index:
	// 		value <= values[index] && docid <= docids[index]
	// If it is appending, it returns 'num_values'. Unless errors are encounted, 
	// this function always returns a non-negative value.
	
	// 1. If it is empty, it returns 0
	if (num_values == 0) return 0;
	aos_assert_r(values, -5);
	aos_assert_r(docids, -5);

	u32 start_idx = 0;

	// Check the first entry
	int rslt = AosIILUtil::valueMatch(value, values[start_idx], isNumAlpha);
	if (rslt < 0)
	{
		// It is to insert in front of the first entry
		return 0;
	}

	if (rslt == 0)
	{
		// The first entry meets the condition.
		goto find_docid;
	}

	// values[0] < value. The first entry does not meet the condition. 
	// Need to search Check the last entry
	if (num_entries == 1)
	{
		// There is only one entry. Need to append.
		return num_entries;
	}

	// Check the last entry
	rslt = AosIILUtil::valueMatch(value, values[num_entries-1], isNumAlpha);
	if (rslt > 0)
	{
		// This means the last entry does not meet the condition. Need to append.
		return num_entries;
	}

	// The last entry holds: 
	// 		value <= values[num_entries-1]
	// Will do the binary search.
	int left = start_idx; 
	int right = num_values-1;
	int found = num_values-1;
	while (left < right)
	{
		int nn = (right + left) >> 1;
		aos_assert_r(values[nn], -5);
		int rslt = AosIILUtil::valueMatch(value, values[nn], isNumAlpha);

		if (rslt < 0)
		{
			// value <= values[nn], 
			if (left == right || left + 1 == right) return nn;
			left = nn + 1;
		}
		else if (rslt > 0)
		{
			// value > values
			if (left == right || left + 1 == right) return found;
			right = nn-1;
		}
		else
		{
			// value == values[index]
			if (left == right || left + 1 == right) 
			{
				found = left;
				goto find_docid;
			}
			right = nn;
		}	
	}

	OmnShouldNeverComeHere;
	return -5;

find_docid:

int AosFindFirstIndexForDeleting(
		const char **values, 
		const u64 *docids,
		const u32 num_values, 
		const OmnString &value, 
		const u64 &docid,
		const bool isNumAlpha);

int AosFindFirstIndexForQuery(
		const char **values, 
		const u32 num_values, 
		const int sidx,
		const AosOpr opr, 
		const OmnString &value, 
		const bool isNumAlpha, 
		const bool reverse)
{
	// This function finds the first entry:
	// 		values opr value
	// If found, it returns the first index (either normal or reverse). If not
	// found, it returns -5.
}

int AosFindFirstIndex(
		const char **values, 
		const u64 *docids,
		const u32 num_values, 
		const int sidx,
		const AosOpr opr, 
		const OmnString &value, 
		const u64 &docid,
		const bool isNumAlpha)
{
	// This function assumes 'values[...]' is sorted. When entries in 'values[...]'
	// are the same, 'docids[...]' are sorted. 
	// This function retrieves the index of the first entry that meets the condition:
	// 		values[index] opr value and
	// 		docids[index] <= docid
	//
	// For instance, if 'opr' is '<', it means:
	// 		values[index] < value && docids[index] <= docid
	//
	// If 'sdix' >= 0, it is the index from which it starts the search. Otherwise, 
	// it searches from the beginning.
	//
	// If found, the index is returned. Otherwise, -5 is returned.
	
	if (num_values == 0) return -5;
	aos_assert_r(values, -5);
	aos_assert_r(docids, -5);

	u32 start_idx = (start_idx < 0)?0:(u32)sidx;
	if (start_idx > num_values) return -5;

	// Check the first entry
	int rslt = AosIILUtil::valueMatch(values[start_idx], value, isNumAlpha);
	int found = -5;
	switch (opr)
	{
	case eAosOpr_gt:
		 if (rslt < 0) return start_idx;
		 return -5;
	 
	case eAosOpr_ge:
		 if (rslt <= 0) return start_idx;
		 return -5;

	case eAosOpr_eq:
		 if (rslt == 0) goto sort_docids;
		 // if (start_idx == num_values-1) return -5;
		 found = -5;
		 break;

	case eAosOpr_lt:
		 // Need to find the first value < values[index], or values[index] > value
		 // If found, it needs to find the first docid <= docids[index]
		 if (rslt > 0) 
		 {
			 // The first entry meets the condition. Need to find the docid
			 goto sort_docids;
		 }

		 // The first entry does not meet the condition. Check whether it has only
		 // one entry.
		 if (start_idx == num_values-1) 
		 {
			 // There is only one entry. Not found
			 return -5;
		 }

		 // Check the last entry.
		 rslt = AosIILUtil::valueMatch(values[num_values-1], value, isNumAlpha);
		 if (rslt <= 0)
		 {
			 // i.e., values[num_values-1] <= value, last entry does not meet 
			 return -5;
		 }

		 // This means that values[start_idx] <= value (not meet the condition) 
		 // and values[num_values-1] > value (hold the condition). 
		 found = num_values-1;
		 goto sort_docids;

	case eAosOpr_le:
		 rslt = AosIILUtil::valueMatch(values[num_values-1], value, isNumAlpha);
		 if (rslt > 0) return -5;
		 if (start_idx == num_values-1) return -5;
		 found = num_values-1;
		 break;

	case eAosOpr_ne:
		 if (rslt != 0) return start_idx;
		 if (start_idx == num_values-1) return -5;
		 found = -5;
		 break;

	default:
		 OmnAlarm << "Invalid opr: " << opr << enderr;
		 return -5;
	}

	// When it comes to this point:
	// 	1. opr is (eAosOpr_lt || eAosOpr_le || eAosOpr_ne || eAosOpr_eq)
	// 	2. start_idx < num_values - 1
	// Need to search [start_idx, num_values-1] for the first one that meets the condition.

	int left = start_idx; 
	int right = num_values-1;
	while (left < right)
	{
		int nn = (right + left) >> 1;
		aos_assert_r(values[nn], -5);
		int rslt = AosIILUtil::valueMatch(value, values[nn], isNumAlpha);

		switch (opr)
		{
		case eAosOpr_lt:
			 if (rslt <= 0)
			 {
				// value <= values, met the condition, need to move left.
				if (left == right || left + 1 == right) return nn;
				found = nn;
			 	right = nn-1;
			 }
			 else
			 {
				// vlaue > values
				if (left == right || left + 1 == right) return found;
				left = nn+1;
			 }
			 break;

		case eAosOpr_le:
			 if (rslt < 0)
			 {
				// value < values, met the condition, need to move left.
				if (left == right || left + 1 == right) return nn;
				found = nn;
			 	right = nn-1;
			 }
			 else
			 {
				// vlaue > values
				if (left == right || left + 1 == right) return found;
				left = nn+1;
			 }
			 break;

		case eAosOpr_eq:
			 if (rslt == 0)
			 {
				// value < values, met the condition, need to move left.
				if (left == right || left + 1 == right) return nn;
				found = nn;
			 	right = nn-1;
			 }
			 else
			 {
				// vlaue > values
				if (left == right || left + 1 == right) return found;
				left = nn+1;
			 }
			 break;

		case eAosOpr_ne:
			 // values[start_idx] == value, this means that we need to search 
			 // from right to the left.
			 if (rslt != 0)
			 {
				// value != values, met the condition, need to move left.
				if (left == right || left + 1 == right) return nn;
				found = nn;
			 	right = nn-1;
			 }
			 else
			 {
				// vlaue == values
				if (left == right || left + 1 == right) return found;
				left = nn+1;
			 }
			 break;

		default:
			 OmnAlarm << "Unrecognized opr: " << opr << enderr;
			 return -5;
		}
	}

	OmnShouldNeverComeHere;
	return -5;
}

#endif

